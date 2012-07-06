/*
 * line_finder.h
 *
 *  Created on: Jun 20, 2012
 *      Author: camielv
 */

#ifndef LINE_FINDER_H_
#define LINE_FINDER_H_
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <math.h>
#include <sstream>

// Pelican settings
//enum filter{ HUE_MIN1 = 175, HUE_MIN2 = 0, HUE_MAX1 = 179, HUE_MAX2 = 10, SAT_MIN = 80, SAT_MAX = 153, VAL_MIN = 240, VAL_MAX = 255 };
//enum settings{ THRESHOLD = 80, MIN_LINE_SIZE = 5, MAX_GAP = 100};

// AR.Drone settings
enum filter{ HUE_MIN1 = 10, HUE_MIN2 = 0, HUE_MAX1 = 25, HUE_MAX2 = 0, SAT_MIN = 51, SAT_MAX = 102, VAL_MIN = 217, VAL_MAX = 255 };
enum settings{ THRESHOLD = 40, MIN_LINE_SIZE = 5, MAX_GAP = 100 };

using namespace cv;

/* Structures of the class */
struct
Coordinate
{
    double x;
    double y;

    Coordinate()
    {
    }

    Coordinate( double x, double y )
    {
        this->x = x;
        this->y = y;
    }
};

struct
Line
{
    /* Line details */
    struct Coordinate start;
    struct Coordinate end;
    double length;
    int strongness;

    /* Formula details*/
    double slope;
    double b;
};


struct
Lines
{
    Line *lines;
    int size;
    int max_size;

    Lines( int size )
    {
        this->lines    = new Line[ size ];
        this->size     = 0;
        this->max_size = size;
    }

    void add(struct Line line)
    {
        if(this->size == this->max_size)
        {
            int max_size = this->max_size * 2;
            struct Line *lines = new Line[max_size];
            memcpy( lines, this->lines, this->max_size * sizeof(struct Line) );

            this->max_size = max_size;
            delete [] this->lines;
            this->lines = lines;
        }

        this->lines[this->size] = line;
        this->size++;
    }
};

struct
Control
{
	double rotation;
	double lateral;  // x-axis
	double linear;   // y-axis
};

class LineFinder
{
public:
	LineFinder( struct Coordinate size );
	void add_frame( bot_ardrone_frame *f );
	struct Control getControl();
	~LineFinder();
private:
	/* Variables */
	struct Control update;
	Mat frame; // The complete frame
	Mat cam_frame; // Resized frame
	Mat image; // The search image (Can be smaller than frame due ROI)

	struct Coordinate size;
    struct Coordinate center;
    bool found;
    Rect ROI;

	/* Functions */
    void process();
    void colorFilter( Mat *image, Mat *filter );
    struct Lines sortLines( vector<Vec4i> lines );
    double calculateAngle( struct Coordinate start, struct Coordinate end);
    struct Coordinate min_point( double *slope, double *b, struct Coordinate min1, struct Coordinate min2 );
    struct Coordinate max_point( double *slope, double *b, struct Coordinate max1, struct Coordinate max2 );
    struct Coordinate shift( struct Coordinate *A, struct Coordinate *B, struct Coordinate *P );
    double calculateSlope( struct Coordinate start, struct Coordinate end );
    double calculateB( struct Coordinate start, double slope );
	double speed( double distance, int type );
};

#endif /* LINE_FINDER_H_ */
