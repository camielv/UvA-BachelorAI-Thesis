#include "global.h"
#include "bot_ardrone_ardronelib.h"
#include "bot_ardrone.h"
#include "ardrone_tool_win32.h"
#include <opencv2/highgui/highgui.hpp>

using namespace cv;


extern "C" {
	HANDLE ardrone_ready;
}

using namespace std;


/* allocate and initialize static instance member (only allocate pointer) */
bot_ardrone_ardronelib *bot_ardrone_ardronelib::myinstance = 0;


bot_ardrone_ardronelib* bot_ardrone_ardronelib::instance()
{
	return myinstance;
}


void bot_ardrone_ardronelib_process_navdata(navdata_unpacked_t *n)
{
	bot_ardrone_ardronelib::instance()->process_measurement(n);
}


void bot_ardrone_ardronelib_process_frame(unsigned char* rgbtexture, int w, int h)
{
	bot_ardrone_ardronelib::instance()->process_frame(rgbtexture, w, h);
}



bot_ardrone_ardronelib::bot_ardrone_ardronelib(bot_ardrone *bot):
	img_bgr565(DRONE_VIDEO_MAX_HEIGHT, DRONE_VIDEO_MAX_WIDTH, CV_8UC2, NULL, 0),
	img_bgra(BOT_ARDRONE_FRAME_H, BOT_ARDRONE_FRAME_W, CV_8UC4, NULL, 0)
{
	myinstance = (bot_ardrone_ardronelib*) this;

	this->bot = bot;

	frame = new bot_ardrone_frame;

	m_counter = 0;


	// start thread
	printf("Connecting to AR.Drone\n");
	ardrone_ready = CreateEvent(NULL, false, false, NULL);
	
	ardronewin32();

	// wait untill the Drone is completely ready (communication started)
	WaitForSingleObject(ardrone_ready, INFINITE);
	printf("AR.Drone is ready\n");
}


bot_ardrone_ardronelib::~bot_ardrone_ardronelib(void)
{
}


void bot_ardrone_ardronelib::init(void)
{
}


void bot_ardrone_ardronelib::control_update(void *control)
{
	bot_ardrone_control *c = (bot_ardrone_control*) control;

	ardronewin32_progress(
		(c->state == BOT_STATE_FLY)?1:0,
		c->velocity[BOT_ARDRONE_LateralVelocity],
		-c->velocity[BOT_ARDRONE_LinearVelocity],
		c->velocity[BOT_ARDRONE_AltitudeVelocity] + c->velocity_compensate[BOT_ARDRONE_AltitudeVelocity],
		c->velocity[BOT_ARDRONE_RotationalVelocity]
	);
}


void bot_ardrone_ardronelib::take_off()
{
	ardronewin32_take_off();
}


void bot_ardrone_ardronelib::land()
{
	ardronewin32_land();
}


void bot_ardrone_ardronelib::recover(bool send)
{
	printf("Recovering\n");
	ardronewin32_recover(send ? 1 : 0);
}


