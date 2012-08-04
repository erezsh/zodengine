#ifndef _ZPSETTINGS_H_
#define _ZPSETTINGS_H_

#include <string>

using namespace std;

class ZPSettings
{
public:
	ZPSettings();

	void LoadDefaults();
	bool LoadSettings(string filename);
	bool SaveSettings(string filename);

	bool loaded_from_file;

	bool ignore_activation;
	bool require_login;
	bool use_database;
	bool use_mysql;
	bool start_map_paused;
	bool bots_start_ignored;
	bool allow_game_speed_change;
	string selectable_map_list;

	string mysql_root_password;
	string mysql_user_name;
	string mysql_user_password;
	string mysql_hostname;
	string mysql_dbname;
};

#endif
