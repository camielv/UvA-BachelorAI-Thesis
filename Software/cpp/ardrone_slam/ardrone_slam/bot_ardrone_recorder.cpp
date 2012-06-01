#include "global.h"
#include "bot_ardrone_recorder.h"
#include "bot_ardrone.h"
#include <io.h>
#include <yaml.h>

#include <cv.hpp>
#include <cxcore.hpp>
#include <opencv2/highgui/highgui.hpp>


HANDLE bot_ardrone_recorder::hMutex;


bot_ardrone_recorder::bot_ardrone_recorder(bot_ardrone *bot)
{
	this->bot = bot;

	frame_counter = 1;
}


bot_ardrone_recorder::~bot_ardrone_recorder(void)
{
	// todo
}


void bot_ardrone_recorder::record_measurement(bot_ardrone_measurement *m)
{
	WaitForSingleObject(hMutex, INFINITE);

	fprintf (file, "---\n");
	fprintf (file, "e: %i\n", BOT_EVENT_MEASUREMENT);
	fprintf (file, "t: %f\n", m->time);
	fprintf (file, "t_pc: %f\n", m->time_pc);
	fprintf (file, "s: %i\n", m->state);
	fprintf (file, "alt: %i\n", m->altitude);

	fprintf (file, "or: [%f, %f, %f]\n", m->or[0], m->or[1], m->or[2]);
	fprintf (file, "accel: [%f, %f, %f]\n", m->accel[0], m->accel[1], m->accel[2]);
	fprintf (file, "vel: [%f, %f, %f]\n", m->vel[0], m->vel[1], m->vel[2]);

	fprintf (file, "gt_loc: [%f, %f, %f]\n", m->gt_loc[0], m->gt_loc[1], m->gt_loc[2]);


	/*
	fprintf (file, "navdata_euler_angles: [%f, %f, %f]\n", m->navdata_euler_angles[0], m->navdata_euler_angles[1], m->navdata_euler_angles[2]);
	fprintf (file, "gyros_offsets: [%f, %f, %f]\n", m->gyros_offsets[0], m->gyros_offsets[1], m->gyros_offsets[2]);
	fprintf (file, "phys_gyro_temp: [%f, %f, %f]\n", m->phys_gyro_temp[0], m->phys_gyro_temp[1], m->phys_gyro_temp[2]);
	fprintf (file, "phys_gyros: [%f, %f, %f]\n", m->phys_gyros[0], m->phys_gyros[1], m->phys_gyros[2]);
	fprintf (file, "raw_gyros: [%f, %f, %f]\n", m->raw_gyros[0], m->raw_gyros[1], m->raw_gyros[2]);
	fprintf (file, "raw_gyros_110: [%f, %f, %f]\n", m->raw_gyros_110[0], m->raw_gyros_110[1], m->raw_gyros_110[2]);
	*/


	ReleaseMutex(hMutex);
}


void bot_ardrone_recorder::record_control(bot_ardrone_control *c)
{
	WaitForSingleObject(hMutex, INFINITE);

	fprintf (file, "---\n");
	fprintf (file, "e: %i\n", BOT_EVENT_CONTROL);
	fprintf (file, "t: %f\n", c->time);
	fprintf (file, "vel: [%f, %f, %f, %f]\n", c->velocity[0], c->velocity[1], c->velocity[2], c->velocity[3]);

	ReleaseMutex(hMutex);
}


void bot_ardrone_recorder::record_frame(bot_ardrone_frame *f)
{
	WaitForSingleObject(hMutex, INFINITE);

	char filename[30];

	sprintf_s(f->filename, 20, "%06d.%s", frame_counter++, BOT_ARDRONE_RECORD_EXT);
	sprintf_s(filename, 30, "%s/%s", dataset_dir, f->filename);

	fprintf (file, "---\n");
	fprintf (file, "e: %i\n", BOT_EVENT_FRAME);
	fprintf (file, "t: %f\n", f->time);
	fprintf (file, "s: %i\n", f->data_size);
	fprintf (file, "f: %s\n", f->filename);

	// PNG
	if (BOT_ARDRONE_RECORD_EXT == "png")
	{
		Mat frame = Mat(BOT_ARDRONE_FRAME_H, BOT_ARDRONE_FRAME_W, CV_8UC4, NULL, 0);
		frame.data = (uchar*) &f->data[4];
		imwrite(filename, frame);
	}
	// RAW
	else
	{
		FILE *frame_out;
		frame_out = fopen(filename, "wb");
		fwrite(f->data, 1, f->data_size, frame_out);
		fclose(frame_out);
	}

	ReleaseMutex(hMutex);
}


