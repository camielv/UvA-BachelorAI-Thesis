/*
 * LineFinder.cpp
 *
 *  Created on: Jun 20, 2012
 *      Author: camielv
 */

#include "bot_ardrone.h"
#include "LineFinder.h"
#include <sstream>

/* Public Functions */
LineFinder::LineFinder( struct Coordinate size )
{
	this->frame     = Mat( Size( (int) 2 * size.x, (int) 2 * size.y ), CV_8UC4, NULL, 0);
	this->cam_frame = Mat( Size( (int) size.x, (int) size.y ), CV_8UC4, NULL, 0);
	this->center    = Coordinate( size.x, size.y );
	this->found     = false;
    this->ROI       = Rect( (int) this->center.x - 100, 0, 200, (int) this->center.y );
}

void
LineFinder::add_frame( bot_ardrone_frame *f )
{
	this->cam_frame.data = (unsigned char*) f->data;
	resize( this->cam_frame, this->frame, Size( 176 * 2, 144 * 2 ) );

	if( found )
	{
		this->image  = Mat( this->frame, this->ROI );
	} else {
		this->image = this->frame;
	}
	this->process();

	// Draw ROI
	rectangle( this->frame, Point(this->center.x - 100, 0), Point(this->center.x + 100, this->center.y), Scalar(250,0,0), 1);
	namedWindow( "Linecontroller" );
	imshow( "Linecontroller", this->frame );


/*	Code for tracking 2 cams

	Mat cam1( this->frame.clone(), Rect(0, 0, 87, 72) );
	rectangle( this->frame, Point(0, 0), Point(87, 72), Scalar(0,0,0), CV_FILLED);
	this->size   = Coordinate( f->w, f->h );
	resize( cam1, cam1, Size(3 * 87, 3 * 72));

	namedWindow( "Cam1" );
	imshow( "Cam1", this->frame );
	cvWaitKey(4);
	namedWindow( "Cam2" );
	imshow( "Cam2", cam1 );
	cvWaitKey(4);
*/	
}


LineFinder::~LineFinder()
{

}


struct Control
LineFinder::getControl()
{
	return this->update;
}

/* Private Functions */
void
LineFinder::process()
{
    Mat image_filter;

    // Color filter
    colorFilter( &this->image, &image_filter );

    // Gaussian Blur
    GaussianBlur( image_filter, image_filter, Size(5, 5), 3, 3);

    // Edge Detection
    Canny( image_filter, image_filter, 50, 200, 3 );

    // Line Segment Detection
    vector<Vec4i> lines;
    HoughLinesP( image_filter, lines, 1, CV_PI / 180, THRESHOLD, MIN_LINE_SIZE, MAX_GAP );
    //std::cout << "Raw line size: " << lines.size() << "\n";

    // Sort lines
	Lines final = sortLines( lines );
    //std::cout << "Line size: " << final.size << "\n";

    // Best line
    Line best = Line();

    for( int i = 0; i < final.size ; i++ )
    {
        if( final.lines[i].strongness * 10 + final.lines[i].length > best.strongness * 10 + best.length )
        {
            best = final.lines[i];
        }
    }

	this->update = Control();

	if( lines.size() > 0 )
	{
//		std::cout << "Line found in frame\n";
		double angle = calculateAngle( best.start, best.end );

		if( angle > CV_PI )
			angle -= 2 * CV_PI;

		if ( found )
		{
			line( this->frame, Point( (int) (best.start.x + this->center.x - 75), (int) best.start.y ), Point( (int) (best.end.x + this->center.x - 100), (int) best.end.y ), Scalar(0,255,0), 3, 8);

			update.linear   = 0.05;
			update.rotation = speed( angle, BOT_ARDRONE_RotationalVelocity );
		} else {
			line( this->frame, Point( (int) best.start.x, (int) best.start.y ), Point( (int) best.end.x, (int) best.end.y ), Scalar(0,255,0), 3, 8);
			struct Coordinate shift_to_center = shift( &best.start, &best.end, &this->center );

			update.lateral  = speed( shift_to_center.x, BOT_ARDRONE_LateralVelocity);
			update.linear   = speed( shift_to_center.y, BOT_ARDRONE_LinearVelocity ); 
			update.rotation = speed( angle, BOT_ARDRONE_RotationalVelocity );

			std::cout << "Lateral: " << update.lateral << " Linear: " << update.linear << " Rotation: " << update.rotation << "\n";

			found = true;
		}
	} else {
//		std::cout << "Line not found in frame\n";
		found = false;
	}
}

void
LineFinder::colorFilter( Mat *image, Mat *filter )
{
    int width  = image->size[0];
    int height = image->size[1];
    Mat image_hsv( width, height, CV_32F );
    Mat   filter1( width, height, CV_32F );
    Mat   filter2( width, height, CV_32F );
    cvtColor( *image, image_hsv, CV_BGR2HSV );
    Scalar filter1_min = Scalar( HUE_MIN1, SAT_MIN, VAL_MIN );
    Scalar filter1_max = Scalar( HUE_MAX1, SAT_MAX, VAL_MAX );
    Scalar filter2_min = Scalar( HUE_MIN2, SAT_MIN, VAL_MIN );
    Scalar filter2_max = Scalar( HUE_MAX2, SAT_MAX, VAL_MAX );

    inRange( image_hsv, filter1_min, filter1_max, filter1 );
    inRange( image_hsv, filter2_min, filter2_max, filter2 );

    bitwise_or( filter1, filter2, *filter );
}

