#include "zclient.h"
#include "common.h"

using namespace COMMON;

ZClient::ZClient() : ZCore()
{
	remote_address = "localhost";
	player_name = "nameless_player";
	our_team = NULL_TEAM;
	our_mode = NOBODY_MODE;
	p_id = -1;
	
	map_data = 0;
	map_data_size = 0;
}

void ZClient::SetRemoteAddress(string ipaddress)
{
	remote_address = ipaddress;
}

void ZClient::ProcessConnect()
{
	//ask for the version
	client_socket.SendMessage(REQUEST_VERSION, NULL, 0);

	//ask for game paused
	client_socket.SendMessage(GET_GAME_PAUSED, NULL, 0);

	//ask for game speed
	client_socket.SendMessage(GET_GAME_SPEED, NULL, 0);

	//ask for settings
	client_socket.SendMessage(REQUEST_SETTINGS, NULL, 0);

	//give our name and mode
	//client_socket.SendMessageAscii(GIVE_PLAYER_NAME, player_name.c_str());
	SendPlayerInfo();
	
	//ask for player list
	RequestPlayerList();

	//ask for the selectable map list
	client_socket.SendMessage(REQUEST_SELECTABLE_MAP_LIST, NULL, 0);

	//ask for the map
	client_socket.SendMessage(REQUEST_MAP, NULL, 0);
}

void ZClient::ProcessDisconnect()
{
	printf("ZClient::disconnected from the game server...\n");
}

void ZClient::SetPlayerName(string player_name_)
{
   player_name = player_name_;
}

void ZClient::SetPlayerTeam(team_type player_team)
{
   our_team = player_team;
}

void ZClient::SetDesiredTeam(team_type player_team)
{
   desired_team = player_team;
}

int ZClient::ProcessMapDownload(char *data, int size)
{
	//static char *map_data = 0;
	//static int map_data_size;
	int pack_num;
	int data_size;
	
	data_size = size - 4;
	
	//1 = done, 0 still going
	
	if(size < 4) return 1;
	
	memcpy(&pack_num, data, 4);
	
	//last packet?
	if(pack_num == -1) 
	{
		if(map_data && map_data_size > 0)
		{
			//load map
			printf("map_data_size:%d\n", map_data_size);
			zmap.Read(map_data, map_data_size);
			
			//free mem
			free(map_data);
			map_data = 0;
			map_data_size = 0;
		}
		
		//SendPlayerInfo();
		RequestObjectList();
		RequestZoneList();
		
		//return "loaded"
		return 1;
	}
	
// 	printf("pushing map data:%d:%d\n", pack_num, data_size);
	
	//first packet?
	if(!pack_num)
	{
		if(map_data) 
		{
			free(map_data);
			map_data = 0;
			map_data_size = 0;
		}
		
		//stuff that data in our buffer
		map_data = (char*)malloc(data_size);
		memcpy(map_data, data + 4, data_size);
		map_data_size += data_size;
	}
	else
	{
		//resize and stuff that data in our buffer
		map_data = (char*)realloc(map_data, map_data_size + data_size);
		memcpy(map_data + map_data_size, data + 4, data_size);
		map_data_size += data_size;
	}

	return 0;
}

void ZClient::RequestObjectList()
{
   client_socket.SendMessage(REQUEST_OBJECTS, NULL, 0);
}

void ZClient::RequestZoneList()
{
   client_socket.SendMessage(REQUEST_ZONES, NULL, 0);
}

void ZClient::SendPlayerInfo()
{
   int the_team;
   
   the_team = desired_team;
   
   client_socket.SendMessageAscii(SET_NAME, player_name.c_str());
   SendPlayerTeam(the_team);
   //client_socket.SendMessage(SET_TEAM, (char*)&the_team, 4);

   SendPlayerMode();
}

void ZClient::SendPlayerTeam(int new_team)
{
	client_socket.SendMessage(SET_TEAM, (char*)&new_team, 4);
}

void ZClient::SendPlayerMode()
{
	player_mode_packet packet;

	packet.mode = our_mode;

	client_socket.SendMessage(SET_PLAYER_MODE, (char*)&packet, sizeof(player_mode_packet));
}

