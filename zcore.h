#ifndef _ZCORE_H_
#define _ZCORE_H_

#include <string>
#include <algorithm>
#include <vector>
#include <time.h>
#include "common.h"
#include "constants.h"
#include "zobject.h"
#include "zmap.h"
#include "event_handler.h"
#include "socket_handler.h"
#include "zbuildlist.h"
#include "zsettings.h"
#include "ztime.h"
#include "zvote.h"
#include "zunitrating.h"
#include "zencrypt_aes.h"
#include "zhud.h"
#include "zolists.h"

#include "bfort.h"
#include "brepair.h"
#include "bradar.h"
#include "brobot.h"
#include "bvehicle.h"
#include "bbridge.h"

#include "cgatling.h"
#include "cgun.h"
#include "chowitzer.h"
#include "cmissilecannon.h"

#include "vjeep.h"
#include "vlight.h"
#include "vmedium.h"
#include "vheavy.h"
#include "vapc.h"
#include "vmissilelauncher.h"
#include "vcrane.h"

#include "rgrunt.h"
#include "rpsycho.h"
#include "rsniper.h"
#include "rtough.h"
#include "rpyro.h"
#include "rlaser.h"

#include "oflag.h"
#include "orock.h"
#include "ogrenades.h"
#include "orockets.h"
#include "ohut.h"
#include "omapobject.h"

#include "abird.h"
#include "ahutanimal.h"

using namespace std;

enum p_vote_choice
{
	P_NULL_VOTE, P_YES_VOTE, P_NO_VOTE, P_PASS_VOTE, P_MAX_VOTE_CHOICES
};

class p_info
{
public:
	p_info()
	{
		clear();
		p_id = next_p_id++;
	}

	p_info(int p_id_)
	{
		clear();
		p_id = p_id_;
	}

	void clear()
	{
		name.clear();
		team = NULL_TEAM;
		mode = NOBODY_MODE;
		vote_choice = P_NULL_VOTE;
		ignored = false;

		logout();
	}

	void logout()
	{
		name.clear();
		logged_in = false;
		db_id = -1;
		activated = false;
		voting_power = 0;
		total_games = 0;
		last_time = 0;

		bot_logged_in = false;
	}

	int real_voting_power()
	{
		const int games_per_vp = 5;

		return voting_power + (total_games / games_per_vp);
	}

	string name;
	team_type team;
	player_mode mode;
	int vote_choice;
	bool ignored;
	int p_id;

	string ip;

	//logged in details
	bool logged_in;
	int db_id;
	bool activated;
	int voting_power;
	int last_time;
	int total_games;

	//bot bypass
	bool bot_logged_in;

	static int next_p_id;
};

class ZCore
{
	public:
		ZCore();
		virtual ~ZCore() {}

		virtual void Setup();
		virtual void Run();
		void SetBotBypassData(char *data, int size);
		static void CreateRandomBotBypassData(char *data, int &size);
		virtual void AllowRun(bool allow_run_ = true) { allow_run = allow_run_; }
	protected:
		void InitEncryption();
		void SetupRandomizer();
		bool CheckRegistration();
		int GetObjectIndex(ZObject* &the_object, vector<ZObject*> &the_list);
		static ZObject* GetObjectFromID_BS(int ref_id, vector<ZObject*> &the_list);
		static ZObject* GetObjectFromID(int ref_id, vector<ZObject*> &the_list);
		void CreateWaypointSendData(int ref_id, vector<waypoint> &waypoint_list, char* &data, int &size);
		ZObject* ProcessWaypointData(char *data, int size, bool is_server = false, int ok_team = -1);
		ZObject* ProcessRallypointData(char *data, int size, bool is_server = false, int ok_team = -1);
		virtual void DeleteObjectCleanUp(ZObject *obj);
		bool CannonPlacable(ZObject *building_obj, int tx, int ty);
		bool AreaIsFortTurret(int tx, int ty);
		void ResetUnitLimitReached();
		bool CheckUnitLimitReached();
		virtual void ResetZoneOwnagePercentages(bool notify_players = false);
		bool CheckWaypoint(ZObject *obj, waypoint *wp);
		bool CheckRallypoint(ZObject *obj, waypoint *wp);
		static bool UnitRequiresActivation(unsigned char ot, unsigned char oid);
		bool CreateObjectOk(unsigned char ot, unsigned char oid, int x, int y, int owner, int blevel, unsigned short extra_links);

		//voting stuff
		int VotesNeeded();
		int VotesFor();
		int VotesAgainst();
		string VoteAppendDescription();

		ZMap zmap;
		ZBuildList buildlist;
		ZSettings zsettings;
		ZUnitRating zunitrating;
		ZEncryptAES zencrypt;
		ZOLists ols;

		ZTime ztime;
		ZVote zvote;
		
		vector<p_info> player_info;
		vector<ZObject*> object_list;
		//vector<string> player_name;
		//vector<team_type> player_team;
		vector<string> selectable_map_list;

		bool allow_run;

		bool unit_limit_reached[MAX_TEAM_TYPES];
		int team_units_available[MAX_TEAM_TYPES];
		float team_zone_percentage[MAX_TEAM_TYPES];

		int max_units_per_team;

		char bot_bypass_data[MAX_BOT_BYPASS_SIZE];
		int bot_bypass_size;

		bool is_registered;
};

#endif
