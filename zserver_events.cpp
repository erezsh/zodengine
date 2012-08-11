#include "zserver.h"

using namespace COMMON;

void ZServer::SetupEHandler()
{
	ehandler.SetParent(this);
	ehandler.AddFunction(TCP_EVENT, DEBUG_EVENT_, test_event);
	ehandler.AddFunction(TCP_EVENT, REQUEST_MAP, request_map_event);
	ehandler.AddFunction(TCP_EVENT, GIVE_PLAYER_NAME, set_name_event);
	ehandler.AddFunction(TCP_EVENT, REQUEST_OBJECTS, send_object_list_event);
	ehandler.AddFunction(TCP_EVENT, REQUEST_ZONES, send_zone_info_list_event);
	ehandler.AddFunction(TCP_EVENT, SET_NAME, set_player_name_event);
	ehandler.AddFunction(TCP_EVENT, SET_TEAM, set_player_team_event);
	ehandler.AddFunction(TCP_EVENT, SEND_WAYPOINTS, rcv_object_waypoints_event);
	ehandler.AddFunction(TCP_EVENT, SEND_RALLYPOINTS, rcv_object_rallypoints_event);
	ehandler.AddFunction(TCP_EVENT, START_BUILDING, start_building_event);
	ehandler.AddFunction(TCP_EVENT, STOP_BUILDING, stop_building_event);
	ehandler.AddFunction(TCP_EVENT, PLACE_CANNON, place_cannon_event);
	ehandler.AddFunction(TCP_EVENT, SEND_CHAT, relay_chat_event);
	ehandler.AddFunction(TCP_EVENT, EJECT_VEHICLE, exit_vehicle_event);
	ehandler.AddFunction(TCP_EVENT, REQUEST_SETTINGS, request_settings_event);
	ehandler.AddFunction(TCP_EVENT, SET_PLAYER_MODE, set_player_mode_event);
	ehandler.AddFunction(TCP_EVENT, REQUEST_PLAYER_LIST, request_player_list_event);
	ehandler.AddFunction(TCP_EVENT, SEND_BOT_BYPASS_DATA, bot_login_bypass_event);
	ehandler.AddFunction(TCP_EVENT, GET_GAME_PAUSED, get_pause_game_event);
	ehandler.AddFunction(TCP_EVENT, SET_GAME_PAUSED, set_pause_game_event);
	ehandler.AddFunction(TCP_EVENT, START_VOTE, start_vote_event);
	ehandler.AddFunction(TCP_EVENT, VOTE_YES, vote_yes_event);
	ehandler.AddFunction(TCP_EVENT, VOTE_NO, vote_no_event);
	ehandler.AddFunction(TCP_EVENT, VOTE_PASS, vote_pass_event);
	ehandler.AddFunction(TCP_EVENT, REQUEST_PLAYER_ID, request_player_id_event);
	ehandler.AddFunction(TCP_EVENT, REQUEST_SELECTABLE_MAP_LIST, request_selectable_map_list_event);
	ehandler.AddFunction(TCP_EVENT, SEND_LOGIN, player_login_event);
	ehandler.AddFunction(TCP_EVENT, REQUEST_LOGINOFF, request_login_required_event);
	ehandler.AddFunction(TCP_EVENT, CREATE_USER, create_user_event);
	ehandler.AddFunction(TCP_EVENT, BUY_REGKEY, buy_regkey_event);
	ehandler.AddFunction(TCP_EVENT, GET_GAME_SPEED, get_game_speed_event);
	ehandler.AddFunction(TCP_EVENT, SET_GAME_SPEED, set_game_speed_event);
	ehandler.AddFunction(TCP_EVENT, ADD_BUILDING_QUEUE, add_building_queue_event);
	ehandler.AddFunction(TCP_EVENT, CANCEL_BUILDING_QUEUE, cancel_building_queue_event);
	ehandler.AddFunction(TCP_EVENT, RESHUFFLE_TEAMS, reshuffle_teams_event);
	ehandler.AddFunction(TCP_EVENT, START_BOT_EVENT, start_bot_event);
	ehandler.AddFunction(TCP_EVENT, STOP_BOT_EVENT, stop_bot_event);
	ehandler.AddFunction(TCP_EVENT, SELECT_MAP, select_map_event);
	ehandler.AddFunction(TCP_EVENT, RESET_MAP, reset_map_event);
	ehandler.AddFunction(TCP_EVENT, REQUEST_VERSION, request_version_event);
	
	ehandler.AddFunction(OTHER_EVENT, CONNECT_EVENT, connect_event);
	ehandler.AddFunction(OTHER_EVENT, DISCONNECT_EVENT, disconnect_event);
}