void ZClient::RequestPlayerList()
{
	client_socket.SendMessage(REQUEST_PLAYER_ID, NULL, 0);
	client_socket.SendMessage(REQUEST_PLAYER_LIST, NULL, 0);
}

void ZClient::SendBotBypassData()
{
	if(bot_bypass_size < 1 || bot_bypass_size > MAX_BOT_BYPASS_SIZE)
	{
		printf("ZClient::SendBotBypassData:: invalid bot_bypass_size\n");
		return;
	}

	client_socket.SendMessage(SEND_BOT_BYPASS_DATA, bot_bypass_data, bot_bypass_size);
}

p_info &ZClient::OurPInfo()
{
	static p_info not_found;

	for(vector<p_info>::iterator i=player_info.begin(); i!=player_info.end();i++)
			if(i->p_id == p_id)
				return *i;

	return not_found;
}

void ZClient::DeleteObjectCleanUp(ZObject *obj)
{

}

ZObject* ZClient::ProcessNewObject(char *data, int size)
{
	object_init_packet *o = (object_init_packet*)data;

	//good packet?
	if(size != sizeof(object_init_packet)) return NULL;

	//this a ok creation?
	if(!CreateObjectOk(o->object_type, o->object_id, o->x, o->y, o->owner, o->blevel, o->extra_links)) return NULL;

	ZObject *new_object_ptr = NULL;

	switch(o->object_type)
	{
	case BUILDING_OBJECT:
		switch(o->object_id)
		{
		case FORT_FRONT:
			new_object_ptr = new BFort(&ztime, &zsettings, (planet_type)zmap.GetMapBasics().terrain_type, true);
			break;
		case FORT_BACK:
			new_object_ptr = new BFort(&ztime, &zsettings, (planet_type)zmap.GetMapBasics().terrain_type, false);
			break;
		case RADAR:
			new_object_ptr = new BRadar(&ztime, &zsettings, (planet_type)zmap.GetMapBasics().terrain_type);
			break;
		case REPAIR:
			new_object_ptr = new BRepair(&ztime, &zsettings, (planet_type)zmap.GetMapBasics().terrain_type);
			break;
		case ROBOT_FACTORY:
			new_object_ptr = new BRobot(&ztime, &zsettings, (planet_type)zmap.GetMapBasics().terrain_type);
			break;
		case VEHICLE_FACTORY:
			new_object_ptr = new BVehicle(&ztime, &zsettings, (planet_type)zmap.GetMapBasics().terrain_type);
			break;
		case BRIDGE_VERT:
			new_object_ptr = new BBridge(&ztime, &zsettings, (planet_type)zmap.GetMapBasics().terrain_type, true, o->extra_links);
			break;
		case BRIDGE_HORZ:
			new_object_ptr = new BBridge(&ztime, &zsettings, (planet_type)zmap.GetMapBasics().terrain_type, false, o->extra_links);
			break;
		}
		break;
	case CANNON_OBJECT:
		switch(o->object_id)
		{
		case GATLING:
			new_object_ptr = new CGatling(&ztime, &zsettings, our_team == o->owner);
			break;
		case GUN:
			new_object_ptr = new CGun(&ztime, &zsettings, our_team == o->owner);
			break;
		case HOWITZER:
			new_object_ptr = new CHowitzer(&ztime, &zsettings, our_team == o->owner);
			break;
		case MISSILE_CANNON:
			new_object_ptr = new CMissileCannon(&ztime, &zsettings, our_team == o->owner);
			break;
		}
		break;
	case VEHICLE_OBJECT:
		switch(o->object_id)
		{
		case JEEP:
			new_object_ptr = new VJeep(&ztime, &zsettings);
			break;
		case LIGHT:
			new_object_ptr = new VLight(&ztime, &zsettings);
			break;
		case MEDIUM:
			new_object_ptr = new VMedium(&ztime, &zsettings);
			break;
		case HEAVY:
			new_object_ptr = new VHeavy(&ztime, &zsettings);
			break;
		case APC:
			new_object_ptr = new VAPC(&ztime, &zsettings);
			break;
		case MISSILE_LAUNCHER:
			new_object_ptr = new VMissileLauncher(&ztime, &zsettings);
			break;
		case CRANE:
			new_object_ptr = new VCrane(&ztime, &zsettings);
			break;
		}

		//for fun
		if(new_object_ptr)
			new_object_ptr->SetDirection(rand()%MAX_ANGLE_TYPES);
		break;
	case ROBOT_OBJECT:
		switch(o->object_id)
		{
		case GRUNT:
			new_object_ptr = new RGrunt(&ztime, &zsettings);
			break;
		case PSYCHO:
			new_object_ptr = new RPsycho(&ztime, &zsettings);
			break;
		case SNIPER:
			new_object_ptr = new RSniper(&ztime, &zsettings);
			break;
		case TOUGH:
			new_object_ptr = new RTough(&ztime, &zsettings);
			break;
		case PYRO:
			new_object_ptr = new RPyro(&ztime, &zsettings);
			break;
		case LASER:
			new_object_ptr = new RLaser(&ztime, &zsettings);
			break;
		}
		break;
	case MAP_ITEM_OBJECT:
		switch(o->object_id)
		{
		case FLAG_ITEM:
			new_object_ptr = new OFlag(&ztime, &zsettings);
			break;
		case ROCK_ITEM:
			new_object_ptr = new ORock(&ztime, &zsettings, (planet_type)zmap.GetMapBasics().terrain_type);
			break;
		case GRENADES_ITEM:
			new_object_ptr = new OGrenades(&ztime, &zsettings);
			break;
		case ROCKETS_ITEM:
			new_object_ptr = new ORockets(&ztime, &zsettings);
			break;
		case HUT_ITEM:
			new_object_ptr = new OHut(&ztime, &zsettings, (planet_type)zmap.GetMapBasics().terrain_type);
			break;
		}

		if(o->object_id >= MAP0_ITEM && o->object_id <= MAP21_ITEM)
			new_object_ptr = new OMapObject(&ztime, &zsettings, o->object_id);

		break;
	}

	if(new_object_ptr) 
	{
		new_object_ptr->SetOwner((team_type)o->owner);
		new_object_ptr->SetCords(o->x, o->y);
		new_object_ptr->SetRefID(o->ref_id);
		new_object_ptr->SetBuildList(&buildlist);
		new_object_ptr->SetConnectedZone(zmap);
		new_object_ptr->SetUnitLimitReachedList(unit_limit_reached);
		new_object_ptr->SetLevel(o->blevel);
		new_object_ptr->SetInitialDrivers();
		new_object_ptr->SetMap(&zmap);
		new_object_ptr->SetMapImpassables(zmap);

		//object_list.push_back(new_object_ptr);
		ols.AddObject(new_object_ptr);

		//   	printf("object added to game:%s of %s\n", new_object_ptr->GetObjectName().c_str(), team_type_string[o->owner].c_str());
	}

	return new_object_ptr;
}

