#pragma once

#include <cv.hpp>


class slam_elevation_map
{
public:
	slam_elevation_map();
	~slam_elevation_map();
	void update(float w_x, float w_y, float w_h, unsigned char conf);
	void update(float w_x, float w_y, float w_h, unsigned char conf, float r_mm);
	short* get_array();
	bool is_updated(int* dst, bool reset_roi = false);
	void worldpos_to_cell(float *pos);
	void save_map();

	float resolution;

	unsigned int w;
	unsigned int h;

	unsigned int origin_x;
	unsigned int origin_y;

	bool map_updated;
	int roi[4];

private:
	cv::Mat map;
	cv::Mat map_p;

	void update_cell(int x, int y, short h, unsigned char conf);
	void worldpos_to_cell(float x, float y, int *cellxy);
	void update_roi(int x, int y);
};