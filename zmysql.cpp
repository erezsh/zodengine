#include "zmysql.h"

const string users_table = "users";
const string online_history_table = "online_history";
const string affiliate_table = "affiliate";

ZMysql::ZMysql()
{
	details_loaded = false;

#ifndef DISABLE_MYSQL
	mysql_conn = NULL;
#endif
}

ZMysql::~ZMysql()
{
	string err_msg;

	Disconnect(err_msg);
}

bool ZMysql::LoadDetails(string &error_msg, ZPSettings &psettings)
{
#ifndef DISABLE_MYSQL
	host_name = psettings.mysql_hostname;
	database_name = psettings.mysql_dbname;
	login_name = psettings.mysql_user_name;
	login_password = psettings.mysql_user_password;
	root_password = psettings.mysql_root_password;

	if(!host_name.length()) return false;
	if(!database_name.length()) return false;
	if(!login_name.length()) return false;
	if(!login_password.length()) return false;
	if(!root_password.length()) return false;

	details_loaded = true;

	return true;
#else
	error_msg = "mysql not supported with this executable, please recompile with mysql support.";
	return false;
#endif
}

bool ZMysql::LoadDetails(string &error_msg, string filename)
{
#ifndef DISABLE_MYSQL
	error_msg = "reading from file not yet programmed.";
	return false;
#else
	error_msg = "mysql not supported with this executable, please recompile with mysql support.";
	return false;
#endif
}

bool ZMysql::Connect(string &error_msg)
{
#ifndef DISABLE_MYSQL

	//disconnect if already existing
	if(mysql_conn) Disconnect(error_msg);

	//init
	mysql_conn = mysql_init(NULL);

	//it create?
	if(!mysql_conn)
	{
		error_msg = "mysql init error: " + string(mysql_error(mysql_conn));
		return false;
	}

	//connect
	if(mysql_real_connect(mysql_conn, host_name.c_str(), login_name.c_str(), login_password.c_str(), database_name.c_str(), 0, NULL, CLIENT_FOUND_ROWS))
		return true;
	else
	{
		error_msg = "mysql connect error: " + string(mysql_error(mysql_conn));
		mysql_close(mysql_conn);
		mysql_conn = NULL;
		return false;
	}

	return true;
#else
	error_msg = "mysql not supported with this executable, please recompile with mysql support.";
	return false;
#endif
}

bool ZMysql::Disconnect(string &error_msg)
{
#ifndef DISABLE_MYSQL
	if(mysql_conn)
	{
		mysql_close(mysql_conn);
		mysql_conn = NULL;
	}
	return true;
#else
	error_msg = "mysql not supported with this executable, please recompile with mysql support.";
	return false;
#endif
}

bool ZMysql::CreateUserTable(string &error_msg)
{
#ifndef DISABLE_MYSQL
	string sql_cmd;

	//connected?
	if(!mysql_conn && !Connect(error_msg)) 
		return false;

	//format command
	sql_cmd = "\
	CREATE TABLE " + users_table + " (\
	tabID int NOT NULL AUTO_INCREMENT,\
	PRIMARY KEY(tabID),\
	username varchar(256),\
	loginname varchar(256),\
	password varchar(256),\
	email varchar(256),\
	activated tinyint,\
	voting_power int,\
	wins int,\
	loses int,\
	total_games int,\
	creation_ip varchar(50),\
	last_ip varchar(50),\
	creation_time int,\
	last_time int,\
	total_time int,\
	aff_creates int,\
	aff_buys int)";

	//send command
	if(mysql_query(mysql_conn, sql_cmd.c_str()))
	{
		error_msg = "mysql create table error: " + string(mysql_error(mysql_conn));
		return false;
	}

	return true;
#else
	error_msg = "mysql not supported with this executable, please recompile with mysql support.";
	return false;
#endif
}

