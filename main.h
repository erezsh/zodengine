#ifndef _MAIN_H_
#define _MAIN_H_

#include "constants.h"
#include <string>

using namespace std;

class input_options
{
	public:
		bool read_connect_address;
		bool read_map_name;
		bool read_map_list;
		bool read_is_windowed;
		bool read_resolution;
		bool read_is_dedicated;
		bool read_display_help;
		bool read_display_version;
		bool read_player_name;
		bool read_player_team;
		bool read_sound_off;
		bool read_music_off;
		bool read_disable_zcursor;
		bool read_settings;
		bool read_p_settings;
		bool read_opengl_off;
		bool read_start_bot[MAX_TEAM_TYPES];
		bool read_run_tray;
		bool read_loginname;
		bool read_password;
		
		string connect_address;
		string map_name;
		string map_list;
		string resolution;
		string player_name;
		string player_team_str;
		string settings_filename;
		string p_settings_filename;
		string loginname;
		string password;
		int team;
		int resolution_width;
		int resolution_height;
		
		input_options()
		{
			read_connect_address = false;
			read_map_name = false;
			read_map_list = false;
			read_is_windowed = false;
			read_resolution = false;
			read_is_dedicated = false;
			read_display_help = false;
			read_display_version = false;
			read_sound_off = false;
			read_music_off = false;
			read_settings = false;
			read_p_settings = false;
			read_opengl_off = false;
			read_run_tray = false;
			read_loginname = false;
			read_password = false;
			team = 0;

			for(int i=0;i<MAX_TEAM_TYPES;i++)
				read_start_bot[i] = false;
		}
		
		int getoptions(int argc, char **argv);
		int checkoptions();
		void setdefaults();
};

#endif

