#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/ei_widgetclass.h"
#include "../include/ei_widget.h"
#include "../include/ei_draw.h"
#include "../include/ei_placer.h"
#include "../include/ei_event.h"
#include "../include/ei_application.h"

#include "src_include/polygone.h"
#include "src_include/arc.h"
#include "src_include/widget.h"

static ei_widgetclass_t * SENTINELLE;
int toplevel_act = 0;
static ei_point_t mouse_prev;
static ei_surface_t surface_bouton_alternatif;
int entete_h;
uint64_t dansBoutton = 0;
double temps = 0;
double eps = 0.5;
uint64_t FullScreen = 0;

void set_widget_infront(ei_widget_t* widget);
void draw_rectangle(ei_surface_t surface,
                    ei_rect_t* rectangle,
                    const ei_color_t* color,
                    ei_rect_t* clipper,
                    ei_relief_t relief,
                    int border_width);
/* CLASS FRAME */

ei_widgetclass_name_t frame_name = "frame";

void* frame_allocfunc (void)
{
        ei_frame_widget_t* frame = malloc(sizeof(ei_frame_widget_t));
        memset(frame, 0, sizeof(ei_frame_widget_t));
        return (void*)frame;
}

void frame_releasefunc (struct ei_widget_t* widget)
{
        free((ei_frame_widget_t*) widget);
}

void frame_drawfunc (struct ei_widget_t* widget,
                     ei_surface_t surface,
                     ei_surface_t pick_surface,
                     ei_rect_t * clipper)
{
        /* clipper := parent->content_rect */
        ei_frame_widget_t* frame = (ei_frame_widget_t*)widget;
        ei_rect_t* rectangle = &widget->screen_location;
        draw_rectangle(surface, rectangle, &frame->color, clipper,
                       frame->relief, frame->border_width);
        draw_rectangle(pick_surface, rectangle, widget->pick_color, clipper,
                       ei_relief_none, 0);
        if (frame->img != NULL) {
                ei_surface_t img_surface = frame->img;
                hw_surface_lock(img_surface);
                ei_copy_surface(surface, widget->content_rect, img_surface,
                                frame->img_rect, EI_FALSE);
                hw_surface_unlock(img_surface);
        }
        if (frame->text != NULL && strlen(frame->text) > 0) {
                int width, height;
                hw_text_compute_size(frame->text, frame->text_font,
                                     &width, &height);
                int delta_x = rectangle->size.width - width;
                int x = delta_x < 0 ? 0 : delta_x/2;
                x += rectangle->top_left.x;
                int delta_y = rectangle->size.height - height;
                int y = delta_y < 0 ? 0 : delta_y/2;
                y += rectangle->top_left.y;
                ei_point_t where = (ei_point_t){x, y};
                ei_draw_text(surface, &where, frame->text, frame->text_font,
                             &frame->text_color, clipper);
        }
}

void frame_setdefaultsfunc (struct ei_widget_t* widget)
{
        ei_frame_widget_t* frame = (ei_frame_widget_t*)widget;
        if (widget->parent != NULL) {
                ei_widget_t* parent = widget->parent;
                ei_rect_t parent_rect = *parent->content_rect;
                ei_size_t child_size = {parent_rect.size.width/2,
                                        parent_rect.size.height/2};
                ei_point_t child_point = {parent_rect.top_left.x +
                                          parent_rect.size.width/4,
                                          parent_rect.top_left.y +
                                          parent_rect.size.height/4};
                widget->screen_location = (ei_rect_t){child_point, child_size};
                widget->content_rect = &widget->screen_location;
        }
        else {
                ei_size_t root_size = hw_surface_get_size(
                        ei_app_root_surface());
                widget->screen_location = (ei_rect_t){(ei_point_t){0,0},
                                                      root_size};
                widget->content_rect = &widget->screen_location;
        }
        /* Param par default Frame */
        widget->requested_size = (ei_size_t){0, 0};
        frame->color = ei_default_background_color;
        frame->border_width = 0;
        frame->relief = ei_relief_none;
        frame->text = NULL;
        frame->text_font = ei_default_font;
        frame->text_color = ei_font_default_color;
        frame->text_anchor = ei_anc_center;
        frame->img = NULL;
        frame->img_rect = NULL;
        frame->img_anchor = ei_anc_center;
}