void ZClient::ProcessZoneInfo(char *data, int size)
{
	zone_info_packet *pi = (zone_info_packet*)data;

	//good packet?
	if(size != sizeof(zone_info_packet)) return;

	//will this data crash us?
	if(pi->owner < 0) return;
	if(pi->owner >= MAX_TEAM_TYPES) return;
	if(pi->zone_number < 0) return;
	if(pi->zone_number >= (int)zmap.GetZoneInfoList().size()) return;

	//printf("ProcessZoneInfo::id:%d owner:%d\n", pi->zone_number, pi->owner);

	//set the zone info
	zmap.GetZoneInfoList()[pi->zone_number].owner = (team_type)pi->owner;

	//for the buildings
	ResetZoneOwnagePercentages();
}

ZObject* ZClient::ProcessObjectTeam(char *data, int size)
{
	ZObject *obj;

	//got the header?
	if(size < sizeof(object_team_packet)) return NULL;

	object_team_packet *pi = (object_team_packet*)data;

	//packet good?
	if(size != sizeof(object_team_packet) + (pi->driver_amount * sizeof(driver_info_s))) return NULL;

	//will this data crash us?
	if(pi->owner < 0) return NULL;
	if(pi->owner >= MAX_TEAM_TYPES) return NULL;

	obj = GetObjectFromID(pi->ref_id, object_list);

	if(!obj) return NULL;

	obj->SetOwner((team_type)pi->owner);
	//obj->SetDriver(pi->driver_type, 1);

	obj->SetDriverType(pi->driver_type);
	obj->ClearDrivers();

	int shift_amt = sizeof(object_team_packet);
	for(int i=0;i<pi->driver_amount;i++)
	{
		obj->AddDriver(*(driver_info_s*)(data + shift_amt));
		shift_amt += sizeof(driver_info_s);
	}

	//printf("ProcessObjectTeam:obj driver set to %s\n", robot_type_string[obj->GetDriver()].c_str());

	return obj;
}

