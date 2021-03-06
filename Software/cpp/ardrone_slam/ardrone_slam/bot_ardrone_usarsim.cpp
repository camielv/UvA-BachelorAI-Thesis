#include "global.h"
#include "bot_ardrone_usarsim.h"
#include "bot_ardrone.h"
#include "usarsim_msgparser.h"
#include "mysocket.h"

using namespace std;

bot_ardrone_usarsim::bot_ardrone_usarsim(bot_ardrone *bot):
	img_bgr(BOT_ARDRONE_FRAME_H, BOT_ARDRONE_FRAME_W, CV_8UC3, NULL, 0),
	img_bgra(BOT_ARDRONE_FRAME_H, BOT_ARDRONE_FRAME_W, CV_8UC4, NULL, 0)
{
	this->bot = bot;

	frame = new bot_ardrone_frame;


	/* timer */
	QueryPerformanceCounter((LARGE_INTEGER *)&time_start);
	QueryPerformanceFrequency((LARGE_INTEGER *)&time_freq);


	/* sockets */
	printf("Connecting to USARSim\n");
	control_socket = new mysocket(BOT_ARDRONE_USARSIM_SOCKET_CONTROL, USARSIM_PORT, USARSIM_IP, NULL, BOT_ARDRONE_USARSIM_CONTROL_BUFSIZE, (botinterface*) this);
	printf("Connecting to UPIS\n");
	frame_socket = new mysocket(BOT_ARDRONE_USARSIM_SOCKET_FRAME, UPIS_PORT, USARSIM_IP, frame_buffer, BOT_ARDRONE_USARSIM_FRAME_BLOCKSIZE, (botinterface*) this);
}


bot_ardrone_usarsim::~bot_ardrone_usarsim(void)
{
}


void bot_ardrone_usarsim::init(void)
{
	//control_send("INIT {ClassName USARBot.ARDrone} {Name ARDrone} {Location 0.0,0.0,1.28}\r\n");

	// doolhof
	//control_send("INIT {ClassName USARBot.ARDrone} {Name ARDrone} {Location -51.7,5.9,-4.4}\r\n");

	// doolhof: 8-experiment
	control_send("INIT {ClassName USARBot.ARDrone} {Name ARDrone} {Location -51.6,-4.2,-6.3}\r\n");

	// zebrapad
	//control_send("INIT {ClassName USARBot.ARDrone} {Name ARDrone} {Location -19.3,57.1,-1.1}\r\n");

	// 0,0,0
	//control_send("INIT {ClassName USARBot.ARDrone} {Name ARDrone} {Location 0.0,10.0,-3.0}\r\n");

	// gym
	//control_send("INIT {ClassName USARBot.ARDrone} {Name ARDrone} {Location 0.0,1.0,1.64}\r\n");

	control_send("SET {Type Viewports} {Config SingleView} {Viewport1 Camera2}\r\n");
	//control_send("SET {Type Camera} {Robot ARDrone} {Name Camera2} {Client 10.0.0.2}\r\n");
	//control_send("SET {Type Viewports} {Config QuadView} {Viewport1 Camera} {Viewport2 Camera2}\r\n");

	// at 50cm height
	char *msg = "DRIVE {AltitudeVelocity 0.0} {LinearVelocity 0.0} {LateralVelocity 0.0} {RotationalVelocity 0.0} {Normalized false}\r\n";
	control_send(msg);
}


void bot_ardrone_usarsim::control_update(void *control)
{
	bot_ardrone_control *c = (bot_ardrone_control*) control;

	char msg[200];

	sprintf_s(msg, 200, "DRIVE {AltitudeVelocity %f} {LinearVelocity %f} {LateralVelocity %f} {RotationalVelocity %f} {Normalized false}\r\n",
		c->velocity[BOT_ARDRONE_AltitudeVelocity],
		c->velocity[BOT_ARDRONE_LinearVelocity],
		c->velocity[BOT_ARDRONE_LateralVelocity],
		c->velocity[BOT_ARDRONE_RotationalVelocity]);

	control_send(msg);
}


void bot_ardrone_usarsim::control_send(char *message)
{
	control_socket->send(message);
}


void bot_ardrone_usarsim::socket_callback(int id, char *message, int bytes)
{
	switch (id)
	{
		case BOT_ARDRONE_USARSIM_SOCKET_CONTROL:
			if (bytes >= BOT_ARDRONBOT_EVENT_FRAME_BUFSIZE)
				printf("ERROR: USARSIM MEASUREMENT BUFFER FULL!\n");
			else
				process_measurement(message, bytes);
			break;

		case BOT_ARDRONE_USARSIM_SOCKET_FRAME:
			process_frame(message, bytes);
			break;
	}
}


