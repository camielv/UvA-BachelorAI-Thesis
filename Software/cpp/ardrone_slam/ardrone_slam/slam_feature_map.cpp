#include "global.h"
#include "slam_feature_map.h"
//#include <map>

// TMP
#include <opencv2/highgui/highgui.hpp>


using namespace std;
using namespace cv;


slam_feature_map::slam_feature_map(void):
	descriptors(1000, 64, CV_32F),
	keypoints_wc(1000, 1, CV_32FC3),
	descriptors_grid(200, 200, CV_32SC3),
	keypoints_t(1000, 1, CV_32S)
{
	descriptors_grid		= Scalar(0, 0, 0);
	descriptors_count		= 0;

	// once
	g_idx		= (unsigned short*) &cell_value[0];
	g_response	= (float*) &cell_value[1];
	g_time		= (clock_t*) &cell_value[2];
}


slam_feature_map::~slam_feature_map(void)
{

}

void slam_feature_map::update(vector<KeyPoint>& keypoints, Mat& descriptors, vector<Point3f>& wc)
{
	int i, idx;

	time = clock();
	time_threshold = clock() - 5 * CLOCKS_PER_SEC;

	int size = keypoints.size();
	unsigned short *x = NULL;
	unsigned short *y = NULL;

	// double the size of the matrix
	if (descriptors_count + size > this->descriptors.rows)
	{
		int s = this->descriptors.rows * 2;
		this->descriptors.resize(s);
		this->keypoints_wc.resize(s);
		this->keypoints_t.resize(s);
	}


	x = new unsigned short[size]; // faster than vector
	y = new unsigned short[size]; // faster than vector

	// convert all at once
	worldpos_to_dgridpos(wc, x, y);


	for (i = 0; i < descriptors.rows; i++)
	{
		if (!cell_inside_descriptors_grid(x[i], y[i]))
			continue;

		cell_value	= descriptors_grid.at<Vec3i>(y[i], x[i]);

		// descriptor found for cell: overwrite?
		if (*g_idx > 0)
		{
			if (keypoints[i].response <= *g_response || *g_time < time_threshold)
			{
				//printf("skipped: %i (time: %i, response: %f)\n", *g_idx, *g_time, *g_response);
				continue;
			}

			idx = *g_idx;
			//printf("overwrite: %i (%f vs %f)\n", idx, keypoints[i].response, *g_response);
		}

		// not in buffer -> add
		else
		{
			idx = descriptors_count++;
			//printf("add: %i\n", idx);
		}


		memcpy_s(
			this->descriptors.data + (idx * SLAM_DESCRIPTOR_SIZE),
			this->descriptors.rows * SLAM_DESCRIPTOR_SIZE,
			descriptors.data + (i * SLAM_DESCRIPTOR_SIZE),
			SLAM_DESCRIPTOR_SIZE
		);

		keypoints_wc.at<Vec3f>(idx)	= wc[i];
		keypoints_t.at<int>(idx)	= time;
		//printf("write time: %i\n", time);
		//printf("write info: %i = (%u, %u)\n", idx, x[i], y[i]);

		// write grid value <index, response, time>
		*g_idx			= (unsigned int) idx;
		*g_response		= keypoints[i].response;
		*g_time			= time;

		descriptors_grid.at<Vec3i>(y[i], x[i]) = cell_value;
	}

	//printf("updated map: %i features\n", descriptors_count);

	delete [] x;
	delete [] y;
}


