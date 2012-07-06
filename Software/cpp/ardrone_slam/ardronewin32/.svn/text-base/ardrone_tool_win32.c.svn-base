/********************************************************************
 *                    COPYRIGHT PARROT 2010
 ********************************************************************
 *       PARROT - A.R.Drone SDK Windows Client Example
 *-----------------------------------------------------------------*/
/**
 * @file ardrone_tool_win32.c 
 * @brief Main program of all client programs using ARDroneTool.
 *
 * @author Stephane Piskorski <stephane.piskorski.ext@parrot.fr>
 * @date   Sept, 8. 2010
 * @warning  This file is a modified version of the 'ardrone_tool.c'
 *			 of the A.R. Drone SDK
 *
 *******************************************************************/

#pragma warning( disable : 4996 ) // disable deprecation warning 

#include <stdlib.h>

#include <ardrone_tool/ardrone_tool.h>
#include <ardrone_tool/ardrone_tool_configuration.h>
#include <ardrone_tool/Control/ardrone_control.h>
#include <ardrone_tool/Navdata/ardrone_navdata_client.h>
#include <ardrone_tool/Video/video_stage.h>
#include <ardrone_tool/UI/ardrone_input.h>
#include <VP_Com/vp_com.h>
#include <ardrone_tool/Com/config_com.h>

#include <ardrone_tool_win32.h>



extern HANDLE ardrone_ready;



/*---------------------------------------------------------------------------------------------------------------------
Tests the network connection to the drone by fetching the drone version number
through the FTP server embedded on the drone.
This is how FreeFlight checks if a drone sofware update is required.

The FTP connection process is a quick and (very)dirty one. It uses FTP passive mode.
---------------------------------------------------------------------------------------------------------------------*/
int test_drone_connection()
{
	vp_com_socket_t ftp_client;
	static Write ftp_write = NULL;
	static Read  ftp_read = NULL;
	int timeout_windows = 1000; /*milliseconds*/

	/* Connects to the FTP server */
	wifi_config_socket(&ftp_client,VP_COM_CLIENT,FTP_PORT,WIFI_ARDRONE_IP);
	ftp_client.protocol = VP_COM_TCP;
	if(VP_FAILED(vp_com_init(wifi_com())))
		return -1;
	if(VP_FAILED(vp_com_open(wifi_com(), &ftp_client, &ftp_read, &ftp_write)))
		return -2;
	setsockopt((int32_t)ftp_client.priv, 
								SOL_SOCKET, 
								SO_RCVTIMEO, 
								(const char*)&timeout_windows, sizeof(timeout_windows)
								);

	/* Clean up */
	vp_com_close(wifi_com(), &ftp_client);

	return 0;
}




/*---------------------------------------------------------------------------------------------------------------------
Main application function
---------------------------------------------------------------------------------------------------------------------*/

int ardronewin32()
{
	START_THREAD(ihm, 0);

	return 1;
}

DEFINE_THREAD_ROUTINE(ihm, data)
{
	C_RESULT res;

	WSADATA wsaData = {0};
	int iResult = 0;


	/* Initializes Windows socket subsystem */
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		wprintf(L"WSAStartup failed: %d\n", iResult);
		return 1;
	}

 
	/* Initializes communication sockets */	
	res = test_drone_connection(); // Nick disabled the press enter (wait)
	if(res!=0)
	{
		printf("%s","Could not detect the drone version ... press <Enter> to try connecting anyway.\n");
		getchar();
		//WSACleanup();
		exit(-1);
	}


	res = ardrone_tool_setup_com( NULL );
	if( FAILED(res) )
	{
		PRINT("Wifi initialization failed.\n");
		return -1;
	}


	START_THREAD(video_stage, 0);

	res = ardrone_tool_init(WIFI_ARDRONE_IP, strlen(WIFI_ARDRONE_IP), NULL, ARDRONE_CLIENT_APPNAME, ARDRONE_CLIENT_USRNAME);

	ardrone_tool_set_refresh_time(20); // 20 ms

	ardrone_at_reset_com_watchdog();


	// config
	ardrone_control_config.euler_angle_max = 0.20943951f; // 12 degrees
	ardrone_control_config.video_channel	= ZAP_CHANNEL_VERT;
	ardrone_control_config.video_codec		= UVLC_CODEC; //P264_CODEC;
	ardrone_control_config.navdata_demo		= FALSE;
	ardrone_control_config.altitude_max		= 10000;
	ardrone_control_config.control_vz_max	= 1000.0f;
	ardrone_control_config.outdoor			= FALSE;
	//ardrone_control_config.flight_without_shell = TRUE;


	ARDRONE_TOOL_CONFIGURATION_ADDEVENT(video_channel, &ardrone_control_config.video_channel, (ardrone_tool_configuration_callback) ardrone_demo_config_callback);
	//ARDRONE_TOOL_CONFIGURATION_ADDEVENT(video_channel, &ardrone_control_config.video_channel, NULL);
	//ARDRONE_TOOL_CONFIGURATION_ADDEVENT(video_codec, &ardrone_control_config.video_codec, NULL);
	ARDRONE_TOOL_CONFIGURATION_ADDEVENT (navdata_demo, &ardrone_control_config.navdata_demo, NULL);
	ARDRONE_TOOL_CONFIGURATION_ADDEVENT (altitude_max, &ardrone_control_config.altitude_max, NULL);
	ARDRONE_TOOL_CONFIGURATION_ADDEVENT (control_vz_max, &ardrone_control_config.control_vz_max, NULL);
	//ARDRONE_TOOL_CONFIGURATION_ADDEVENT (outdoor, &ardrone_control_config.outdoor, NULL);
	//ARDRONE_TOOL_CONFIGURATION_ADDEVENT (flight_without_shell, &ardrone_control_config.flight_without_shell, NULL);
	ARDRONE_TOOL_CONFIGURATION_ADDEVENT (euler_angle_max, &ardrone_control_config.euler_angle_max, NULL);


	// flat trim
	ardrone_at_set_flat_trim();


	//SetEvent(ardrone_ready);
	//ardrone_demo_redirect_to_interface = 1;


	while( VP_SUCCEEDED(res) && ardrone_tool_exit() == FALSE )
	{
		res = ardrone_tool_update();
	}

	JOIN_THREAD(video_stage);

	res = ardrone_tool_shutdown();

	WSACleanup();

	return (THREAD_RET)res;
}