void bot_ardrone_usarsim::process_measurement(char *message, int bytes)
{
	int sensor;
	int pos;
	int lineoffset = 0;
	bot_ardrone_measurement *m = NULL;

	message[bytes] = '\0';
	string msg(message);


	while ((pos = msg.find('\n', lineoffset)) != string::npos)
	{
		// get line
		string line(message, lineoffset, pos-lineoffset-1);
		lineoffset = pos+1;

		string type(line, 0, 3);

		// skip NFO's en RES'
		if (type == "NFO" || type == "RES")
		{
			return;
		}

		// BOT_ARDRONBOT_EVENT_MEASUREMENT_STA
		else if (type == "STA")
		{
			bot->battery = (int) (((float)usarsim_msgparser_int(&line, "{Battery") / (float)BOT_ARDRONE_BATTERYLIFE)*100.0f);
		}

		// BOT_ARDRONBOT_EVENT_MEASUREMENT_SEN
		else if (type == "SEN")
		{
			if (m == NULL)
			{
				m = new bot_ardrone_measurement;
				m->time = get_time();
				m->time_pc = GetTickCount() / 1000.0;
				//m->type = BOT_ARDRONBOT_EVENT_MEASUREMENT_SEN;
			}

			sensor = usarsim_msgparser_type(&line);

			switch (sensor)
			{
				case BOT_ARDRONE_SENSOR_GT:
				{
					//printf("%s\n", line.c_str());

					usarsim_msgparser_float3(&line, "{Location", m->gt_loc);
					m->gt_loc[0] *= 1000.0f;
					m->gt_loc[1] *= 1000.0f;
					m->gt_loc[2] *= 1000.0f;
					break;
				}

				case BOT_ARDRONE_SENSOR_IMU:
				{
					//printf("%s\n", line.c_str());

					/*
					usarsim_msgparser_float3(&line, "{Orientation", m->or);
					
					// rad to mili-degrees
					m->or[0] = usarsim_msgparser_rad_to_mildeg(m->or[0]); // x
					m->or[1] = usarsim_msgparser_rad_to_mildeg(m->or[1]); // y
					m->or[2] = usarsim_msgparser_rad_to_mildeg(m->or[2]); // z
					*/
					break;
				}

				case BOT_ARDRONE_SENSOR_SONAR:
				{
					m->altitude = (int) (usarsim_msgparser_float(&line, "Name Sonar1 Range")*1000.0f);
					break;
				}

				case BOT_ARDRONE_SENSOR_ACCEL:
					// Accelerations are received in m/s2
					// a = dv/dt = (vfinal - vinitial) / (tfinal - tinitial) 
					usarsim_msgparser_float3(&line, "{Acceleration", m->accel);
					m->accel[0] = usarsim_msgparser_ms2_to_mg(m->accel[0]); // m -> cm
					m->accel[1] = usarsim_msgparser_ms2_to_mg(m->accel[1]);
					m->accel[2] = usarsim_msgparser_ms2_to_mg(m->accel[2]);

					usarsim_msgparser_float3(&line, "{Velocity", m->vel);
					m->vel[0] *= 1000.0f; // m -> mm
					m->vel[1] *= 1000.0f;
					m->vel[2] *= 1000.0f;

					usarsim_msgparser_float3(&line, "{Orientation", m->or);
					// rad to mili-degrees
					m->or[0] = usarsim_msgparser_rad_to_mildeg(m->or[0]); // x
					m->or[1] = usarsim_msgparser_rad_to_mildeg(m->or[1]); // y
					m->or[2] = usarsim_msgparser_rad_to_mildeg(m->or[2]); // z

					break;
			}
		}
	} // get line

	if (m != NULL)
		bot->measurement_received(m);
}


void bot_ardrone_usarsim::process_frame(char *message, int bytes)
{
	// update frame size and buffer pointer
	frame->data_size += bytes;
	frame_socket->buffer += bytes;


	// we dont know image size (bytes) yet
	if (frame->dest_size == 0 && frame->data_size >= 9)
	{
		unsigned long sizebytes;
		memcpy_s(&sizebytes, 4, frame_buffer + 1, 4);
		frame->dest_size = ntohl(sizebytes);

		memcpy_s(&frame->w, 2, frame_buffer + 5, 2);
		memcpy_s(&frame->h, 2, frame_buffer + 7, 2);
		frame->w = ntohs(frame->w);
		frame->h = ntohs(frame->h);

		frame->time = bot->get_clock(); // get clock time now
	}


	// image complete
	if (frame->dest_size > 0 && frame->data_size-5 >= frame->dest_size) {
		if (USARIM_FRAME_USERAW || !frame_socket->bytes_waiting())
		{
			//frame->data_size -= 9; //5;

			if (check_frame())
			{
				img_bgr.data = (uchar*) frame_buffer + 9;
				img_bgra.data = (uchar*) frame->data;
				cvtColor(img_bgr, img_bgra, CV_BGR2BGRA, 4);
				//blur(img_bgra, img_bgra, cv::Size(3,3));
				//slam_module_frame::add_noise(img_bgra);
				frame->data_size = BOT_ARDRONE_FRAME_W * BOT_ARDRONE_FRAME_H * 4;

				bot->frame_received(frame);
			}


			frame = new bot_ardrone_frame;
			frame->time = get_time();
			frame_socket->buffer = frame_buffer;


			// slam enabled and queue not empty -> wait
			if (BOT_ARDRONE_USARSIM_FRAME_MODE == 1 && bot->slam_state)
			{
				bot->slamcontroller->queue_frame.wait_until_empty(INFINITE);
			}
			// slam not (yet) enabled, or fixed fps mode
			else if ((BOT_ARDRONE_USARSIM_FRAME_MODE == 1 && !bot->slam_state) || BOT_ARDRONE_USARSIM_FRAME_MODE == 2)
			{
				Sleep(BOT_ARDRONE_USARSIM_FRAME_REQDELAY - 30);
			}

			frame_socket->send("OK");
		}
	}
}


bool bot_ardrone_usarsim::check_frame()
{
	if (frame->w != BOT_ARDRONE_FRAME_W || frame->h != BOT_ARDRONE_FRAME_H)
	{
		printf("ERROR: received frame size does not match frame width defined in global (%i, %i)\n",
			BOT_ARDRONE_FRAME_W,
			BOT_ARDRONE_FRAME_H
		);

		return false;
	}

	return true;
}


double bot_ardrone_usarsim::get_time()
{
	__int64 cur_time;
	QueryPerformanceCounter((LARGE_INTEGER *)&cur_time);

	return (cur_time - time_start) * 1.0 / time_freq;
}