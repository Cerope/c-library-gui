#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../include/hw_interface.h"
#include "../include/ei_types.h"
#include "../include/ei_draw.h"

#include "src_include/polygone.h"

/*
   Fonction pour l'affichage d'un polygone rempli d'une couleur donnée
   On le remplis par ligne horizontale (scanline)
   @param surface : la surface sur laquelle on dessine le polygone
   @first_point : une liste de points chainée représentant le polygone
   (Note : le premier point et le dernier doivent être confondus, le polygone
   doit être fermé)
   @param color : la couleur de remplissage du polygone
   @param clipper : la zone de clipper, on ne dessine que dans cette surface
 */
void  ei_draw_polygon(ei_surface_t surface,
                      const ei_linked_point_t* first_point,
                      const ei_color_t color,
                      const ei_rect_t* clipper)
{
        ei_size_t size = hw_surface_get_size(surface);
        /* On se limite a creer un tableau de taille la différence entre ymax et ymin */
        /* Initialisation de ces valeurs */
        int ymin=(first_point->point).y, ymax=(first_point->point).y;
        const ei_linked_point_t* premier_point = first_point;
        while(premier_point->next != NULL) {
                if((premier_point->point).y < ymin) { ymin = (premier_point->point).y; }
                if((premier_point->point).y > ymax) { ymax = (premier_point->point).y; }
                premier_point = premier_point->next;
        }
        int taille = ymax - ymin;
        /* On initialise TC */
        ei_liste_repere ei_tc[taille+1];
        for(int i=0; i <= taille; i++) {
                ei_tc[i] = (ei_liste_repere){NULL, 0};
        }
        /*On fait attention a bien prendre ymin pour l'index, ici on le met à 0
           alors que il faut prendre TC[y] normalement, le TC[y] dans l'enonce
           correspond à TC[y-ymin] */
        while(first_point->next != NULL) {
                ei_point_t point_a = first_point->point;
                ei_point_t point_b = first_point->next->point;
                /* Chaque segment n'est présent qu'une seule fois dans TC,
                   à l'index correspondant au ymin de ses 2 extrémités */
                if (point_a.y < point_b.y) {
                        insertion_trie_cellules(0, point_a, point_b,
                                                &ei_tc[point_a.y-ymin]);
                }
                else if(point_b.y < point_a.y) {
                        insertion_trie_cellules(1, point_a, point_b,
                                                &ei_tc[point_b.y-ymin]);
                }
                first_point = first_point->next;
        }

        /* Debut de l'algorithme*/
        /* On initialise TCA le tableau des cotés actifs */
        ei_liste_repere ei_tca = {NULL, 0};
        uint32_t index_y = 0; /* C'est OK car on est assuré d'avoir une intersection */
        do {
                /* Deplacer les cotes de TC(y) dans TCA */
                /* On pourrai mettre un attribut queue dans ei_liste_repere pour faciliter */

                if (ei_tca.debut==NULL) { /* Pour la premiere iteration, tca est vide */
                        ei_tca = ei_tc[index_y];
                }
                /* tca non vide: on ajoute tc[y] en fin de tca */
                else{
                        ei_cellule *cell_parcours = ei_tca.debut;
                        while(cell_parcours->next != NULL) {
                                cell_parcours = cell_parcours->next;
                        }
                        cell_parcours->next = ei_tc[index_y].debut;
                        ei_tca.taille += ei_tc[index_y].taille;
                }

                /* Supprimer de TCA les cotes tels que ymax = y */
                ei_cellule *cell_cour = ei_tca.debut;
                /* Cas ou il faut redefinir la tete */
                /* La variable boucle permet de continuer à redefinir la tete tant que
                   celle-ci remplit la condition ymax = y */
                int boucle = 1;
                while(boucle) {
                        if(cell_cour == NULL) {
                                break;
                        }
                        if (cell_cour->y_max == index_y + ymin) {
                                ei_tca.debut = cell_cour->next;
                                ei_tca.taille -= 1;
                                cell_cour = cell_cour->next;
                        }
                        else{
                                boucle = 0;
                        }
                }
                /* Maintenant on est assuré que la tete n'est pas a redefinir , on parcourt
                   les autres */
                if(cell_cour != NULL) {
                        while(cell_cour != NULL && cell_cour->next != NULL ) {
                                if(cell_cour->next->y_max == index_y + ymin) {
                                        cell_cour->next = cell_cour->next->next;
                                        ei_tca.taille -=1;
                                        cell_cour = cell_cour->next;
                                }
                                else{
                                        cell_cour = cell_cour->next;
                                }
                        }
                }
                /* Trier TCA par abscisse croissant des intersecitons de cote avec la scanline */
                trie_liste(&ei_tca);
                /* Modifier les pixels de l'image sur la scanline, dans les intervals intérieurs
                   au polygone */
                /* On arrondit x dans TCA selon les regles enoncées et on affiche*/
                ei_cellule *cell_display = ei_tca.debut;
                uint8_t parite = 0; // 0 si pair, 1 si impair
                while(cell_display != NULL) {
                        /* On utilise parité pour remplir les intervalles intérieurs au polygone */
                        parite = (parite + 1)%2;

                        if(cell_display->next != NULL && parite) {
                                ei_point_t p1 = {ceilf(cell_display->x_ymin),
                                                 index_y + ymin};
                                /* On fait une ligne horizontale entre les intersections deux à deux,
                                   uniquement intérieur grace à parité */
                                horizontal_line(surface,
                                                size,
                                                color,
                                                (ei_point_t){ceilf(cell_display->x_ymin),
                                                             index_y + ymin},
                                                (ei_point_t){floorf(cell_display->next->x_ymin),
                                                             index_y + ymin},
                                                floorf(cell_display->next->x_ymin) -
                                                ceilf(cell_display->x_ymin),
                                                clipper);
                        }


                        cell_display = cell_display->next;
                }
                /*Incrémenter y*/
                index_y++;
                /*Mettre à jour les abscisses d'intersection des côtés de TCA avec la nouvelle scanline */
                ei_cellule *cell_maj = ei_tca.debut;
                /* On ajoute la reciproque de la pente aux x_ymin des cotes dans TCA */
                while(cell_maj != NULL) {
                        cell_maj->x_ymin += cell_maj->reciproque_pente;
                        cell_maj = cell_maj->next;
                }

        } while (index_y <= taille && ei_tca.debut != NULL);
        /* La condition index_y <= taille est non nécessaire mais permet de s'assurer
           que la boucle terminera en cas de problème */


}