void ZServer::test_event(ZServer *p, char *data, int size, int player)
{
	char message[500];
	
	if(size) printf("ZServer::test_event:%s... from player %d\n", data, player);
	
	sprintf(message, "hello player %d", player);
	p->server_socket.SendMessageAscii(player, DEBUG_EVENT_, message);
}

void ZServer::connect_event(ZServer *p, char *data, int size, int player)
{
	p_info new_entry;

	p->BroadCastNews("a player connected");

	if(player < (int)p->player_info.size())
	{
		printf("ZServer::connect_event: tried to add a player that is already on the list [p:%d] vs [size:%s]\n", player, p->player_info.size());
		return;
	}

	//set ip
	if(size) new_entry.ip = data;

	p->player_info.push_back(new_entry);

	{
		add_remove_player_packet p1;

		p1.p_id = new_entry.p_id;

		//add it
		p->server_socket.SendMessageAll(ADD_LPLAYER, (char*)&p1, sizeof(add_remove_player_packet));
	}
}

void ZServer::disconnect_event(ZServer *p, char *data, int size, int player)
{
	p_info new_entry;

	p->BroadCastNews("a player disconnected");

	if(player >= (int)p->player_info.size())
	{
		printf("ZServer::disconnect_event: tried to remove a player that is not on the list [p:%d] vs [size:%s]\n", player, p->player_info.size());
		return;
	}

	//logout player
	p->LogoutPlayer(player, false);

	//announce delete
	{
		add_remove_player_packet packet;

		packet.p_id = p->player_info[player].p_id;

		p->server_socket.SendMessageAll(DELETE_LPLAYER, (char*)&packet, sizeof(add_remove_player_packet));
	}

	p->player_info.erase(p->player_info.begin() + player);
}

void ZServer::request_map_event(ZServer *p, char *data, int size, int player)
{
	
	const int buf_size = 1024 * 4;
	char buf[buf_size + 4];
	int pack_num = -1;
	int read_in;
	int read_point = 0;
	
	if(p->zmap.Loaded())
	{
		char *map_data;
		int map_data_size;

		p->zmap.GetMapData(map_data, map_data_size);

		while(map_data_size > 0)
		{
			pack_num++;

			if(map_data_size >= buf_size)
			{
				memcpy(buf + 4, map_data + read_point, buf_size);
				read_in = buf_size;
				read_point += read_in;
				map_data_size -= read_in;
			}
			else
			{
				memcpy(buf + 4, map_data + read_point, map_data_size);
				read_in = map_data_size;
				read_point += read_in;
				map_data_size = 0;
			}

			memcpy(buf, &pack_num, 4);
			p->server_socket.SendMessage(player, STORE_MAP, buf, 4 + read_in);
		}
	}

	pack_num = -1;
	memcpy(buf, &pack_num, 4);
	p->server_socket.SendMessage(player, STORE_MAP, buf, 4);

	/*
	//send the map file
	if(p->zmap.Loaded())
	{
		FILE *fp;
		
		fp = fopen(p->map_name.c_str(), "rb");
		
		if(fp)
		{
			pack_num = 0;
			
			while(1)
			{
				read_in = fread(buf + 4, 1, buf_size, fp);
				
// 				printf("read_in:%d\n", read_in);
				
				if(read_in > 0)
				{
					memcpy(buf, &pack_num, 4);
					p->server_socket.SendMessage(player, STORE_MAP, buf, 4 + read_in);
					
					pack_num++;
				}
				else
					break;
			}
			
			pack_num = -1;

			fclose(fp);
		}
	}
	
	memcpy(buf, &pack_num, 4);
	p->server_socket.SendMessage(player, STORE_MAP, buf, 4);
	*/
}

void ZServer::set_name_event(ZServer *p, char *data, int size, int player)
{
	//if(!size) return;
	//
	////is the last char a null character?
	//if(data[size]) return;
	//
	//printf("p->player_info.size():%d\n", p->player_info.size());

	//p->player_info[player].name = data;
}

void ZServer::send_object_list_event(ZServer *p, char *data, int size, int player)
{
	for(vector<ZObject*>::iterator i=p->object_list.begin(); i!=p->object_list.end(); i++)
	{
//		object_init_packet object_info;

		//pack the object info
		p->RelayNewObject(*i, player);

		//send health
		p->RelayObjectHealth(*i, player);

		//send building state and repair anim info
		p->RelayBuildingState(*i, player);

		//send building queue list - sent with state
		//p->RelayObjectBuildingQueue(*i, player);

		//send group information - sent with RelayNewObject
		//p->RelayObjectGroupInfo(*i, player);

		//send grenade amount
		p->RelayObjectGrenadeAmount(*i, player);

		//send rally points
		p->RelayObjectRallyPoints(*i, player);
	}
}

