#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "../include/hw_interface.h"
#include "../include/ei_types.h"
#include "../include/ei_draw.h"

int minimum(int a, int b) {
        return a < b ? a : b;
}

int maximum(int a, int b) {
        return a > b ? a : b;
}


void draw_rectangle(ei_surface_t surface,
                    ei_rect_t* rectangle,
                    const ei_color_t* color,
                    ei_rect_t* clipper,
                    ei_relief_t relief,
                    int border_width)
{
        int x, y, x_max, y_max;
        ei_size_t size = hw_surface_get_size(surface);
        if (rectangle == NULL) {
                x = 0;
                y = 0;
                x_max = size.width;
                y_max = size.height;
        }
        else {
                x = minimum(size.width, rectangle->top_left.x);
                y = minimum(size.height, rectangle->top_left.y);
                x_max = minimum(size.width, x + rectangle->size.width);
                y_max = minimum(size.height, y + rectangle->size.height);
                if (x < 0) x = 0;
                if (y < 0) y = 0;
        }
        if (clipper != NULL) {
                if (clipper->top_left.x > size.width)
                        clipper->top_left.x = size.width;
                if (clipper->top_left.y > size.height)
                        clipper->top_left.y = size.height;
                x = maximum(x, clipper->top_left.x);
                y = maximum(y, clipper->top_left.y);
                x_max = minimum(x_max, clipper->top_left.x + clipper->size.width);
                y_max = minimum(y_max, clipper->top_left.y + clipper->size.height);
        }
        ei_color_t* light = malloc(sizeof(ei_color_t));
        ei_color_t* dark = malloc(sizeof(ei_color_t));
        light->red = color->red + 40 > 0xff ? 0xff : color->red + 40;
        light->green = color->green + 40 > 0xff ? 0xff : color->green + 40;
        light->blue = color->blue + 40 > 0xff ? 0xff : color->blue + 40;
        light->alpha = color->alpha;
        dark->red = color->red - 40 < 0x00 ? 0x00 : color->red - 40;
        dark->green = color->green - 40 < 0x00 ? 0x00 : color->green - 40;
        dark->blue = color->blue - 40 < 0x00 ? 0x00 : color->blue - 40;
        dark->alpha = color->alpha;
        uint32_t couleur = ei_map_rgba (surface, color);
        uint32_t light_couleur = ei_map_rgba (surface, light);
        uint32_t dark_couleur = ei_map_rgba (surface, dark);
        uint32_t* ptrpixel = (uint32_t*) hw_surface_get_buffer(surface);
        ptrpixel += x + y*size.width;
        for (int j=y; j < y_max; j++) {
                for (int i=x; i < x_max; i++, ptrpixel++) {
                        *ptrpixel = couleur;
                        for (int k=0; k<border_width; k++) {
                                if (j == y+k || i == x+k) {
                                        if (relief == ei_relief_raised) {
                                                *ptrpixel = light_couleur;
                                                break;
                                        }
                                        else if (relief == ei_relief_sunken) {
                                                *ptrpixel = dark_couleur;
                                                break;
                                        }
                                        else if (border_width) {
                                                *ptrpixel = dark_couleur;
                                        }
                                        else {
                                                *ptrpixel = couleur;
                                                break;
                                        }

                                }
                                else if (j == y_max-k-1 || i == x_max-k-1) {
                                        if (relief == ei_relief_raised) {
                                                *ptrpixel = dark_couleur;
                                                break;
                                        }
                                        else if (relief == ei_relief_sunken) {
                                                *ptrpixel = light_couleur;
                                                break;
                                        }
                                        else if (border_width) {
                                                *ptrpixel = dark_couleur;
                                        }
                                        else {
                                                *ptrpixel = couleur;
                                                break;
                                        }
                                }
                        }
                }
                ptrpixel+= size.width - (x_max - x);
        }
}

uint32_t ei_map_rgba (ei_surface_t surface, const ei_color_t* color)
{
        uint32_t pix;
        int ir, ig, ib, ia;
        hw_surface_get_channel_indices(surface, &ir, &ig, &ib, &ia);
        pix = (color->red << 8*ir)
              + (color->green << 8*ig)
              + (color->blue << 8*ib)
              + ((ia == -1) ? 0 : color->alpha << 8*ia);
        return pix;
}

/* inClipper renvoie 1 si le point de coordonnée x,y
      appartient au clipper et renvoie 0 sinon       */
int inClipper(int x, int y, const ei_rect_t* clipper)
{
        if (clipper == NULL) return 1;
        else {
                return (clipper->top_left.x < x && x < clipper->top_left.x +
                        clipper->size.width) &&
                       (clipper->top_left.y < y && y < clipper->top_left.y +
                        clipper->size.height);
        }
}

void vertical_line(ei_surface_t surface,
                   ei_size_t size,
                   const ei_color_t color,
                   ei_point_t p1,
                   ei_point_t p2,
                   int delta_y,
                   const ei_rect_t* clipper)
{
        /* Droite verticale */
        int x1 = p1.x, y1 = p1.y;
        int x2 = p2.x, y2 = p2.y;
        if (delta_y < 0) {
                int tmp = y1;
                y1 = y2;
                y2 = tmp;
        }

        uint32_t couleur = ei_map_rgba (surface, &color);
        uint32_t* ptrpixel = (uint32_t*) hw_surface_get_buffer(surface);
        uint32_t* adDebut = ptrpixel;
        for (ptrpixel += x1 + y1*size.width;
             ptrpixel <= adDebut + x2 + y2*size.width;
             ptrpixel += size.width, y1++) {
                if (inClipper(x1, y1, clipper)) {
                        *ptrpixel = couleur;
                }
        }
}

