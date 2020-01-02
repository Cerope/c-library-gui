#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../include/hw_interface.h"
#include "../include/ei_utils.h"
#include "../include/ei_draw.h"
#include "../include/ei_types.h"

void test_mouette(ei_surface_t surface, ei_rect_t* clipper)
{
	ei_color_t		color		= { 255, 255, 255, 255 };
	ei_linked_point_t	pts[5];
	pts[0].point.x = 100;
	pts[0].point.y = 100;
	pts[0].next = &(pts[1]);
	pts[1].point.x = 200;
	pts[1].point.y = 200;
	pts[1].next = &(pts[2]);
	pts[2].point.x = 200;
	pts[2].point.y = 100;
	pts[2].next = &(pts[3]);
	pts[3].point.x = 100;
	pts[3].point.y = 200;
	pts[3].next = &(pts[4]);
	pts[4].point.x = 100;
	pts[4].point.y = 100;
	pts[4].next = NULL;

	ei_draw_polygon(surface, pts, color, clipper);
}

/*
 * ei_main --
 *
 *	Main function of the application.
 */
int ei_main(int argc, char** argv)
{
	ei_size_t		win_size	= ei_size(800, 600);
	ei_surface_t		main_window	= NULL;

	ei_point_t 		top_left = {0, 0};
	ei_size_t			rect_size = ei_size(800, 600);
	ei_rect_t			clipper;
	clipper.top_left = top_left;
	clipper.size = rect_size;
	ei_rect_t*		clipper_ptr = &clipper;

	hw_init();

	main_window = hw_create_window(&win_size, EI_FALSE);

	/* Lock the drawing surface, paint it white. */
	hw_surface_lock	(main_window);

	/* Draw polygones. */
	//test_polygon_losange(main_window, clipper_ptr);
	//test_polygon_square(main_window, clipper_ptr);
	test_mouette(main_window, clipper_ptr );
	//test_square	(main_window, clipper_ptr);
	//test_octogone	(main_window, clipper_ptr);

	/* Unlock and update the surface. */
	hw_surface_unlock(main_window);
	hw_surface_update_rects(main_window, NULL);

	/* Wait for a character on command line. */
	getchar();

	hw_quit();
	return (EXIT_SUCCESS);
}
