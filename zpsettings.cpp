#include "zpsettings.h"
#include "common.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace COMMON;

ZPSettings::ZPSettings()
{
	LoadDefaults();
}

void ZPSettings::LoadDefaults()
{
	loaded_from_file = false;

	ignore_activation = 1;
	require_login = 0;
	use_database = 0;
	use_mysql = 0;
	start_map_paused = 1;
	bots_start_ignored = 0;
	allow_game_speed_change = 1;
	selectable_map_list.clear();
	mysql_root_password = "password";
	mysql_user_name = "user";
	mysql_user_password = "password";
	mysql_hostname = "localhost";
	mysql_dbname = "zod_db";
}

bool ZPSettings::LoadSettings(string filename)
{
	FILE *fp;

	fp=fopen(filename.c_str(), "r");

	if(!fp)
	{
		printf("ZPSettings::LoadSettings::could not open '%s' to read settings\n", filename.c_str());
		return false;
	}

	const int buf_size = 500;
	char cur_line[buf_size];
	char variable[buf_size];
	char value[buf_size];
	bool loaded = false;

	while(fgets(cur_line , buf_size , fp))
	{
		clean_newline(cur_line, buf_size);

		if(strlen(cur_line))
		{
			int pt = 0;

			//don't read comment lines
			if(cur_line[0] == '#') continue;

			//parse this line
			split(variable, cur_line, '=', &pt, buf_size, buf_size);
			split(value, cur_line, '=', &pt, buf_size, buf_size);

			if(!strcmp(variable, "use_database"))
				use_database = atoi(value)!=0;
			else if(!strcmp(variable, "use_mysql"))
				use_mysql = atoi(value) != 0;
			else if(!strcmp(variable, "ignore_activation"))
				ignore_activation = atoi(value) != 0;
			else if(!strcmp(variable, "require_login"))
				require_login = atoi(value) != 0;
			else if(!strcmp(variable, "start_map_paused"))
				start_map_paused = atoi(value) != 0;
			else if(!strcmp(variable, "bots_start_ignored"))
				bots_start_ignored = atoi(value) != 0;
			else if(!strcmp(variable, "allow_game_speed_change"))
				allow_game_speed_change = atoi(value) != 0;
			else if(!strcmp(variable, "selectable_map_list"))
				selectable_map_list = value;
			else if(!strcmp(variable, "mysql_root_password"))
				mysql_root_password = value;
			else if(!strcmp(variable, "mysql_user_name"))
				mysql_user_name = value;
			else if(!strcmp(variable, "mysql_user_password"))
				mysql_user_password = value;
			else if(!strcmp(variable, "mysql_hostname"))
				mysql_hostname = value;
			else if(!strcmp(variable, "mysql_dbname"))
				mysql_dbname = value;

			loaded = true;
		}
	}

	fclose(fp);

	loaded_from_file = loaded;

	return loaded;
}

bool ZPSettings::SaveSettings(string filename)
{
	FILE *fp;

	fp=fopen(filename.c_str(), "w");

	if(!fp)
	{
		printf("ZPSettings::SaveSettings::could not open '%s' to save settings\n", filename.c_str());
		return false;
	}

	fprintf(fp, "ignore_activation=%d\n", ignore_activation);
	fprintf(fp, "require_login=%d\n", require_login);
	fprintf(fp, "use_database=%d\n", use_database);
	fprintf(fp, "use_mysql=%d\n", use_mysql);
	fprintf(fp, "start_map_paused=%d\n", start_map_paused);
	fprintf(fp, "bots_start_ignored=%d\n", bots_start_ignored);
	fprintf(fp, "allow_game_speed_change=%d\n", allow_game_speed_change);
	fprintf(fp, "selectable_map_list=%s\n", selectable_map_list.c_str());
	fprintf(fp, "mysql_root_password=%s\n", mysql_root_password.c_str());
	fprintf(fp, "mysql_user_name=%s\n", mysql_user_name.c_str());
	fprintf(fp, "mysql_user_password=%s\n", mysql_user_password.c_str());
	fprintf(fp, "mysql_hostname=%s\n", mysql_hostname.c_str());
	fprintf(fp, "mysql_dbname=%s\n", mysql_dbname.c_str());

	fclose(fp);

	return true;
}

