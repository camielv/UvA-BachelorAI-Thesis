/********************************************************************
 *                    COPYRIGHT PARROT 2010
 ********************************************************************
 *       PARROT - A.R.Drone SDK Windows Client Example
 *-----------------------------------------------------------------*/
/**
 * @file custom_code.c 
 * @brief User added code
 *
 * @author sylvain.gaeremynck@parrot.com
 * @date 2009/07/01
 *
 * @author Stephane Piskorski <stephane.piskorski.ext@parrot.fr>
 * @date   Sept, 8. 2010
 *
 *******************************************************************/



#include <custom_code.h>

//ARDroneLib
	#include <ardrone_tool/ardrone_time.h>
	#include <ardrone_tool/Navdata/ardrone_navdata_client.h>
	#include <ardrone_tool/Control/ardrone_control.h>
	#include <ardrone_tool/UI/ardrone_input.h>

//Common
	#include <config.h>
	#include <ardrone_api.h>

//VP_SDK
	#include <ATcodec/ATcodec_api.h>
	#include <VP_Os/vp_os_print.h>
	#include <VP_Api/vp_api_thread_helper.h>
	#include <VP_Os/vp_os_signal.h>

//Local project
	#include <Video/video_stage.h>

//Global variables

