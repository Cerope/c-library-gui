#ifndef FRAME_H
#define FRAME_H
#include "../include/ei_types.h"
#include "../include/hw_interface.h"
#include "../include/ei_widget.h"

extern int ID;
extern int entete_h;
extern int toplevel_act;
/* WIDGET FRAME */
typedef struct {
    struct ei_widget_t widget;
    ei_color_t color;
    int border_width;
    ei_relief_t relief;
    char* text;
    ei_font_t text_font;
    ei_color_t text_color;
    ei_anchor_t text_anchor;
    ei_surface_t img;
    ei_rect_t* img_rect;
    ei_anchor_t img_anchor;
} ei_frame_widget_t;

void* frame_allocfunc (void);

void frame_releasefunc (struct ei_widget_t* widget);

void frame_drawfunc (struct ei_widget_t* widget,
                     ei_surface_t surface,
                     ei_surface_t pick_surface,
                     ei_rect_t * clipper);

void frame_setdefaultsfunc (struct ei_widget_t* widget);

void frame_geomnotifyfunc (struct ei_widget_t* widget,
                          ei_rect_t rect);

ei_bool_t frame_handlefunc (struct ei_widget_t* widget,
                       struct ei_event_t * event);

/* WIDGET BUTTON */
typedef struct {
    struct ei_widget_t widget;
    ei_color_t color;
    int border_width;
    int corner_radius;
    ei_relief_t relief;
    char* text;
    ei_font_t text_font;
    ei_color_t text_color;
    ei_anchor_t text_anchor;
    ei_surface_t img;
    ei_rect_t* img_rect;
    ei_anchor_t img_anchor;
    ei_callback_t callback;
    void* user_param;
} ei_button_widget_t;

void* button_allocfunc (void);

void button_releasefunc (struct ei_widget_t* widget);

void button_drawfunc (struct ei_widget_t* widget,
                     ei_surface_t surface,
                     ei_surface_t pick_surface,
                     ei_rect_t * clipper);

void button_setdefaultsfunc (struct ei_widget_t* widget);

void button_geomnotifyfunc (struct ei_widget_t* widget,
                          ei_rect_t rect);

ei_bool_t button_handlefunc (struct ei_widget_t* widget,
                       struct ei_event_t * event);

/* WIDGET TOPLEVEL */
typedef struct {
    struct ei_widget_t widget;
    ei_size_t requested_size;
    ei_color_t color;
    int border_width;
    char* title;
    ei_bool_t closable;
    ei_axis_set_t resizable;
    ei_size_t* min_size;
} ei_toplevel_widget_t;

void* toplevel_allocfunc (void);

void toplevel_releasefunc (struct ei_widget_t* widget);

void toplevel_drawfunc (struct ei_widget_t* widget,
                     ei_surface_t surface,
                     ei_surface_t pick_surface,
                     ei_rect_t * clipper);

void toplevel_setdefaultsfunc (struct ei_widget_t* widget);

void toplevel_geomnotifyfunc (struct ei_widget_t* widget,
                          ei_rect_t rect);

ei_bool_t toplevel_handlefunc (struct ei_widget_t* widget,
                       struct ei_event_t * event);

/* Renvoie la surface de picking */
ei_surface_t ei_app_pick_surface();

/* Associe le pick_id  correspondant a la couleur */
uint32_t pick_id_from_color (ei_surface_t surface, const uint32_t color);

ei_rect_t * intersection(const ei_rect_t * rect1, const ei_rect_t * rect2);
#endif
