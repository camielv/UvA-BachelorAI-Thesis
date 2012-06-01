#pragma once

#include <cv.hpp>
#include "slam_feature_map.h"


class slam_visual_map
{
public:
	slam_visual_map();
	~slam_visual_map();
	byte* get_array();
	void update(cv::Mat& frame, std::vector<cv::Point2f>& lc, std::vector<cv::Point3f>& wc);

	void frame_to_canvas(cv::Mat& frame, cv::Mat& frameT, std::vector<cv::Point2f>& lc, std::vector<cv::Point3f>& wc);
	bool is_updated(int* dst, bool reset_roi = false);
	bool corner_at_wc(cv::Point2f wc);
	void save_canvas();


	cv::Mat canvas;
	slam_feature_map feature_map;


	float resolution;
	float resolution_inv;

	unsigned int w;
	unsigned int h;

	unsigned int origin_x;
	unsigned int origin_y;

	bool map_updated;
	int frame_roi[4];
	int sync_roi[4];

private:
	cv::Size canvasSize;
	cv::Mat undoTranslate;

	cv::TermCriteria termcrit;

	std::map<int, unsigned short> cell_best_keypoints;
	std::map<int, unsigned short>::iterator it;

	void worldpos_to_canvaspos(cv::Point3f& src, cv::Point2f& dst);
	void canvaspos_to_worldpos(cv::Point2f& src, cv::Point2f& dst);
	void update_roi(cv::Point2f& p, int *roi);
	void update_roi(int *src, int *dst);
};