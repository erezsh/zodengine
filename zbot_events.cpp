#include "zbot.h"

using namespace COMMON;

void ZBot::SetupEHandler()
{
	ehandler.SetParent(this);

	//ehandler.AddFunction(TCP_EVENT, DEBUG_EVENT_, test_event);
	//ehandler.AddFunction(TCP_EVENT, STORE_MAP, store_map_event);
	//ehandler.AddFunction(TCP_EVENT, ADD_NEW_OBJECT, add_new_object_event);
	//ehandler.AddFunction(TCP_EVENT, SET_ZONE_INFO, set_zone_info_event);
	//ehandler.AddFunction(TCP_EVENT, SEND_LOC, set_object_loc_event);

	ehandler.AddFunction(TCP_EVENT, DEBUG_EVENT_, test_event);
	ehandler.AddFunction(TCP_EVENT, STORE_MAP, store_map_event);
	ehandler.AddFunction(TCP_EVENT, ADD_NEW_OBJECT, add_new_object_event);
	ehandler.AddFunction(TCP_EVENT, SET_ZONE_INFO, set_zone_info_event);
	ehandler.AddFunction(TCP_EVENT, NEWS_EVENT, display_news_event);
	ehandler.AddFunction(TCP_EVENT, SEND_WAYPOINTS, set_object_waypoints_event);
	ehandler.AddFunction(TCP_EVENT, SEND_RALLYPOINTS, set_object_rallypoints_event);
	ehandler.AddFunction(TCP_EVENT, SEND_LOC, set_object_loc_event);
	ehandler.AddFunction(TCP_EVENT, SET_OBJECT_TEAM, set_object_team_event);
	ehandler.AddFunction(TCP_EVENT, SET_ATTACK_OBJECT, set_object_attack_object_event);
	ehandler.AddFunction(TCP_EVENT, DELETE_OBJECT, delete_object_event);
	ehandler.AddFunction(TCP_EVENT, UPDATE_HEALTH, set_object_health_event);
	ehandler.AddFunction(TCP_EVENT, END_GAME, end_game_event);
	ehandler.AddFunction(TCP_EVENT, RESET_GAME, reset_game_event);
	ehandler.AddFunction(TCP_EVENT, FIRE_MISSILE, fire_object_missile_event);
	ehandler.AddFunction(TCP_EVENT, DESTROY_OBJECT, destroy_object_event);
	ehandler.AddFunction(TCP_EVENT, SET_BUILDING_STATE, set_building_state_event);
	ehandler.AddFunction(TCP_EVENT, SET_BUILT_CANNON_AMOUNT, set_building_cannon_list_event);
	ehandler.AddFunction(TCP_EVENT, COMP_MSG, set_computer_message_event);
	ehandler.AddFunction(TCP_EVENT, OBJECT_GROUP_INFO, set_object_group_info_event);
	ehandler.AddFunction(TCP_EVENT, DO_CRANE_ANIM, do_crane_anim_event);
	ehandler.AddFunction(TCP_EVENT, SET_REPAIR_ANIM, set_repair_building_anim_event);
	ehandler.AddFunction(TCP_EVENT, SET_SETTINGS, set_settings_event);
	ehandler.AddFunction(TCP_EVENT, SET_LID_OPEN, set_lid_open_event);
	ehandler.AddFunction(TCP_EVENT, SNIPE_OBJECT, snipe_object_event);
	ehandler.AddFunction(TCP_EVENT, DRIVER_HIT_EFFECT, driver_hit_effect_event);
	ehandler.AddFunction(TCP_EVENT, CLEAR_PLAYER_LIST, clear_player_list_event);
	ehandler.AddFunction(TCP_EVENT, ADD_LPLAYER, add_player_event);
	ehandler.AddFunction(TCP_EVENT, DELETE_LPLAYER, delete_player_event);
	ehandler.AddFunction(TCP_EVENT, SET_LPLAYER_NAME, set_player_name_event);
	ehandler.AddFunction(TCP_EVENT, SET_LPLAYER_TEAM, set_player_team_event);
	ehandler.AddFunction(TCP_EVENT, SET_LPLAYER_MODE, set_player_mode_event);
	ehandler.AddFunction(TCP_EVENT, SET_LPLAYER_IGNORED, set_player_ignored_event);
	ehandler.AddFunction(TCP_EVENT, SET_LPLAYER_LOGINFO, set_player_mode_event);
	ehandler.AddFunction(TCP_EVENT, SET_LPLAYER_VOTEINFO, set_player_voteinfo_event);
	ehandler.AddFunction(TCP_EVENT, UPDATE_GAME_PAUSED, update_game_paused_event);
	ehandler.AddFunction(TCP_EVENT, UPDATE_GAME_SPEED, update_game_speed_event);
	ehandler.AddFunction(TCP_EVENT, VOTE_INFO, set_vote_info_event);
	ehandler.AddFunction(TCP_EVENT, GIVE_PLAYER_ID, set_player_id_event);
	ehandler.AddFunction(TCP_EVENT, GIVE_SELECTABLE_MAP_LIST, set_selectable_map_list_event);
	ehandler.AddFunction(TCP_EVENT, GIVE_LOGINOFF, display_login_event);
	ehandler.AddFunction(TCP_EVENT, SET_GRENADE_AMOUNT, set_grenade_amount_event);
	ehandler.AddFunction(TCP_EVENT, PICKUP_GRENADE_ANIM, nothing_event);
	ehandler.AddFunction(TCP_EVENT, DO_PORTRAIT_ANIM, nothing_event);
	ehandler.AddFunction(TCP_EVENT, TEAM_ENDED, nothing_event);
	ehandler.AddFunction(TCP_EVENT, SET_TEAM, set_team_event);
	ehandler.AddFunction(TCP_EVENT, SET_BUILDING_QUEUE_LIST, set_build_queue_list_event);
	ehandler.AddFunction(TCP_EVENT, REQUEST_VERSION, nothing_event);
	ehandler.AddFunction(TCP_EVENT, GIVE_VERSION, nothing_event);

	ehandler.AddFunction(OTHER_EVENT, CONNECT_EVENT, connect_event);
	ehandler.AddFunction(OTHER_EVENT, DISCONNECT_EVENT, disconnect_event);
}