bool ZMysql::AddUser(string &error_msg, ZMysqlUser &nu, bool &user_added)
{
#ifndef DISABLE_MYSQL
	char sql_cmd_c[5000];

	//easier this way
	user_added = false;

	//connected?
	if(!mysql_conn && !Connect(error_msg)) 
		return false;

	//already exist?
	bool user_exists;
	if(!CheckUserExistance(error_msg, nu, user_exists))
		return false;
	else if(user_exists)
		return true;

	/*
	sql_cmd = "\
	CREATE TABLE " + users_table + " (\
	tabID int NOT NULL AUTO_INCREMENT,\
	PRIMARY KEY(tabID),\
	username varchar(256),\
	loginname varchar(256),\
	password varchar(256),\
	email varchar(256),\
	activated tinyint,\
	voting_power int,\
	wins int,\
	loses int,\
	total_games int,\
	creation_ip varchar(50),\
	last_ip varchar(50),\
	creation_time int,\
	last_time int,\
	total_time int,\
	aff_creates int,\
	aff_buys int)";
	*/

	sprintf(sql_cmd_c,"INSERT INTO %s \
		( username, loginname, password, email,\
		activated, voting_power, wins, loses,\
		total_games, creation_ip, last_ip,\
		creation_time, last_time, total_time,\
		aff_creates, aff_buys ) \
		VALUES ('%s', '%s', '%s', '%s', '%d', '%d', '%d', '%d', '%d', '%s', '%s', '%d', '%d', '%d', '%d', '%d')",
		users_table.c_str(),
		nu.username.c_str(),
		nu.loginname.c_str(),
		nu.password.c_str(),
		nu.email.c_str(),
		nu.activated,
		nu.voting_power,
		nu.wins,
		nu.loses,
		nu.total_games,
		nu.creation_ip.c_str(),
		nu.last_ip.c_str(),
		nu.creation_time,
		nu.last_time,
		nu.total_time,
		nu.aff_creates,
		nu.aff_buys);

	if(!mysql_query(mysql_conn, sql_cmd_c))
	{
		//successful
		printf("successful mysql user add:'%s'\n", nu.username.c_str());
		user_added = true;
		return true;
	}
	else
	{
		//failed
		error_msg = "mysql add user error: " + string(mysql_error(mysql_conn));
		return false;
	}
#else
	error_msg = "mysql not supported with this executable, please recompile with mysql support.";
	return false;
#endif
}

bool ZMysql::CheckUserExistance(string &error_msg, ZMysqlUser &user, bool &user_exists)
{
#ifndef DISABLE_MYSQL
	char sql_cmd_c[5000];

	user_exists = false;

	//connected?
	if(!mysql_conn && !Connect(error_msg)) 
		return false;

	//format
	sprintf(sql_cmd_c, "SELECT * FROM %s WHERE (username = '%s' OR loginname = '%s')",
		users_table.c_str(),
		user.username.c_str(),
		user.loginname.c_str());

	//check
	if(!mysql_query(mysql_conn, sql_cmd_c))
	{
		MYSQL_RES *res;

		res = mysql_store_result(mysql_conn);

		//successful query now was anything found?
		if(mysql_num_rows(res) > 0)
			user_exists = true;
		else
			user_exists = false;

		mysql_free_result(res);
		
		return true;
	}
	else
	{
		//failed
		error_msg = "mysql check user error: " + string(mysql_error(mysql_conn));
		return false;
	}

	return true;
#else
	error_msg = "mysql not supported with this executable, please recompile with mysql support.";
	return false;
#endif
}

bool ZMysql::LoginUser(string &error_msg, string loginname, string password, ZMysqlUser &user, bool &user_found)
{
#ifndef DISABLE_MYSQL
	char sql_cmd_c[5000];
	
	//connected?
	if(!mysql_conn && !Connect(error_msg)) 
		return false;

	//format
	sprintf(sql_cmd_c, "SELECT * FROM %s WHERE (loginname = '%s' AND password = '%s')",
		users_table.c_str(),
		loginname.c_str(),
		password.c_str());

	//check
	if(!mysql_query(mysql_conn, sql_cmd_c))
	{
		MYSQL_RES *res;
		MYSQL_ROW row;

		res = mysql_store_result(mysql_conn);

		//successful query now was anything found?
		if((row = mysql_fetch_row(res)))
		{
			user_found = true;

			/*
			sql_cmd = "\
				CREATE TABLE " + users_table + " (\
				tabID int NOT NULL AUTO_INCREMENT,\
				PRIMARY KEY(tabID),\
				username varchar(256),\
				loginname varchar(256),\
				password varchar(256),\
				email varchar(256),\
				activated tinyint,\
				voting_power int,\
				wins int,\
				loses int,\
				total_games int,\
				creation_ip varchar(50),\
				last_ip varchar(50),\
				creation_time int,\
				last_time int,\
				total_time int)";
			*/

			user.tabID = atoi(row[0]);
			user.username = row[1];
			user.loginname = row[2];
			user.password = row[3];
			user.email = row[4];
			user.activated = atoi(row[5]);
			user.voting_power = atoi(row[6]);
			user.wins = atoi(row[7]);
			user.loses = atoi(row[8]);
			user.total_games = atoi(row[9]);
			user.creation_ip = row[10];
			user.last_ip = row[11];
			user.creation_time = atoi(row[12]);
			user.last_time = atoi(row[13]);
			user.total_time = atoi(row[14]);
			user.aff_creates = atoi(row[15]);
			user.aff_buys = atoi(row[16]);
		}
		else
		{
			user_found = false;
			user.clear();
		}

		mysql_free_result(res);
		
		return true;
	}
	else
	{
		//failed
		error_msg = "mysql check user error: " + string(mysql_error(mysql_conn));
		return false;
	}

	return true;
#else
	error_msg = "mysql not supported with this executable, please recompile with mysql support.";
	return false;
#endif
}