void frame_geomnotifyfunc (struct ei_widget_t* widget,
                           ei_rect_t rect)
{
}

ei_bool_t frame_handlefunc (struct ei_widget_t* widget,
                            struct ei_event_t * event)
{
        if (widget != ei_app_root_widget() &&
            event->type == ei_ev_mouse_buttondown) {
                ei_event_set_active_widget(widget);
                set_widget_infront(widget);
        }
        return EI_FALSE;
}

void      ei_frame_register_class   ()
{
        ei_widgetclass_t * frame_class = malloc(sizeof(ei_widgetclass_t));
        char name[] = "frame";
        strcpy(frame_class->name, name);
        frame_class->allocfunc = frame_allocfunc;
        frame_class->releasefunc = &frame_releasefunc;
        frame_class->drawfunc = &frame_drawfunc;
        frame_class->setdefaultsfunc = &frame_setdefaultsfunc;
        frame_class->geomnotifyfunc = &frame_geomnotifyfunc;
        frame_class->handlefunc = &frame_handlefunc;
        ei_widgetclass_register(frame_class);
}
/* CLASS  BUTTON */
ei_widgetclass_name_t button_name = "button";

void* button_allocfunc (void)
{
        ei_button_widget_t* button = malloc(sizeof(ei_button_widget_t));
        memset(button, 0, sizeof(ei_button_widget_t));
        return (void*)button;
}

void button_releasefunc (struct ei_widget_t* widget)
{
        free((ei_button_widget_t*)widget);
}

void button_drawfunc (struct ei_widget_t* widget,
                      ei_surface_t surface,
                      ei_surface_t pick_surface,
                      ei_rect_t * clipper)
{
        ei_size_t root_size = hw_surface_get_size(surface);
        ei_rect_t root_rect = {(ei_point_t){0, 0}, root_size};
        ei_rect_t* clip = malloc(sizeof(ei_rect_t));
        clip = intersection(&root_rect, clipper);
        ei_button_widget_t * button = (ei_button_widget_t*) widget;
        ei_size_t size = hw_surface_get_size(surface);
        const int nombre_points = 6; // raisonnable
        Bouton (surface,
                &widget->screen_location,
                button->color,
                button->corner_radius,
                button->border_width,
                nombre_points,
                clip,
                button->text,
                button->text_color,
                ei_style_normal,
                widget->screen_location.size.height / 3,
                button->relief == ei_relief_sunken ? 1 : 0);

        rectangleArrondi (pick_surface,
                          &widget->screen_location,
                          *widget->pick_color,
                          button->corner_radius,
                          nombre_points,
                          clip);
        if (button->img != NULL) {
                ei_surface_t img_surface = button->img;
                hw_surface_lock(img_surface);
                ei_copy_surface(surface, widget->content_rect, img_surface,
                                button->img_rect, EI_FALSE);
                hw_surface_unlock(img_surface);
        }
}
void button_setdefaultsfunc (struct ei_widget_t* widget)
{

        ei_button_widget_t * button = (ei_button_widget_t*) widget;
        /* Param par default Button */
        widget->requested_size = (ei_size_t){0, 0};
        button->color = ei_default_background_color;
        button->border_width = k_default_button_border_width;
        button->corner_radius = k_default_button_corner_radius;
        button->relief = ei_relief_raised;
        button->text = NULL;
        button->text_font = ei_default_font;
        button->text_color = ei_font_default_color;
        button->text_anchor = ei_anc_center;
        button->img = NULL;
        button->img_rect = NULL;
        button->img_anchor = ei_anc_center;
        button->callback = NULL;
        button->user_param = NULL;

        ei_point_t topleft = widget->parent == NULL ?
                             (ei_point_t){0, 0} : widget->parent->content_rect->top_left;
        widget->screen_location = (ei_rect_t){topleft,
                                              (ei_size_t){2*button->border_width, 2*button->border_width}};
        widget->content_rect = &widget->screen_location;
}