void bot_ardrone_ardronelib::process_measurement(navdata_unpacked_t *n)
{
	bot_ardrone_measurement *m = new bot_ardrone_measurement;
	m->time = (double) (n->navdata_time.time >> 21);
	m->time += (n->navdata_time.time & 0x001FFFFF) * 0.000001; // microseconds to decimals
	m->time_pc = GetTickCount() / 1000.0;

	last_measurement = m->time;

	// battery
	bot->battery = n->navdata_demo.vbat_flying_percentage;
	
	if (m_counter % 1500 == 0)
		printf("Battery: %i%%\n", bot->battery);

	// state (3 = fly)
	m->state = n->navdata_demo.ctrl_state >> 16;

	m->altitude = n->navdata_altitude.altitude_raw;

	m->navdata_euler_angles[0] = (float) n->navdata_demo.altitude;

	m->or[0] = n->navdata_demo.phi;
	m->or[1] = n->navdata_demo.theta;
	m->or[2] = n->navdata_demo.psi;

	// Accelerations are received in mg
	// They are displayed in g: conversion gain is 1/1000
	// gravity = 1g
	/*
	m->accel[0] = n->navdata_phys_measures.phys_accs[0]; // x-dir
	m->accel[1] = n->navdata_phys_measures.phys_accs[1]; // y-dir
	m->accel[2] = n->navdata_phys_measures.phys_accs[2]; // z-dir
	*/

	m->accel[0] = (float) n->navdata_raw_measures.raw_accs[0];
	m->accel[1] = (float) -n->navdata_raw_measures.raw_accs[1];
	m->accel[2] = (float) -n->navdata_raw_measures.raw_accs[2];

	m->vel[0] = n->navdata_demo.vx;
	m->vel[1] = n->navdata_demo.vy;
	m->vel[2] = n->navdata_altitude.altitude_vz;

	m->gt_loc[0] = n->navdata_demo.drone_camera_trans.v[0];
	m->gt_loc[1] = n->navdata_demo.drone_camera_trans.v[1];
	m->gt_loc[2] = n->navdata_demo.drone_camera_trans.v[2];



	/* temp */
	/*
	m->navdata_euler_angles[0] = n->navdata_euler_angles.phi_a;
	m->navdata_euler_angles[1] = n->navdata_euler_angles.theta_a;
	m->navdata_euler_angles[2] = 0.0f;

	m->gyros_offsets[0] = n->navdata_gyros_offsets.offset_g[0];
	m->gyros_offsets[1] = n->navdata_gyros_offsets.offset_g[1];
	m->gyros_offsets[2] = n->navdata_gyros_offsets.offset_g[2];

	m->phys_gyro_temp[0] = (float) n->navdata_phys_measures.gyro_temp;
	m->phys_gyro_temp[1] = 0.0f;
	m->phys_gyro_temp[2] = 0.0f;

	m->phys_gyros[0] = n->navdata_phys_measures.phys_gyros[0] * DEG_TO_RAD;
	m->phys_gyros[1] = n->navdata_phys_measures.phys_gyros[1] * DEG_TO_RAD;
	m->phys_gyros[2] = n->navdata_phys_measures.phys_gyros[2] * DEG_TO_RAD;

	m->raw_gyros[0] = n->navdata_raw_measures.raw_gyros[0];
	m->raw_gyros[1] = n->navdata_raw_measures.raw_gyros[1];
	m->raw_gyros[2] = n->navdata_raw_measures.raw_gyros[2];

	m->raw_gyros_110[0] = n->navdata_raw_measures.raw_gyros_110[0];
	m->raw_gyros_110[1] = n->navdata_raw_measures.raw_gyros_110[1];
	m->raw_gyros_110[2] = 0.0f;
	*/


	m_counter++;

	bot->measurement_received(m);
}


void bot_ardrone_ardronelib::process_frame(unsigned char* rgbtexture, int w, int h)
{
	// size check
	if (w*h*4 > BOT_ARDRONBOT_EVENT_FRAME_BUFSIZE)
	{
		printf("ARDRONE FRAME TOO LARGE... SKIPPING FRAME (w: %i, h: %i\n", w, h);
		return;
	}

	frame = new bot_ardrone_frame;
	frame->time = last_measurement; // OK?
	frame->w = (short) BOT_ARDRONE_FRAME_W;
	frame->h = (short) BOT_ARDRONE_FRAME_H;
	frame->data_start = frame->data;

	img_bgr565.data		= rgbtexture;
	img_bgra.data		= (unsigned char*) frame->data;
	Mat crop(img_bgr565, Rect(0, 0, BOT_ARDRONE_FRAME_W, BOT_ARDRONE_FRAME_H));
	cvtColor(crop, img_bgra, CV_BGR5652BGRA, 4);

#ifdef BOT_ARDRONE_DISPLAY_CAM
	namedWindow("Bottom cam");
//	cvResizeWindow("Bottom cam", 1024, 800);
	imshow("Bottom cam", image);
	cvWaitKey(4);
#endif

	frame->data_size = w*h*4;

	
	namedWindow( "Frame" );
	imshow( "Frame", img_bgra );
	cvWaitKey(4);

	bot->frame_received(frame);
}