void demo_video_client_process()
{
	video_stage_config_t *config = video_stage_get();

	if (ardrone_demo_redirect_to_interface)
		bot_ardrone_ardronelib_process_frame((unsigned char*) config->data, config->widthImage, config->heightImage);
}


void ardrone_demo_config_callback(unsigned int success)
{
	ardrone_tool_configuration_callback cb = (ardrone_tool_configuration_callback) ardrone_demo_config_callback;

	if (success && ++ardrone_nr_configs_suc <= ardrone_demo_nr_configs)
	{
		switch (ardrone_nr_configs_suc)
		{
		/*
		case 1:
			ARDRONE_TOOL_CONFIGURATION_ADDEVENT(video_codec, &ardrone_control_config.video_codec, cb);
			break;
		*/

		case 1:
			ARDRONE_TOOL_CONFIGURATION_ADDEVENT (navdata_demo, &ardrone_control_config.navdata_demo, cb);
			break;

		case 2:
			ARDRONE_TOOL_CONFIGURATION_ADDEVENT (altitude_max, &ardrone_control_config.altitude_max, cb);
			break;

		case 3:
			ARDRONE_TOOL_CONFIGURATION_ADDEVENT (control_vz_max, &ardrone_control_config.control_vz_max, cb);
			break;

		case 4:
			ARDRONE_TOOL_CONFIGURATION_ADDEVENT (outdoor, &ardrone_control_config.outdoor, cb);
			break;

		case 5:
			SetEvent(ardrone_ready);
			ardrone_demo_redirect_to_interface = 1;
			break;
		}
	}
}


void ardronewin32_take_off()
{
	ardrone_tool_set_ui_pad_start(1);
}


void ardronewin32_land()
{
	ardrone_tool_set_ui_pad_start(0);
}


void ardronewin32_progress(int enable, float roll, float pitch, float gaz, float yaw)
{
	ardrone_at_set_progress_cmd(enable, roll, pitch, gaz, yaw);
}


void ardronewin32_recover(int send)
{
	if (send == 1)
		ardrone_tool_set_ui_pad_select(1);
	else
		ardrone_tool_set_ui_pad_select(0);
}





inline C_RESULT demo_navdata_client_init( void* data )
{
  return C_OK;
}

inline C_RESULT demo_navdata_client_process( const navdata_unpacked_t* const navdata )
{
    navdata_unpacked_t *nd = (navdata_unpacked_t*)navdata;

	if (ardrone_demo_redirect_to_interface)
		bot_ardrone_ardronelib_process_navdata(nd);	

	return C_OK;
}

inline C_RESULT demo_navdata_client_release( void )
{
  return C_OK;
}



/* Implementing thread table in which you add routines of your application and those provided by the SDK */
BEGIN_THREAD_TABLE
  THREAD_TABLE_ENTRY( ihm, 20 )
  THREAD_TABLE_ENTRY( ardrone_control, 20 )
  THREAD_TABLE_ENTRY( navdata_update, 20 )
  THREAD_TABLE_ENTRY( video_stage, 20 )
END_THREAD_TABLE


/* 
Registering the navdata handling function to 'navdata client' which is part 
of the ARDroneTool.
You can add as many navdata handlers as you want.
Terminate the table with a NULL pointer.
*/
BEGIN_NAVDATA_HANDLER_TABLE
  NAVDATA_HANDLER_TABLE_ENTRY(demo_navdata_client_init, demo_navdata_client_process, demo_navdata_client_release, NULL)
END_NAVDATA_HANDLER_TABLE