void button_geomnotifyfunc (struct ei_widget_t* widget,
                            ei_rect_t rect)
{
}

ei_bool_t button_handlefunc (struct ei_widget_t* widget,
                             struct ei_event_t * event)
{
        ei_button_widget_t* button = (ei_button_widget_t*) widget;
        if (event->type == ei_ev_mouse_buttondown) {
                ei_relief_t relief = ei_relief_sunken;
                ei_button_configure(widget, NULL, NULL, NULL, NULL,
                                    &relief, NULL, NULL, NULL, NULL, NULL, NULL,
                                    NULL, NULL, NULL);
                ei_event_set_active_widget(widget);
                set_widget_infront(widget);
                return EI_TRUE;
        }


        if (event->type == ei_ev_mouse_buttonup) {
                ei_relief_t relief = ei_relief_raised;
                ei_button_configure(widget, NULL, NULL, NULL, NULL,
                                    &relief, NULL,
                                    NULL, NULL, NULL, NULL, NULL,
                                    NULL, NULL, NULL);
                if (inClipper(event->param.mouse.where.x,
                              event->param.mouse.where.y, &widget->screen_location)) {
                        button->callback(widget, event, button->user_param);
                }
                ei_event_set_active_widget(NULL);
                return EI_TRUE;
        }
        return EI_FALSE;
}

void      ei_button_register_class  ()
{
        ei_widgetclass_t * button_class = malloc(sizeof(ei_widgetclass_t));
        /* A verifier le type */
        char name[] = "button";
        strcpy(button_class->name, name);
        button_class->allocfunc = button_allocfunc;
        button_class->releasefunc = &button_releasefunc;
        button_class->drawfunc = &button_drawfunc;
        button_class->setdefaultsfunc = &button_setdefaultsfunc;
        button_class->geomnotifyfunc = &button_geomnotifyfunc;
        button_class->handlefunc = &button_handlefunc;
        ei_widgetclass_register(button_class);
}

/* TOPLEVEL */
ei_widgetclass_name_t toplevel_name = "toplevel";

void* toplevel_allocfunc (void)
{
        ei_toplevel_widget_t* toplevel = malloc(sizeof(ei_toplevel_widget_t));
        memset(toplevel, 0, sizeof(ei_toplevel_widget_t));
        return (void*)toplevel;
}

void toplevel_releasefunc (struct ei_widget_t* widget)
{
        free((ei_toplevel_widget_t*)widget);
}

