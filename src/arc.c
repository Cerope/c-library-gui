#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../include/ei_types.h"
#include "../include/ei_draw.h"

#define PI 3.14159265359

/* optimisation possible du module :
   à plusieurs reprise je fais des insertions en queue
   il serait préférable de rendre cette insertion en queue
   en temps constant                */

void inserer_queue(ei_linked_point_t **pl, ei_point_t point)
{
        ei_point_t origine = {0, 0};
        /* on crée une sentinelle pour ne pas avoir à traiter le cas de la
           liste NULL */
        ei_linked_point_t sent = { origine, *pl };
        ei_linked_point_t *queue = &sent;
        /* on va en fin de liste */
        while (queue->next != NULL) {
                queue = queue->next;
        }
        /* on cree une nouvelle cellule et on l'ajoute */
        queue->next = malloc(sizeof(ei_linked_point_t));
        queue->next->point = point;
        queue->next->next = NULL;
        *pl = sent.next;
}


ei_linked_point_t* concat (ei_linked_point_t* l1, ei_linked_point_t* l2)
{
        /* si la première liste est null on renvoie l2 */
        if (l1 == NULL) { return l2; }
        ei_linked_point_t* cur = malloc(sizeof(ei_linked_point_t));
        cur = l1;
        /* on va en fin de liste et on relie */
        while (cur->next != NULL) {
                cur = cur->next;
        }
        /* cur correspond à la derniere cellule */
        cur->next = l2;
        return l1;
}

void free_list (ei_linked_point_t* l)
{
        ei_linked_point_t* prec;
        while (l != NULL) {
                prec = l;
                l = l->next;
                free(prec);

        }
}

ei_linked_point_t* arc (const ei_point_t origine,
                        const int rayon,
                        const float angle_initial,
                        const float angle_final,
                        const int nombre_points)
{
        ei_linked_point_t* points_arc = NULL; // liste vide
        /* on crée un pas */
        float pas = (angle_final - angle_initial) / (float) nombre_points;
        float angle_actuel = angle_initial;
        for (int i = 0; i <= nombre_points; i++) {
                /* pas par pas on crée un point et on l'ajoute à notre liste */
                ei_point_t point = {origine.x + rayon * cos(angle_actuel),
                                    origine.y + rayon * sin(angle_actuel)};
                inserer_queue(&points_arc, point);
                angle_actuel += pas;
        }
        return points_arc;
}

void sousBoutonL (ei_surface_t surface,
                  const ei_rect_t* bouton,
                  const ei_color_t color,
                  const int rayon,
                  const int nombre_points,
                  const ei_rect_t* clipper)
{
        ei_point_t top_left = bouton->top_left;
        ei_point_t top_right = {top_left.x + bouton->size.width, top_left.y};
        ei_point_t bottom_left = {top_left.x, top_left.y + bouton->size.height};
        /*
           tl pour top_left
           tr pour top_right
           etc...
         */
        ei_linked_point_t* arc_tl = arc(top_left, rayon, PI, 3*PI/2, nombre_points);
        ei_linked_point_t* arc_tr = arc(top_right, rayon, 3*PI/2, 7*PI/4, nombre_points/2);
        ei_linked_point_t* arc_bl = arc(bottom_left, rayon, 3*PI/4, PI, nombre_points/2);
        /* on relie nos arcs entre-eux avec concat */
        ei_linked_point_t* polygone = concat (arc_bl, arc_tl);
        polygone = concat (polygone, arc_tr);
        /* on crée et ajoute les points au milieu */
        ei_point_t middle_left = {top_left.x + bouton->size.width / 3,
                                  top_left.y + bouton->size.height / 2};
        ei_point_t middle_right = {top_left.x + 2 * bouton->size.width / 3,
                                   top_left.y + bouton->size.height / 2};
        inserer_queue(&polygone, middle_right);
        inserer_queue(&polygone, middle_left);
        /* on met le premier point à la fin car c'est une condition pour
           draw polygon */
        inserer_queue(&polygone, arc_bl->point);
        ei_draw_polygon (surface, polygone, color, clipper);
        /* on libère */
        free_list(polygone);
}

