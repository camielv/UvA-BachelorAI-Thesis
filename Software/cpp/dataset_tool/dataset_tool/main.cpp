#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <time.h>
#include <direct.h>
#include <string>

/* Namespaces */
using namespace std;

/* Prototypes */
void sleep( double time );

double getTime( double *t1, double *t2 );

double getFPS( double *t1, double *t2 );

string getDateTime();


/* Functions */
int
main()
{
	cv::VideoCapture camera;
	cv::Mat image;

	/* Camera address */
	const string address = "http://user:ssa@192.168.0.90/axis-cgi/mjpg/video.cgi?resolution=640x480&req_fps=30&.mjpg";
	int counter = 0;
	string folder = getDateTime();
	char filename[100];
	char buffer[10];

	printf( "> Creating folder...\n" );

	if ( mkdir( folder.c_str() ) != 0 )
	{
		printf( "> Unable to create folder\n" );
		sleep(5);
		return -1;
	}

	printf( "> Folder created: %s\n> Connecting to camera...\n", folder.c_str() );


	if ( !camera.open( address ) )
	{
		printf( "> Camera not found\n" );
		sleep(5);
		return -1;
	}

	printf( "> Camera found\n> Creating dataset" );

	while( 1 )
	{
		double t1 = (double) clock();
		if( !camera.read( image ) ) {
			printf( "> No frame to grab" );
			cv::waitKey();
		}
		cv::imshow( "Camera Window", image);

		strcpy( filename, folder.c_str() );
		strcat( filename, "\\" );
		strcat( filename, itoa(counter++, buffer, 10) );
		strcat( filename, ".png" );

		cv::imwrite( filename, image );

		double t2 = (double) clock();
		printf( "> Time: %g ms\n> FPS: %.2g\n", getTime(&t1, &t2), getFPS(&t1, &t2));
		if( cv::waitKey(1) >= 0 ) break;
	}

	printf( "> Closing programme..." );
	sleep(5);

	return 0;
}

/* sleep function pauses a amount of time in seconds*/
void
sleep( double time )
{
	clock_t target = clock() + time * 1000;
	while ( clock() < target ) { }
}

double
getFPS( double *t1, double *t2 )
{
	return 1. / getTime( t1, t2 );
}

double
getTime( double *t1, double *t2)
{
	return (1 / (*t2 - *t1)) * CLOCKS_PER_SEC;
}

string
getDateTime()
{
	time_t rawtime;
	struct tm *TimeObject;
	char name[80];

	time( &rawtime );
	TimeObject = localtime( &rawtime );

	strftime( name, sizeof(name), "%d-%b-%Y__%H_%M_%S", TimeObject );
	return name;
}