void toplevel_drawfunc (struct ei_widget_t* widget,
                        ei_surface_t surface,
                        ei_surface_t pick_surface,
                        ei_rect_t * clipper)
{
        ei_toplevel_widget_t* toplevel = (ei_toplevel_widget_t*)widget;
        surface_bouton_alternatif = surface;
        ei_rect_t entete = {(ei_point_t){widget->screen_location.top_left.x,
                                         widget->screen_location.top_left.y
                                         - entete_h},
                            (ei_size_t){widget->screen_location.size.width,
                                        entete_h}};
        ei_point_t p_content = {widget->screen_location.top_left.x,
                                widget->screen_location.top_left.y};
        ei_rect_t content = {p_content,
                             (ei_size_t){widget->screen_location.size.width,
                                         widget->screen_location.size.height}};
        ei_color_t color = toplevel->color;
        ei_color_t* border_color = malloc(sizeof(ei_color_t));
        border_color->red = color.red - 40;
        border_color->green = color.green - 40;
        border_color->blue = color.blue - 40;
        border_color->alpha = color.alpha;

        draw_rectangle(surface, &entete, border_color, clipper, ei_relief_none, 0);
        draw_rectangle(surface, &content, &toplevel->color, clipper, ei_relief_none,
                       toplevel->border_width);
        draw_rectangle(pick_surface, &entete, widget->pick_color,
                       clipper, ei_relief_none, 0);
        draw_rectangle(pick_surface, &content, widget->pick_color,
                       clipper, ei_relief_none, 0);
        if (toplevel->title != NULL) {
                ei_font_t font = ei_default_font;
                ei_color_t color = ei_font_default_color;
                ei_point_t where_title = {widget->screen_location.top_left.x +
                                          entete_h + 10,
                                          widget->screen_location.top_left.y -
                                          entete_h };
                ei_draw_text(surface, &where_title, toplevel->title,
                             font, &color, clipper);
        }
        if (toplevel->closable) {
                ei_color_t red = {0xff, 0x00, 0x00, 0xff};
                ei_color_t dark_red = {0xb7, 0x00, 0x00, 0xff};
                ei_point_t where = {entete.top_left.x + entete_h/2,
                                    entete.top_left.y + entete_h/2};
                ei_rect_t root_rect = {(ei_point_t){0,0},
                                       hw_surface_get_size(surface)};
                if((uint64_t)widget == dansBoutton) {draw_circle(surface,
                                                            &where, entete_h/3, dark_red, 20,
                                                            intersection(&root_rect,&entete));}
                else{draw_circle(surface, &where, entete_h/3,
                                 red, 20, intersection(&root_rect,&entete));}
        }

        if (toplevel->resizable) {
                ei_point_t topleft_resize = {widget->screen_location.top_left.x
                                             + widget->screen_location.size.width - 15,
                                             widget->screen_location.top_left.y
                                             + widget->screen_location.size.height
                                             - 15};
                ei_size_t size_resize = {15, 15};
                ei_rect_t resize_square = {topleft_resize, size_resize};
                draw_rectangle(surface, &resize_square, border_color, clipper,
                               ei_relief_none, 0);
        }
}


void toplevel_setdefaultsfunc(struct ei_widget_t *widget) {
        ei_toplevel_widget_t *toplevel = (ei_toplevel_widget_t *)widget;
        ei_widget_t *parent = widget->parent;
        ei_rect_t parent_rect = parent->screen_location;
        parent_rect.top_left.y += entete_h;
        ei_size_t default_size = {320, 240};
        widget->screen_location = (ei_rect_t){parent_rect.top_left,
                                              default_size};
        widget->content_rect = malloc(sizeof(ei_rect_t));
        widget->content_rect->size = widget->screen_location.size;
        widget->content_rect->size.height -= entete_h;
        widget->content_rect->top_left = widget->screen_location.top_left;
        widget->content_rect->top_left.y += entete_h;
        widget->requested_size = default_size;
        /* Param par défault de Toplevel */
        toplevel->min_size = malloc(sizeof(ei_size_t));
        toplevel->min_size->width = 160;
        toplevel->min_size->height = 120;
        widget->placer_params->h_data = 120;
        widget->placer_params->w_data = 160;
        toplevel->color = ei_default_background_color;
        toplevel->border_width = 4;
        toplevel->title = "Toplevel";
        toplevel->closable = EI_TRUE;
        toplevel->resizable = ei_axis_both;
}

void toplevel_geomnotifyfunc(struct ei_widget_t *widget, ei_rect_t rect) {
}

/* move the widget and it's children */
void move(ei_widget_t *widget, int x, int y) {
        if (widget != NULL) {
                ei_widget_t *child = widget->children_head;
                widget->screen_location.top_left.x += x;
                widget->screen_location.top_left.y += y;
                while (child != NULL) {
                        move(child, x, y);
                        child = child->next_sibling;
                }
        }
}