/*
void slam_feature_map::update(vector<KeyPoint>& keypoints, Mat& descriptors, vector<Point3f>& wc)
{
	int i;
	unsigned short *p_idx;
	float *p_response;

	unsigned short idx;
	float response;
	double idx_and_response;
	clock_t time = clock();

	int size = keypoints.size();
	unsigned short *x = NULL;
	unsigned short *y = NULL;

	// double the size of the matrix
	if (descriptors_count + size > this->descriptors.rows)
	{
		int s = this->buffer_descriptors.rows * 2;
		this->buffer_descriptors.resize(s);
		this->buffer_keypoints_wc.resize(s);
		this->buffer_info.resize(s);
	}


	x = new unsigned short[size]; // faster than vector
	y = new unsigned short[size]; // faster than vector

	// convert all at once
	worldpos_to_dgridpos(wc, x, y);


	for (i = 0; i < descriptors.rows; i++)
	{
		if (!cell_inside_descriptors_grid(x[i], y[i]) || descriptors_grid.at<unsigned short>(y[i], x[i]) > 0)
			continue;


		// already in buffer, but better response
		idx_and_response = buffer_descriptors_grid.at<double>(y[i], x[i]);
		if (idx_and_response != 0.0)
		{
			p_idx = (unsigned short*) &idx_and_response;
			p_response = (float*) ((char*) &idx_and_response + 2);

			if (keypoints[i].response <= *p_response)
				continue;

			printf("    overwrite (%f vs %f)\n", keypoints[i].response, *p_response);
			idx = *p_idx;
		}

		// not in buffer -> add
		else
		{
			idx = buffer_count++;
		}


		memcpy_s(
			this->buffer_descriptors.data + (idx * SLAM_DESCRIPTOR_SIZE),
			this->buffer_descriptors.rows * SLAM_DESCRIPTOR_SIZE,
			descriptors.data + (i * SLAM_DESCRIPTOR_SIZE),
			SLAM_DESCRIPTOR_SIZE
		);

		buffer_keypoints_wc.at<Vec3f>(idx) = wc[i];
		buffer_info.at<Vec3i>(idx) = Vec3i(x[i], y[i], time);
		//printf("write info: %i = (%u, %u)\n", idx, x[i], y[i]);

		// write grid value <idx, response>
		response = keypoints[i].response;
		memcpy_s(&idx_and_response, 2, &idx, 2);
		memcpy_s((char*) &idx_and_response + 2, 4, &response, 4);

		buffer_descriptors_grid.at<double>(y[i], x[i]) = idx_and_response;
	}


	printf("buffer count: %i\n", buffer_count);

	delete [] x;
	delete [] y;
}
*/


/*
void slam_feature_map::flush_buffer()
{
	double dt = ((double)clock() - buffer_last_flush) / CLOCKS_PER_SEC;
	if (dt < 1.0)
		return;


	int i, flush_length;
	unsigned short x, y;
	int threshold = clock() - CLOCKS_PER_SEC * 5;
	flush_length = 0;

	if (buffer_count == 0)
		return;


	// find part of the buffer to flush
	for (i = 0; i < buffer_count; i++)
	{
		if (this->buffer_info.at<Vec3i>(i)[2] <= threshold)
		{
			// write to descriptor grid
			x = (unsigned short) buffer_info.at<Vec3i>(i)[0];
			y = (unsigned short) buffer_info.at<Vec3i>(i)[1];
			//printf("read: %i = (%u, %u)\n", i, x, y);
			descriptors_grid.at<unsigned short>(y, x) = (unsigned short) (descriptors_count + i - 1);

			flush_length++;
		}
		else
		{
			break;
		}
	}

	printf("performing flush: %i/%i\n", flush_length, buffer_count);
	if (flush_length == 0)
		return;


	// double the size of the dest matrix
	if (descriptors_count + flush_length > descriptors.rows)
	{
		descriptors.resize(descriptors.rows * 2);
		keypoints_wc.resize(keypoints_wc.rows * 2);
	}


	// copy data
	memcpy_s(
		descriptors.data + (descriptors_count * SLAM_DESCRIPTOR_SIZE),
		descriptors.rows * SLAM_DESCRIPTOR_SIZE,
		buffer_descriptors.data + SLAM_DESCRIPTOR_SIZE,
		flush_length * SLAM_DESCRIPTOR_SIZE
	);
	memcpy_s(
		keypoints_wc.data + (descriptors_count * SLAM_WC_SIZE),
		descriptors.rows * SLAM_WC_SIZE,
		buffer_keypoints_wc.data + SLAM_WC_SIZE,
		flush_length * SLAM_WC_SIZE
	);


	// shrink buffer
	Range rows = Range(flush_length, buffer_count);

	buffer_descriptors	= Mat(buffer_descriptors, rows);
	buffer_keypoints_wc	= Mat(buffer_keypoints_wc, rows);
	buffer_info			= Mat(buffer_info, rows);

	buffer_count -= flush_length;
	descriptors_count += flush_length;

	// update buffer grid indices
	unsigned short *p_idx;
	double idx_and_response;
	p_idx = (unsigned short*) &idx_and_response;

	for (i = 0; i < buffer_count; i++)
	{
		x = (unsigned short) buffer_info.at<Vec3i>(i)[0];
		y = (unsigned short) buffer_info.at<Vec3i>(i)[1];
		idx_and_response = buffer_descriptors_grid.at<double>(y, x);
		*p_idx = (unsigned short) i;
		buffer_descriptors_grid.at<double>(y, x) = idx_and_response;
	}
}
*/


