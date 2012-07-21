#include "MotionDetector.h"

MotionDetector::MotionDetector()
{
    this->stitch = Mat::zeros( Size(640, 240), CV_8UC3 );
	this->first = true;
    this->window = Size(3,3);
    this->criteria = cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, .3 );
    this->type = OPTICAL_FLOW;
}

MotionDetector::~MotionDetector()
{

}

void
MotionDetector::add_frame( Mat *image )
{
    this->image = *image;

    switch( this->type )
    {
    case OPTICAL_FLOW:

        if( first )
        {
            this->first = false;
        } else {
            this->opticalFlow();
            /*namedWindow( "Optical Flow" );
            imshow( "Optical Flow", this->optical_flow );*/
            namedWindow( "Disparity" );
            imshow( "Disparity", this->stitch );
            this->merge();
            //while( ! (waitKey(1) >= 0) ) {}
        }
        this->prevImage    = this->image.clone();
        this->disparity    = this->image.clone();
        this->optical_flow = this->image.clone();
        break;

    case BACKGROUND_SUBTRACTION:

        if( first ) {
            this->first = false;
            this->image.convertTo( this->prevImage, CV_32FC4 );
        } else {
            this->backgroundSubtraction();
            namedWindow( "Background Subtraction" );
            imshow( "Background Subtraction", this->difference );
            while( ! (waitKey(1) >= 0) ) {}
        }
        break;

    default:
        return;
    }
}


void
MotionDetector::opticalFlow()
{
    Mat gray;

    // Convert prevImage to gray goodfeaturestotrack only takes grayscale
    cvtColor( this->prevImage, gray, CV_RGB2GRAY );

    // Find features in previous picture
    goodFeaturesToTrack(
            gray,
            this->frame1_features,
            FEATURES,
            .01,
            .01,
            noArray(),
            3,
            false,
            0.04 );

    // No features return
    if( this->frame1_features.size() == 0)
        return;

    cornerSubPix(
            gray,
            this->frame1_features,
            this->window,
            Size(-1, -1),
            this->criteria);

    // Release gray image
    gray.release();

    // Calculate Optical Flow with new image
    calcOpticalFlowPyrLK(
            this->prevImage,            // Previous frame
            this->image,                // New frame
            this->frame1_features,      // Features previous frame
            this->frame2_features,      // Features current frame
            this->found,                // Found features
            this->error,                // Error features
            this->window,               // Size of the search window
            5,
            this->criteria );

    int counter = (int) this->found.size();
    float horizontal_average = 0;

    // Draw Optical Flow
    for(int i = 0; i <  counter; i++)
    {
        if( found[i] )
        {
            Point frame1, frame2;
            frame1 = frame1_features[i];
            frame2 = frame2_features[i];

            horizontal_average = frame2.x - frame1.x;

            line( this->optical_flow, frame1, frame2, Scalar( 255, 0, 0), 0.1, CV_AA, 0 );
        }

    }

    // Calculate fundamental matrix
    Mat fundamental;

    fundamental = findFundamentalMat(
            frame1_features,
            frame2_features,
            FM_RANSAC,
            3.,
            0.999,
            noArray());

    Mat H1(4, 4, prevImage.type() );
    Mat H2(4, 4, prevImage.type() );

    // Hartley's Algorithm to rectify uncalibrated camera
    stereoRectifyUncalibrated(
            frame1_features,
            frame2_features,
            fundamental,
            this->prevImage.size(),
            H1,
            H2);

    // Rectify Uncalibrated
    Mat rectified1( this->prevImage.size(), this->prevImage.type() );
    Mat rectified2( this->image.size(), this->image.type() );
    warpPerspective( this->prevImage, rectified1, H1, this->prevImage.size() );
    warpPerspective( this->image, rectified2, H2, this->image.size() );

    Mat left, right;

    // On basis of optical flow know what image is right and what image is left.
    if( horizontal_average > 0 )
    {
        cvtColor( rectified1, right, CV_RGB2GRAY);
        cvtColor( rectified2, left, CV_RGB2GRAY);
        //cvtColor( this->prevImage, right, CV_RGB2GRAY);
        //cvtColor( this->image, left, CV_RGB2GRAY);
    } else {
        cvtColor( rectified1, left, CV_RGB2GRAY);
        cvtColor( rectified2, right, CV_RGB2GRAY);
        //cvtColor( this->prevImage, left, CV_RGB2GRAY);
        //cvtColor( this->image, right, CV_RGB2GRAY);
    }

    // Calculate disparity
    Mat disparity;
    StereoBM Disparity = StereoBM( 2, 16 );
    Disparity.operator ()( left, right, disparity, CV_32F );
    cut( &disparity );
}


void
MotionDetector::backgroundSubtraction()
{
    // (1 - alpha) * Current Image + alpha * Previous Image
    // Channels are processed independently
    accumulateWeighted( this->image, this->prevImage, 0.100/*0.020*/ );
    // Convert to right image format
    this->prevImage.convertTo( this->difference, CV_8UC4 );
    // Absolute difference between combined image and current image.
    absdiff( this->image, this->difference, this->difference );
    // Convert to grayscale
    cvtColor( this->difference, this->difference, CV_RGB2GRAY );
    // Threshold image
    threshold( this->difference, this->difference, 14, 255, CV_THRESH_BINARY );
}


void
MotionDetector::cut( Mat *image )
{
    for( int i = 0; i < image->size[0]; i++ )
    {
        for( int j = 0; j < image->size[1]; j++ )
        {
            if( image->at<float>(i, j) < THRESHOLD )
            {
                this->disparity.at<Vec3b>(i, j) = Vec3b( 0, 0, 0 );
            }
        }
    }
}


void
MotionDetector::merge()
{
    Mat ROI( this->stitch, Rect(0, 0, 320, 240) );
    this->optical_flow.copyTo( ROI );
    Mat ROI2( this->stitch, Rect(320, 0, 320, 240) );
    this->disparity.copyTo( ROI2 );
}


Mat *
MotionDetector::getStitch()
{
    return &this->stitch;
}
