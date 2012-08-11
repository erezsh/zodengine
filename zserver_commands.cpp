#include "zserver.h"

using namespace COMMON;

void ParseCommandContents(string contents, string *output, int max_values)
{
	unsigned int pos = 0;
	unsigned int last_pos = 0;

	printf("contents:'%s'\n", contents.c_str());

	//clear
	for(int i=0; i<max_values; i++) output[i].clear();

	//pack
	for(int i=0; i<max_values; i++)
	{
		pos = contents.find(',', last_pos);

		if(pos == string::npos)
			output[i] = contents.substr(last_pos);
		else
			output[i] = contents.substr(last_pos, pos - last_pos);

		//remove all initial spaces
		{
			unsigned int cpos;

			cpos = output[i].find_first_not_of(' ');

			if(cpos == string::npos)
				output[i].clear();
			else if(cpos != 0)
				output[i] = output[i].substr(cpos);
		}

		printf("output[%d]:'%s' pos:%d\n", i, output[i].c_str(), pos);

		last_pos = pos;
		last_pos++;

		if(pos == string::npos) break;
	}
}

void ZServer::ProcessPlayerCommand(int player, string full_command)
{
	string command, contents;
	unsigned int pos;

	pos = full_command.find(' ');
	command = full_command.substr(0, pos);
	if(pos != string::npos) contents = full_command.substr(pos+1);

	if(command == "help")
		PlayerCommand_Help(player, contents);
	else if(command == "listcommands")
		PlayerCommand_ListCommands(player, contents);
	else if(command == "login")
		PlayerCommand_Login(player, contents);
	else if(command == "logout")
		PlayerCommand_Logout(player, contents);
	else if(command == "createuser")
		PlayerCommand_CreateUser(player, contents);
	else if(command == "pause")
		PlayerCommand_PauseGame(player, contents);
	else if(command == "resume")
		PlayerCommand_ResumeGame(player, contents);
	else if(command == "listmaps")
		PlayerCommand_ListMaps(player, contents);
	else if(command == "changemap")
		PlayerCommand_ChangeMap(player, contents);
	else if(command == "startbot")
		PlayerCommand_StartBot(player, contents);
	else if(command == "stopbot")
		PlayerCommand_StopBot(player, contents);
	else if(command == "playerinfo")
		PlayerCommand_PlayerInfo(player, contents);
	else if(command == "currentmap")
		PlayerCommand_CurrentMap(player, contents);
	else if(command == "resetgame")
		PlayerCommand_ResetGame(player, contents);
	else if(command == "changeteam")
		PlayerCommand_ChangeTeam(player, contents);
	else if(command == "reshuffleteams")
		PlayerCommand_ReshuffleTeams(player, contents);
	else if(command == "buyregistration")
		PlayerCommand_BuyRegistration(player, contents);
	else if(command == "changespeed")
		PlayerCommand_ChangeSpeed(player, contents);
	else if(command == "version")
		PlayerCommand_Version(player, contents);
	else
		PlayerCommand_NotFound(player, contents);

	printf("processing full_command:'%s' command:'%s'\n", full_command.c_str(), command.c_str());
}

void ZServer::PlayerCommand_NotFound(int player, string contents)
{
	SendNews(player, "command not found, please type /help or /listcommands", 0, 0, 0);
}