void slam_feature_map::get_local_descriptors(Mat& map_descriptors, Mat& map_keypoints, Mat& map_keypoints_t, Point3f& wc, float radius)
{
	// return whole map
	if (radius <= 0.0f)
	{
		printf("Get descriptors of whole map: DEPRICATED\n");

		// return all keypoints + descriptors
		Range rows = Range(1, descriptors_count);

		map_descriptors = Mat(descriptors, rows);
		map_keypoints = Mat(keypoints_wc, rows);

		return;
	}

	int i = 0;
	unsigned short *indices;
	unsigned short x, y;

	Vec3i cell_value;
	unsigned short *g_idx;
	clock_t* g_time;

	clock_t time_threshold = clock() - 5 * CLOCKS_PER_SEC;

	// once
	g_idx		= (unsigned short*) &cell_value[0];
	g_time		= (clock_t*) &cell_value[2];


	worldpos_to_dgridpos(wc, &x, &y);

	int r = (int) floor(radius * 0.01f);


	int x2, y2, w, h;
	x2 = max(0, x - r);
	y2 = max(0, y - r);
	w = min(descriptors_grid.cols - x2, r * 2);
	h = min(descriptors_grid.rows - y2, r * 2);

	if (x2 >= descriptors_grid.cols || y2 >= descriptors_grid.rows)
		return;

	if (w <= 0 || h <= 0)
		return;

	indices = new unsigned short[w * h];

	Mat grid(descriptors_grid, Rect(x2, y2, w, h));

	for (x2 = 0; x2 < grid.cols; x2++)
	{
		for (y2 = 0; y2 < grid.rows; y2++)
		{
			cell_value	= grid.at<Vec3i>(y2, x2);

			if (*g_idx > 0 && *g_time <= time_threshold)
				indices[i++] = *g_idx;
		}
	}

	map_descriptors	= Mat(i, descriptors.cols, descriptors.type());
	map_keypoints	= Mat(i, keypoints_wc.cols, keypoints_wc.type());
	map_keypoints_t	= Mat(i, keypoints_t.cols, keypoints_t.type());

	int descriptors_rowsize	= SLAM_DESCRIPTOR_SIZE;
	int keypoint_rowsize	= 3 * sizeof(float);
	int keypoint_t_rowsize	= 1 * sizeof(int);

	for (int j = 0; j < i; j++)
	{
		memcpy_s(map_descriptors.data + j * descriptors_rowsize, descriptors_rowsize,
			descriptors.data + indices[j] * descriptors_rowsize, descriptors_rowsize);

		memcpy_s(map_keypoints.data + j * keypoint_rowsize, keypoint_rowsize,
			keypoints_wc.data + indices[j] * keypoint_rowsize, keypoint_rowsize);

		memcpy_s(map_keypoints_t.data + j * keypoint_t_rowsize, keypoint_t_rowsize,
			keypoints_t.data + indices[j] * keypoint_t_rowsize, keypoint_t_rowsize);
	}

	//printf("retured %i features\n", i);

	delete [] indices;
}


void slam_feature_map::worldpos_to_dgridpos(std::vector<cv::Point3f>& src, unsigned short *x, unsigned short *y)
{
	for (size_t i = 0; i < src.size(); i++)
	{
		x[i] = (unsigned short) floor(src[i].x * 0.01f + 0.5f) + 100;
		y[i] = (unsigned short) floor(src[i].y * 0.01f + 0.5f) + 100;
	}
}


void slam_feature_map::worldpos_to_dgridpos(cv::Point3f& src, unsigned short *x, unsigned short *y)
{
	*x = (unsigned short) floor(src.x * 0.01f + 0.5f) + 100;
	*y = (unsigned short) floor(src.y * 0.01f + 0.5f) + 100;
}


inline bool slam_feature_map::cell_inside_descriptors_grid(unsigned short x, unsigned short y)
{
	return (x >= 0 && y >= 0 && x < 200 && y < 200);
}


bool slam_feature_map::inside(Mat& m, Rect& r)
{
	Size size = m.size();

	if (r.x + r.width >= size.width || r.y + r.height >= size.height || r.x - r.width < 0 || r.y - r.height < 0)
		return false;

	return true;
}