void ZBot::nothing_event(ZBot *p, char *data, int size, int dummy)
{

}

void ZBot::test_event(ZBot *p, char *data, int size, int dummy)
{
	if(size) printf("ZBot::test_event:%s...\n", data);
}

void ZBot::connect_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessConnect();

	p->SendBotBypassData();
}

void ZBot::disconnect_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessDisconnect();
}

void ZBot::store_map_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessMapDownload(data, size);
}

void ZBot::add_new_object_event(ZBot *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessNewObject(data, size);

	if(!obj) return;

	//add to short flag list?
	unsigned char ot, oid;

	obj->GetObjectID(ot, oid);

	if(ot == MAP_ITEM_OBJECT)
	{
		if(oid == FLAG_ITEM || oid == GRENADES_ITEM) p->flag_object_list.push_back(obj);
		else
		{
			//we don't care about the rest of the map object garbage
			p->ols.DeleteObject(obj);
		}
	}
}

void ZBot::set_zone_info_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessZoneInfo(data, size);
}

void ZBot::display_news_event(ZBot *p, char *data, int size, int dummy)
{

}

void ZBot::set_object_waypoints_event(ZBot *p, char *data, int size, int dummy)
{
	ZObject *our_object;

	our_object = p->ProcessWaypointData(data, size);

	//did any objects get their waypoint list updated?
	if(!our_object) return;
}

void ZBot::set_object_rallypoints_event(ZBot *p, char *data, int size, int dummy)
{
	ZObject *our_object;

	our_object = p->ProcessRallypointData(data, size);

	//did any objects get their waypoint list updated?
	if(!our_object) return;
}

void ZBot::set_object_loc_event(ZBot *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessObjectLoc(data, size);
}

void ZBot::set_object_team_event(ZBot *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessObjectTeam(data, size);
}

void ZBot::set_object_attack_object_event(ZBot *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessObjectAttackObject(data, size);
}

void ZBot::delete_object_event(ZBot *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessDeleteObject(data, size);

	if(!obj) return;

	//clean up all parts of the bot who use this pointer
	vector<ZObject*>::iterator i;
	for(i=p->object_list.begin();i!=p->object_list.end();i++)
		(*i)->RemoveObject(obj);
}

