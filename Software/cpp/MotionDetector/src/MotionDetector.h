#ifndef MOTIONDETECTOR_H_
#define MOTIONDETECTOR_H_
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

enum optical_flow{ FEATURES = 400, THRESHOLD = 5 };
enum algorithm{ OPTICAL_FLOW, BACKGROUND_SUBTRACTION };

class MotionDetector {
public:
	MotionDetector();
	~MotionDetector();
	void add_frame( Mat *image );
    Mat *getStitch();
private:
	int type;
	bool first;
	Mat image;
	Mat prevImage;
	Mat difference;
	Mat optical_flow;
	Mat disparity;
	Mat stitch;
	vector<Mat> rgb;
	vector<Mat> prevrgb;

	/* Optical Flow */
    vector<Point2f> frame1_features, frame2_features;
    vector<uchar> found;
    vector<float> error;
    Size window;
    CvTermCriteria criteria;

	void backgroundSubtraction();
	void opticalFlow();
	void cut( Mat *image );
	void merge();
};

#endif /* MOTIONDETECTOR_H_ */