void ZServer::PlayerCommand_Help(int player, string contents)
{
	if(!contents.length() || contents == "help")
	{
		SendNews(player, "help usage: /help command", 0, 0, 0);
		SendNews(player, "help purpose: explain how to use a command and what it is used for", 0, 0, 0);
		PlayerCommand_ListCommands(player, contents);
	}
	else if(contents == "listcommands")
	{
		SendNews(player, "listcommands usage: /listcommands", 0, 0, 0);
		SendNews(player, "listcommands purpose: list all of the available commands", 0, 0, 0);
	}
	else if(contents == "login")
	{
		SendNews(player, "login usage: /login username, password", 0, 0, 0);
		SendNews(player, "login purpose: log into your username", 0, 0, 0);
	}
	else if(contents == "logout")
	{
		SendNews(player, "logout usage: /logout", 0, 0, 0);
		SendNews(player, "logout purpose: log out of your username", 0, 0, 0);
	}
	else if(contents == "createuser")
	{
		SendNews(player, "createuser usage: /createuser username, loginname, password, email", 0, 0, 0);
		SendNews(player, "createuser purpose: create a new user", 0, 0, 0);
	}
	else if(contents == "pause")
	{
		SendNews(player, "pause usage: /pause", 0, 0, 0);
		SendNews(player, "pause purpose: pauses the game", 0, 0, 0);
	}
	else if(contents == "resume")
	{
		SendNews(player, "resume usage: /resume", 0, 0, 0);
		SendNews(player, "resume purpose: resumes the game", 0, 0, 0);
	}
	else if(contents == "listmaps")
	{
		SendNews(player, "listmaps usage: /listmaps", 0, 0, 0);
		SendNews(player, "listmaps purpose: lists available maps to be used with /changemap", 0, 0, 0);
	}
	else if(contents == "changemap")
	{
		SendNews(player, "changemap usage: /changemap map_number", 0, 0, 0);
		SendNews(player, "changemap purpose: reset game to desired map, use /listmaps to get the map_number", 0, 0, 0);
	}
	else if(contents == "startbot")
	{
		SendNews(player, "startbot usage: /startbot team_color", 0, 0, 0);
		SendNews(player, "startbot purpose: start a bot", 0, 0, 0);
	}
	else if(contents == "stopbot")
	{
		SendNews(player, "stopbot usage: /stopbot team_color", 0, 0, 0);
		SendNews(player, "stopbot purpose: stop a bot", 0, 0, 0);
	}
	else if(contents == "playerinfo")
	{
		SendNews(player, "playerinfo usage: /playerinfo", 0, 0, 0);
		SendNews(player, "playerinfo purpose: gives details on your logged in user", 0, 0, 0);
	}
	else if(contents == "currentmap")
	{
		SendNews(player, "currentmap usage: /currentmap", 0, 0, 0);
		SendNews(player, "currentmap purpose: gives the name of the current map", 0, 0, 0);
	}
	else if(contents == "resetgame")
	{
		SendNews(player, "resetgame usage: /resetgame", 0, 0, 0);
		SendNews(player, "resetgame purpose: resets the current game", 0, 0, 0);
	}
	else if(contents == "changeteam")
	{
		SendNews(player, "changeteam usage: /changeteam team_color", 0, 0, 0);
		SendNews(player, "changeteam purpose: change your team", 0, 0, 0);
	}
	else if(contents == "reshuffleteams")
	{
		SendNews(player, "reshuffleteams usage: /reshuffleteams", 0, 0, 0);
		SendNews(player, "reshuffleteams purpose: randomly places players on new teams and preserves balance", 0, 0, 0);
	}
	else if(contents == "buyregistration")
	{
		SendNews(player, "buyregistration usage: /buyregistration", 0, 0, 0);
		SendNews(player, "buyregistration purpose: downloads an offline registration key from the server for a cost in voting power", 0, 0, 0);
	}
	else if(contents == "changespeed")
	{
		SendNews(player, "changespeed usage: /changespeed multiplier_number", 0, 0, 0);
		SendNews(player, "changespeed purpose: changes the game speed. half speed is 50, double speed is 200", 0, 0, 0);
	}
	else if(contents == "version")
	{
		SendNews(player, "version usage: /version", 0, 0, 0);
		SendNews(player, "version purpose: returns the version of the server", 0, 0, 0);
	}
}

void ZServer::PlayerCommand_ListCommands(int player, string contents)
{
	SendNews(player, "command list: help, listcommands, login, logout, createuser, pause, resume, listmaps, changemap, startbot, stopbot", 0, 0, 0);
	SendNews(player, "command list: playerinfo, currentmap, resetgame, changeteam, reshuffleteams, buyregistration, changespeed, version", 0, 0, 0);
}

void ZServer::PlayerCommand_Login(int player, string contents)
{
	const int max_values = 2;
	string values[max_values];

	ParseCommandContents(contents, values, max_values);

	for(int i=0;i<max_values;i++)
		if(!values[i].length())
		{
			SendNews(player, "command error: invalid input(s)", 0, 0, 0);
			return;
		}

	AttemptPlayerLogin(player, values[0], values[1]);
}

void ZServer::PlayerCommand_Logout(int player, string contents)
{
	LogoutPlayer(player);
}