bool ZMysql::UpdateUserVariable(string &error_msg, int tabID, string varname, string value)
{
#ifndef DISABLE_MYSQL
	char sql_cmd_c[5000];
	
	//connected?
	if(!mysql_conn && !Connect(error_msg)) 
		return false;

	//sprintf(sql_cmd, "UPDATE payout_entry SET was_paid='1' WHERE tabID='%d'", tabID);

	//format
	sprintf(sql_cmd_c, "UPDATE %s SET %s='%s' WHERE tabID='%d'",
		users_table.c_str(),
		varname.c_str(),
		value.c_str(),
		tabID);

	if(!mysql_query(mysql_conn, sql_cmd_c))
	{
		//successful
		return true;
	}
	else
	{
		//failed
		error_msg = "mysql add user error: " + string(mysql_error(mysql_conn));
		return false;
	}

	return true;
#else
	error_msg = "mysql not supported with this executable, please recompile with mysql support.";
	return false;
#endif
}

bool ZMysql::UpdateUserVariable(string &error_msg, int tabID, string varname, int value)
{
	char conversion[100];

	sprintf(conversion, "%d", value);

	return UpdateUserVariable(error_msg, tabID, varname, conversion);
}

bool ZMysql::IncreaseUserVariable(string &error_msg, int tabID, string varname, int amount)
{
#ifndef DISABLE_MYSQL
	char sql_cmd_c[5000];
	
	//connected?
	if(!mysql_conn && !Connect(error_msg)) 
		return false;

	//sprintf(sql_cmd, "UPDATE payout_entry SET was_paid='1' WHERE tabID='%d'", tabID);

	//format
	sprintf(sql_cmd_c, "UPDATE %s SET %s=%s+%d WHERE tabID='%d'",
		users_table.c_str(),
		varname.c_str(),
		varname.c_str(),
		amount,
		tabID);

	if(!mysql_query(mysql_conn, sql_cmd_c))
	{
		//successful
		return true;
	}
	else
	{
		//failed
		error_msg = "mysql add user error: " + string(mysql_error(mysql_conn));
		return false;
	}

	return true;
#else
	error_msg = "mysql not supported with this executable, please recompile with mysql support.";
	return false;
#endif
}

bool ZMysql::IPInUserTable(string &error_msg, string ip, int &users_found)
{
#ifndef DISABLE_MYSQL
	char sql_cmd_c[5000];

	users_found = 0;

	//connected?
	if(!mysql_conn && !Connect(error_msg)) 
		return false;

	//format
	sprintf(sql_cmd_c, "SELECT * FROM %s WHERE (creation_ip = '%s' OR last_ip = '%s')",
		users_table.c_str(),
		ip.c_str(),
		ip.c_str());

	//check
	if(!mysql_query(mysql_conn, sql_cmd_c))
	{
		MYSQL_RES *res;

		res = mysql_store_result(mysql_conn);

		//successful query now was anything found?
		users_found = mysql_num_rows(res);

		mysql_free_result(res);
		
		return true;
	}
	else
	{
		//failed
		error_msg = "mysql ip found error: " + string(mysql_error(mysql_conn));
		return false;
	}

	return true;
#else
	error_msg = "mysql not supported with this executable, please recompile with mysql support.";
	return false;
#endif
}

bool ZMysql::CreateOnlineHistoryTable(string &error_msg)
{
#ifndef DISABLE_MYSQL
	string sql_cmd;

	//connected?
	if(!mysql_conn && !Connect(error_msg)) 
		return false;

	//format command
	sql_cmd = "\
	CREATE TABLE " + online_history_table + " (\
	tabID int NOT NULL AUTO_INCREMENT,\
	PRIMARY KEY(tabID),\
	time_stamp int,\
	player_count int,\
	tray_player_count int)";

	//send command
	if(mysql_query(mysql_conn, sql_cmd.c_str()))
	{
		error_msg = "mysql create table error: " + string(mysql_error(mysql_conn));
		return false;
	}

	return true;
#else
	error_msg = "mysql not supported with this executable, please recompile with mysql support.";
	return false;
#endif
}

