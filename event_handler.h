#ifndef _EVENTHANDLER_H_
#define _EVENTHANDLER_H_

#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

//these transfer packets must have a uniform makeup / size
#pragma pack(1)

struct object_init_packet
{
	int x, y;
	int ref_id;
	char owner;
	unsigned char object_type;
	unsigned char object_id;
	char blevel;
	unsigned short extra_links;
	int health;
};

struct zone_info_packet
{
	int zone_number;
	char owner;
};

struct object_team_packet
{
	int ref_id;
	char owner;
	char driver_type;
	char driver_amount;
};

struct attack_object_packet
{
	int ref_id;
	int attack_object_ref_id;
};

struct object_health_packet
{
	int ref_id;
	int health;
};

struct fire_missile_packet
{
	int ref_id;
	int x, y;
};

struct destroy_object_packet
{
	int ref_id;
	int fire_missile_amount;
	int killer_ref_id;
	bool destroy_object;
	bool do_fire_death;
	bool do_missile_death;
};

struct start_building_packet
{
	int ref_id;
	unsigned char ot, oid;
};

struct set_building_state_packet
{
	int ref_id;
	int state;
	double init_offset;
	double prod_time;
	unsigned char ot, oid;
};

struct place_cannon_packet
{
	int ref_id;
	int tx, ty;
	unsigned char oid;
};

struct computer_msg_packet
{
	int ref_id;
	int sound;
};

struct eject_vehicle_packet
{
	int ref_id;
};

struct crane_anim_packet
{
	int ref_id;
	int rep_ref_id;
	bool on;
};

struct repair_building_anim_packet
{
	int ref_id;
	bool on;
	double remaining_time;
	bool play_sound;
};

struct set_lid_state_packet
{
	int ref_id;
	bool lid_open;
};

struct snipe_object_packet
{
	int ref_id;
};

struct driver_hit_packet
{
	int ref_id;
};

struct player_mode_packet
{
	char mode;
};

struct add_remove_player_packet
{
	int p_id;
};

struct set_player_int_packet
{
	int p_id;
	int value;
};

struct set_player_loginfo_packet
{
	int p_id;
	int db_id;
	int voting_power;
	int total_games;
	bool activated;
	bool logged_in;
	bool bot_logged_in;
};

struct update_game_paused_packet
{
	bool game_paused;
};

struct vote_info_packet
{
	bool in_progress;
	int vote_type;
	int value;
};

struct player_id_packet
{
	int p_id;
};

struct loginoff_packet
{
	bool show_login;
};

struct obj_grenade_amount_packet
{
	int ref_id;
	int grenade_amount;
};

union int_packet
{
	int value;
	int ref_id;
	int team;
	int map_num;
};

union float_packet
{
	float value;
	float game_speed;
};

struct do_portrait_anim_packet
{
	int ref_id;
	int anim_id;
};

struct team_ended_packet
{
	int team;
	bool won;
};

struct buy_registration_packet
{
	char buf[16];
};

struct add_building_queue_packet
{
	int ref_id;
	unsigned char ot, oid;
};

struct cancel_building_queue_packet
{
	int ref_id;
	int list_i;
	unsigned char ot, oid;
};

#define MAX_VERSION_PACKET_CHARS 50
struct version_packet
{
	char version[MAX_VERSION_PACKET_CHARS];
};

//undo the packet standardize command
#pragma pack()

enum pre_event_type
{
	TCP_EVENT, SDL_EVENT, OTHER_EVENT
};

enum tcp_event
{
	DEBUG_EVENT_, REQUEST_MAP, GIVE_PLAYER_NAME, STORE_MAP, REQUEST_OBJECTS, 
	REQUEST_ZONES, ADD_NEW_OBJECT, SET_ZONE_INFO, SET_NAME, SET_TEAM,
	NEWS_EVENT, SEND_WAYPOINTS, SEND_RALLYPOINTS, SEND_LOC, SET_OBJECT_TEAM, 
	SET_ATTACK_OBJECT, DELETE_OBJECT, UPDATE_HEALTH, END_GAME, RESET_GAME, FIRE_MISSILE,
	DESTROY_OBJECT, START_BUILDING, STOP_BUILDING, SET_BUILDING_STATE,
	SET_BUILT_CANNON_AMOUNT, PLACE_CANNON, SEND_CHAT, COMP_MSG, OBJECT_GROUP_INFO,
	EJECT_VEHICLE, DO_CRANE_ANIM, SET_REPAIR_ANIM, REQUEST_SETTINGS, SET_SETTINGS,
	SET_LID_OPEN, SNIPE_OBJECT, DRIVER_HIT_EFFECT, SET_PLAYER_MODE, REQUEST_PLAYER_LIST,
	CLEAR_PLAYER_LIST, ADD_LPLAYER, DELETE_LPLAYER, SET_LPLAYER_NAME, SET_LPLAYER_TEAM, 
	SET_LPLAYER_MODE, SET_LPLAYER_IGNORED, SET_LPLAYER_LOGINFO, SET_LPLAYER_VOTEINFO, 
	SEND_BOT_BYPASS_DATA, UPDATE_GAME_PAUSED, GET_GAME_PAUSED, SET_GAME_PAUSED, 
	START_VOTE, VOTE_YES, VOTE_NO, VOTE_PASS, VOTE_INFO, GIVE_PLAYER_ID, REQUEST_PLAYER_ID, 
	REQUEST_SELECTABLE_MAP_LIST, GIVE_SELECTABLE_MAP_LIST, SEND_LOGIN, REQUEST_LOGINOFF,
	GIVE_LOGINOFF, CREATE_USER, SET_GRENADE_AMOUNT, PICKUP_GRENADE_ANIM, DO_PORTRAIT_ANIM,
	TEAM_ENDED, POLL_BUY_REGKEY, BUY_REGKEY, RETURN_REGKEY, GET_GAME_SPEED, SET_GAME_SPEED,
	UPDATE_GAME_SPEED, ADD_BUILDING_QUEUE, SET_BUILDING_QUEUE_LIST, CANCEL_BUILDING_QUEUE,
	RESHUFFLE_TEAMS, START_BOT_EVENT, STOP_BOT_EVENT, SELECT_MAP, RESET_MAP, REQUEST_VERSION,
	GIVE_VERSION,
	MAX_TCP_EVENTS
};

