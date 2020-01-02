#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../include/hw_interface.h"
#include "../include/ei_utils.h"
#include "../include/ei_draw.h"
#include "../include/ei_types.h"

ei_rect_t * intersection(const ei_rect_t * rect1, const ei_rect_t * rect2);

/* COPY_SURFACE */

int   ei_copy_surface  (ei_surface_t destination,
																								const ei_rect_t* dst_rect,
																								const ei_surface_t source,
																								const ei_rect_t* src_rect,
																								const ei_bool_t alpha)
{
								int width, height;
								ei_size_t dst_size = hw_surface_get_size(destination);
								ei_size_t src_size = hw_surface_get_size(source);


								uint32_t * dst_ptr_pixel = (uint32_t *) hw_surface_get_buffer(destination);
								uint32_t * src_ptr_pixel = (uint32_t *) hw_surface_get_buffer(source);

								if (dst_rect == NULL) {
																// Si NULL on prend toute la surface
																ei_point_t top_left = {0, 0};
																ei_rect_t dst_tmp;
																dst_tmp.top_left = top_left;
																dst_tmp.size = dst_size;
																dst_rect = &dst_tmp;
								}
								if (src_rect == NULL) {
																// Si NULL on prend toute la surface
																ei_point_t top_left = {0, 0};
																ei_rect_t src_tmp;
																src_tmp.top_left = top_left;
																src_tmp.size = src_size;
																src_rect = &src_tmp;
								}

								/* test de dépassement de la fenetre source */
								int src_x0 = src_rect->top_left.x;
								int src_y0 = src_rect->top_left.y;
								ei_size_t src_rect_size = src_rect->size;
								if (src_x0 + src_rect_size.width > src_size.width)
																src_rect_size.width = src_size.width - src_x0;
								if (src_y0 + src_rect_size.height > src_size.height)
																src_rect_size.height = src_size.height - src_y0;

								/* test de dépassement de la fenetre destination */
								int dst_x0 = dst_rect->top_left.x;
								int dst_y0 = dst_rect->top_left.y;
								ei_size_t dst_rect_size = dst_rect->size;
								if (dst_x0 + dst_rect_size.width > dst_size.width)
																dst_rect_size.width = dst_size.width - dst_x0;
								if (dst_y0 + dst_rect_size.height > dst_size.height)
																dst_rect_size.height = dst_size.height - dst_y0;


								// On accepte les coordonnées négatives
								// afin de clipper les bords gauche et supérieur
								// cf tests/text.c
								if (dst_x0 < 0) {
																src_x0 -= dst_x0;
																src_rect_size.width += dst_x0;
																dst_x0 = 0;
								}
								if (dst_y0 < 0) {
																src_y0 -= dst_y0;
																src_rect_size.height += dst_y0;
																dst_y0 = 0;
								}
								if (src_x0 < 0) {
																dst_x0 -= src_x0;
																dst_rect_size.width += src_x0;
																src_x0 = 0;
								}
								if (src_y0 < 0) {
																dst_y0 -= src_y0;
																dst_rect_size.height += src_y0;
																src_y0 = 0;
								}

								/* si src_rect plus grand que dst_rect */
								width = src_rect_size.width > dst_rect_size.width ?
																dst_rect_size.width : src_rect_size.width;
								height = src_rect_size.height > dst_rect_size.height ?
																	dst_rect_size.height : src_rect_size.height;

								src_ptr_pixel += src_x0 + src_y0*src_size.width;
								dst_ptr_pixel += dst_x0 + dst_y0*dst_size.width;


								/* Operation de copie de surface */
								for (int y = 0;
													y < height;
													y++,
													src_ptr_pixel = src_ptr_pixel + src_size.width - width,
													dst_ptr_pixel = dst_ptr_pixel + dst_size.width - width) {
																for (int x = 0;
																					x < width;
																					x++,
																					src_ptr_pixel++, dst_ptr_pixel++) {
																								if (alpha) {
																																/* Calcul de l'intensite des couleurs (:= alpha) */
																																uint32_t src_color = *src_ptr_pixel;
																																uint32_t dst_color = *dst_ptr_pixel;
																																uint64_t alpha_color = 0;

																																int ir, ig, ib, ia;
																																hw_surface_get_channel_indices(source, &ir, &ig, &ib, &ia);
																																uint64_t src_red = (src_color >> 8*ir) & 0xff;
																																uint64_t src_green = (src_color >> 8*ig) & 0xff;
																																uint64_t src_blue = (src_color >> 8*ib) & 0xff;
																																uint64_t src_alpha = (src_color >> 8*ia) & 0xff;
																																hw_surface_get_channel_indices(destination, &ir, &ig, &ib, &ia);
																																uint64_t dst_red = (dst_color >> 8*ir) & 0xff;
																																uint64_t dst_green = (dst_color >> 8*ig) & 0xff;
																																uint64_t dst_blue = (dst_color >> 8*ib) & 0xff;

																																alpha_color += ( (src_red*src_alpha +
																																																		(255 - src_alpha)*dst_red) >> 8 ) << 8*ir;
																																alpha_color += ( (src_green*src_alpha +
																																																		(255 - src_alpha)*dst_green) >> 8 ) << 8*ig;
																																alpha_color += ( (src_blue*src_alpha +
																																																		(255 - src_alpha)*dst_blue) >> 8 ) << 8*ib;
																																*dst_ptr_pixel = alpha_color;
																								}
																								else *dst_ptr_pixel = *src_ptr_pixel;
																}
								}
								return 0;
}