void yaml_float_seq(yaml_document_t &document, yaml_node_t *node, float *f)
{
	yaml_node_item_t *item;
	yaml_node_t* value;

	int i = 0;

	for (item = node->data.sequence.items.start; item < node->data.sequence.items.top; item ++)
	{
		value	= yaml_document_get_node(&document, *item);
		f[i++]	= (float) atof((char*) value->data.scalar.value);
	}
}


void bot_ardrone_recorder::playback(char *dataset)
{
	bool map_saved = false;
	char filename[40];
	last_event_time = DBL_MAX; // play first event without delay

	sprintf_s(dataset_dir, 40, "dataset/%s", dataset);
	sprintf_s(filename, 40, "%s/output.yaml", dataset_dir);

	int event_type = -1;

	file = fopen (filename , "r");

	yaml_parser_t parser;
	yaml_document_t document;
    yaml_node_t *node;
    yaml_node_pair_t *pair;
	yaml_node_t* key;
	yaml_node_t* value;
	char* key_s;
	char* value_s;

	//
	bot_ardrone_measurement *m;
	bot_ardrone_frame *f;
	bot_ardrone_control *c;
	//

    /* Clear the objects. */
	memset(&parser, 0, sizeof(parser));

	yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, file);


	int done = 0;

	while (!done)
    {
		yaml_parser_load(&parser, &document);

		if (!yaml_document_get_root_node(&document))
			break;

		node = document.nodes.start;

		assert(node->type == YAML_MAPPING_NODE); // first node should be mapping node!

		for (pair = node->data.mapping.pairs.start; pair < node->data.mapping.pairs.top; pair ++) {
			key		= yaml_document_get_node(&document, pair->key);
			value	= yaml_document_get_node(&document, pair->value);
			key_s	= (char*) key->data.scalar.value;
			value_s	= (char*) value->data.scalar.value;

			if (strcmp(key_s, "e") == 0)
			{
				event_type = atoi((char*) value->data.scalar.value);

				switch (event_type)
				{
					case BOT_EVENT_MEASUREMENT:
						m = new bot_ardrone_measurement;
						break;

					case BOT_EVENT_FRAME:
						f = new bot_ardrone_frame;
						break;

					case BOT_EVENT_CONTROL:
						c = &bot->control;
						break;
				}

				continue;
			}


			switch (event_type)
			{
				case BOT_EVENT_MEASUREMENT:
					if (strcmp(key_s, "t") == 0)
					{
						m->time = atof(value_s);
						wait_for_event(m->time);
						//printf("time: %f\n", m->time);
					}
					if (strcmp(key_s, "t_pc") == 0)
					{
						m->time_pc = atof(value_s);

						/*
						if (m->time_pc >= 8311.0 && m->time_pc <= 8312.0)
						{
							bot->get_slam()->on(SLAM_MODE_VISUALLOC);
							//map_saved = true;
						}
						
						if (m->time_pc >= 8323.0 && !map_saved)
						{
							//bot->get_slam()->map.visual_map.save_canvas();
							bot->get_slam()->off(SLAM_MODE_MAP);
							map_saved = true;
						}
						*/

						//printf("time: %f\n", m->time);
					}
					else if (strcmp(key_s, "s") == 0)
					{
						m->state = atoi(value_s);
					}
					else if (strcmp(key_s, "alt") == 0)
					{
						m->altitude = atoi(value_s);
					}
					else if (strcmp(key_s, "or") == 0)
					{
						yaml_float_seq(document, value, m->or);
					}
					else if (strcmp(key_s, "accel") == 0)
					{
						yaml_float_seq(document, value, m->accel);
					}
					else if (strcmp(key_s, "vel") == 0)
					{
						yaml_float_seq(document, value, m->vel);
					}
					else if (strcmp(key_s, "gt_loc") == 0)
					{
						yaml_float_seq(document, value, m->gt_loc);
					}

					break;


				case BOT_EVENT_FRAME:
					if (strcmp(key_s, "t") == 0)
					{
						f->time = atof(value_s);
						wait_for_event(f->time);
					}
					else if (strcmp(key_s, "s") == 0)
					{
						f->data_size = atoi(value_s);
					}
					else if (strcmp(key_s, "f") == 0)
					{
						char filename[40];
						sprintf_s(filename, 40, "%s/%s", dataset_dir, value_s);

						FILE *frame_in;
						frame_in = fopen(filename, "rb");

						// for old dataset only
						//fseek (frame_in, 4, SEEK_SET);
						//fread(f->data, 1, f->data_size - 4, frame_in);

						fread(f->data, 1, f->data_size, frame_in);
						fclose(frame_in);

						//Mat img_bgra(144, 176, CV_8UC4);
						//img_bgra.data		= (unsigned char*) f->data;

						//Mat img_resized(BOT_ARDRONE_FRAME_H, BOT_ARDRONE_FRAME_W, CV_8UC4);

						//resize(img_bgra, img_resized, Size(BOT_ARDRONE_FRAME_W, BOT_ARDRONE_FRAME_H));

						//memcpy_s(f->data, BOT_ARDRONBOT_EVENT_FRAME_BUFSIZE, img_resized.data, BOT_ARDRONE_FRAME_W * BOT_ARDRONE_FRAME_H * 4);

						//Mat img_bgr(BOT_ARDRONE_FRAME_H, BOT_ARDRONE_FRAME_W, CV_8UC3, NULL, 0);
						//Mat img_bgr(BOT_ARDRONE_FRAME_H, BOT_ARDRONE_FRAME_W, CV_8UC4, NULL, 0);
						//Mat img_bgra(BOT_ARDRONE_FRAME_H, BOT_ARDRONE_FRAME_W, CV_8UC4);

						//Mat img_bgra(BOT_ARDRONE_FRAME_H, BOT_ARDRONE_FRAME_W, CV_8UC4, NULL, 0);

						//img_bgr.data		= (unsigned char*) f->data;
						//cvtColor(img_bgr, img_bgra, CV_BGR2BGRA, 4);

						//memcpy_s(f->data, BOT_ARDRONBOT_EVENT_FRAME_BUFSIZE, img_bgra.data, img_bgra.rows * img_bgra.cols * 4);
						
						//imshow("Image:", img_bgra);
						//cvWaitKey(4);
						
					}

					break;


				case BOT_EVENT_CONTROL:
					if (strcmp(key_s, "t") == 0)
					{
						c->time = atof(value_s);
						//wait_for_event(c->time);
					}
					else if (strcmp(key_s, "vel") == 0)
					{
						yaml_float_seq(document, value, c->velocity);
					}
			}
		}



		/* done with YAML document */
		switch (event_type)
		{
			case BOT_EVENT_MEASUREMENT:
				bot->measurement_received(m);
				break;

			case BOT_EVENT_FRAME:
				bot->frame_received(f);
				/*
				if (f->time > 8.5f)
				{
					printf("FRAME DONE!\n");
					Sleep(500);
				}
				*/
				break;

			case BOT_EVENT_CONTROL:
				bot->control_update(c);
				break;
		}


		yaml_document_delete(&document);
		event_type = -1;
    }

    yaml_parser_delete(&parser);

	//Sleep(999999); // keep UI open
}


void bot_ardrone_recorder::prepare_dataset()
{
	// file already open check?

	int i = 1;
	char filename[25];

	sprintf_s(filename, 25, "dataset/%03d", i);

	while ((_access(filename, 0)) == 0)
	{
		sprintf_s(filename, 25, "dataset/%03d", ++i);
	}

	// dir
	sprintf_s(dataset_dir, 25, "%s", filename);
	CreateDirectory(dataset_dir, NULL);

	// filename
	sprintf_s(filename, 25, "%s/output.yaml", dataset_dir);

	// file
	//fout.open(filename, ios::out); // SLOW :(
	fopen_s (&file, filename , "w");

	printf("Created dataset %03d\n", i);


	// mutex
	hMutex = CreateMutex( 
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL); 

    if (hMutex == NULL) 
        printf("CreateMutex error: %d\n", GetLastError());
}


void bot_ardrone_recorder::wait_for_event(double time)
{
	int wait = int((time - last_event_time) * 1000.0);
	last_event_time = time;

	if (wait > 0)
		Sleep(wait * 1);
}