void sousBoutonR (ei_surface_t surface,
                  const ei_rect_t* bouton,
                  const ei_color_t color,
                  const int rayon,
                  const int nombre_points,
                  const ei_rect_t* clipper)
{
        /* mêmes commentaires que pour sousBoutonL
           l'idée générale est toujours de bien relier nos listes de points
           crées avec arc */
        ei_point_t top_left = bouton->top_left;
        ei_point_t top_right = {top_left.x + bouton->size.width, top_left.y};
        ei_point_t bottom_left = {top_left.x, top_left.y + bouton->size.height};
        ei_point_t bottom_right = {top_left.x + bouton->size.width, top_left.y +
                                   bouton->size.height};
        ei_linked_point_t* arc_tr = arc(top_right, rayon, 7*PI/4, 2*PI, nombre_points/2);
        ei_linked_point_t* arc_bl = arc(bottom_left, rayon, PI/2, 3*PI/4, nombre_points/2);
        ei_linked_point_t* arc_br = arc(bottom_right, rayon, 0, PI/2, nombre_points);
        ei_linked_point_t* polygone = concat (arc_tr, arc_br);
        polygone = concat (polygone, arc_bl);
        ei_point_t middle_left = {top_left.x + bouton->size.height / 2,
                                  top_left.y + bouton->size.height / 2};
        ei_point_t middle_right = {top_left.x - bouton->size.height /2 +
                                   bouton->size.width,
                                   top_left.y + bouton->size.height / 2};
        inserer_queue(&polygone, middle_left);
        inserer_queue(&polygone, middle_right);
        inserer_queue(&polygone, arc_tr->point);
        ei_draw_polygon (surface, polygone, color, clipper);
        free_list(polygone);
}

void rectangleArrondi (ei_surface_t surface,
                       const ei_rect_t* bouton,
                       const ei_color_t color,
                       const int rayon,
                       const int nombre_points,
                       const ei_rect_t* clipper)
{
        /* même idées qu'auparavant */
        ei_point_t top_left = bouton->top_left;
        ei_point_t top_right = {top_left.x + bouton->size.width, top_left.y};
        ei_point_t bottom_left = {top_left.x, top_left.y + bouton->size.height};
        ei_point_t bottom_right = {top_left.x + bouton->size.width, top_left.y +
                                   bouton->size.height};
        ei_linked_point_t* arc_tl = arc(top_left, rayon, PI, 3*PI/2, nombre_points);
        ei_linked_point_t* arc_tr = arc(top_right, rayon, 3*PI/2, 2*PI, nombre_points);
        ei_linked_point_t* arc_bl = arc(bottom_left, rayon, PI/2, PI, nombre_points);
        ei_linked_point_t* arc_br = arc(bottom_right, rayon, 0, PI/2, nombre_points);
        ei_linked_point_t* polygone = concat (arc_tl, arc_tr);
        polygone = concat(polygone, arc_br);
        polygone = concat(polygone, arc_bl);
        inserer_queue(&polygone, arc_tl->point);
        ei_draw_polygon (surface, polygone, color, clipper);
        free_list(polygone);
}

void Bouton (ei_surface_t surface,
             const ei_rect_t* bouton,
             const ei_color_t color,
             const int rayon,
             const int epsilon,
             const int nombre_points,
             const ei_rect_t* clipper,
             const char* text,
             const ei_color_t text_color,
             const ei_fontstyle_t style,
             const int font_size,
             const int pushed)
{
        ei_color_t light_color = {180, 180, 180, 0xff};
        ei_color_t dark_color = {70, 70, 70, 0xff};
        /* on appelle nos sousBoutons avec un argument couleur qui change en fonction du booléen pushed */
        sousBoutonL(surface, bouton, pushed ? dark_color : light_color,
                    rayon + epsilon, nombre_points, clipper);
        sousBoutonR(surface, bouton, pushed ? light_color : dark_color,
                    rayon + epsilon, nombre_points, clipper);
        // on ajoute epsilon aux rayons précédent pour que les sousboutons soient plus gros et soient visible
        rectangleArrondi(surface, bouton, color, rayon, nombre_points, clipper);
        ei_font_t font = ei_default_font;

        /* si le texte n'est pas NULL on l'affiche en le centrant */
        if (text != NULL) {
                int width, height;
                hw_text_compute_size(text, font, &width, &height);
                int delta_x = bouton->size.width - width;
                int x = delta_x < 0 ? 0 : delta_x/2;
                x += bouton->top_left.x;
                int delta_y = bouton->size.height - height;
                int y = delta_y < 0 ? 0 : delta_y/2;
                y += bouton->top_left.y;
                ei_point_t where = (ei_point_t){x, y};
                ei_draw_text(surface, &where, text, font, &text_color, clipper);
        }
}

void draw_circle(ei_surface_t surface,
                 ei_point_t* where,
                 int rayon,
                 const ei_color_t color,
                 int nombre_points,
                 ei_rect_t* clipper)
{
        /* on crée l'arc 0 PI */
        ei_linked_point_t* p_arc = arc(*where, rayon, 0, 2*PI, nombre_points);
        /* on ajoute le premier point en queue pour draw_polygon */
        inserer_queue(&p_arc, p_arc->point);
        ei_draw_polygon(surface, p_arc, color, clipper);
}