ei_bool_t toplevel_handlefunc(struct ei_widget_t *widget,
                              struct ei_event_t *event) {
        ei_toplevel_widget_t *toplevel = (ei_toplevel_widget_t *)widget;

        if (event->type == ei_ev_mouse_buttondown) {
                ei_event_set_active_widget(widget);
                set_widget_infront(widget);

                if (event->param.mouse.where.y - widget->screen_location.top_left.y <
                    0) {
                        if (event->param.mouse.where.x < widget->screen_location.top_left.x +
                            entete_h / 2 - entete_h / 3 ||
                            event->param.mouse.where.x > widget->screen_location.top_left.x +
                            entete_h / 2 + entete_h / 3 ||
                            event->param.mouse.where.y > widget->screen_location.top_left.y -
                            entete_h / 2 + entete_h / 3 ||
                            event->param.mouse.where.y < widget->screen_location.top_left.y -
                            entete_h / 2 - entete_h / 3) {
                                // Double clic sur l'entete pour passer en FullScreen
                                if(temps == 0) {temps = hw_now();}
                                else{
                                        if(hw_now()-temps < eps) {
                                                if (FullScreen != (uint64_t)widget) {
                                                        float rw=0, rh=0, rx=0, ry=0;
                                                        int x=0, y=entete_h;
                                                        int w=widget->parent->content_rect->size.width,
                                                            h=widget->parent->content_rect->size.height - entete_h;
                                                        ei_place(widget, NULL,
                                                                 &x, &y, &w, &h,
                                                                 &rx, &ry, &rw, &rh);
                                                        FullScreen = (uint64_t)widget;
                                                }
                                                else {
                                                        float rw=0, rh=0, rx=0, ry=0;
                                                        int x=0, y=entete_h;
                                                        int w = widget->requested_size.width;
                                                        int h = widget->requested_size.height;
                                                        ei_place(widget, NULL,
                                                                 &x, &y, &w, &h,
                                                                 &rx, &ry, &rw, &rh);
                                                        FullScreen = 0;
                                                }
                                        }
                                        else{
                                                temps = hw_now();
                                        }
                                }
                                toplevel_act = 1;
                        } else {
                                if (widget->parent->children_head == widget) {
                                        widget->parent->children_head = widget->next_sibling;
                                }
                                else
                                {
                                        for (ei_widget_t *current = widget->parent->children_head;
                                             current != NULL; current = current->next_sibling) {
                                                if (current->next_sibling == widget) {
                                                        current->next_sibling = widget->next_sibling;
                                                        if (current->next_sibling == NULL) {
                                                                current->parent->children_tail = current;
                                                        }
                                                        break;
                                                }
                                        }
                                }
                                //widget->placer_params = NULL;
                                ei_placer_forget(widget);
                                return EI_TRUE;
                        }
                }
                ei_point_t topleft_resize = {widget->screen_location.top_left.x +
                                             widget->screen_location.size.width - 15,
                                             widget->screen_location.top_left.y +
                                             widget->screen_location.size.height - 15};
                ei_size_t size_resize = {15, 15};
                ei_rect_t resize_square = {topleft_resize, size_resize};
                if (inClipper(event->param.mouse.where.x, event->param.mouse.where.y,
                              &resize_square)) {
                        /* RESIZE */
                        FullScreen = 0;
                        toplevel_act = 2;
                }
                ei_event_set_active_widget(widget);
                mouse_prev = event->param.mouse.where;
                return EI_TRUE;
        }

        if (event->type == ei_ev_mouse_move) {
                if (event->param.mouse.where.x > widget->screen_location.top_left.x +
                    entete_h / 2 - entete_h / 3 &&
                    event->param.mouse.where.x < widget->screen_location.top_left.x +
                    entete_h / 2 + entete_h / 3 &&
                    event->param.mouse.where.y < widget->screen_location.top_left.y -
                    entete_h / 2 + entete_h / 3 &&
                    event->param.mouse.where.y > widget->screen_location.top_left.y -
                    entete_h / 2 - entete_h / 3) {
                        dansBoutton = (uint64_t)widget;
                }
                else{dansBoutton = 0;}
                if (toplevel_act == 1) {
                        int x_curr = event->param.mouse.where.x;
                        int y_curr = event->param.mouse.where.y;
                        int x_prev = mouse_prev.x;
                        int y_prev = mouse_prev.y;
                        int dx = x_curr - x_prev;
                        int dy = y_curr - y_prev;
                        int toplevel_x = dx + widget->screen_location.top_left.x;
                        int toplevel_y = dy + widget->screen_location.top_left.y;
                        /* appel au placer sur les descendants */
                        ei_place(widget, NULL, &toplevel_x, &toplevel_y, NULL,
                                 NULL, NULL, NULL,
                                 NULL, NULL);
                        mouse_prev = event->param.mouse.where;
                        return EI_TRUE;
                } else if (toplevel_act == 2) {
                        int X = event->param.mouse.where.x, Y = event->param.mouse.where.y;
                        int DX = (toplevel->resizable == ei_axis_x ||
                                  toplevel->resizable == ei_axis_both)
                                 ? X - mouse_prev.x + widget->screen_location.size.width
                                 : widget->screen_location.size.width;
                        int DY = (toplevel->resizable == ei_axis_y ||
                                  toplevel->resizable == ei_axis_both)
                                 ? Y - mouse_prev.y + widget->screen_location.size.height
                                 : widget->screen_location.size.height;
                        if (DX <= toplevel->min_size->width)
                                DX = toplevel->min_size->width;
                        if (DY <= toplevel->min_size->height)
                                DY = toplevel->min_size->height;
                        ei_place(widget, NULL, NULL, NULL, &DX, &DY, NULL,
                                 NULL, NULL, NULL);
                        mouse_prev = event->param.mouse.where;
                        return EI_TRUE;
                }
        }

        if (event->type == ei_ev_mouse_buttonup) {
                ei_event_set_active_widget(NULL);
                toplevel_act = 0;
                return EI_TRUE;
        }
        return EI_FALSE;
}