void ZServer::send_zone_info_list_event(ZServer *p, char *data, int size, int player)
{
   int i;
   
   for(i=0;i<(int)p->zmap.GetZoneInfoList().size(); i++)
   {
      zone_info_packet packet_info;
      
      packet_info.zone_number = i;
      packet_info.owner = p->zmap.GetZoneInfoList()[i].owner;
      
      p->server_socket.SendMessage(player, SET_ZONE_INFO, (char*)&packet_info, sizeof(zone_info_packet));
   }
}

void ZServer::set_player_name_event(ZServer *p, char *data, int size, int player)
{
	int i;
	char message[500];

	//checks
	if(size <= 0) return;

	//it got a null character?
	for(i=0;i<size;i++)
		if(!data[i])
			break;

	if(i == size) return;

	//max size?
	if(size > MAX_PLAYER_NAME_SIZE)
		data[MAX_PLAYER_NAME_SIZE] = 0;

	if(p->psettings.require_login)
	{
		p->SendNews(player, "set name error: login required, please type /help", 0, 0, 0);
		return;
	}

	sprintf(message, "player '%s' set their name to '%s'", p->player_info[player].name.c_str(), data);

	p->player_info[player].name = data;

	p->BroadCastNews(message);

	//update player lists
	p->RelayLPlayerName(player);
	//{
	//	char *data;
	//	int size;

	//	size = sizeof(int) + p->player_info[player].name.size() + 1;
	//	data = (char*)malloc(size);

	//	memcpy(data, &(p->player_info[player].p_id), sizeof(int));
	//	memcpy(data+sizeof(int), p->player_info[player].name.c_str(), p->player_info[player].name.size());
	//	*(data+sizeof(int)+p->player_info[player].name.size()) = 0;

	//	p->server_socket.SendMessageAll(SET_LPLAYER_NAME, data, size);

	//	free(data);
	//}
}

void ZServer::set_player_team_event(ZServer *p, char *data, int size, int player)
{
	int the_team;
//	char message[500];

	if(size != 4) return;

	the_team = *(int*)data;

	if(the_team < NULL_TEAM) return;
	if(the_team >= MAX_TEAM_TYPES) return;

	p->ChangePlayerTeam(player, the_team);

	p->player_info[player].team = (team_type)the_team;

	//sprintf(message, "player '%s' set their team to the %s team", p->player_info[player].name.c_str(), team_type_string[the_team].c_str());
	//p->BroadCastNews(message);

	//update player lists
	p->RelayLPlayerTeam(player);
	//{
	//	set_player_int_packet packet;

	//	packet.p_id = p->player_info[player].p_id;
	//	packet.value = p->player_info[player].team;

	//	p->server_socket.SendMessageAll(SET_LPLAYER_TEAM, (char*)&packet, sizeof(set_player_int_packet));
	//}
}

void ZServer::rcv_object_waypoints_event(ZServer *p, char *data, int size, int player)
{
	ZObject *our_object;

	//logged in?
	//if(p->psettings.require_login && !p->player_info[player].logged_in)
	if(p->LoginCheckDenied(player))
	{
		p->SendNews(player, "move unit error: login required, please type /help", 0, 0, 0);
		return;
	}

	//ignored?
	if(p->player_info[player].ignored)
	{
		p->SendNews(player, "move unit error: player currently ignored", 0, 0, 0);
		return;
	}

	//do server tests
	{
		int expected_packet_size;
		int waypoint_amount;
		int ref_id;
		ZObject *our_object;

		//does it hold the header info?
		if(size < 8) return;

		//get header
		ref_id = ((int*)data)[0];
		waypoint_amount = ((int*)data)[1];

		expected_packet_size = 8 + (waypoint_amount * sizeof(waypoint));

		//should we toss this packet for bad data?
		if(size != expected_packet_size) return;

		//find our object
		our_object = GetObjectFromID(ref_id, p->object_list);

		//not found?
		if(!our_object) return;

		//passes activation check?
		unsigned char ot, oid;
		our_object->GetObjectID(ot, oid);
		if(!p->ActivationCheckPassed(player) && UnitRequiresActivation(ot, oid))
		{
			p->SendNews(player, "move unit error: activation required, please visit www.nighsoft.com", 0, 0, 0);
			return;
		}
	}

	our_object = p->ProcessWaypointData(data, size, true, p->player_info[player].team);

	//did any objects get their waypoint list updated?
	if(!our_object) return;

	//check to make sure this isn't a bunk list
	//p->CheckObjectWaypoints(our_object);

	//just left cannon = false
	our_object->SetJustLeftCannon(false);

	//clone
	our_object->CloneMinionWayPoints();

	//now relay
	p->RelayObjectWayPoints(our_object);

	//check to see if we need to force it to stop
	if(!our_object->GetWayPointList().size())
	{
		our_object->StopMove();
		p->RelayObjectLoc(our_object);

		for(vector<ZObject*>::iterator i=our_object->GetMinionList().begin(); i!=our_object->GetMinionList().end(); i++)
		{
			(*i)->StopMove();
			p->RelayObjectLoc((*i));
		}
	}
}

