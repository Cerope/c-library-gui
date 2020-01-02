#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/ei_application.h"
#include "../include/hw_interface.h"
#include "../include/ei_widgetclass.h"
#include "../include/ei_widget.h"
#include "../include/ei_types.h"
#include "../include/ei_placer.h"
#include "../include/ei_event.h"
#include "../include/ei_draw.h"

#include "src_include/widget.h"

/* Pour utiliser le root_widget, le déclarer en prototype et importer ../include/widget.h */
static ei_widget_t* active_widget;
ei_default_handle_func_t default_func = NULL;

/* Permet de définir la fonction par defaut */
void ei_event_set_default_handle_func(ei_default_handle_func_t func){
        default_func = func;
}

ei_default_handle_func_t ei_event_get_default_handle_func(){
        return default_func;
}

void ei_event_set_active_widget(ei_widget_t *widget)
{
        active_widget = widget;
}

/* Met le widget en parametre au premier plan */
void set_widget_infront(ei_widget_t* widget)
{
        ei_widget_t* current = malloc(sizeof(ei_widget_t));
        ei_widget_t* sibling = malloc(sizeof(ei_widget_t));
        current = widget;

        while (current->parent != NULL && current->parent->parent != NULL) {
                current = current->parent;

        }

        if (current->parent->children_tail != current) {
                sibling = current->parent->children_head;
                if (current == current->parent->children_head) {
                        current->parent->children_head = current->next_sibling;
                        current->parent->children_tail->next_sibling = current;
                        current->parent->children_tail = current;
                        current->next_sibling = NULL;
                }
                else {
                        while (sibling->next_sibling != current) {
                                sibling = sibling->next_sibling;
                        }
                        current->parent->children_tail->next_sibling = current;
                        current->parent->children_tail = current;
                        sibling->next_sibling = current->next_sibling;
                        current->next_sibling = NULL;
                }
        }
}


ei_widget_t* ei_event_get_active_widget()
{
        return active_widget;
}
