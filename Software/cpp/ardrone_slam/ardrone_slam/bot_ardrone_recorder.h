#pragma once
#include <windows.h>
#include <fstream>


class bot_ardrone;
struct bot_ardrone_measurement;
struct bot_ardrone_control;
struct bot_ardrone_frame;

using namespace std;


class bot_ardrone_recorder
{
public:
	bot_ardrone_recorder(bot_ardrone *bot);
	~bot_ardrone_recorder(void);

	/* record */
	void record_measurement(bot_ardrone_measurement *m);
	void record_control(bot_ardrone_control *c);
	void record_frame(bot_ardrone_frame *f);

	/* playback */
	void playback(char *dataset);

	/* other */
	void prepare_dataset();

private:
	void wait_for_event(double time);
	double last_event_time;

	bot_ardrone *bot;
	char dataset_dir[25];
	//ifstream fin;
	int frame_counter;

	// resource sharing
	static HANDLE hMutex;

	/* faster */
	FILE *file;
};