void ZServer::rcv_object_rallypoints_event(ZServer *p, char *data, int size, int player)
{
	ZObject *our_object;

	//logged in?
	if(p->LoginCheckDenied(player))
	{
		p->SendNews(player, "set rally points error: login required, please type /help", 0, 0, 0);
		return;
	}

	//ignored?
	if(p->player_info[player].ignored)
	{
		p->SendNews(player, "set rally points error: player currently ignored", 0, 0, 0);
		return;
	}

	our_object = p->ProcessRallypointData(data, size, true, p->player_info[player].team);

	//did any objects get their waypoint list updated?
	if(!our_object) return;

	//now relay
	p->RelayObjectRallyPoints(our_object);
}

void ZServer::start_building_event(ZServer *p, char *data, int size, int player)
{
	start_building_packet *pi = (start_building_packet*)data;
	ZObject *obj;

	//good packet?
	if(size != sizeof(start_building_packet)) return;

	obj = p->GetObjectFromID(pi->ref_id, p->object_list);

	if(!obj) return;

	//is this a building that can build?
	if(!obj->ProducesUnits()) return;
	//unsigned char ot, oid;
	//obj->GetObjectID(ot, oid);
	//if(ot != BUILDING_OBJECT) return;
	//if(!(oid == FORT_FRONT || oid == FORT_BACK || oid == ROBOT_FACTORY || oid == VEHICLE_FACTORY)) return;

	//is the team business kosher?
	if(obj->GetOwner() == NULL_TEAM) return;
	if(p->player_info[player].team != obj->GetOwner()) return;

	//logged in?
	//if(p->psettings.require_login && !p->player_info[player].logged_in)
	if(p->LoginCheckDenied(player))
	{
		p->SendNews(player, "start production error: login required, please type /help", 0, 0, 0);
		return;
	}

	//ignored?
	if(p->player_info[player].ignored)
	{
		p->SendNews(player, "start production error: player currently ignored", 0, 0, 0);
		return;
	}

	//ok it is a building that can build so lets set its new production
	if(obj->SetBuildingProduction(pi->ot, pi->oid))
	{
		//now let us tell everyone the building's new state
		p->RelayBuildingState(obj);

		//tell the person who did it
		computer_msg_packet send_data;
		send_data.ref_id = obj->GetRefID();
		send_data.sound = COMP_STARTING_MANUFACTURE_SND;
		p->server_socket.SendMessage(player, COMP_MSG, (char*)&send_data, sizeof(computer_msg_packet));
	}
}

void ZServer::stop_building_event(ZServer *p, char *data, int size, int player)
{
	int ref_id;
	ZObject *obj;

	//good packet?
	if(size != sizeof(int)) return;

	ref_id = *(int*)data;

	obj = p->GetObjectFromID(ref_id, p->object_list);

	if(!obj) return;

	//is this a building that can build?
	unsigned char ot, oid;

	obj->GetObjectID(ot, oid);

	if(ot != BUILDING_OBJECT) return;
	if(!(oid == FORT_FRONT || oid == FORT_BACK || oid == ROBOT_FACTORY || oid == VEHICLE_FACTORY)) return;

	//is the team business kosher?
	if(obj->GetOwner() == NULL_TEAM) return;
	if(p->player_info[player].team != obj->GetOwner()) return;

	//logged in?
	//if(p->psettings.require_login && !p->player_info[player].logged_in)
	if(p->LoginCheckDenied(player))
	{
		p->SendNews(player, "stop production error: login required, please type /help", 0, 0, 0);
		return;
	}

	//ignored?
	if(p->player_info[player].ignored)
	{
		p->SendNews(player, "stop production error: player currently ignored", 0, 0, 0);
		return;
	}

	//stop it!!!
	if(obj->StopBuildingProduction())
	{
		//now let us tell everyone the building's new state
		p->RelayBuildingState(obj);

		//tell the person who did it
		computer_msg_packet send_data;
		send_data.ref_id = obj->GetRefID();
		send_data.sound = COMP_MANUFACTURING_CANCELED_SND;
		p->server_socket.SendMessage(player, COMP_MSG, (char*)&send_data, sizeof(computer_msg_packet));
	}
}

