#pragma once

#include "slam_visual_map.h"
#include "slam_feature_map.h"
#include "slam_elevation_map.h"


class slam_map
{
public:
	slam_map();
	~slam_map();

	slam_visual_map visual_map;
	slam_elevation_map elevation_map;
	slam_feature_map feature_map;

	void update(std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors, std::vector<cv::Point3f>& wc);
	void get_local_descriptors(cv::Mat& map_descriptors, cv::Mat& map_keypoints, cv::Mat& map_keypoints_t, cv::Point3f& wc, float radius = 0.0f);

private:

};

