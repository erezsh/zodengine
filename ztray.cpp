#include "ztray.h"
#include "common.h"

using namespace COMMON;

ZTray::ZTray() : ZClient()
{
	player_name = "Tray";
	our_mode = TRAY_MODE;

	update_func = NULL;

	SetupEHandler();
	
	client_socket.SetEventList(&ehandler.GetEventList());
}

void ZTray::Setup()
{
	while(!client_socket.Start(remote_address.c_str()))
		uni_pause(10000);
}

void ZTray::Run()
{
	while(1)
	{
		//check for tcp events
		client_socket.Process();
		
		//process events
		ehandler.ProcessEvents();
		
		uni_pause(10);
	}
}

void ZTray::SetupEHandler()
{
	ehandler.SetParent(this);

	//tie the rest to nothing
	for(int i=0;i<MAX_TCP_EVENTS;i++)
		ehandler.AddFunction(TCP_EVENT, i, nothing_event);

	ehandler.AddFunction(TCP_EVENT, DEBUG_EVENT_, test_event);
	ehandler.AddFunction(TCP_EVENT, CLEAR_PLAYER_LIST, clear_player_list_event);
	ehandler.AddFunction(TCP_EVENT, ADD_LPLAYER, add_player_event);
	ehandler.AddFunction(TCP_EVENT, DELETE_LPLAYER, delete_player_event);
	ehandler.AddFunction(TCP_EVENT, SET_LPLAYER_NAME, set_player_name_event);
	ehandler.AddFunction(TCP_EVENT, SET_LPLAYER_TEAM, set_player_team_event);
	ehandler.AddFunction(TCP_EVENT, SET_LPLAYER_MODE, set_player_mode_event);
	ehandler.AddFunction(TCP_EVENT, SET_LPLAYER_IGNORED, set_player_ignored_event);
	ehandler.AddFunction(TCP_EVENT, SET_LPLAYER_LOGINFO, set_player_loginfo_event);
	ehandler.AddFunction(TCP_EVENT, SET_LPLAYER_VOTEINFO, set_player_voteinfo_event);

	ehandler.AddFunction(OTHER_EVENT, CONNECT_EVENT, connect_event);
	ehandler.AddFunction(OTHER_EVENT, DISCONNECT_EVENT, disconnect_event);
}

void ZTray::ProcessDisconnect()
{
	while(!client_socket.Start(remote_address.c_str()))
		uni_pause(10000);
}

void ZTray::SetUpdateCallBack(void (*update_func_)(ZTray*))
{
	update_func = update_func_;
}

vector<p_info> &ZTray::GetPlayerInfoList()
{
	return player_info;
}

void ZTray::UpdateDisplay()
{
	string team_players[MAX_TEAM_TYPES];
	string spectators;
	bool bot_player[MAX_TEAM_TYPES];
	bool bot_player_ignored[MAX_TEAM_TYPES];
	bool have_bot_players;
	int tray_players;
	int nobodies;
	char c_message[50];

	if(update_func)
	{
		update_func(this);
		return;
	}

#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif

	if(!player_info.size())
	{
		printf("No one is currently connected\n");
		return;
	}

	//init
	tray_players = 0;
	nobodies = 0;
	have_bot_players = false;
	for(int i=0;i<MAX_TEAM_TYPES;i++)
	{
		bot_player[i] = false;
		bot_player_ignored[i] = false;
	}

	for(vector<p_info>::iterator i=player_info.begin(); i!=player_info.end(); i++)
	{
		//char message[500];

		//sprintf(message, "p:'%s' t:%s m:%s\n", i->name.c_str(), team_type_string[i->team].c_str(), player_mode_string[i->mode].c_str());

		//AddNewsEntry(message);

		switch(i->mode)
		{
		case NOBODY_MODE:
			nobodies++;
			break;
		case PLAYER_MODE:
			if(team_players[i->team].size())
				team_players[i->team] += ", " + i->name;
			else
				team_players[i->team] += i->name;
			break;
		case BOT_MODE:
			bot_player[i->team] = true;
			bot_player_ignored[i->team] = i->ignored;
			have_bot_players = true;
			break;
		case SPECTATOR_MODE:
			if(spectators.size())
				spectators += ", " + i->name;
			else
				spectators += i->name;
			break;
		case TRAY_MODE:
			tray_players++;
			break;
		}
	}

	printf("--- Players connected ---\n");
	for(int i=0; i<MAX_TEAM_TYPES; i++)
		if(team_players[i].size() || bot_player[i])
		{
			if(bot_player[i])
			{
				if(bot_player_ignored[i])
					printf(string(team_type_string[i] + " team -b: " + team_players[i] + "\n").c_str());
				else
					printf(string(team_type_string[i] + " team +b: " + team_players[i] + "\n").c_str());
			}
			else
				printf(string(team_type_string[i] + " team: " + team_players[i] + "\n").c_str());
		}

	if(spectators.size())
		printf(string("spectators: " + spectators + "\n").c_str());

	if(tray_players)
	{
		sprintf(c_message, "tray players: %d\n", tray_players);
		printf(c_message);
	}

	if(nobodies)
	{
		sprintf(c_message, "nobodies: %d\n", nobodies);
		printf(c_message);
	}

	printf("-------------------------\n");
}

void ZTray::connect_event(ZTray *p, char *data, int size, int dummy)
{
	//p->ProcessConnect();
	p->SendPlayerInfo();
	
	//ask for player list
	p->RequestPlayerList();
}

void ZTray::disconnect_event(ZTray *p, char *data, int size, int dummy)
{
	p->ProcessDisconnect();
}

void ZTray::nothing_event(ZTray *p, char *data, int size, int dummy)
{

}

void ZTray::test_event(ZTray *p, char *data, int size, int dummy)
{

}

void ZTray::clear_player_list_event(ZTray *p, char *data, int size, int dummy)
{
	p->player_info.clear();

	p->UpdateDisplay();
}

void ZTray::add_player_event(ZTray *p, char *data, int size, int dummy)
{
	p->ProcessAddLPlayer(data, size);

	p->UpdateDisplay();
}

void ZTray::delete_player_event(ZTray *p, char *data, int size, int dummy)
{
	p->ProcessDeleteLPlayer(data, size);

	p->UpdateDisplay();
}

void ZTray::set_player_name_event(ZTray *p, char *data, int size, int dummy)
{
	p->ProcessSetLPlayerName(data, size);

	p->UpdateDisplay();
}

void ZTray::set_player_team_event(ZTray *p, char *data, int size, int dummy)
{
	p->ProcessSetLPlayerTeam(data, size);

	p->UpdateDisplay();
} 

void ZTray::set_player_mode_event(ZTray *p, char *data, int size, int dummy)
{
	p->ProcessSetLPlayerMode(data, size);

	p->UpdateDisplay();
}

void ZTray::set_player_ignored_event(ZTray *p, char *data, int size, int dummy)
{
	p->ProcessSetLPlayerIgnored(data, size);

	p->UpdateDisplay();
}

void ZTray::set_player_voteinfo_event(ZTray *p, char *data, int size, int dummy)
{
	p->ProcessSetLPlayerVoteInfo(data, size);

	p->UpdateDisplay();
}

void ZTray::set_player_loginfo_event(ZTray *p, char *data, int size, int dummy)
{
	p->ProcessSetLPlayerLogInfo(data, size);

	p->UpdateDisplay();
}