void ZServer::place_cannon_event(ZServer *p, char *data, int size, int player)
{
	place_cannon_packet *pi = (place_cannon_packet*)data;
	ZObject *obj, *new_obj;

	//good packet?
	if(size != sizeof(place_cannon_packet)) return;

	obj = p->GetObjectFromID(pi->ref_id, p->object_list);

	if(!obj) return;

	if(!obj->HaveStoredCannon(pi->oid)) return;

	//this object belong to a zone?
	if(!obj->GetConnectedZone()) return;

	//this player can place this cannon?
	if(obj->GetOwner() == NULL_TEAM) return;
	if(p->player_info[player].team != obj->GetOwner()) return;

	//this placement place ok?
	if(!p->CannonPlacable(obj, pi->tx, pi->ty)) return;

	//remove it
	obj->RemoveStoredCannon(pi->oid);

	//place it
	new_obj = p->CreateObject(CANNON_OBJECT, pi->oid, pi->tx * 16, pi->ty * 16, obj->GetOwner());

	//is this area a fort turret?
	if(p->AreaIsFortTurret(pi->tx, pi->ty)) new_obj->SetEjectableCannon(false);

	//announce new object
	p->RelayNewObject(new_obj);

	//announce amount of cannons in building
	p->RelayBuildingBuiltCannons(obj);

	//as with all creation / deletions
	p->ProcessChangeObjectAmount();
}

void ZServer::relay_chat_event(ZServer *p, char *data, int size, int player)
{
	string out_message;
	string in_message;

	//we are not relaying nothing
	if(size <= 1) return;

	//is there a null terminator?
	if(data[size-1]) return;

	//is rest of it ascii?

	//is it a command?
	in_message = data;
	if(!in_message.length()) return;
	if(in_message[0] == '/')
	{
		p->ProcessPlayerCommand(player, in_message.substr(1));
		return;
	}

	//prep message
	out_message = p->player_info[player].name + ":: " + in_message;

	int t;
	t = p->player_info[player].team;

	p->BroadCastNews(out_message.c_str(), team_color[t].r * 0.3, team_color[t].g * 0.3, team_color[t].b * 0.3);
}

void ZServer::exit_vehicle_event(ZServer *p, char *data, int size, int player)
{
	eject_vehicle_packet *pi = (eject_vehicle_packet*)data;
	ZObject *obj, *new_obj;

	//good packet?
	if(size != sizeof(eject_vehicle_packet)) return;

	obj = p->GetObjectFromID(pi->ref_id, p->object_list);

	if(!obj) return;

	//same team?
	if(obj->GetOwner() == NULL_TEAM) return;
	if(p->player_info[player].team != obj->GetOwner()) return;

	//an object that can eject?
	if(!obj->CanEjectDrivers()) return;

	//create robot(s)
	if(obj->GetDrivers().size())
	{
		if(obj->GetDriverType() >= 0 && obj->GetDriverType() < MAX_ROBOT_TYPES)
		{
			int x, y;

			obj->GetCords(x, y);

			vector<driver_info_s>::iterator driver_i;
			
			driver_i = obj->GetDrivers().begin();

			new_obj = p->CreateRobotGroup(obj->GetDriverType(), x, y, obj->GetOwner(), NULL, obj->GetDrivers().size());

			//annouce leader
			{
				p->RelayNewObject(new_obj);

				//health
				new_obj->SetHealth(driver_i->driver_health, p->zmap);
				p->UpdateObjectHealth(new_obj);
			}

			//just left a cannon?
			unsigned char ot, oid;
			obj->GetObjectID(ot, oid);
			if(ot == CANNON_OBJECT) new_obj->SetJustLeftCannon(true);

			//and the minions...
			driver_i++;
			for(vector<ZObject*>::iterator i=new_obj->GetMinionList().begin(); i!=new_obj->GetMinionList().end() && driver_i!=obj->GetDrivers().end(); i++)
			{
				p->RelayNewObject(*i);

				//health
				new_obj->SetHealth(driver_i->driver_health, p->zmap);
				p->UpdateObjectHealth(*i);

				//just left cannon?
				if(ot == CANNON_OBJECT) new_obj->SetJustLeftCannon(true);
			}

			//new_obj = p->CreateObject(ROBOT_OBJECT, obj->GetDriverType(), x, y, obj->GetOwner(), p->object_list);

			////announce new object
			//p->RelayNewObject(new_obj);

			////health
			//new_obj->SetHealth(obj->GetDriverHealth());
			//p->UpdateObjectHealth(new_obj, current_time());

			//clear out "drivers"
			obj->ClearDrivers();
		}

		//we need to kill the waypoints too
		{
//			char *data;
//			int size;

			//clear
			obj->GetWayPointList().clear();

			//now relay
			p->RelayObjectWayPoints(obj);

			//force it to stop moving
			if(obj->StopMove()) p->RelayObjectLoc(obj);

			//force it to stop attacking
			obj->SetAttackObject(NULL);
			p->RelayObjectAttackObject(obj);
			//obj->CreateAttackObjectData(data, size);
			//p->server_socket.SendMessageAll(SET_ATTACK_OBJECT, data, size);
			//free(data);
		}
	}

	//new team
	p->ResetObjectTeam(obj, NULL_TEAM);
}