enum sdl_event
{
	RESIZE_EVENT, 
    LCLICK_EVENT, 
    LUNCLICK_EVENT, 
    RCLICK_EVENT, 
    RUNCLICK_EVENT, 
	MCLICK_EVENT, 
    MUNCLICK_EVENT, 
	WHEELUP_EVENT,
	WHEELDOWN_EVENT,
    KEYDOWN_EVENT_, 
	KEYUP_EVENT_, 
	MOTION_EVENT,
    MAX_SDL_EVENTS
};

enum other_event
{
	CONNECT_EVENT, DISCONNECT_EVENT, MAX_OTHER_EVENTS
};

class Event
{
	public:
		Event(int event_type, int event_number, int player, char* data, int size, bool domemcpy = true);
		~Event();
		
		int player;
		int event_type;
		int event_number;
		char *data;
		int size;
	private:
};

#define MAX_EVENT_TYPES 5
#define MAX_FUNCTIONS 200


template <typename T>
class EventHandler
{
	public:
		EventHandler();
		void SetParent(T* parent);
		void AddEvent(Event *new_event);
		void AddFunction(int event_type, int event_number, void (*tfp)(T *, char *, int, int));
		void ProcessEvents();
		int ProcessEvent(int event_type, int event_number, char *data, int size, int player);
		list<Event*> &GetEventList();
	private:
		void (*fp[MAX_EVENT_TYPES][MAX_FUNCTIONS])(T *, char *, int, int);
		list<Event*> event_list;
		T* parent;
};

template <typename T> EventHandler<T>::EventHandler()
{
	int i, j;
	
	for(i=0;i<MAX_EVENT_TYPES;i++)
		for(j=0;j<MAX_FUNCTIONS;j++)
			fp[i][j] = NULL;
}

template <typename T> void EventHandler<T>::SetParent(T* parent_)
{
	parent = parent_;
}

template <typename T> list<Event*> &EventHandler<T>::GetEventList()
{
	return event_list;
}

template <typename T> void EventHandler<T>::AddEvent(Event *new_event)
{
	event_list.push_back(new_event);
}

template <typename T> void EventHandler<T>::AddFunction(int event_type, int event_number, void (*tfp)(T *,char *, int, int))
{
	if(event_type < 0 || event_type >= MAX_EVENT_TYPES)
	{
		printf("EventHandler::attempting to attach function to out of bounds event %d:%d\n", event_type, event_number);
		return;
	}
	
	if(event_number < 0 || event_number >= MAX_FUNCTIONS)
	{
		printf("EventHandler::attempting to attach function to out of bounds event %d:%d\n", event_type, event_number);
		return;
	}
	
	if(fp[event_type][event_number])
	{
		printf("EventHandler::attempting to attach function already attached event %d:%d\n", event_type, event_number);
	}
	
	fp[event_type][event_number] = tfp;
}

template <typename T> int EventHandler<T>::ProcessEvent(int event_type, int event_number, char *data, int size, int player)
{
	if(event_type < 0 || event_type >= MAX_EVENT_TYPES)
	{
		printf("EventHandler::attempting to process invalid event %d:%d\n", event_type, event_number);
		return 0;
	}
		
	if(event_number < 0 || event_number >= MAX_FUNCTIONS)
	{
		printf("EventHandler::attempting to process invalid event %d:%d\n", event_type, event_number);
		return 0;
	}
		
	if(!fp[event_type][event_number])
	{
		printf("EventHandler::no function attached to event %d:%d\n", event_type, event_number);
		return 0;
	}
		
	fp[event_type][event_number](parent, data, size, player);

	return 1;
}

template <typename T> void EventHandler<T>::ProcessEvents()
{
	list<Event*>::iterator i;

	//process the list
	for(i=event_list.begin(); i != event_list.end(); ++i)
	{
		Event* &e = *i;
		int &event_type = e->event_type;
		int &event_number = e->event_number;

		if(!ProcessEvent(event_type, event_number, e->data, e->size, e->player)) continue;
		
		//if(event_type < 0 || event_type >= MAX_EVENT_TYPES)
		//{
		//	printf("EventHandler::attempting to process invalid event %d:%d\n", event_type, event_number);
		//	continue;
		//}
		//
		//if(event_number < 0 || event_number >= MAX_FUNCTIONS)
		//{
		//	printf("EventHandler::attempting to process invalid event %d:%d\n", event_type, event_number);
		//	continue;
		//}
		//
		//if(!fp[event_type][event_number])
		//{
		//	printf("EventHandler::no function attached to event %d:%d\n", event_type, event_number);
		//	continue;
		//}
		//
		//fp[event_type][event_number](parent, e->data, e->size, e->player);

		delete e;
	}

	//no more
	event_list.clear();
}

#endif
