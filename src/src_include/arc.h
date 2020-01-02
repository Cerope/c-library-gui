#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../../include/ei_types.h"
#include "../../include/ei_draw.h"

/*
insère le paramètre point en queue de la liste de points pl
@param pl : un pointeur sur une liste de point
@param point : le point à ajouter en queue de liste
*/
void inserer_queue(ei_linked_point_t **pl, ei_point_t point);

/*
concatène les deux listes l1 & l2
@params l1, l2 : les deux listes à concatèner dans l'ordre suivant l1 -> l2
@return : la liste concaténée
*/
ei_linked_point_t* concat (ei_linked_point_t* l1, ei_linked_point_t* l2);

/*
crée une liste de points représentant un arc de cercle
@param origine : le centre de l'arc de cercle
@param rayon : le rayon entre l'arc et l'origine
@param angle_initial : l'angle de départ de l'arc
@param angle_final : l'angle final de l'arc
@param nombre_points : le nombre de points qui représente l'arc
i.e que plus ce paramètres est élevé plus l'arc sera beau
@return : la liste de points
 */
ei_linked_point_t* arc (const ei_point_t origine,
                        const int rayon,
                        const float angle_initial,
                        const float angle_final,
                        const int nombre_points);

/*
dessine le sous bouton "gauche" lors du dessin d'un bouton en relief
@param surface : la surface sur laquelle dessiner
@param bouton : un pointeur sur un rectangle qui représentera le rectangle
central du bouton
@param color : la couleur du sous bouton
@param rayon : le rayon définissant la largeur des arcs
@param nombre_points : le nombre de points par arc définissant
la beauté des arcs
@param clipper : un pointeur sur un rectangle en dehors duquel on
ne doit pas dessiner
*/
void sousBoutonL (ei_surface_t surface,
                  const ei_rect_t* bouton,
                  const ei_color_t color,
                  const int rayon,
                  const int nombre_points,
                  const ei_rect_t* clipper);

/*
dessine le sous bouton "gauche" lors du dessin d'un bouton en relief
@param surface : la surface sur laquelle dessiner
@param bouton : un pointeur sur un rectangle qui représentera le rectangle
central du bouton
@param color : la couleur du sous bouton
@param rayon : le rayon définissant la largeur des arcs
@param nombre_points : le nombre de points par arc définissant
la beauté des arcs
@param clipper : un pointeur sur un rectangle en dehors duquel on
ne doit pas dessiner
*/
void sousBoutonR (ei_surface_t surface,
                  const ei_rect_t* bouton,
                  const ei_color_t color,
                  const int rayon,
                  const int nombre_points,
                  const ei_rect_t* clipper);

/*
dessine un rectangle arrondi
@param surface : la surface sur laquelle dessiner
@param bouton : un pointeur sur un rectangle qui représentera le rectangle
central du bouton
@param color : la couleur du rectangle
@param rayon : le rayon définissant la largeur des arcs
@param nombre_points: le nombre de points par arc définissant la beauté
des arcs
@param clipper : un pointeur sur un rectangle en dehors duquel on ne doit
pas dessiner
*/
void rectangleArrondi (ei_surface_t surface,
                       const ei_rect_t* bouton,
                       const ei_color_t color,
                       const int rayon,
                       const int nombre_points,
                       const ei_rect_t* clipper);

/*
dessine un bouton à l'aide des fonctions précédentes
@param surface : la surface sur laquelle dessiner
@param bouton : un pointeur sur un rectangle qui représentera le rectangle
central du bouton
@param color : la couleur du bouton
@param rayon : le rayon definissant la largeur des arcs
@param epsilon : définit le décalage apparent entre la partie centrale
et la partie en relief
@param nombre_points : le nombre de points par arc
@param text, text_color, style, font_size : si text est non NULL alors
affiche text au centre du bouton avec la taille font_size en couleur
text_color et en style *style
@param pushed : modifie la colorisation pour donner l'impression d'enfoncé
*/
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
             const int pushed);

/*
affiche un cercle
@param surface : la surface sur laquelle dessiner
@param where : un pointeur sur l'origine
@param rayon : le rayon du cercle
@param color : la couleur du cercle
@param nombre_points : le nombre de points pour représenter le cercle
@param clipper : un pointeur sur un rectangle en dehors duquel il ne faut pas dessiner
*/
 void draw_circle(ei_surface_t surface,
                  ei_point_t* where,
                  int rayon,
                  const ei_color_t color,
                  int nombre_points,
                  ei_rect_t* clipper);
