#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../include/hw_interface.h"
#include "../include/ei_utils.h"
#include "../src/src_include/polygone.h"
#include "../include/ei_types.h"
#include "../src/src_include/arc.h"

int ei_main(int argc, char** argv)
{
    /* WINDOW */
	ei_size_t		win_size	= ei_size(800, 600);
	ei_surface_t		main_window	= NULL;

	/* test arc */
	ei_color_t white = {0xff,0xff,0xff,0xff};
	ei_color_t red = {0xff, 0, 0, 0xff};

	ei_point_t top_left = {50,50};
	ei_point_t top_left2 = {450,450};

	ei_size_t button_size = {300, 100};
	ei_rect_t button = {top_left, button_size};
	ei_rect_t button2 = {top_left2, button_size};

    /* Initialisation de la fenetre */
	hw_init();
	main_window = hw_create_window(&win_size, EI_FALSE);

	/* Lock the drawing surface, paint it white. */
	hw_surface_lock	(main_window);

	//sousBoutonR(main_window, &button, red, 30, 10, NULL);

	Bouton(main_window, &button, red, 20, 8, 8, NULL, "42", white, ei_style_normal, 30, 1);
	Bouton(main_window, &button2, red, 20, 8, 8, NULL, "福", white, ei_style_normal, 30, 0);

	hw_surface_unlock(main_window);
	hw_surface_update_rects(main_window, NULL);

	/* Wait for a character on command line. */
	getchar();

	hw_quit();
	return (EXIT_SUCCESS);
}
