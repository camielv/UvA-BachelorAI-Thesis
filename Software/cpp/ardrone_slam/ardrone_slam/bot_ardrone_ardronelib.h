#pragma once
#include "botinterface.h"
#include <windows.h>
#include <string>

#include <ardrone_api.h>

#include <cv.hpp>
#include <cxcore.hpp>

#define DRONE_VIDEO_MAX_WIDTH 512
#define DRONE_VIDEO_MAX_HEIGHT 512

class bot_ardrone;
struct bot_ardrone_control;
struct bot_ardrone_measurement;
struct bot_ardrone_frame;


using namespace std;


// C application calls these functions
extern "C" {
	void bot_ardrone_ardronelib_process_navdata(navdata_unpacked_t *n);
	void bot_ardrone_ardronelib_process_frame(unsigned char* rgbtexture, int w, int h);
};


class bot_ardrone_ardronelib : public botinterface
{
public:
	bot_ardrone_ardronelib(bot_ardrone *bot);
	~bot_ardrone_ardronelib(void);

	void init();

	/* control */
	void control_update(void *control);
	void take_off();
	void land();
	void recover(bool send);

	/* handlers */
	void process_measurement(navdata_unpacked_t *n);
	void process_frame(unsigned char* rgbtexture, int w, int h);

	static bot_ardrone_ardronelib* instance();

	static bot_ardrone_ardronelib* myinstance;
	bot_ardrone_frame *frame;

private:
	bot_ardrone *bot;
	HANDLE ardrone_thread;

	cv::Mat img_bgr565;
	cv::Mat img_bgra;

	// temp
	unsigned int m_counter;
	double last_measurement;
};