ZObject* ZClient::ProcessObjectAttackObject(char *data, int size)
{
	attack_object_packet *pi = (attack_object_packet*)data;
	ZObject *obj;
	ZObject *attack_obj;

	//good packet?
	if(size != sizeof(attack_object_packet)) return NULL;

	obj = GetObjectFromID(pi->ref_id, object_list);
	attack_obj = GetObjectFromID(pi->attack_object_ref_id, object_list);

	if(!obj) return NULL;

	obj->SetAttackObject(attack_obj);

	return obj;
}

ZObject* ZClient::ProcessDeleteObject(char *data, int size)
{
	int ref_id;
	ZObject *obj;

	if(size != sizeof(int)) return NULL;

	ref_id = *(int*)data;

	obj = GetObjectFromID(ref_id, object_list);

	if(!obj) return NULL;

	//clean up while it still exists
	DeleteObjectCleanUp(obj);

	//vaporize it
	ols.DeleteObject(obj);

	return obj;
}

ZObject* ZClient::ProcessObjectHealthTeam(char *data, int size)
{
	object_health_packet *pi = (object_health_packet*)data;
	ZObject *obj;

	//good packet?
	if(size != sizeof(object_health_packet)) return NULL;

	obj = GetObjectFromID(pi->ref_id, object_list);

	if(!obj) return NULL;

	obj->SetHealth(pi->health, zmap);

	return obj;
}

ZObject* ZClient::ProcessFireMissile(char *data, int size)
{
	fire_missile_packet *pi = (fire_missile_packet*)data;
	ZObject *obj;

	//good packet?
	if(size != sizeof(fire_missile_packet)) return NULL;

	obj = GetObjectFromID(pi->ref_id, object_list);

	if(!obj) return NULL;

	obj->FireMissile(pi->x, pi->y);

	return obj;
}

ZObject* ZClient::ProcessObjectLoc(char *data, int size)
{
	ZObject *obj;
	int ref_id;
	object_location new_loc;

	if(size != 4 + sizeof(object_location)) return NULL;

	ref_id = ((int*)data)[0];
	memcpy(&new_loc, data+4, sizeof(object_location));

	obj = GetObjectFromID(ref_id, object_list);

	if(!obj) return NULL;

	obj->SetLoc(new_loc);

	return obj;
}

ZObject* ZClient::ProcessBuildingState(char *data, int size)
{
	set_building_state_packet *pi = (set_building_state_packet*)data;
	ZObject *obj;

	//good packet?
	if(size != sizeof(set_building_state_packet)) return NULL;

	obj = GetObjectFromID(pi->ref_id, object_list);

	if(!obj) return NULL;

	obj->ProcessSetBuildingStateData(data, size);

	return obj;
}

ZObject* ZClient::ProcessBuildingQueueList(char *data, int size)
{
	ZObject *obj;
	int ref_id;

	//does it hold the header info?
	if(size < 8) return NULL;

	//get header
	ref_id = ((int*)data)[0];

	obj = GetObjectFromID(ref_id, object_list);

	if(!obj) return NULL;

	obj->ProcessBuildingQueueData(data, size);

	return obj;
}

ZObject* ZClient::ProcessBuildingCannonList(char *data, int size)
{
	int ref_id;
	ZObject *obj;

	//good packet?
	if(size < 8) return NULL;

	ref_id = *(int*)(data);

	obj = GetObjectFromID(ref_id, object_list);

	if(!obj) return NULL;

	obj->ProcessSetBuiltCannonData(data, size);

	return obj;
}

