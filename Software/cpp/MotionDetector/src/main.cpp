/*
 * main.cpp
 *
 *  Created on: Jun 20, 2012
 *      Author: camielv
 */
#define RECORD

#include "MotionDetector.h"

int main()
{
    MotionDetector *MD = new MotionDetector();
    Mat image;

    // Frame
    namedWindow( "Video" );

#ifdef RECORD
    const string NAME = "video.avi";
    VideoWriter *writer = new VideoWriter( NAME, CV_FOURCC('M', 'J', 'P', 'G'), 5, Size(640, 240), 1);
#endif

    // Store dataset
    for(int i = 0; i < 238; i++)
    {
        std::stringstream name;
        name << "../datasets/dataset-6/ardrone" << i << ".png";

        image = cv::imread( name.str(), 1 );
        MD->add_frame( &image );

        imshow( "Video", image );
        waitKey(4);

#ifdef RECORD
        writer->write( *MD->getStitch() );
#endif
    }

#ifdef RECORD
    writer->release();
#endif
    destroyAllWindows();
    return 0;
}
