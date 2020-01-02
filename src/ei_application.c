#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/ei_application.h"
#include "../include/ei_draw.h"
#include "../include/ei_event.h"
#include "../include/ei_placer.h"
#include "../include/ei_types.h"
#include "../include/ei_widget.h"
#include "../include/ei_widgetclass.h"
#include "../include/hw_interface.h"

#include "src_include/widget.h"

static ei_surface_t root_surface;
static ei_surface_t pick_surface;
static ei_widget_t *root_widget;
static ei_linked_rect_t *invalidate_rects = NULL;
ei_bool_t QUIT = EI_FALSE;

int ID;
/* Hauteur de l'entete pour un toplevel */
int entete_h = 30;
ei_default_handle_func_t default_func;

ei_widget_t *ei_widget_from_pick_id(uint32_t pick_id, ei_widget_t *root);
void draw(ei_widget_t *widget, ei_rect_t* clipper);

void ei_app_create(ei_size_t *main_window_size, ei_bool_t fullscreen) {
        hw_init();
        default_func = ei_event_get_default_handle_func();
        root_surface = hw_create_window(main_window_size, fullscreen);
        pick_surface = hw_surface_create(root_surface, main_window_size, EI_TRUE);
        ei_frame_register_class();
        ei_button_register_class();
        ei_toplevel_register_class();
        root_widget = ei_widget_create("frame", NULL);
}

void ei_app_free() {
        ei_widget_destroy(root_widget);
}

void ei_app_run() {
        ei_event_t event;
        ei_widget_t *current;
        ei_bool_t boo;

        /* affichage initial */
        hw_surface_lock(ei_app_root_surface());
        hw_surface_lock(ei_app_pick_surface());
        draw(ei_app_root_widget(), NULL);
        hw_surface_unlock(ei_app_root_surface());
        hw_surface_unlock(ei_app_pick_surface());
        hw_surface_update_rects(ei_app_root_surface(), NULL);

        while (!QUIT) {
                hw_surface_lock(ei_app_root_surface());
                hw_surface_lock(ei_app_pick_surface());
                ei_linked_rect_t* cur_rect = invalidate_rects;
                ei_linked_rect_t* tmp = malloc(sizeof(ei_linked_rect_t));
                invalidate_rects = NULL;
                draw(ei_app_root_widget(), NULL);
                hw_surface_unlock(ei_app_root_surface());
                hw_surface_unlock(ei_app_pick_surface());

                hw_surface_update_rects(ei_app_root_surface(), NULL);
                hw_event_wait_next(
                        &event); // Fills the structure event
                                 // with info about the new event
                if (ei_event_get_active_widget() != NULL) {

                        boo = ei_event_get_active_widget()->wclass->handlefunc(
                                ei_event_get_active_widget(), &event);
                } else {
                        if(default_func != NULL) {
                                boo = default_func(&event);
                        }
                }

                ei_point_t mouse, mouse_end;
                ei_widget_t *current;
                if (event.type > 3 /* = mouse_events */ && !boo) {
                        mouse = event.param.mouse.where;
                        current = ei_widget_pick(&mouse);
                        if (current != NULL) {
                                current->wclass->handlefunc(current, &event);
                        }
                }
                if (!boo) {
                        ei_default_handle_func_t handle_func = ei_event_get_default_handle_func();
                        if (handle_func != NULL) {
                                handle_func(&event);
                        }
                }
        }
}

void ei_app_invalidate_rect(ei_rect_t *rect) {
        /* On n'utilise pas invalidate rect dans notre code
         */
        struct ei_linked_rect_t *liste = malloc(sizeof(ei_linked_rect_t));
        liste->rect = *rect;
        liste->next = invalidate_rects;
        invalidate_rects = liste;
}

void ei_app_quit_request() {
        QUIT = EI_TRUE;
}

ei_widget_t *ei_app_root_widget() {
        return root_widget;
}

ei_surface_t ei_app_root_surface() {
        return root_surface;
}

ei_surface_t ei_app_pick_surface() {
        return pick_surface;
}

void draw(ei_widget_t *widget, ei_rect_t* clipper) {
        // Dessine le widget passé en parametre puis
        // appelle la methode sur ses fils
        // de children_head a children_tail
        if (widget->parent != NULL) {
                ei_size_t root_size = hw_surface_get_size(root_surface);
                ei_rect_t *clip = malloc(sizeof(ei_rect_t));
                clip->size = widget->parent->content_rect->size;
                clip->top_left = widget->parent->content_rect->top_left;
                // On s'assure de bien clipper
                if (clip->top_left.x < 0)
                        clip->top_left.x = 0;
                if (clip->top_left.y < 0)
                        clip->top_left.y = 0;
                if (clip->top_left.x + clip->size.width > root_size.width)
                        clip->size.width = -clip->top_left.x + root_size.width;
                if (clip->top_left.y + clip->size.height > root_size.height)
                        clip->size.height = -clip->top_left.y + root_size.height;
                clip = (clipper != NULL) ? intersection(clip, clipper) : clip;

                if (widget->placer_params != NULL) {
                        widget->wclass->drawfunc(widget, ei_app_root_surface(),
                                                 ei_app_pick_surface(), clip);
                }
        } else {
                widget->wclass->drawfunc(widget, ei_app_root_surface(),
                                         ei_app_pick_surface(), clipper);
        }

        ei_widget_t *child = widget->children_head;
        while (child != NULL) {
                draw(child, clipper);
                child = child->next_sibling;
        }
}
