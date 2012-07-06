#pragma once

#include <cv.hpp>
#include <cxcore.hpp>


class slam_feature_map
{
public:
	slam_feature_map();
	~slam_feature_map();

	/* map */
	cv::Mat descriptors; // descriptors
	cv::Mat descriptors_grid; // grid with index to descriptors matrix row (index u16, response float 32, time int 32) = 80
	cv::Mat keypoints_wc; // local world coordinates
	cv::Mat keypoints_t; // time
	unsigned short descriptors_count;


	/* buffer */
	/*
	cv::Mat buffer_descriptors;
	cv::Mat buffer_descriptors_grid;
	cv::Mat buffer_keypoints_wc;
	cv::Mat buffer_info;
	unsigned short buffer_count;
	clock_t buffer_last_flush;
	//std::vector<cv::KeyPoint> buffer_keypoints;
	*/

	void update(std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors, std::vector<cv::Point3f>& wc);
	//void flush_buffer();
	void get_local_descriptors(cv::Mat& map_descriptors, cv::Mat& map_keypoints, cv::Mat& map_keypoints_t, cv::Point3f& wc, float radius = 0.0f);
	bool inside(cv::Mat& m, cv::Rect& r);
	bool cell_inside_descriptors_grid(unsigned short x, unsigned short y);

private:
	cv::Vec3i cell_value;
	unsigned short *g_idx;
	float *g_response;
	clock_t* g_time;
	clock_t time, time_threshold;

	//std::map<int, unsigned short> cell_best_keypoints;
	//std::map<int, unsigned short>::iterator it;

	void worldpos_to_dgridpos(std::vector<cv::Point3f>& src, unsigned short *x, unsigned short *y);
	void worldpos_to_dgridpos(cv::Point3f& src, unsigned short *x, unsigned short *y);
};