void      ei_toplevel_register_class  ()
{
        ei_widgetclass_t * toplevel_class = malloc(sizeof(ei_widgetclass_t));
        char name[] = "toplevel";
        strcpy(toplevel_class->name, name);
        toplevel_class->allocfunc = toplevel_allocfunc;
        toplevel_class->releasefunc = &toplevel_releasefunc;
        toplevel_class->drawfunc = &toplevel_drawfunc;
        toplevel_class->setdefaultsfunc = &toplevel_setdefaultsfunc;
        toplevel_class->geomnotifyfunc = &toplevel_geomnotifyfunc;
        toplevel_class->handlefunc = &toplevel_handlefunc;
        ei_widgetclass_register(toplevel_class);
}

void      ei_widgetclass_register   (ei_widgetclass_t* widgetclass)
{
        /* Il y a 3 classes de widgets .. */
        if (SENTINELLE == NULL) {
                SENTINELLE = widgetclass;
                SENTINELLE->next = NULL;
        }
        else if (SENTINELLE->next == NULL) {
                SENTINELLE->next = widgetclass;
                SENTINELLE->next->next = NULL;
        }
        else {
                SENTINELLE->next->next = widgetclass;
                SENTINELLE->next->next->next = NULL;
        }
}

ei_widgetclass_t* ei_widgetclass_from_name  (ei_widgetclass_name_t name)
{
        ei_widgetclass_t * current = SENTINELLE;
        while (current != NULL && strcmp(current->name,name) != 0) {
                current = current->next;
        }
        return current;

}