void ZServer::PlayerCommand_CreateUser(int player, string contents)
{
	const int max_values = 4;
	string values[max_values];

	ParseCommandContents(contents, values, max_values);

	for(int i=0;i<max_values;i++)
		if(!values[i].length())
		{
			SendNews(player, "command error: invalid input(s)", 0, 0, 0);
			return;
		}

	AttemptCreateUser(player, values[0], values[1], values[2], values[3]);
	return;

	///createuser username, loginname, password, email

	if(!psettings.use_database)
	{
		SendNews(player, "create user error: no database used", 0, 0, 0);
		return;
	}

	//player already logged in?
	if(player_info[player].logged_in)
	{
		SendNews(player, "create user error: please log off first", 0, 0, 0);
		return;
	}

	if(psettings.use_mysql)
	{
		ZMysqlUser user;
		string err_msg;

		user.username = values[0];
		user.loginname = values[1];
		user.password = values[2];
		user.email = values[3];

		user.creation_time = (int)time(0);
		user.creation_ip = player_info[player].ip;

		if(!user.format_okay())
		{
			char message[500];

			sprintf(message, "create user error: only alphanumeric characters and entries under %d characters long allowed", MAX_PLAYER_NAME_SIZE);
			SendNews(player, message, 0, 0, 0);

			return;
		}

		bool user_exists;
		if(!zmysql.CheckUserExistance(err_msg, user, user_exists))
		{
			printf("mysql error: %s\n", err_msg.c_str());

			SendNews(player, "create user error: error with the user database", 0, 0, 0);
		}
		else
		{
			if(user_exists)
			{
				SendNews(player, "create user error: user already exists", 0, 0, 0);
			}
			else
			{
				bool user_added;

				if(!zmysql.AddUser(err_msg, user, user_added))
				{
					printf("mysql error: %s\n", err_msg.c_str());

					SendNews(player, "create user error: error with the user database", 0, 0, 0);
				}
				else
				{
					if(user_added)
					{
						AwardAffiliateCreation(user.creation_ip);

						SendNews(player, string("user " + user.username + " created").c_str(), 0, 0, 0);
					}
					else
						SendNews(player, "create user error: unknown error", 0, 0, 0);
				}
			}
		}
	}
}

void ZServer::PlayerCommand_PauseGame(int player, string contents)
{
	if(ztime.IsPaused()) return;

	StartVote(PAUSE_VOTE, -1, player);

	//PauseGame();
}

void ZServer::PlayerCommand_ResumeGame(int player, string contents)
{
	if(!ztime.IsPaused()) return;

	StartVote(RESUME_VOTE, -1, player);

	//ResumeGame();
}

void ZServer::PlayerCommand_ListMaps(int player, string contents)
{
	int i;

	for(i=0; i<(int)selectable_map_list.size();)
	{
		string send_str;

		for(int j=0;j<4 && i<(int)selectable_map_list.size();j++,i++)
		{
			char num_c[50];

			sprintf(num_c, "%d. ", i);

			if(send_str.length())
				send_str += ", ";

			send_str += num_c + selectable_map_list[i];
		}

		send_str.insert(0, "map list: ");
		
		SendNews(player, send_str.c_str(), 0, 0, 0);
	}
}

void ZServer::PlayerCommand_ChangeMap(int player, string contents)
{
	const int max_values = 1;
	string values[max_values];

	ParseCommandContents(contents, values, max_values);

	for(int i=0;i<max_values;i++)
		if(!values[i].length())
		{
			SendNews(player, "command error: invalid input(s)", 0, 0, 0);
			return;
		}

	int map_num;

	map_num = atoi(values[0].c_str());

	StartVote(CHANGE_MAP_VOTE, map_num, player);
}

void ZServer::PlayerCommand_StartBot(int player, string contents)
{
	const int max_values = 1;
	string values[max_values];

	ParseCommandContents(contents, values, max_values);

	for(int i=0;i<max_values;i++)
		if(!values[i].length())
		{
			SendNews(player, "command error: invalid input(s)", 0, 0, 0);
			return;
		}

	int team_num = -1;

	for(int i=0;i<MAX_TEAM_TYPES;i++)
		if(team_type_string[i] == values[0])
		{
			team_num = i;
			break;
		}

	if(team_num == -1 || team_num==NULL_TEAM)// || !TeamHasBot(team_num))
	{
		string available_teams;
		string send_str;

		for(int i=NULL_TEAM+1;i<MAX_TEAM_TYPES;i++)
			//if(TeamHasBot(i))
		{
			if(available_teams.length())
				available_teams += ", ";

			available_teams += team_type_string[i];
		}

		send_str = "start bot error: invalid team, available teams: " + available_teams;
		SendNews(player, send_str.c_str(), 0, 0, 0);

		return;
	}

	StartVote(START_BOT, team_num, player);
}

void ZServer::PlayerCommand_StopBot(int player, string contents)
{
	const int max_values = 1;
	string values[max_values];

	ParseCommandContents(contents, values, max_values);

	for(int i=0;i<max_values;i++)
		if(!values[i].length())
		{
			SendNews(player, "command error: invalid input(s)", 0, 0, 0);
			return;
		}

	int team_num = -1;

	for(int i=0;i<MAX_TEAM_TYPES;i++)
		if(team_type_string[i] == values[0])
		{
			team_num = i;
			break;
		}

	if(team_num == -1 || !TeamHasBot(team_num))
	{
		string available_teams;
		string send_str;

		for(int i=0;i<MAX_TEAM_TYPES;i++)
			if(TeamHasBot(i))
		{
			if(available_teams.length())
				available_teams += ", ";

			available_teams += team_type_string[i];
		}

		send_str = "start bot error: invalid team, available teams: " + available_teams;
		SendNews(player, send_str.c_str(), 0, 0, 0);

		return;
	}

	StartVote(STOP_BOT, team_num, player);
}