/*
   Effectue un tri sur une liste suivant les x_ymin croissant, utilisé pour trier TCA
   @param liste : la liste à trier
 */
void trie_liste(ei_liste_repere *liste){
        /* ici on va ranger les elements dans l'ordre croisssant avant de les afficher. */
        ei_cellule *temp, *temp1, *temp2;
        float min;
        float reciproque;
        int y_max;
        for(temp=liste->debut; temp!=NULL; temp=temp->next)
        {
                temp2=temp;
                min=temp->x_ymin;
                reciproque = temp->reciproque_pente;
                y_max = temp->y_max;
                for(temp1=temp->next; temp1!=NULL; temp1=temp1->next)
                {
                        if(min > temp1->x_ymin)
                        {
                                /* le temp2 temporaire est l'adresse de l'élement où se trouve le minimum */
                                temp2=temp1;
                                min=temp2->x_ymin;
                                reciproque = temp2->reciproque_pente;
                                y_max = temp2->y_max;

                        }
                }
                /* On echange les 2 elements... */
                temp2->x_ymin=temp->x_ymin;
                temp2->reciproque_pente=temp->reciproque_pente;
                temp2->y_max = temp->y_max;
                temp->x_ymin=min;
                temp->reciproque_pente=reciproque;
                temp->y_max = y_max;
        }
}

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
                             ei_liste_repere *liste_courante ){
        ei_cellule *newCellule = malloc(sizeof(ei_cellule));
        ei_cellule *previous = NULL;
        /* Cas 0 : point_a.y < point_b.y */
        if(cas == 0) {
                newCellule->y_max = point_b.y;
                newCellule->x_ymin = point_a.x;
                newCellule->reciproque_pente = (float)(point_a.x - point_b.x)/
                                               (point_a.y - point_b.y);
                newCellule->next = NULL;
        }
        /* cas 1 : point_b.y < point_a.y */
        else{
                newCellule->y_max = point_a.y;
                newCellule->x_ymin = point_b.x;
                newCellule->reciproque_pente = (float)(point_b.x - point_a.x)/
                                               (point_b.y - point_a.y);
                newCellule->next = NULL;
        }
        /* CAS LISTE VIDE */
        if (liste_courante->debut == NULL) {
                liste_courante->debut = newCellule;
                liste_courante->taille = 1;
                return;
        }
        /* CAS EN PREMIER, REDEFINIR TETE DE LISTE */
        else if (newCellule->x_ymin < liste_courante->debut->x_ymin) {
                newCellule->next = liste_courante->debut;
                liste_courante->debut = newCellule;
                liste_courante->taille += 1;
                return;
        }
        /* CAS NORMAL */
        else{
                previous = liste_courante->debut;
                while(previous->next != NULL) {
                        if(newCellule->x_ymin < previous->next->x_ymin) {
                                newCellule->next = previous->next;
                                previous->next = newCellule;
                                liste_courante->taille += 1;
                                return;
                        }
                        previous = previous->next;
                }
                /* ON EST ARRIVE A LA FIN DE LA LISTE */
                previous->next = newCellule;
                liste_courante->taille +=1;
                return;

        }
}