void horizontal_line(ei_surface_t surface,
                     ei_size_t size,
                     const ei_color_t color,
                     ei_point_t p1,
                     ei_point_t p2,
                     int delta_x,
                     const ei_rect_t* clipper)
{
        /* Droite horizontale */
        int x1 = p1.x, y1 = p1.y;
        int x2 = p2.x, y2 = p2.y;
        if (delta_x < 0) {
                int tmp = x1;
                x1 = x2;
                x2 = tmp;
        }
        uint32_t couleur = ei_map_rgba (surface, &color);
        uint32_t* ptrpixel = (uint32_t*) hw_surface_get_buffer(surface);
        uint32_t* adDebut = ptrpixel;
        for (ptrpixel += x1 + y1*size.width;
             ptrpixel <= adDebut + x2 + y2*size.width;
             ptrpixel++, x1++) {
                if (inClipper(x1, y1, clipper)) {
                        *ptrpixel = couleur;
                }
        }
}
/* On applique l'algorithme de bresenham */
void bresenham(ei_surface_t surface,
               ei_size_t size,
               const ei_color_t color,
               ei_point_t p1,
               ei_point_t p2,
               const ei_rect_t* clipper)
{
        int x1 = p1.x, y1 = p1.y;
        int x2 = p2.x, y2 = p2.y;
        int delta_x = x2 - x1;
        int delta_y = y2 - y1;
        int E = 0;
        uint32_t couleur = ei_map_rgba (surface, &color);
        uint32_t* ptrpixel = (uint32_t*) hw_surface_get_buffer(surface);
        ptrpixel += x1 + y1*size.width;
        if (abs(delta_x) > abs(delta_y)) {
                if (x1 > x2) {bresenham(surface, size, color, p2, p1, clipper); return;}
                int ystepPtr = (delta_y > 0) ? size.width : -size.width;
                int ystep = (delta_y > 0) ? 1 : -1;
                while (x1 < x2) {
                        x1++;
                        ptrpixel++;
                        E += abs(delta_y);
                        if (E << 1 > delta_x) {
                                y1 += ystep;
                                ptrpixel += ystepPtr;
                                E -= delta_x;
                        }
                        if (inClipper(x1, y1, clipper)) {
                                *ptrpixel = couleur;
                        }
                }
        } else {
                if (y1 > y2) {bresenham(surface, size, color, p2, p1, clipper); return;}
                int xstepPtr = (delta_x > 0) ? 1 : -1;
                int xstep = (delta_x > 0) ? 1 : -1;
                while (y1 < y2) {
                        y1++;
                        ptrpixel += size.width;
                        E += abs(delta_x);
                        if (E << 1 > delta_y) {
                                x1 += xstep;
                                ptrpixel += xstepPtr;
                                E -= delta_y;
                        }
                        if (inClipper(x1, y1, clipper)) {
                                *ptrpixel = couleur;
                        }
                }
        }
}

void ei_draw_line(ei_surface_t surface,
                  ei_color_t color,
                  ei_point_t p1,
                  ei_point_t p2,
                  const ei_rect_t* clipper)
{
        int x1 = p1.x, y1 = p1.y;
        int x2 = p2.x, y2 = p2.y;
        int delta_x = x2 - x1;
        int delta_y = y2 - y1;
        ei_size_t size = hw_surface_get_size(surface);
        /* Droite verticale */
        if (delta_x == 0) vertical_line(surface, size, color, p1, p2,
                                        delta_y, clipper);
        /* Droite horizontale */
        else if (delta_y == 0) horizontal_line(surface, size, color, p1, p2,
                                               delta_x, clipper);
        else {
                bresenham(surface, size, color, p1, p2, clipper);
        }
}

void      ei_draw_polyline  (ei_surface_t surface,
                             const ei_linked_point_t* first_point,
                             const ei_color_t color,
                             const ei_rect_t*   clipper)
{
        while (first_point->next != NULL) {
                ei_draw_line(surface, color, first_point->point,
                             first_point->next->point, clipper);
                first_point = first_point->next;
        }
}

void      ei_fill     (ei_surface_t surface,
                       const ei_color_t*  color,
                       const ei_rect_t* clipper)
{
        uint32_t couleur = ei_map_rgba (surface, color);
        ei_size_t size = hw_surface_get_size(surface);
        uint32_t* ptrpixel = (uint32_t*) hw_surface_get_buffer(surface);
        *ptrpixel = couleur;
        int x0 = clipper == NULL ? 0 : clipper->top_left.x;
        int y0 = clipper == NULL ? 0 : clipper->top_left.y;
        ei_size_t rect_size = clipper == NULL ? size : clipper->size;

        /* si le clipper depasse du cadre de la fenetre
         * alors on redefinis le clipper pour qu'il arrive
         * juste au bord de la fenetre */
        if (x0 + rect_size.width > size.width)
                rect_size.width = size.width - x0;
        if (y0 + rect_size.height > size.height)
                rect_size.height = size.height - y0;
        ptrpixel += x0 + y0*size.width;
        for (int y = 0;
             y < rect_size.height;
             y++, ptrpixel = ptrpixel + size.width - rect_size.width) {
                for (int x = 0; x < rect_size.width; x++, ptrpixel++) {
                        *ptrpixel = couleur;
                }
        }
}
