#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/ei_types.h"
#include "../include/ei_widget.h"
#include "../include/ei_widgetclass.h"
#include "../include/ei_placer.h"
#include "../include/ei_application.h"
#include "../include/hw_interface.h"
#include "../include/ei_event.h"
#include "../include/ei_draw.h"

#include "src_include/widget.h"
#include "src_include/polygone.h"
#include "src_include/arc.h"

int toplevel_act;
int entete_h;
// Initialisation de ID qui
// identifie un unique widget
int ID = 1;

void add_child(ei_widget_t* parent, ei_widget_t* child);
void default_params(ei_widget_t* widget);
void recursion_destroy(ei_widget_t *widget);

void add_child(ei_widget_t* parent, ei_widget_t* child)
{
        // Ajoute un enfant à un parent
        if (parent != NULL) {
                if (parent->children_head == NULL) {
                        parent->children_head = child;
                        parent->children_tail = child;
                }
                else {
                        ei_widget_t* last = parent->children_tail;
                        last->next_sibling = child;
                        parent->children_tail = child;
                }
        }
}

void default_params(ei_widget_t* widget)
{
        /* Initialisation de placer_params */
        ei_placer_params_t* parametres = malloc(sizeof(ei_placer_params_t));
        parametres->anchor_data = (ei_anchor_t)ei_anc_none;
        parametres->anchor = &parametres->anchor_data;

        parametres->x_data = 0;
        parametres->y_data = 0;
        parametres->w_data = 0;
        parametres->h_data = 0;
        parametres->rx_data = 0;
        parametres->ry_data = 0;
        parametres->rw_data = 0;
        parametres->rh_data = 0;

        parametres->x = &parametres->x_data;
        parametres->y = &parametres->y_data;
        parametres->w = &parametres->w_data;
        parametres->h = &parametres->h_data;
        parametres->rx = &parametres->rx_data;
        parametres->ry = &parametres->ry_data;
        parametres->rw = &parametres->rw_data;
        parametres->rh = &parametres->rh_data;

        widget->placer_params = parametres;
}

ei_widget_t* ei_widget_create (ei_widgetclass_name_t class_name,
                               ei_widget_t* parent)
{
        ei_widgetclass_t* wclass = ei_widgetclass_from_name(class_name);
        ei_widget_t* widget = wclass->allocfunc();
        add_child(parent, widget);
        ID++;
        widget->pick_id = ID;
        widget->pick_color = malloc(sizeof(ei_color_t));
        widget->pick_color->red = ID;
        widget->pick_color->green = 0;
        widget->pick_color->blue = 0;
        widget->pick_color->alpha = -1;

        widget->parent = parent;
        widget->wclass = wclass;
        widget->content_rect = (ei_rect_t*)NULL;
        default_params(widget);
        wclass->setdefaultsfunc(widget);
        return widget;
}