ZObject* ZClient::ProcessObjectGroupInfo(char *data, int size)
{
	int ref_id;
	ZObject *obj;

	//needs to atleast hold the ref_id at this point
	if(size < 4) return NULL;

	ref_id = *(int*)data;

	obj = GetObjectFromID(ref_id, object_list);

	if(!obj) return NULL;

	obj->ProcessGroupInfoData(data, size, object_list);

	return obj;
}

ZObject* ZClient::ProcessObjectLidState(char *data, int size)
{
	set_lid_state_packet *pi = (set_lid_state_packet*)data;
	ZObject *obj;

	//good packet?
	if(size != sizeof(set_lid_state_packet)) return NULL;

	obj = GetObjectFromID(pi->ref_id, object_list);

	if(!obj) return NULL;

	obj->SetLidState(pi->lid_open);

	return obj;
}

ZObject* ZClient::ProcessSetGrenadeState(char *data, int size)
{
	obj_grenade_amount_packet *pi = (obj_grenade_amount_packet*)data;
	ZObject *obj;

	//good packet?
	if(size != sizeof(obj_grenade_amount_packet)) return NULL;

	obj = GetObjectFromID(pi->ref_id, object_list);

	if(!obj) return NULL;

	obj->SetGrenadeAmount(pi->grenade_amount);

	return obj;
}

bool ZClient::ProcessZSettings(char *data, int size)
{
	//good packet?
	if(size != sizeof(ZSettings)) return false;

	//stuff it
	memcpy(&zsettings, data, sizeof(ZSettings));

	return true;
}

bool ZClient::ProcessAddLPlayer(char *data, int size)
{
	add_remove_player_packet *pi = (add_remove_player_packet*)data;

	//good packet?
	if(size != sizeof(add_remove_player_packet)) return false;

	player_info.push_back(p_info(pi->p_id));

	return true;
}

bool ZClient::ProcessDeleteLPlayer(char *data, int size)
{
	add_remove_player_packet *pi = (add_remove_player_packet*)data;

	//good packet?
	if(size != sizeof(add_remove_player_packet)) return false;

	for(vector<p_info>::iterator i=player_info.begin(); i!=player_info.end();)
	{
		if(pi->p_id == i->p_id)
			i = player_info.erase(i);
		else
			i++;
	}

	return true;
}

bool ZClient::ProcessSetLPlayerName(char *data, int size)
{
	int p_id;

	//good packet?
	if(size < 5) return false;

	//last char must be null
	if(data[size-1]) return false;

	memcpy(&p_id, data, sizeof(int));

	for(vector<p_info>::iterator i=player_info.begin(); i!=player_info.end(); i++)
		if(p_id == i->p_id)
			i->name = (data+sizeof(int));

	return true;
}

bool ZClient::ProcessSetLPlayerTeam(char *data, int size)
{
	set_player_int_packet *pi = (set_player_int_packet*)data;

	//good packet?
	if(size != sizeof(set_player_int_packet)) return false;

	if(pi->value < 0) return false;
	if(pi->value >= MAX_TEAM_TYPES) return false;

	for(vector<p_info>::iterator i=player_info.begin(); i!=player_info.end(); i++)
		if(pi->p_id == i->p_id)
			i->team = (team_type)pi->value;

	return true;
}

bool ZClient::ProcessSetLPlayerMode(char *data, int size)
{
	set_player_int_packet *pi = (set_player_int_packet*)data;

	//good packet?
	if(size != sizeof(set_player_int_packet)) return false;

	if(pi->value < 0) return false;
	if(pi->value >= MAX_PLAYER_MODES) return false;

	for(vector<p_info>::iterator i=player_info.begin(); i!=player_info.end(); i++)
		if(pi->p_id == i->p_id)
			i->mode = (player_mode)pi->value;

	return true;
}

bool ZClient::ProcessSetLPlayerIgnored(char *data, int size)
{
	set_player_int_packet *pi = (set_player_int_packet*)data;

	//good packet?
	if(size != sizeof(set_player_int_packet)) return false;

	if(pi->value < 0) return false;
	if(pi->value >= MAX_PLAYER_MODES) return false;

	for(vector<p_info>::iterator i=player_info.begin(); i!=player_info.end(); i++)
		if(pi->p_id == i->p_id)
			i->ignored = (player_mode)pi->value;

	return true;
}

