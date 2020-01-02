#include <stdio.h>
#include <stdlib.h>

#include "ei_application.h"
#include "ei_event.h"
#include "hw_interface.h"
#include "ei_widget.h"
#include "../src/src_include/widget.h"

//ei_surface_t root_surface;
//ei_widget_t* root_widget;
/*
 * ei_main --
 *
 *	Main function of the application.
 */
int ei_main(int argc, char** argv)
{
	ei_size_t	screen_size		= {900, 600};
	ei_color_t	root_bgcol		= {0x52, 0x7f, 0xb4, 0xff};


	/* Create the application and change the color of the background. */
	ei_app_create(&screen_size, EI_FALSE);
	ei_frame_configure(ei_app_root_widget(), NULL, &root_bgcol, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	/* Create, configure and place the frame on screen.*/
	ei_widget_t*	frame;
	ei_size_t	frame_size		= {400,200};
    int place_x = -10;
    int place_y = -10;
    float rx=0.5, ry=0.5;
    ei_anchor_t anch = ei_anc_none;
	ei_color_t	frame_color		= {0xf8, 0x88, 0x30, 0xff};
	ei_relief_t	frame_relief		= ei_relief_raised;

	int		frame_border_width	= 6;
	frame = ei_widget_create("frame", ei_app_root_widget());
	ei_frame_configure	(frame, &frame_size, &frame_color,
				 &frame_border_width, &frame_relief, NULL, NULL, NULL, NULL,
				 NULL, NULL, NULL);
	ei_place(frame, &anch, &place_x, &place_y, NULL, NULL, &rx, &ry, NULL, NULL );
    ei_surface_t img_surface = hw_image_load("misc/klimt.jpg", ei_app_root_surface());

    ei_frame_configure	(frame, NULL, NULL,
				 NULL, NULL, NULL, NULL, NULL, NULL,
				 &img_surface, NULL, NULL);

	/* Run the application's main loop. */
        ei_app_run();
        getchar();

	/* We just exited from the main loop. Terminate the application (cleanup). */
	ei_app_free();

	return (EXIT_SUCCESS);
}