/* FIN COPY_SURFACE */

/* **** DRAW_TEXT **** */

/* */
int max(int x, int y) {
								return (x > y) ? x : y;
}
int min(int x, int y) {
								return (x > y) ? y : x;
}

/* Méthode intersection:
 * renvoie si il existe le rectangle correspondant à l'intersection des deux rectangles
 * si un rectangle est NULL, renvoit le second rectangle */
ei_rect_t * intersection(const ei_rect_t * rect1, const ei_rect_t * rect2)
{
								ei_rect_t * intersect = malloc(sizeof(ei_rect_t));
								int x_min, y_min, x_max, y_max;
								int width_min, height_min, width_max, height_max;
								if (rect1->top_left.x < rect2->top_left.x) {
																x_min = rect1->top_left.x;
																x_max = rect2->top_left.x;
																width_min = rect1->size.width;
																width_max = rect2->size.width;
								}
								else {
																x_min = rect2->top_left.x;
																x_max = rect1->top_left.x;
																width_min = rect2->size.width;
																width_max = rect1->size.width;
								}
								if (rect1->top_left.y < rect2->top_left.y) {
																y_min = rect1->top_left.y;
																y_max = rect2->top_left.y;
																height_min = rect1->size.height;
																height_max = rect2->size.height;
								}
								else {
																y_min = rect2->top_left.y;
																y_max = rect1->top_left.y;
																height_min = rect2->size.height;
																height_max = rect1->size.height;
								}
								intersect->size.width = min(width_max, max(
																							width_min - (x_max - x_min), 0));
								intersect->size.height = min(height_max, max(
																							height_min - (y_max - y_min), 0));
								intersect->top_left.x = x_max;
								intersect->top_left.y = y_max;
								return intersect;
}


void   ei_draw_text  (ei_surface_t surface,
																						const ei_point_t* where,
																						const char*  text,
																						const ei_font_t font,
																						const ei_color_t* color,
																						const ei_rect_t* clipper)
{
								/* On recupere les surfaces et rectangles necessaires */
								ei_size_t size = hw_surface_get_size(surface);
								ei_surface_t surface_text = hw_text_create_surface(text, font, color);
								ei_rect_t text_rectangle = hw_surface_get_rect(surface_text);
								ei_rect_t dest_rectangle =  {*where, text_rectangle.size};
								ei_rect_t * intersec = malloc(sizeof(ei_rect_t));
								/* Si clipper NULL on prend toute la surface */
								if (clipper == NULL) {
																ei_rect_t * clip = malloc(sizeof(ei_rect_t));
																clip->size.width = size.width;
																clip->size.height = size.height;
																clip->top_left.x = 0;
																clip->top_left.y = 0;
																intersec = intersection(&dest_rectangle, clip);
								}
								/* Sinon on fait l'intersection */
								else {
																intersec = intersection(&dest_rectangle, clipper);
								}
								/* On ajuste le rectangle de texte */
								text_rectangle.top_left.x += intersec->top_left.x > dest_rectangle.top_left.x ?
																																					intersec->top_left.x - dest_rectangle.top_left.x : 0;
								text_rectangle.top_left.y += intersec->top_left.y > dest_rectangle.top_left.y ?
																																					intersec->top_left.y - dest_rectangle.top_left.y : 0;
								/* On copie la surface de texte sur la surface de destination */
								ei_copy_surface(surface, intersec, surface_text, &text_rectangle, EI_TRUE);
								hw_surface_free(surface_text);
								free(intersec);
}


/* FIN DRAW_TEXT */

void   ei_draw_image  (ei_surface_t surface,
																							const ei_point_t* where,
																							const char*  filename,
																							const ei_rect_t* clipper)
{
								/* On recupere les surfaces et rectangles necessaires */
								ei_surface_t img_surface = hw_image_load(filename, surface);
								ei_size_t size = hw_surface_get_size(surface);
								ei_rect_t text_rectangle = {(ei_point_t){0, 0}, hw_surface_get_size(img_surface)};
								ei_rect_t dest_rectangle =  {*where, text_rectangle.size};
								ei_rect_t * intersec = malloc(sizeof(ei_rect_t));
								/* Si clipper NULL on prend toute la surface */
								if (clipper == NULL) {
																ei_rect_t * clip = malloc(sizeof(ei_rect_t));
																clip->size.width = size.width;
																clip->size.height = size.height;
																clip->top_left.x = 0;
																clip->top_left.y = 0;
																intersec = intersection(&dest_rectangle, clip);
								}
								/* Sinon on fait l'intersection */
								else {
																intersec = intersection(&dest_rectangle, clipper);
								}
								/* On ajuste le rectangle de texte */
								text_rectangle.top_left.x += intersec->top_left.x > dest_rectangle.top_left.x ?
																																					intersec->top_left.x - dest_rectangle.top_left.x : 0;
								text_rectangle.top_left.y += intersec->top_left.y > dest_rectangle.top_left.y ?
																																					intersec->top_left.y - dest_rectangle.top_left.y : 0;
								/* On copie la surface de texte sur la surface de destination */
								ei_copy_surface(surface, intersec, img_surface, &text_rectangle, EI_TRUE);
								hw_surface_free(img_surface);
								free(intersec);
}
