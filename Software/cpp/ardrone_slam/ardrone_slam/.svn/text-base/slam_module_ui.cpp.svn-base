#include "global.h"
#include "slam_module_ui.h"
#include "slam.h"
#include "slam_map.h"

#include "terrain3d.h"

#include <opencv2/highgui/highgui.hpp>

using namespace cv;

float slam_module_ui::waypoint[2];


slam_module_ui::slam_module_ui(slam *controller)
{
	this->controller = controller;
	this->map = &controller->map;

	initialized = false;

	memset(slam_module_ui::waypoint, 0, 2 * sizeof(float));
}


slam_module_ui::~slam_module_ui(void)
{
}


void slam_module_ui::update()
{
	if (!initialized)
		init();

	if (clock() - prev_update > 0.5f * CLOCKS_PER_SEC)
	{
		int roi[4];

		if (map->elevation_map.is_updated(roi, true))
			terrain->update_elevation_map(roi);

		if (map->visual_map.is_updated(roi, true))
			terrain->update_texture(roi);

		controller->get_world_position(pos);
		map->elevation_map.worldpos_to_cell(pos);

		prev_update = clock();

		terrain->render();

		return;
	}

	if (terrain->requires_render())
		terrain->render();
}


void slam_module_ui::display_canvas()
{
	Mat subCanvas(map->visual_map.canvas, Rect(700, 1200, 2300, 2800));
	Mat resized(800, 800, CV_8UC4);
	resize(subCanvas, resized, Size(800, 800));

	cvNamedWindow("Image:", CV_WINDOW_AUTOSIZE);
	imshow("Image:", resized);
	cvWaitKey(4);

	Sleep(99999);
}


void slam_module_ui::init()
{
	initialized = true;

	prev_update = clock();


	/* 3D Terrain */
	terrain = new terrain3d(
		map->elevation_map.get_array(),
		map->elevation_map.w,
		map->elevation_map.h,
		map->visual_map.get_array(),
		pos,
		waypoint
	);


	HWND consoleWindow = GetConsoleWindow();
	//SetFocus(consoleWindow);
	BringWindowToTop(consoleWindow);

	/* OpenCV window */
	//cvNamedWindow("Image:", CV_WINDOW_AUTOSIZE);
}