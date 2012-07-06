#pragma once

#include <stdio.h>

#define PRINT_DEBUG false


/* SLAM */
#define SLAM_USE_QUEUE true // use a queue to store the controldata and sensor data
#define SLAM_SURF_HESSIANTHRESHOLD 50.0 // 70.0f
#define SLAM_SURF_MIN_RESPONSE 500.0 // 1000.0f
#define SLAM_ELEVATION_MAP_DEFAULT_SIZE 200 // 10m * 10m in each direction + 1 for center
#define SLAM_DESCRIPTOR_SIZE 64 * sizeof(float)
#define SLAM_WC_SIZE 3 * sizeof(float)
//#define SLAM_LOC_WRITE_STATE_DIRECTLY true // remove line if false
//#define SLAM_VISUALMOTION_WRITE_STATE_DIRECTLY true // remove line if false
#define SLAM_LOC_UPDATE_YAW false

#define SLAM_MODE_VISUALMOTION 0x01
#define SLAM_MODE_ACCEL 0x02
#define SLAM_MODE_VEL 0x04
#define SLAM_MODE_VISUALLOC 0x08
#define SLAM_MODE_MAP 0x10
#define SLAM_MODE_ELEVMAP 0x20
#define SLAM_MODE_VEL_OR 0x40


/* USARSIM */
#define USARSIM_IP "127.0.0.1"
#define USARSIM_PORT 3000
#define UPIS_PORT 5003


/* ARDRONE */
#define BOT_ARDRONBOT_EVENT_FRAME_BUFSIZE  307200 //101376		// at least: 176*144*4 (RGBA) bytes
#define USARIM_FRAME_USERAW true
#define BOT_ARDRONE_RECORD_EXT "raw" //"raw"
#define BOT_ARDRONE_BATTERYLIFE 720 // 720s, 12 minutes
#define BOT_ARDRONE_RECORD_FRAMES true
#define BOT_ARDRONE_FRAME_W /*320 */176
#define BOT_ARDRONE_FRAME_H /*240 */144
#define BOT_ARDRONE_CAM_FOV 32.0f // camera FOV / 2
#define BOT_ARDRONE_SONAR_FOV 25.0f // sonar angle FOV
#define BOT_ARDRONE_CONTROL_VZ_MAX 1000.0f // mm/s
//#define BOT_ARDRONE_DISPLAY_CAM
#define BOT_ARDRONE_CALIB_MEASUREMENTS 200
#define BOT_ARDRONE_USE_ONBOARD_OR 
//#define BOT_ARDRONE_USE_HOVER_MODE // active onboard deacceleration

	/* USARSim */
	#define BOT_ARDRONE_USARSIM_FRAME_BLOCKSIZE 4096
	#define BOT_ARDRONE_USARSIM_CONTROL_BUFSIZE 2000
	#define BOT_ARDRONE_USARSIM_FRAME_REQDELAY 100 // at least 20 ms
	#define BOT_ARDRONE_USARSIM_FRAME_MODE 1 // 1: request new frame when SLAM queue empty, 2: fixed framerate

	/* keyboard */
	#define BOT_ARDRONE_KEYBOARD_VEL 0.095f


extern bool exit_application;
extern bool stop_behavior;


/* conversion */
#define MG_TO_MM2 9.80665003f
#define MD_TO_RAD 1.745329252e-05f

/*
#ifndef DEG_TO_RAD
	#define DEG_TO_RAD 0.0174532925f
#endif
*/

#ifndef M_PI
	#define M_PI 3.14159265358979323846f
#endif