void ZServer::PlayerCommand_PlayerInfo(int player, string contents)
{
	char num_str[50];
	string send_str;

	send_str = "player info: name: '" + player_info[player].name + "'";
	SendNews(player, send_str.c_str(), 0, 0, 0);

	send_str = "player info: team: " + team_type_string[player_info[player].team];
	SendNews(player, send_str.c_str(), 0, 0, 0);

	if(player_info[player].logged_in)
	{
		send_str = "player info: logged in: yes";
		SendNews(player, send_str.c_str(), 0, 0, 0);
		
		if(player_info[player].activated)
			send_str = "player info: activated: yes";
		else
			send_str = "player info: activated: yes";
		SendNews(player, send_str.c_str(), 0, 0, 0);

		sprintf(num_str, "%d", player_info[player].voting_power);
		send_str = "player info: voting power: " + string(num_str);
		SendNews(player, send_str.c_str(), 0, 0, 0);

		sprintf(num_str, "%d", player_info[player].real_voting_power());
		send_str = "player info: real voting power: " + string(num_str);
		SendNews(player, send_str.c_str(), 0, 0, 0);
	}
	else
	{
		send_str = "player info: logged in: no";
		SendNews(player, send_str.c_str(), 0, 0, 0);
	}
}

void ZServer::PlayerCommand_CurrentMap(int player, string contents)
{
	string send_str;

	send_str = "current map: " + map_name;
	SendNews(player, send_str.c_str(), 0, 0, 0);
}

void ZServer::PlayerCommand_ResetGame(int player, string contents)
{
	StartVote(RESET_GAME_VOTE, -1, player);
}

void ZServer::PlayerCommand_ChangeTeam(int player, string contents)
{
	const int max_values = 1;
	string values[max_values];
	string send_str;

	ParseCommandContents(contents, values, max_values);

	for(int i=0;i<max_values;i++)
		if(!values[i].length())
		{
			SendNews(player, "command error: invalid input(s)", 0, 0, 0);
			return;
		}

	int team_num = -1;

	for(int i=0;i<MAX_TEAM_TYPES;i++)
		if(team_type_string[i] == values[0])
		{
			team_num = i;
			break;
		}

	if(team_num == -1)
	{
		send_str = "change team error: invalid team, example command usage: /changeteam red ... or /changeteam blue";
		SendNews(player, send_str.c_str(), 0, 0, 0);

		return;
	}

	int previous_team;

	previous_team = player_info[player].team;

	if(team_num == previous_team)
	{
		send_str = "change team error: you are already on that team";
		SendNews(player, send_str.c_str(), 0, 0, 0);

		return;
	}

	if(ChangePlayerTeam(player, team_num))
	{
		send_str = player_info[player].name + " has changed from the " + team_type_string[previous_team] + " team to the " + team_type_string[player_info[player].team] + " team";
		BroadCastNews(send_str.c_str(), 0, 0, 0);
	}
}

void ZServer::PlayerCommand_ReshuffleTeams(int player, string contents)
{
	StartVote(RESHUFFLE_TEAMS_VOTE, -1, player);
}

void ZServer::PlayerCommand_BuyRegistration(int player, string contents)
{
	server_socket.SendMessage(player, POLL_BUY_REGKEY, NULL, 0);
}

void ZServer::PlayerCommand_ChangeSpeed(int player, string contents)
{
	const int max_values = 1;
	string values[max_values];
	int new_speed;

	ParseCommandContents(contents, values, max_values);

	for(int i=0;i<max_values;i++)
		if(!values[i].length())
		{
			SendNews(player, "command error: invalid input(s)", 0, 0, 0);
			return;
		}

	new_speed = atoi(values[0].c_str());

	//if(!psettings.allow_game_speed_change)
	//{
	//	SendNews(player, "changing the game speed is not allowed on this server", 0, 0, 0);
	//	return;
	//}

	//if(!new_speed || new_speed < 0)
	//{
	//	SendNews(player, "new game speed must be above zero", 0, 0, 0);
	//	return;
	//}

	StartVote(CHANGE_GAME_SPEED, new_speed, player);

	//ChangeGameSpeed(new_speed / 100.0);
}

void ZServer::PlayerCommand_Version(int player, string contents)
{
	//string version_str;

	//version_str = "the server version is ";
	//version_str += GAME_VERSION;

	//SendNews(player, version_str.c_str(), 0, 0, 0);

	RelayVersion(player);
}
