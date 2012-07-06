#include "global.h"
#include "slam_elevation_map.h"
#include <math.h>


slam_elevation_map::slam_elevation_map(void):
	map(2 * SLAM_ELEVATION_MAP_DEFAULT_SIZE, 2 * SLAM_ELEVATION_MAP_DEFAULT_SIZE, CV_16S),
	map_p(2 * SLAM_ELEVATION_MAP_DEFAULT_SIZE, 2 * SLAM_ELEVATION_MAP_DEFAULT_SIZE, CV_8U)
{
	map = 0;
	map_p = 0;

	map_updated = false;
	memset(roi, -1, 4 * sizeof(int));

	resolution = 0.02f; // 1 cell is 50x50mm

	w = 2 * SLAM_ELEVATION_MAP_DEFAULT_SIZE;
	h = 2 * SLAM_ELEVATION_MAP_DEFAULT_SIZE;

	origin_x = SLAM_ELEVATION_MAP_DEFAULT_SIZE;
	origin_y = SLAM_ELEVATION_MAP_DEFAULT_SIZE;
}

slam_elevation_map::~slam_elevation_map(void)
{

}

/* accepts world coordinates */
void slam_elevation_map::update(float w_x, float w_y, float w_h, unsigned char conf)
{
	int p[2];
	short h = (short) -w_h;
	
	worldpos_to_cell(w_x, w_y, p);
	update_cell(p[0], p[1], h, conf);
}

void slam_elevation_map::update(float w_x, float w_y, float w_h, unsigned char conf, float r_mm)
{
	int p[2];
	int x, y;

	short h = (short) -w_h;

	worldpos_to_cell(w_x, w_y, p);
	x = p[0];
	y = p[1];

	// convert mm to cell radius
	int r = (int) floor(r_mm * resolution);
	int r2 = r * r;

	for (int y2 = -r; y2 <= r; y2++)
	{
		for (int x2 = -r; x2 <= r; x2++)
		{

			if (conf == 2 || (x2*x2*y2*y2 <= r2))
				update_cell(x+x2, y+y2, h, conf);

		}
	}
}

void slam_elevation_map::update_cell(int x, int y, short h, unsigned char conf)
{
	// tmp
	if (x < 0 || y < 0 
		||
		x >= 2 * SLAM_ELEVATION_MAP_DEFAULT_SIZE || y >= 2 * SLAM_ELEVATION_MAP_DEFAULT_SIZE)
		return;

	if (map.at<short>(x, y) == h || map_p.at<char>(x, y) > conf)
		return;

	map.at<short>(x, y) = h;
	map_p.at<char>(x, y) = conf;

	update_roi(x, y);
	map_updated = true;
}

short* slam_elevation_map::get_array()
{
	return (short*) map.data;
}

bool slam_elevation_map::is_updated(int* roi, bool reset_roi)
{
	if (!map_updated)
		return false;

	memcpy_s(roi, 4 * sizeof(int), this->roi, 4 * sizeof(int));

	if (reset_roi)
	{
		memset(this->roi, -1, 4 * sizeof(int));
		map_updated = false;
	}

	return true;
}

void slam_elevation_map::worldpos_to_cell(float x, float y, int *cellxy)
{
	// round: add 0.5 and floor
	cellxy[0] = (int) floor(x * resolution + 0.5f) + origin_x;
	cellxy[1] = (int) floor(y * resolution + 0.5f) + origin_y;
}

void slam_elevation_map::worldpos_to_cell(float *pos)
{
	pos[0] *= resolution;
	pos[1] *= resolution;
	pos[2] = -pos[2];
}

void slam_elevation_map::update_roi(int x, int y)
{
	if (roi[0] == -1 || x < roi[0])
		roi[0] = x;
	
	if (roi[1] == -1 || x > roi[1])
		roi[1] = x;

	if (roi[2] == -1 || y < roi[2])
		roi[2] = y;

	if (roi[3] == -1 || y > roi[3])
		roi[3] = y;
}

void slam_elevation_map::save_map()
{
	FILE *log;
	fopen_s (&log, "dataset/elevation_log.txt" , "w");

	for (unsigned int y = 0; y < w; y++)
	{
		for (unsigned int x = 0; x < h; x++)
		{
			fprintf(log, "%i", map.at<short>(x, y));
			if (x < 399)
				fprintf(log, ",");

		}

		fprintf(log, "\n");
	}

}