#ifndef EI_STRUCTURES_H
#define EI_STRUCTURES_H

#include <stdint.h>
#include <stdlib.h>
#include "../../include/ei_types.h"
#include "../../include/hw_interface.h"

typedef struct _ei_cellule ei_cellule;
typedef struct _ei_liste_repere ei_liste_repere;

/* Structure d'une cellule */
struct _ei_cellule{
  int y_max;
  float x_ymin;
  float reciproque_pente;
  ei_cellule* next;
} ;

/* Liste contenant les cellules, avec un attribut taille */
struct _ei_liste_repere{
    ei_cellule *debut;
    int taille;
};

/*
Permet d'insérer une cellule dans une liste triée, utilisée pour insérer
les valeurs de TC[y] dans TCA, qui est trié à la fin de l'itération précédente
@param cas : Permet de préciser quel point entre point_a et point_b y le moins elevée
            On définit la cellule à insérer en fonction du cas
@param point_a : le premier point
@param point_b : le deuxieme point
@param liste_courante : la liste sur laquelle on travaille, TCA en pratique

*/
void insertion_trie_cellules(uint8_t cas, ei_point_t point_a, ei_point_t point_b,
                             ei_liste_repere *liste_courante);


 /*
 Effectue un tri sur une liste suivant les x_ymin croissant, utilisé pour trier TCA
 @param liste : la liste à trier
 */
void trie_liste(ei_liste_repere *liste);

/*
Permet de vérifier si une coordonnée est bien dans un clipper
@param x : coordonnée x
@param y : coordonnée y
@param clipper : rectangle, on vérifie que le point (x,y) est bien dedans
*/
int inClipper(int x, int y, const ei_rect_t* clipper);

/*
Dessine une ligne verticale
@param surface : surface sur laquelle dessiner
@param size : taille de la surface
@param color : couleur de la ligne
@param p1 : premiere extrémité
@param p2 : deuxieme extrémité
@param delta_y : différence des y des 2 points
@param clipper : clipper pour dessiner
*/
void vertical_line(ei_surface_t surface,
                   ei_size_t size,
                   const ei_color_t color,
                   ei_point_t p1,
                   ei_point_t p2,
                   int delta_y,
                   const ei_rect_t* clipper);

 /*
 Dessine une ligne horizontale
 @param surface : surface sur laquelle dessiner
 @param size : taille de la surface
 @param color : couleur de la ligne
 @param p1 : premiere extrémité
 @param p2 : deuxieme extrémité
 @param delta_x : différence des x des 2 points
 @param clipper : clipper pour dessiner
 */
void horizontal_line(ei_surface_t surface,
                     ei_size_t size,
                     const ei_color_t color,
                     ei_point_t p1,
                     ei_point_t p2,
                     int delta_x,
                     const ei_rect_t* clipper);
#endif
