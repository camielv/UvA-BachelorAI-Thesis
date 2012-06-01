/********************************************************************
 *                    COPYRIGHT PARROT 2010
 ********************************************************************
 *       PARROT - A.R.Drone SDK Windows Client Example
 *-----------------------------------------------------------------*/
/**
 * @file navdata.c 
 * @brief Navdata handling code
 *
 * @author sylvain.gaeremynck@parrot.com
 * @date 2009/07/01
 *
 * @author Stephane Piskorski <stephane.piskorski.ext@parrot.fr>
 * @date   Sept, 8. 2010
 *
 *******************************************************************/


#include <ardrone_tool/Navdata/ardrone_navdata_client.h>

#include <ardrone_tool_win32.h>

#include <custom_code.h>


inline C_RESULT demo_navdata_client_init( void* data )
{
  return C_OK;
}


inline C_RESULT demo_navdata_client_process( const navdata_unpacked_t* const navdata )
{
    navdata_unpacked_t *nd = (navdata_unpacked_t*)navdata;

	//printf("schedule: %i\n", (nd->ardrone_state & ARDRONE_USER_FEEDBACK_START));

	bot_ardrone_ardronelib_process_navdata(nd);	

	return C_OK;
}


inline C_RESULT demo_navdata_client_release( void )
{
  return C_OK;
}


BEGIN_NAVDATA_HANDLER_TABLE
  NAVDATA_HANDLER_TABLE_ENTRY(demo_navdata_client_init, demo_navdata_client_process, demo_navdata_client_release, NULL)
END_NAVDATA_HANDLER_TABLE