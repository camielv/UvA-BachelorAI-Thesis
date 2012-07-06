#ifndef _ARDRONE_TOOL_WIN32_H_
#define _ARDRONE_TOOL_WIN32_H_

#pragma warning( disable : 4996 ) // disable deprecation warning 


#define WIFI_NETWORK_NAME "ardrone_RC130"
#define WIFI_BROADCAST "192.168.1.255"
#define WIFI_ARDRONE_IP "192.168.1.1"
#define WIFI_MOBILE_IP "192.168.1.2"
#define ARDRONE_CLIENT_APPNAME "ardrone_slam"
#define ARDRONE_CLIENT_USRNAME "Nick"


#ifdef __cplusplus

extern "C" {
	int ardronewin32();
	void ardronewin32_take_off();
	void ardronewin32_land();
	void ardronewin32_progress(int enable, float roll, float pitch, float gaz, float yaw);
	void ardronewin32_recover(int send);
}

#endif

int ardrone_demo_nr_configs = 5;
int ardrone_nr_configs_suc = 0;
int ardrone_demo_redirect_to_interface = 0;

void ardrone_demo_config_callback(unsigned int success);

extern void bot_ardrone_ardronelib_process_navdata(navdata_unpacked_t *n);

extern void bot_ardrone_ardronelib_process_frame(unsigned char* rgbtexture, int w, int h);

// Default implementation for weak functions
C_RESULT ardrone_tool_init_custom(int argc, char **argv) { return C_OK; }
C_RESULT ardrone_tool_update_custom() { return C_OK; }
C_RESULT ardrone_tool_display_custom() { return C_OK; }
C_RESULT ardrone_tool_shutdown_custom() { return C_OK; }
bool_t   ardrone_tool_exit() { return FALSE; }
C_RESULT ardrone_tool_check_argc_custom( int32_t argc) { return C_OK; }
void ardrone_tool_display_cmd_line_custom( void ) {}
bool_t ardrone_tool_parse_cmd_line_custom( const char* cmd ) { return TRUE; }

#endif