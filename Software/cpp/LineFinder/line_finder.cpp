/*
 * line_finder.cpp
 *
 *  Created on: Jun 20, 2012
 *      Author: camielv
 */

#include "line_finder.h"

/* Public Functions */
line_finder::line_finder( struct Coordinate size )
{
    this->size   = size;
    this->center = Coordinate( size.x / 2, size.y / 2 );
    this->found  = true;
	this->ROI    = Rect( this->center.x - 100, 0, 200, this->center.y );
}

void
line_finder::findLine( Mat *image )
{
    Mat image_filter;

    if( this->found )
    {
        Mat ROI( *image, this->ROI );
        colorFilter( &ROI, &image_filter  );
        namedWindow( "Test" );
        imshow( "Test", ROI );
    } else {
        colorFilter( image, &image_filter );
    }

    // Gaussian Blur
    GaussianBlur( image_filter, image_filter, Size(5, 5), 3, 3);

    // Edge Detection
    Canny( image_filter, image_filter, 50, 200, 3 );

    // Line Segment Detection
    vector<Vec4i> lines;
    HoughLinesP( image_filter, lines, 1, CV_PI / 180, THRESHOLD, MIN_LINE_SIZE, MAX_GAP );
    //  std::cout << "Raw line size: " << lines.size() << "\n";

    // Sort lines
    Lines final = sortLines( lines, &this->size );
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

    if( lines.size() > 0 )
    {
        if( found )
            line( *image, Point( best.start.x + this->center.x - 100, best.start.y ), Point( best.end.x + this->center.x - 100, best.end.y ), Scalar(0,255,0), 3, 8);
        else
            line( *image, Point( best.start.x, best.start.y ), Point( best.end.x, best.end.y ), Scalar(0,255,0), 3, 8);
    }
    line( *image, Point( this->size.x / 2, this->size.y / 2), Point( this->size.x / 2 , this->size.y / 2 ), Scalar(0,0,255), 3, 8);

    namedWindow( "Video" );
    imshow( "Video", *image );

    if( lines.size() > 0 )
    {
        this->found = true;
 /*
        std::cout << "Size: (" << size_image.x << ", " << size_image.y << ")\n";
        std::cout << "Start: (" << best.top.x << ", " << best.top.y << ")\n";
        std::cout << "End: (" << best.bottom.x << ", " << best.bottom.y << ")\n";
        std::cout << "Formula: y=" << best.slope << "x + " << best.b << "\n";
        std::cout << "Angle: " << calculateAngle( best.top, best.bottom ) << "\n";
        std::cout << "Translation: " << translation( best.bottom, &size_image ).x << "\n";
        std::cout << "Sum: " << translation( best.bottom, &size_image ).x + best.bottom.x << "\n";

        std::cout << "Size: (" << this->size.x << ", " << this->size.y << ")\n";
        Coordinate move = shift( &best.bottom, &best.top, &this->center );
        std::cout << "Shift: (" << move.x << ", " << move.y << ")\n";
*/
        while( !(waitKey(1) >= 0) )
        {

        }
    } else {
        this->found = false;
    }
}

/* Private Functions */
void
line_finder::colorFilter( Mat *image, Mat *filter )
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
line_finder::sortLines( vector<Vec4i> lines, struct Coordinate *size )
{
    Lines sorted = Lines( lines.size() );

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
                    sorted.lines[i].start       = min_point( &slope, &b, sorted.lines[i].start, start );
                    sorted.lines[i].end         = max_point( &slope, &b, sorted.lines[i].end  , end   );
                    sorted.lines[i].slope       = calculateSlope( sorted.lines[i].start, sorted.lines[i].end );
                    sorted.lines[i].b           = calculateB( sorted.lines[i].start, sorted.lines[i].slope );
                    sorted.lines[i].top         = Coordinate( calculateX( &sorted.lines[i], 0       ), 0       );
                    sorted.lines[i].bottom      = Coordinate( calculateX( &sorted.lines[i], size->y ), size->y );
                    sorted.lines[i].length      = sqrt( pow( fabs( sorted.lines[i].end.x - sorted.lines[i].start.x), 2 ) + pow( fabs( sorted.lines[i].end.y - sorted.lines[i].start.y), 2 ) );
                    sorted.lines[i].strongness += 1;

                    add += 1;
                    break;
                }
            }

            if( add )
                continue;
        }
        // If not similar add it to the database
        Line line;
        line.start      = start;
        line.end        = end;
        line.slope      = slope;
        line.b          = b;
        line.top        = Coordinate( calculateX( &line, 0       ), 0       );
        line.bottom     = Coordinate( calculateX( &line, size->y ), size->y );
        line.length     = sqrt( pow( fabs(line.end.x - line.start.x), 2 ) + pow( fabs(line.end.y - line.start.y), 2 ) );
        line.strongness = 1;

        sorted.add( line );
    }

    return sorted;
}

double
line_finder::calculateAngle( struct Coordinate start, struct Coordinate end)
{
    double x = end.x - start.x;
    double y = end.y - start.y;
    return atan2( x, y );
}

double
line_finder::calculateSlope( struct Coordinate start, struct Coordinate end )
{
    if( start.x == end.x )
        return 0;
    return ( end.y - start.y ) / ( end.x - start.x);
}

double
line_finder::calculateB( struct Coordinate start, double slope )
{
    return start.y - (start.x * slope);
}

double
line_finder::calculateX( Line *line, double y )
{
    if ( line->slope == 0)
        return line->start.x;
    return (y - line->b) / line->slope;
}

double
line_finder::calculateY( Line *line, double x )
{
    return line->slope * x + line->b;
}

struct Coordinate
line_finder::translation( struct Coordinate start, struct Coordinate *size )
{
    return Coordinate( size->x / 2 - start.x, size->y / 2 - start.y);
}

struct Coordinate
line_finder::min_point( double *slope, double *b, struct Coordinate min1, struct Coordinate min2 )
{
    if( *slope >= 0 )
    {
        return Coordinate( std::min( min1.x, min2.x ), std::min( min1.y, min2.y ) );
    }
    return Coordinate( std::min( min1.x, min2.x ), std::max( min1.y, min2.y ) );
}

struct Coordinate
line_finder::max_point( double *slope, double *b, struct Coordinate max1, struct Coordinate max2 )
{
    if( *slope >= 0 )
    {
        return Coordinate( std::max( max1.x, max2.x ), std::max( max1.y, max2.y ) );
    }
    return Coordinate( std::max( max1.x, max2.x ), std::min( max1.y, max2.y ) );
}


struct Coordinate
line_finder::shift( struct Coordinate *A, struct Coordinate *B, struct Coordinate *P )
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

    return Coordinate( closest.x - P->x , P->y - closest.y );
}
