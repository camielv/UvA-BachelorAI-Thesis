#include "slam_map.h"

using namespace std;
using namespace cv;


slam_map::slam_map()
{
}


slam_map::~slam_map()
{
}

/* visual map */


/* feature map */
void slam_map::update(vector<KeyPoint>& keypoints, Mat& descriptors, vector<Point3f>& wc)
{
	// also perform when no update being performed (no new features found in frames)
	//feature_map.flush_buffer();

	feature_map.update(keypoints, descriptors, wc);
}


void slam_map::get_local_descriptors(Mat& map_descriptors, Mat& map_keypoints, Mat& map_keypoints_t, Point3f& wc, float radius)
{
	feature_map.get_local_descriptors(map_descriptors, map_keypoints, map_keypoints_t, wc, radius);
}


/* elevation map */
