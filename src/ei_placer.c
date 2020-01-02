#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/ei_draw.h"
#include "../include/ei_placer.h"
#include "../include/ei_types.h"
#include "../include/ei_utils.h"
#include "../include/ei_widget.h"
#include "../include/ei_widgetclass.h"
#include "../include/hw_interface.h"

#include "src_include/widget.h"

int entete_h;

void placer_children(ei_widget_t* widget);

void ei_place(struct ei_widget_t *widget, ei_anchor_t *anchor, int *x, int *y,
              int *width, int *height, float *rel_x, float *rel_y,
              float *rel_width, float *rel_height)
{
        ei_size_t widget_size;
        ei_point_t widget_p;
        ei_rect_t inner_rect;

        ei_rect_t ancien_rect = (ei_rect_t){(ei_point_t){widget->screen_location.top_left.x,
                                                         widget->screen_location.top_left.y},
                                            (ei_size_t) {widget->screen_location.size.width,
                                                         widget->screen_location.size.height}};

        if (x != NULL) {
                widget->placer_params->x = x;
                widget->placer_params->x_data = *x;
        }
        if (y != NULL) {
                widget->placer_params->y = y;
                widget->placer_params->y_data = *y;
        }
        if (rel_x != NULL) {
                widget->placer_params->rx = rel_x;
                widget->placer_params->rx_data = *rel_x;
        }
        if (rel_y != NULL) {
                widget->placer_params->ry = rel_y;
                widget->placer_params->ry_data = *rel_y;
        }
        if (width != NULL) {
                widget->placer_params->w = width;
                widget->placer_params->w_data = *width;
        }
        if (height != NULL) {
                widget->placer_params->h = height;
                widget->placer_params->h_data = *height;
        }
        if (rel_width != NULL) {
                widget->placer_params->rw = rel_width;
                widget->placer_params->rw_data = *rel_width;
        }
        if (rel_height != NULL) {
                widget->placer_params->rh = rel_height;
                widget->placer_params->rh_data = *rel_height;
        }

        // Calcul du nouveau screen_location:
        // on realise les calculs intermediaires avec les variables
        // widget_size et widget_p
        widget_size.width = widget->parent->content_rect->size.width *
                            widget->placer_params->rw_data;
        widget_size.width += widget->placer_params->w_data;
        widget_size.height = widget->parent->content_rect->size.height *
                             widget->placer_params->rh_data;
        widget_size.height += widget->placer_params->h_data;

        // On s'assure que la nouvelle taille est suffisante:
        // pour toplevel on regarde l'attribut min_size
        if (strcmp(widget->wclass->name, "toplevel") == 0) {
                ei_toplevel_widget_t* toplevel = (ei_toplevel_widget_t*) widget;
                if (widget_size.width < toplevel->min_size->width) {
                        widget_size.width = toplevel->min_size->width;
                }
                if (widget_size.height < toplevel->min_size->height) {
                        widget_size.height = toplevel->min_size->height;
                }
        }
        // sinon on regarde requested_size
        else {
                if (widget_size.width < widget->requested_size.width) {
                        widget_size.width = widget->requested_size.width;
                }
                if (widget_size.height < widget->requested_size.height) {
                        widget_size.height = widget->requested_size.height;
                }
        }
        widget_p.x = widget->parent->content_rect->top_left.x +
                     widget->parent->content_rect->size.width *
                     widget->placer_params->rx_data;
        widget_p.x += widget->placer_params->x_data;
        widget_p.y = widget->parent->content_rect->top_left.y +
                     widget->parent->content_rect->size.height *
                     widget->placer_params->ry_data;
        widget_p.y += widget->placer_params->y_data;

        // On modifie widget_p selon l'ancre choisie
        if (anchor != NULL) {
                widget->placer_params->anchor = anchor;
                widget->placer_params->anchor_data = *anchor;
        }

        switch (widget->placer_params->anchor_data) {

        case ei_anc_center:
                widget_p =
                        (ei_point_t){widget_p.x - (widget_size.width >> 1),
                                     widget_p.y - (widget_size.height >> 1)};
                break;
        case ei_anc_north:
                widget_p =
                        (ei_point_t){widget_p.x - (widget_size.width >> 1),
                                     widget_p.y};
                break;
        case ei_anc_northeast:
                widget_p =
                        (ei_point_t){widget_p.x - widget_size.width,
                                     widget_p.y};
                break;
        case ei_anc_east:
                widget_p =
                        (ei_point_t){widget_p.x - widget_size.width,
                                     widget_p.y - (widget_size.height >> 1)};
                break;
        case ei_anc_southeast:
                widget_p =
                        (ei_point_t){widget_p.x - widget_size.width,
                                     widget_p.y - widget_size.height};
                break;
        case ei_anc_south:
                widget_p =
                        (ei_point_t){widget_p.x - (widget_size.width >> 1),
                                     widget_p.y - widget_size.height};
                break;
        case ei_anc_southwest:
                widget_p =
                        (ei_point_t){widget_p.x,
                                     widget_p.y - widget_size.height};
                break;
        case ei_anc_west:
                widget_p =
                        (ei_point_t){widget_p.x,
                                     widget_p.y - (widget_size.height >> 1)};
                break;
        case ei_anc_none:
                break;
        case ei_anc_northwest:
                break;
        }
        /* MAJ du widget */
        /* On appelle jamais placer pour root_widget */
        if (widget->parent == NULL) {
                printf("appel à ei_place pour root_widget\n");
        }
        else {
                widget->screen_location.top_left = widget_p;
                widget->screen_location.size = widget_size;
                widget->content_rect->top_left = widget_p;
                widget->content_rect->size = widget_size;
        }
        ei_rect_t nouveau_rect = (ei_rect_t){(ei_point_t){widget->screen_location.top_left.x,
                                                          widget->screen_location.top_left.y},
                                             (ei_size_t) {widget->screen_location.size.width,
                                                          widget->screen_location.size.height}};
        placer_children(widget);
}

void ei_placer_run(struct ei_widget_t *widget)
{
        ei_place(widget, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

void ei_placer_forget(struct ei_widget_t *widget)
{
        free(widget->placer_params);
        widget->placer_params = NULL;
}

void placer_children(ei_widget_t* widget)
{
        /* Parcours des descendants de widget
         * et appel a ei_placer_run */
        ei_widget_t* current = malloc(sizeof(ei_widget_t));
        if (widget->children_head != NULL) {
                current = widget->children_head;
                while (current != NULL) {
                        ei_placer_run(current);
                        current = current->next_sibling;
                }
        }
        free(current);
}
