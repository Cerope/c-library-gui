#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../include/hw_interface.h"
#include "../include/ei_utils.h"
#include "../include/ei_draw.h"
#include "../include/ei_types.h"
#include "../include/ei_widget.h"
#include "../include/ei_widgetclass.h"
#include "../include/ei_application.h"

#include "../src/src_include/widget.h"


/*
 *
 *
 *	Main function of the application.
 */
int ei_main(int argc, char** argv)
{
    /* WINDOW */
	ei_size_t		win_size	= ei_size(800, 600);
	ei_surface_t		main_window	= NULL;

    /* COLORS */

    /* Point where */
    /* Initialisation de la fenetre */
	hw_init();
	main_window = hw_create_window(&win_size, EI_FALSE);

	/* Lock the drawing surface, paint it white. */
	hw_surface_lock	(main_window);
    ei_app_create(main_window, EI_FALSE);
		/* Creation d'un nouveau frame */
		ei_widget_t * frame = ei_widget_create("frame", ei_app_root_widget());
		ei_size_t size = {200, 200};
		const ei_color_t blue = {0x00, 0x00, 0xff, 0xff};
		ei_frame_configure(frame, &size, &blue, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    ei_app_run();
    ei_app_free();
    /* */


	/* Unlock and update the surface. */
	hw_surface_unlock(main_window);
	hw_surface_update_rects(main_window, NULL);

	/* Wait for a character on command line. */
	getchar();

	hw_quit();
	return (EXIT_SUCCESS);
}