void ZBot::set_object_health_event(ZBot *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessObjectHealthTeam(data, size);
}

void ZBot::end_game_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessEndGame();
}

void ZBot::reset_game_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessResetGame();

	//our extra stuff
	p->flag_object_list.clear();
}

void ZBot::fire_object_missile_event(ZBot *p, char *data, int size, int dummy)
{
	//this function starts an effect
	//which is not available for the bot
	//p->ProcessFireMissile(data, size);
}

void ZBot::destroy_object_event(ZBot *p, char *data, int size, int dummy)
{
	destroy_object_packet *pi = (destroy_object_packet*)data;
	ZObject *obj;
	int i;

	//good packet?
	if(size < (int)sizeof(destroy_object_packet)) return;

	obj = p->GetObjectFromID(pi->ref_id, p->object_list);

	if(!obj) return;

	//good packet (double check)?
	if(size != (int)(sizeof(destroy_object_packet) + (sizeof(fire_missile_info) * pi->fire_missile_amount))) return;

	obj->SetHealth(0, p->zmap);
	//obj->DoDeathEffect();

	for(i=0;i<pi->fire_missile_amount;i++)
	{
		fire_missile_info missile_info;

		memcpy((char*)&missile_info, data + sizeof(destroy_object_packet) + (sizeof(fire_missile_info) * i), sizeof(fire_missile_info));
		//obj->FireTurrentMissile(missile_info.missile_x, missile_info.missile_y, missile_info.missile_offset_time);
	}

	if(pi->destroy_object)
	{
		ZObject::RemoveObjectFromList(obj, p->object_list);
		p->DeleteObjectCleanUp(obj);
	}
}

void ZBot::set_building_state_event(ZBot *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessBuildingState(data, size);
}

void ZBot::set_building_cannon_list_event(ZBot *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessBuildingCannonList(data, size);
}

void ZBot::set_computer_message_event(ZBot *p, char *data, int size, int dummy)
{
	//not sure if we need this for now.
}

void ZBot::set_object_group_info_event(ZBot *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessObjectGroupInfo(data, size);
}

void ZBot::do_crane_anim_event(ZBot *p, char *data, int size, int dummy)
{
	//not needed
}

void ZBot::set_repair_building_anim_event(ZBot *p, char *data, int size, int dummy)
{
	//not needed
}

void ZBot::set_settings_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessZSettings(data, size);
}

void ZBot::set_lid_open_event(ZBot *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessObjectLidState(data, size);
}

void ZBot::snipe_object_event(ZBot *p, char *data, int size, int dummy)
{
	//not needed
}

void ZBot::driver_hit_effect_event(ZBot *p, char *data, int size, int dummy)
{
	//not needed
}

void ZBot::clear_player_list_event(ZBot *p, char *data, int size, int dummy)
{
	p->player_info.clear();
}

void ZBot::add_player_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessAddLPlayer(data, size);
}

void ZBot::delete_player_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessDeleteLPlayer(data, size);
}

void ZBot::set_player_name_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessSetLPlayerName(data, size);
}

void ZBot::set_player_team_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessSetLPlayerTeam(data, size);
}

void ZBot::set_player_mode_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessSetLPlayerMode(data, size);
}

void ZBot::set_player_ignored_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessSetLPlayerIgnored(data, size);
}

void ZBot::set_player_loginfo_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessSetLPlayerLogInfo(data, size);
}

void ZBot::set_player_voteinfo_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessSetLPlayerVoteInfo(data, size);
}

void ZBot::update_game_paused_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessUpdateGamePaused(data, size);
}

void ZBot::update_game_speed_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessUpdateGameSpeed(data, size);
}

void ZBot::set_vote_info_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessVoteInfo(data, size);
}

void ZBot::set_player_id_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessPlayerID(data, size);
}

void ZBot::set_selectable_map_list_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessSelectableMapList(data, size);
}

void ZBot::display_login_event(ZBot *p, char *data, int size, int dummy)
{

}

void ZBot::set_grenade_amount_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessSetGrenadeState(data, size);
}

void ZBot::set_team_event(ZBot *p, char *data, int size, int dummy)
{
	p->ProcessSetTeam(data, size);
}

void ZBot::set_build_queue_list_event(ZBot *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessBuildingQueueList(data, size);
}