void ei_widget_destroy(ei_widget_t *widget) {

        if (widget == ei_event_get_active_widget()) {
                ei_event_set_active_widget(NULL);
                toplevel_act = 0;
        }
        if (widget->parent != NULL) {
                if (widget->parent->children_head == widget) {
                    if(widget->parent->children_tail == widget){
                          widget->parent->children_head = NULL;
                          widget->parent->children_tail = NULL;
                        }
                        else{
                            widget->parent->children_head = widget->next_sibling;
                        }
                } else {
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
        }
        recursion_destroy(widget);

}

void recursion_destroy(ei_widget_t *widget)
{
        // Free les pointeurs des widgets
        // en partant des widgets les plus "bas"
        // dans l'arbre genealogique et en remontant
        ei_widget_t *child = widget->children_head;
        ei_widget_t* sibling = malloc(sizeof(ei_widget_t*));
        if (child == NULL) {
                widget->wclass->releasefunc(widget);
        } else {
                while (child != NULL) {
                        sibling = child->next_sibling;
                        recursion_destroy(child);
                        child = sibling;
                }
        }
}

ei_widget_t* ei_widget_from_pick_id(uint32_t pick_id, ei_widget_t* root)
{
        // Parcours des descendants de root_widget
        // afin de trouver le widget correspond au pick_id
        ei_widget_t* current;
        if (root == NULL) {
                return NULL;
        } else {
                if (root->pick_id == pick_id) {
                        return root;
                } else {
                        ei_widget_t* fils = root->children_head;
                        if (fils == NULL) {return NULL;}
                        while (fils != NULL) {
                                current = ei_widget_from_pick_id(pick_id, fils);
                                if (current != NULL) {
                                        return current;
                                }
                                fils = fils->next_sibling;
                        }
                }
        }
}

uint32_t pick_id_from_color (ei_surface_t surface, const uint32_t color)
{
        int ir, ig, ib, ia;
        hw_surface_get_channel_indices(surface, &ir, &ig, &ib, &ia);
        uint32_t red = (color >> 8*ir) & 0xff;
        return red;
}

ei_widget_t* ei_widget_pick(ei_point_t* where)
{
        ei_surface_t pick_surface = ei_app_pick_surface();
        uint32_t* ptrpixel = (uint32_t*) hw_surface_get_buffer(pick_surface);
        ei_size_t size = hw_surface_get_size(pick_surface);
        ptrpixel += where->x + where->y * size.width;
        return ei_widget_from_pick_id(pick_id_from_color(pick_surface,
                                                         *ptrpixel),
                                      ei_app_root_widget());
}

void ei_frame_configure   (ei_widget_t*   widget,
                           ei_size_t*   requested_size,
                           const ei_color_t*  color,
                           int*     border_width,
                           ei_relief_t*   relief,
                           char**     text,
                           ei_font_t*   text_font,
                           ei_color_t*    text_color,
                           ei_anchor_t*   text_anchor,
                           ei_surface_t*    img,
                           ei_rect_t**    img_rect,
                           ei_anchor_t*   img_anchor)
{
        ei_frame_widget_t* frame = (ei_frame_widget_t*) widget;
        if (color != NULL) frame->color = *color;
        if (border_width != NULL) frame->border_width = *border_width;
        if (relief != NULL) frame->relief = *relief;
        if (text != NULL) {
                if (frame->text != NULL) {
                        free(frame->text);
                }
                frame->text = malloc(strlen(*text)+1);
                strcpy(frame->text, *text);
        }
        if (text_font != NULL) frame->text_font = *text_font;
        if (text_color != NULL) frame->text_color = *text_color;
        if (text_anchor != NULL) frame->text_anchor = *text_anchor;
        if (img != NULL) frame->img = *img;
        if (img_rect != NULL) frame->img_rect = *img_rect;
        if (img_anchor != NULL) frame->img_anchor = *img_anchor;
        if (requested_size != NULL) {
                /* Requested_Size doit pouvoir contenir
                 * image et text */
                widget->requested_size = *requested_size;
                ei_size_t content_size = (ei_size_t){0, 0};
                if (frame->img != NULL) {
                        ei_size_t img_size = hw_surface_get_size(frame->img);
                        content_size.width += img_size.width;
                        content_size.height += img_size.height;
                }
                if (frame->text != NULL) {
                        int text_w, text_h;
                        hw_text_compute_size(frame->text, frame->text_font,
                                             &text_w, &text_h);
                        content_size.width += text_w;
                        content_size.height += text_h;
                }
                if (requested_size->width < content_size.width) {
                        requested_size->width = content_size.width;
                }
                if (requested_size->height < content_size.height) {
                        requested_size->height = content_size.height;
                }
                widget->screen_location.size = *requested_size;
                widget->content_rect = &widget->screen_location;
                widget->placer_params->w_data = widget->screen_location.size.width;
                widget->placer_params->h_data = widget->screen_location.size.height;
        }
}

void      ei_button_configure   (ei_widget_t*   widget,
                                 ei_size_t*   requested_size,
                                 const ei_color_t*  color,
                                 int*     border_width,
                                 int*     corner_radius,
                                 ei_relief_t*   relief,
                                 char**     text,
                                 ei_font_t*   text_font,
                                 ei_color_t*    text_color,
                                 ei_anchor_t*   text_anchor,
                                 ei_surface_t*    img,
                                 ei_rect_t**    img_rect,
                                 ei_anchor_t*   img_anchor,
                                 ei_callback_t*   callback,
                                 void**     user_param)
{
        ei_button_widget_t* button = (ei_button_widget_t*) widget;
        if (color != NULL) button->color = *color;
        if (border_width != NULL) button->border_width = *border_width;
        if (corner_radius != NULL) button->corner_radius = *corner_radius;
        if (relief != NULL) button->relief = *relief;
        if (text != NULL) button->text = *text;
        if (text_font != NULL) button->text_font = *text_font;
        if (text_color != NULL) button->text_color = *text_color;
        if (text_anchor != NULL) button->text_anchor = *text_anchor;
        if (img != NULL) button->img = *img;
        if (img_rect != NULL) button->img_rect = *img_rect;
        if (img_anchor != NULL) button->img_anchor = *img_anchor;
        if(callback != NULL) button->callback = *callback;
        if (user_param != NULL) button->user_param = *user_param;
        if (requested_size != NULL) {
                widget->requested_size = *requested_size;
        }

        /* Requested_Size doit pouvoir contenir
         * image et text */
        //int maj = 0;
        ei_size_t content_size = (ei_size_t){0, 0};
        if (button->img != NULL) {
                ei_size_t img_size = hw_surface_get_size(button->img);
                content_size.width += img_size.width;
                content_size.height += img_size.height;
        }
        if (button->text != NULL) {
                int text_w, text_h;
                hw_text_compute_size(button->text, button->text_font,
                                     &text_w, &text_h);
                content_size.width += text_w;
                content_size.height += text_h;
        }
        if (widget->requested_size.width < content_size.width) {
                widget->requested_size.width = content_size.width;
        }
        if (widget->requested_size.height < content_size.height) {
                widget->requested_size.height = content_size.height;
        }
}

void      ei_toplevel_configure   (ei_widget_t*   widget,
                                   ei_size_t*   requested_size,
                                   ei_color_t*    color,
                                   int*     border_width,
                                   char**     title,
                                   ei_bool_t*   closable,
                                   ei_axis_set_t*   resizable,
                                   ei_size_t**    min_size)
{
        ei_toplevel_widget_t* toplevel = (ei_toplevel_widget_t*) widget;
        if (color != NULL) toplevel->color = *color;
        if (border_width != NULL) toplevel->border_width = *border_width;
        if (min_size != NULL) toplevel->min_size = *min_size;
        if (title != NULL) {
                toplevel->title = *title;
                int w, h;
                hw_text_compute_size(*title, ei_default_font, &w, &h);
                w += 10 + entete_h;
                if (toplevel->min_size->width < w) toplevel->min_size->width = w;
                if (toplevel->min_size->height < h) toplevel->min_size->height = h;
        }
        if (closable != NULL) toplevel->closable = *closable;
        if (resizable != NULL) toplevel->resizable = *resizable;

        /* MAJ de la geometrie */
        if (requested_size != NULL) {
                /* If Resizable
                 *      If Requested_Size > Min_Size
                 *          maj Screen_Location */
                if (requested_size->width < toplevel->min_size->width)
                        requested_size->width = toplevel->min_size->width;
                if (requested_size->height < toplevel->min_size->height)
                        requested_size->height = toplevel->min_size->height;
                toplevel->requested_size = *requested_size;
                widget->screen_location.size = *requested_size;
                widget->placer_params->w_data = requested_size->width;
                widget->placer_params->h_data = requested_size->height;
                widget->content_rect->size.height = widget->screen_location.size.height;
                widget->content_rect->top_left.y = widget->screen_location.top_left.y;
        }
}