void ZServer::request_settings_event(ZServer *p, char *data, int size, int player)
{
	p->server_socket.SendMessage(player, SET_SETTINGS, (char*)&p->zsettings, sizeof(ZSettings));
}

void ZServer::set_player_mode_event(ZServer *p, char *data, int size, int player)
{
	player_mode_packet *pi = (player_mode_packet*)data;

	//good packet?
	if(size != sizeof(player_mode_packet)) return;

	//good setting
	if(pi->mode < 0) return;
	if(pi->mode >= MAX_PLAYER_MODES) return;

	p->player_info[player].mode = (player_mode)pi->mode;

	//ignore bot?
	if(p->player_info[player].mode == BOT_MODE && p->psettings.bots_start_ignored && !p->player_info[player].ignored)
	{
		p->player_info[player].ignored = true;
		p->RelayLPlayerIgnored(player);
	}

	//update player lists
	p->RelayLPlayerMode(player);

	//set history stuff
	p->CheckOnlineHistoryPlayerCount();
	p->CheckOnlineHistoryTrayPlayerCount();

	//printf("ZServer::set_player_mode_event:p:%d m:%s\n", player, player_mode_string[p->player_info[player].mode].c_str());
}

void ZServer::request_player_list_event(ZServer *p, char *data, int size, int player)
{
	//clear list
	p->server_socket.SendMessage(player, CLEAR_PLAYER_LIST, NULL, 0);

	//send the list
	//for(vector<p_info>::iterator i=p->player_info.begin(); i!=p->player_info.end(); i++)
	for(int i=0;i<(int)p->player_info.size();i++)
	{
		p->RelayLAdd(i, player);
		
		p->RelayLPlayerName(i, player);
		p->RelayLPlayerTeam(i, player);
		p->RelayLPlayerMode(i, player);
		p->RelayLPlayerIgnored(i, player);
		p->RelayLPlayerLoginInfo(i, player);
		p->RelayLPlayerVoteChoice(i, player);
	}

}

void ZServer::bot_login_bypass_event(ZServer *p, char *data, int size, int player)
{
	if(p->bot_bypass_size < 1 || p->bot_bypass_size > MAX_BOT_BYPASS_SIZE)
	{
		printf("ZServer::bot_login_bypass_event:: invalid bot_bypass_size\n");
		return;
	}

	//size ok?
	if(size != p->bot_bypass_size) return;

	//is it a match?
	for(int i=0;i<size;i++)
		if(data[i] != p->bot_bypass_data[i])
			return;

	//do a bot bypass

	//clear up
	p->player_info[player].logout();

	//set
	p->player_info[player].name = "Bot";
	p->player_info[player].bot_logged_in = true;

	//tell everyone
	p->RelayLPlayerName(player);
	p->RelayLPlayerLoginInfo(player);
}

void ZServer::get_pause_game_event(ZServer *p, char *data, int size, int player)
{
	p->RelayGamePaused(player);
}

void ZServer::set_pause_game_event(ZServer *p, char *data, int size, int player)
{
	update_game_paused_packet *pi = (update_game_paused_packet*)data;

	//good packet?
	if(size != sizeof(update_game_paused_packet)) return;

	if(pi->game_paused && p->ztime.IsPaused()) return;
	if(!pi->game_paused && !p->ztime.IsPaused()) return;

	if(pi->game_paused)
		p->StartVote(PAUSE_VOTE, -1, player);
	else
		p->StartVote(RESUME_VOTE, -1, player);

	//if(pi->game_paused)
	//	p->PauseGame();
	//else
	//	p->ResumeGame();
}

