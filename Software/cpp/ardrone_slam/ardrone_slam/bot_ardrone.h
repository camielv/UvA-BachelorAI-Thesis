#pragma once

#include "bot.h"
#include "botinterface.h"
#include "bot_ardrone_recorder.h"
#include "slam.h"
#include "LineFinder.h"

#include <time.h>

#define BOT_ARDRONE_Velocity 0
#define BOT_ARDRONE_AltitudeVelocity 0
#define BOT_ARDRONE_LinearVelocity 1
#define BOT_ARDRONE_LateralVelocity 2
#define BOT_ARDRONE_RotationalVelocity 3

#define BOT_ARDRONE_INTERFACE_NONE 0x00
#define BOT_ARDRONE_INTERFACE_USARSIM 0x01
#define BOT_ARDRONE_INTERFACE_ARDRONELIB 0x02

#define BOT_ARDRONBOT_EVENT_MEASUREMENT_SEN 0
#define BOT_ARDRONBOT_EVENT_MEASUREMENT_STA 1

#define BOT_ARDRONE_SENSOR_UNKNOW 0
#define BOT_ARDRONE_SENSOR_GT 1
#define BOT_ARDRONE_SENSOR_IMU 2
#define BOT_ARDRONE_SENSOR_SONAR 3
#define BOT_ARDRONE_SENSOR_ACCEL 4


struct bot_ardrone_control {
	double time;
	float velocity[4];
	float velocity_compensate[4];
	bot_state state;

	bot_ardrone_control();
};

struct bot_ardrone_measurement {
	unsigned char bot_id;
	double time;
	double time_pc;
	unsigned char state;
	int altitude;		// mm

	/* Acceleration: x, y, z in mili-g (x direction = F/B, y direction is L/R, z direction is U/D
	 * measurement from navdata->navdata_phys_measures.phys_accs
	 * 1 g = 9.80665 m/s2 = 32.17405 ft/s2
	 */
	float accel[3];

	/* Orientation: pitch, roll, yaw (milli-degrees)
	 * measured by an angular speed sensor */
	float or[3];

	/* estimated linear velocity: x, y, z (mm/s)
	 * The linear speeds (vx/vy/vz) are given in the local frame
	 * (so you only have to apply a psi rotation to get
	 * them on the ground frame) */
	float vel[3];

	/* USARSim only */
	float gt_loc[3];
	float gt_or[3];


	/* temp */
	float navdata_euler_angles[3];
	float gyros_offsets[3];
	float phys_gyro_temp[3];
	float phys_gyros[3];
	float raw_gyros[3];
	float raw_gyros_110[3];

	bot_ardrone_measurement();
};

struct bot_ardrone_frame {
	double time;
	unsigned short w;
	unsigned short h;
	char *data;
	char *data_start;
	int data_size;
	int dest_size;
	char filename[25];

	bot_ardrone_frame();
	~bot_ardrone_frame();
};

class bot_ardrone
{
public:
	void lineupdate( Control update );
	void move(float val, int type);
	bot_ardrone(unsigned char id, unsigned char botinterface, unsigned char slam_mode);
	~bot_ardrone(void);
	void control_set(int type, int opt, float val);
	float control_get(int type, int opt);
	void control_update();
	void control_update(bot_ardrone_control *c);
	void control_reset();
	void take_off();
	void land();
	void recover(bool send);
	void measurement_received(bot_ardrone_measurement *m);
	void frame_received(bot_ardrone_frame *f);
	static double get_clock();
	void set_record();
	void set_playback(char *dataset);
	slam* get_slam();
	void set_slam(bool state);
	void get_slam_pos(float *pos);
	void get_slam_or(float *or);
	void set_follow_line();

	static clock_t start_clock;
	unsigned char id;
	unsigned char i_id;
	botinterface *i;
	bot_ardrone_control control;
	bot_ardrone_recorder *recorder;
	bool record, playback;
	int battery;		// percentage (0-100%)

	// Slam
	clock_t last_measurement_time;
	bool slam_state;
	slam *slamcontroller;

	// Line following
	bool line_follow_state;
	int counter;
    LineFinder *linecontroller;
};