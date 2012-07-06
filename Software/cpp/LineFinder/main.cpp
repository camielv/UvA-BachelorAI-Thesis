/*
 * main.cpp
 *
 *  Created on: Jun 20, 2012
 *      Author: camielv
 */

#include "line_finder.h"

int main()
{
    line_finder find( Coordinate( 2*177, 2*144 ) );
//    const string NAME = "video.avi";
//    VideoWriter *writer = new VideoWriter( NAME, CV_FOURCC('M', 'J', 'P', 'G'), 15, Size(176*2, 144*2), 1);
//    Mat intro = imread( "uva.png", 1 );
    Mat image;
    Mat resized;

    // Frame
    namedWindow( "Video" );

/*
    // Store introduction template
    for(int i = 0; i < 100; i++)
    {
       writer->write(intro);
       imshow( "Video", intro );
       if( waitKey(1) >= 0 ) break;
    }
*/
    // Store dataset
	
    for(int i = 0; i < 724; i++)
    {
        std::stringstream name;
        name << "ardrone/" << i << ".png";
        image = imread( name.str(), 1 );
        resize( image, resized, Size(176*2, 144*2), 2, 2);
        find.findLine( &resized );
//        writer->write( resized );
        imshow( "Video", resized );
        if( waitKey(1) >= 0 ) break;
    }

//    writer->release();

    return 0;
}