void ZServer::start_vote_event(ZServer *p, char *data, int size, int player)
{
	vote_info_packet *pi = (vote_info_packet*)data;

	//good packet?
	if(size != sizeof(vote_info_packet)) return;

	if(p->StartVote(pi->vote_type, pi->value))
		p->VoteYes(player);
}

void ZServer::vote_yes_event(ZServer *p, char *data, int size, int player)
{
	p->VoteYes(player);
}

void ZServer::vote_no_event(ZServer *p, char *data, int size, int player)
{
	p->VoteNo(player);
}

void ZServer::vote_pass_event(ZServer *p, char *data, int size, int player)
{
	p->VotePass(player);
}

void ZServer::request_player_id_event(ZServer *p, char *data, int size, int player)
{
	p->GivePlayerID(player);
}

void ZServer::request_selectable_map_list_event(ZServer *p, char *data, int size, int player)
{
	p->GivePlayerSelectableMapList(player);
}

void ZServer::player_login_event(ZServer *p, char *data, int size, int player)
{
	if(size <= 1) return;

	char loginname[500];
	char password[500];
	int i=0;

	split(loginname, data, ',', &i, 500, size-1);
	split(password, data, ',', &i, 500, size-1);

	if(!loginname[0]) return;
	if(!password[0]) return;

	if(!good_user_string(loginname)) return;
	if(!good_user_string(password)) return;

	p->AttemptPlayerLogin(player, loginname, password);
}

void ZServer::request_login_required_event(ZServer *p, char *data, int size, int player)
{
	p->SendPlayerLoginRequired(player);
}

void ZServer::create_user_event(ZServer *p, char *data, int size, int player)
{
	if(size <= 1) return;

	char username[500];
	char loginname[500];
	char password[500];
	char email[500];
	int i=0;

	split(username, data, ',', &i, 500, size-1);
	split(loginname, data, ',', &i, 500, size-1);
	split(password, data, ',', &i, 500, size-1);
	split(email, data, ',', &i, 500, size-1);

	if(!username[0]) return;
	if(!loginname[0]) return;
	if(!password[0]) return;
	if(!email[0]) return;

	if(!good_user_string(username)) return;
	if(!good_user_string(loginname)) return;
	if(!good_user_string(password)) return;
	if(!good_user_string(email)) return;

	p->AttemptCreateUser(player, username, loginname, password, email);
}

void ZServer::buy_regkey_event(ZServer *p, char *data, int size, int player)
{
	buy_registration_packet *pi = (buy_registration_packet*)data;

	//good packet?
	if(size != sizeof(buy_registration_packet)) return;

#ifdef DISABLE_BUYREG
	p->SendNews(player, "buy reg key error: please visit www.nighsoft.com", 0, 0, 0);
	return;
#else
	//using db etc?
	if(!p->psettings.use_database || !p->psettings.use_mysql)
	{
		p->SendNews(player, "buy reg key error: server not configured for selling registration keys", 0, 0, 0);
		return;
	}

	//user logged in?
	if(!p->player_info[player].logged_in)
	{
		p->SendNews(player, "buy reg key error: you must be logged in", 0, 0, 0);
		return;
	}

	//user activated?
	if(!p->player_info[player].activated)
	{
		p->SendNews(player, "buy reg key error: you must be activated, please visit www.nighsoft.com", 0, 0, 0);
		return;
	}

	//user got 1 VP?
	if(p->player_info[player].voting_power - REGISTRATION_COST < 0)
	{
		p->SendNews(player, "buy reg key error: more voting power to spend required, please visit www.nighsoft.com", 0, 0, 0);
		return;
	}

	//kill some voting power
	p->player_info[player].voting_power -= REGISTRATION_COST;
	p->RelayLPlayerLoginInfo(player);

	//mysql
	{
		string err_msg;

		if(!p->zmysql.IncreaseUserVariable(err_msg, p->player_info[player].db_id, "voting_power", -REGISTRATION_COST))
			printf("mysql error: %s\n", err_msg.c_str());
	}

	//send the key
	buy_registration_packet packet;

	p->zencrypt.AES_Encrypt(pi->buf, 16, packet.buf);

	p->server_socket.SendMessage(player, RETURN_REGKEY, (const char*)&packet, sizeof(buy_registration_packet));

	//log the transaction
	printd_reg((char*)string(p->player_info[player].name + " bought a registration key for " + data_to_hex_string((unsigned char*)pi->buf, 16)).c_str());

#endif
}

void ZServer::get_game_speed_event(ZServer *p, char *data, int size, int player)
{
	p->RelayGameSpeed(player);
}