bool ZMysql::InsertOnlineHistoryEntry(string &error_msg, int time_stamp, int player_count, int tray_player_count)
{
#ifndef DISABLE_MYSQL
	char sql_cmd_c[5000];

	//connected?
	if(!mysql_conn && !Connect(error_msg)) 
		return false;

	//insert
	{
		sprintf(sql_cmd_c,"INSERT INTO %s ( time_stamp, player_count, tray_player_count ) VALUES ('%d', '%d', '%d')",
			online_history_table.c_str(),
			time_stamp,
			player_count,
			tray_player_count);

		if(!mysql_query(mysql_conn, sql_cmd_c))
		{
			//successful
		}
		else
		{
			//failed
			error_msg = "mysql insert online history error: " + string(mysql_error(mysql_conn));
			return false;
		}
	}

	//reduce size
	{
		const int max_entries = 24 * 4;
		//format
		sprintf(sql_cmd_c, "SELECT * FROM %s ORDER BY tabID ASC", online_history_table.c_str());

		//check
		if(!mysql_query(mysql_conn, sql_cmd_c))
		{
			MYSQL_RES *res;
			MYSQL_ROW row;
			int row_count;

			res = mysql_store_result(mysql_conn);

			row_count = mysql_num_rows(res);
			for(int i=0; i < row_count - max_entries; i++)
			{
				row = mysql_fetch_row(res);

				//shouldn't happen
				if(!row) break;

				sprintf(sql_cmd_c,"DELETE FROM %s WHERE tabID='%s'",
					online_history_table.c_str(),
					row[0]);

				if(!mysql_query(mysql_conn, sql_cmd_c))
				{
					//successful
				}
				else
				{
					mysql_free_result(res);

					//failed
					error_msg = "mysql insert online history error: " + string(mysql_error(mysql_conn));
					return false;
				}
			}

			mysql_free_result(res);
		}
		else
		{
			//failed
			error_msg = "mysql insert online history error: " + string(mysql_error(mysql_conn));
			return false;
		}

	}

	return true;
#else
	error_msg = "mysql not supported with this executable, please recompile with mysql support.";
	return false;
#endif
}

bool ZMysql::CreateAffiliateTable(string &error_msg)
{
#ifndef DISABLE_MYSQL
	string sql_cmd;

	//connected?
	if(!mysql_conn && !Connect(error_msg)) 
		return false;

	//format command
	sql_cmd = "\
	CREATE TABLE " + affiliate_table + " (\
	tabID int NOT NULL AUTO_INCREMENT,\
	PRIMARY KEY(tabID),\
	ip varchar(50),\
	aff_id int)";

	//send command
	if(mysql_query(mysql_conn, sql_cmd.c_str()))
	{
		error_msg = "mysql create table error: " + string(mysql_error(mysql_conn));
		return false;
	}

	return true;
#else
	error_msg = "mysql not supported with this executable, please recompile with mysql support.";
	return false;
#endif
}

bool ZMysql::GetAffiliateId(string &error_msg, string ip, int &aff_id, bool &ip_found)
{
#ifndef DISABLE_MYSQL
	char sql_cmd_c[5000];

	ip_found = false;
	aff_id = -1;
	
	//connected?
	if(!mysql_conn && !Connect(error_msg)) 
		return false;

	//format
	sprintf(sql_cmd_c, "SELECT * FROM %s WHERE ip = '%s'",
		affiliate_table.c_str(),
		ip.c_str());

	//check
	if(!mysql_query(mysql_conn, sql_cmd_c))
	{
		MYSQL_RES *res;
		MYSQL_ROW row;

		res = mysql_store_result(mysql_conn);

		//successful query now was anything found?
		if((row = mysql_fetch_row(res)))
		{
			/*
			sql_cmd = "\
			CREATE TABLE " + affiliate_table + " (\
			tabID int NOT NULL AUTO_INCREMENT,\
			PRIMARY KEY(tabID),\
			ip varchar(50),\
			aff_id int)";
			*/

			ip_found = true;
			aff_id = atoi(row[2]);
		}

		mysql_free_result(res);
		
		return true;
	}
	else
	{
		//failed
		error_msg = "mysql get affiliate id error: " + string(mysql_error(mysql_conn));
		return false;
	}

	return true;
#else
	error_msg = "mysql not supported with this executable, please recompile with mysql support.";
	return false;
#endif
}
