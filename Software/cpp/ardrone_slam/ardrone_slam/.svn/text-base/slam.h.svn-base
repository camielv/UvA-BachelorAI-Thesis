#pragma once

#include "slam_queue.h"
#include "slam_map.h"
#include "slam_module_frame.h"
#include "slam_module_sensor.h"
#include "slam_module_ui.h"

#include "opencv_ekf.h"


// structs
struct bot_ardrone_frame;
struct bot_ardrone_measurement;
struct bot_ardrone_control;

// threads
static DWORD WINAPI start_process_frame(void* Param);
static DWORD WINAPI start_process_sensor(void* Param);
static DWORD WINAPI start_ui(void* Param);


class slam
{
public:
	slam(unsigned char mode, unsigned char bot_id);
	~slam();
	void slam::run();

	float* get_state();
	bool mode(unsigned char mode);
	void on(unsigned char mode);
	void off(unsigned char mode);

	void add_input_frame(bot_ardrone_frame *f);
	void add_input_framefile(char *filename);
	void add_input_sensor(bot_ardrone_measurement *m);
	void get_world_position(float *pos);
	void update_transition_matrix(float difftime);
	void sensor_pause(double time);
	void sensor_resume();

	/* settings */
	unsigned char _mode;
	unsigned char bot_id;

	/* threads */
	HANDLE thread_process_frame;
	HANDLE thread_process_sensor;
	HANDLE thread_ui;

	/* queues */
	slam_queue<bot_ardrone_frame*> queue_frame;
	slam_queue<bot_ardrone_measurement*> queue_sensor;
	slam_queue<bot_ardrone_control*> queue_control;

	/* modules (processors) */
	slam_module_frame		*m_frame;
	slam_module_sensor		*m_sensor;
	slam_module_ui			*m_ui;

	/* pause */
	HANDLE event_sensor_resume;
	bool m_sensor_paused;
	double m_sensor_paused_time;

	/* Kalman filter */
	cv::ExtendedKalmanFilter EKF;
	/* state vector:
     * p(3), v(3), a(3), q(3), ω(3)
	 */

	/* map */
	slam_map map;

private:
	void init_ekf();

	bool running;
};