void ZServer::set_game_speed_event(ZServer *p, char *data, int size, int player)
{
	float_packet *pi = (float_packet*)data;
	int new_speed;

	//good packet?
	if(size != sizeof(float_packet)) return;

	new_speed = (int)(pi->game_speed * 100.0f);

	p->StartVote(CHANGE_GAME_SPEED, new_speed, player);
}

void ZServer::add_building_queue_event(ZServer *p, char *data, int size, int player)
{
	add_building_queue_packet *pi = (add_building_queue_packet*)data;
	ZObject *obj;

	//good packet?
	if(size != sizeof(add_building_queue_packet)) return;

	obj = p->GetObjectFromID(pi->ref_id, p->object_list);

	if(!obj) return;

	//printf("add queue %d %d\n", pi->ot, pi->oid);

	//is the team business kosher?
	if(obj->GetOwner() == NULL_TEAM) return;
	if(p->player_info[player].team != obj->GetOwner()) return;

	//produces units?
	if(!obj->ProducesUnits()) return;

	//logged in?
	if(p->LoginCheckDenied(player))
	{
		p->SendNews(player, "add queue error: login required, please type /help", 0, 0, 0);
		return;
	}

	//ignored?
	if(p->player_info[player].ignored)
	{
		p->SendNews(player, "add queue error: player currently ignored", 0, 0, 0);
		return;
	}

	//are we starting production or adding to the queue?
	unsigned char bot, boid;
	if(obj->GetBuildUnit(bot, boid) && bot==(unsigned char)-1 && boid==(unsigned char)-1)
	{
		if(obj->SetBuildingProduction(pi->ot, pi->oid)) p->RelayBuildingState(obj);
	}
	else
	{
		if(obj->AddBuildingQueue(pi->ot, pi->oid)) p->RelayObjectBuildingQueue(obj);
	}
}

void ZServer::cancel_building_queue_event(ZServer *p, char *data, int size, int player)
{
	cancel_building_queue_packet *pi = (cancel_building_queue_packet*)data;
	ZObject *obj;

	//good packet?
	if(size != sizeof(cancel_building_queue_packet)) return;

	obj = p->GetObjectFromID(pi->ref_id, p->object_list);

	if(!obj) return;

	//is the team business kosher?
	if(obj->GetOwner() == NULL_TEAM) return;
	if(p->player_info[player].team != obj->GetOwner()) return;

	//produces units?
	if(!obj->ProducesUnits()) return;

	//logged in?
	if(p->LoginCheckDenied(player))
	{
		p->SendNews(player, "cancel queue error: login required, please type /help", 0, 0, 0);
		return;
	}

	//ignored?
	if(p->player_info[player].ignored)
	{
		p->SendNews(player, "cancel queue error: player currently ignored", 0, 0, 0);
		return;
	}

	if(obj->CancelBuildingQueue(pi->list_i, pi->ot, pi->oid)) p->RelayObjectBuildingQueue(obj);
}

void ZServer::reshuffle_teams_event(ZServer *p, char *data, int size, int player)
{
	p->StartVote(RESHUFFLE_TEAMS_VOTE, -1, player);
}

void ZServer::start_bot_event(ZServer *p, char *data, int size, int player)
{
	int_packet *pi = (int_packet*)data;

	//good packet?
	if(size != sizeof(int_packet)) return;

	//team ok?
	if(pi->team < 0) return;
	if(pi->team >= MAX_TEAM_TYPES) return;
	if(pi->team == NULL_TEAM) return;

	p->StartVote(START_BOT, pi->team, player);
}

void ZServer::stop_bot_event(ZServer *p, char *data, int size, int player)
{
	int_packet *pi = (int_packet*)data;

	//good packet?
	if(size != sizeof(int_packet)) return;

	//team ok?
	if(pi->team < 0) return;
	if(pi->team >= MAX_TEAM_TYPES) return;
	if(pi->team == NULL_TEAM) return;

	p->StartVote(STOP_BOT, pi->team, player);
}

void ZServer::select_map_event(ZServer *p, char *data, int size, int player)
{
	int_packet *pi = (int_packet*)data;

	//good packet?
	if(size != sizeof(int_packet)) return;

	p->StartVote(CHANGE_MAP_VOTE, pi->map_num, player);
}

void ZServer::reset_map_event(ZServer *p, char *data, int size, int player)
{
	p->StartVote(RESET_GAME_VOTE, -1, player);
}

void ZServer::request_version_event(ZServer *p, char *data, int size, int player)
{
	p->RelayVersion(player);
}