struct Lines
LineFinder::sortLines( vector<Vec4i> lines )
{
    Lines sorted = Lines( (int) lines.size() );

    for( size_t i = 0; i < lines.size(); i++ )
    {
        // Derive the linear formula from coordinates
        // y = ax + b (where a = slope)
        struct Coordinate start = Coordinate(lines[i][0], lines[i][1]);
        struct Coordinate end   = Coordinate(lines[i][2], lines[i][3]);
        double slope = calculateSlope( start, end );
        double b     = calculateB( start, slope );
/*
        std::cout << i << " Coordinates: (" << lines[i][0] << ", " << lines[i][1] << ") (" << lines[i][2] << ", " << lines[i][3] << ")\n";
        std::cout << i << " Slope: " << slope << "\n";
        std::cout << i << " B: " << b << "\n";
*/
        // Check if there is a line
        if( sorted.size > 0 )
        {
            int add = 0;

            // Check if current line is similar to previous found lines
            for(int i = 0; i < sorted.size; i++)
            {
                if( fabs(sorted.lines[i].slope - slope) < 0.2 &&
                    fabs(sorted.lines[i].b - b)         < 20
                    )
                {
                    sorted.lines[i].start         = min_point( &slope, &b, sorted.lines[i].start, start );
                    sorted.lines[i].end           = max_point( &slope, &b, sorted.lines[i].end  , end   );
                    sorted.lines[i].slope         = calculateSlope( sorted.lines[i].start, sorted.lines[i].end );
                    sorted.lines[i].b             = calculateB( sorted.lines[i].start, sorted.lines[i].slope );
                    sorted.lines[i].length        = sqrt( pow( fabs( sorted.lines[i].end.x - sorted.lines[i].start.x), 2 ) + pow( fabs( sorted.lines[i].end.y - sorted.lines[i].start.y), 2 ) );
                    sorted.lines[i].strongness   += 1;

                    add += 1;
                    break;
                }
            }

            if( add )
                continue;
        }
        // If not similar add it to the database
        Line line;
        line.start         = start;
        line.end           = end;
        line.slope         = slope;
        line.b             = b;
        line.length        = sqrt( pow( fabs(line.end.x - line.start.x), 2 ) + pow( fabs(line.end.y - line.start.y), 2 ) );
        line.strongness    = 1;

        sorted.add( line );
    }

    return sorted;
}

double
LineFinder::calculateAngle( struct Coordinate start, struct Coordinate end)
{
    double x = end.x - start.x;
    double y = end.y - start.y;
    return atan2( x, y );
}

double
LineFinder::calculateSlope( struct Coordinate start, struct Coordinate end )
{
    return ( end.y - start.y ) / ( end.x - start.x);
}

double
LineFinder::calculateB( struct Coordinate start, double slope )
{
    return start.y - (start.x * slope);
}

struct Coordinate
LineFinder::min_point( double *slope, double *b, struct Coordinate min1, struct Coordinate min2 )
{
    if( *slope >= 0 )
    {
        return Coordinate( std::min( min1.x, min2.x ), std::min( min1.y, min2.y ) );
    }
    return Coordinate( std::min( min1.x, min2.x ), std::max( min1.y, min2.y ) );
}

struct Coordinate
LineFinder::max_point( double *slope, double *b, struct Coordinate max1, struct Coordinate max2 )
{
    if( *slope >= 0 )
    {
        return Coordinate( std::max( max1.x, max2.x ), std::max( max1.y, max2.y ) );
    }
    return Coordinate( std::max( max1.x, max2.x ), std::min( max1.y, max2.y ) );
}

struct Coordinate
LineFinder::shift( struct Coordinate *A, struct Coordinate *B, struct Coordinate *P )
{
    Coordinate AP = Coordinate( P->x - A->x, P->y - A->y );
    Coordinate AB = Coordinate( B->x - A->x, B->y - A->y );

    double ab2   = AB.x * AB.x + AB.y * AB.y;
    double ap_ab = AP.x * AB.x + AP.y * AB.y;
    double t     = ap_ab / ab2;

    if( t < 0)
        t = 0;
    else if(t > 1)
        t = 1;

    Coordinate closest = Coordinate( A->x + AB.x * t, A->y + AB.y * t );

    return Coordinate( P->x - closest.x, P->y - closest.y );
}

double
LineFinder::speed( double distance, int type )
{
	switch( type )
	{
	case BOT_ARDRONE_RotationalVelocity:
		if( distance > 0.05 )
			return -0.02;
		else if( distance < -0.05 )
			return 0.02;
		return 0;
	case BOT_ARDRONE_LateralVelocity:
		if( distance > 30 )
			return -0.02;
		else if( distance < -30 )
			return 0.02;
		return 0;
	case BOT_ARDRONE_LinearVelocity:
		if( distance > 20 )
			return 0.02;
		else if( distance < -20 )
			return -0.02;
		return 0;
	default:
		return 0;
	}
}