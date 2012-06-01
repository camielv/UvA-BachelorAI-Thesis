#pragma once
#include "global.h"
#include "botinterface.h"
#include "mysocket.h"
#include <string>

#include <cv.hpp>
#include <cxcore.hpp>

#define BOT_ARDRONE_USARSIM_SOCKET_CONTROL 1
#define BOT_ARDRONE_USARSIM_SOCKET_FRAME 2

// forward declarations
class bot_ardrone;
struct bot_ardrone_control;
struct bot_ardrone_measurement;
struct bot_ardrone_frame;


using namespace std;


class bot_ardrone_usarsim : public botinterface
{
public:
	bot_ardrone_usarsim(bot_ardrone *bot);
	~bot_ardrone_usarsim(void);

	void init();

	/* control */
	void control_update(void *control);
	void control_send(char *message);

	/* handlers */
	void socket_callback(int id, char *message, int bytes);
	void process_measurement(char *message, int bytes);
	void process_frame(char *message, int bytes);
	bool check_frame();

	bot_ardrone_frame *frame;

private:
	bot_ardrone *bot;
	mysocket *control_socket;
	mysocket *frame_socket;
	char frame_buffer[BOT_ARDRONBOT_EVENT_FRAME_BUFSIZE];
	cv::Mat img_bgr;
	cv::Mat img_bgra;

	double get_time();

	__int64 time_start, time_freq;
};