bool ZClient::ProcessSetLPlayerLogInfo(char *data, int size)
{
	set_player_loginfo_packet *pi = (set_player_loginfo_packet*)data;

	//good packet?
	if(size != sizeof(set_player_loginfo_packet)) return false;

	for(vector<p_info>::iterator i=player_info.begin(); i!=player_info.end(); i++)
		if(pi->p_id == i->p_id)
		{
			i->db_id = pi->db_id;
			i->activated = pi->activated;
			i->logged_in = pi->logged_in;
			i->bot_logged_in = pi->bot_logged_in;
			i->voting_power = pi->voting_power;
			i->total_games = pi->total_games;
		}

	return true;
}

bool ZClient::ProcessSetLPlayerVoteInfo(char *data, int size)
{
	set_player_int_packet *pi = (set_player_int_packet*)data;

	//good packet?
	if(size != sizeof(set_player_int_packet)) return false;

	if(pi->value < 0) return false;
	if(pi->value >= P_MAX_VOTE_CHOICES) return false;

	for(vector<p_info>::iterator i=player_info.begin(); i!=player_info.end(); i++)
		if(pi->p_id == i->p_id)
			i->vote_choice = pi->value;

	return true;
}

bool ZClient::ProcessUpdateGamePaused(char *data, int size)
{
	update_game_paused_packet *pi = (update_game_paused_packet*)data;

	//good packet?
	if(size != sizeof(update_game_paused_packet)) return false;

	if(pi->game_paused)
		ztime.Pause();
	else
		ztime.Resume();

	return true;
}

bool ZClient::ProcessUpdateGameSpeed(char *data, int size)
{
	float_packet *pi = (float_packet*)data;

	//good packet?
	if(size != sizeof(float_packet)) return false;

	ztime.SetGameSpeed(pi->game_speed);

	return true;
}

bool ZClient::ProcessVoteInfo(char *data, int size)
{
	vote_info_packet *pi = (vote_info_packet*)data;

	//good packet?
	if(size != sizeof(vote_info_packet)) return false;

	zvote.SetVoteInProgress(pi->in_progress);
	zvote.SetVoteType(pi->vote_type);
	zvote.SetVoteValue(pi->value);

	return true;
}

bool ZClient::ProcessPlayerID(char *data, int size)
{
	player_id_packet *pi = (player_id_packet*)data;

	//good packet?
	if(size != sizeof(player_id_packet)) return false;

	p_id = pi->p_id;

	return true;
}

bool ZClient::ProcessSelectableMapList(char *data, int size)
{
	selectable_map_list.clear();

	if(size <= 1) return true;

	const int buf_size = 500;
	int len = size-1;
	char buf[500];
	int i=0;

	while(i < len)
	{
		split(buf, data, ',', &i, buf_size, len);

		selectable_map_list.push_back(buf);
	}

	//blah
	//{
	//	printf("loaded selectable map list:\n");

	//	for(i=0; i<selectable_map_list.size(); i++)
	//		printf("\t%d) '%s'\n", i, selectable_map_list[i].c_str());
	//}

	return true;
}

bool ZClient::ProcessSetTeam(char *data, int size)
{
	int_packet *pi = (int_packet*)data;

	//good packet?
	if(size != sizeof(int_packet)) return false;

	if(pi->team < 0) return true;
	if(pi->team >= MAX_TEAM_TYPES) return true;

	SetPlayerTeam((team_type)pi->team);

	return true;
}

void ZClient::ProcessEndGame()
{

}

void ZClient::ProcessResetGame()
{
	//clear stuff out
	zmap.ClearMap();
	
	//clear object list
	ols.DeleteAllObjects();
	//for(vector<ZObject*>::iterator obj=object_list.begin(); obj!=object_list.end(); obj++)
	//	delete *obj;

	//object_list.clear();

	//ask for the map
	client_socket.SendMessage(REQUEST_MAP, NULL, 0);
}

