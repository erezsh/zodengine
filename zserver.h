#ifndef _ZSERVER_H_
#define _ZSERVER_H_

#include "zcore.h"
#include "server_socket.h"
#include "zdamagemissile.h"
#include "zpsettings.h"
#include "zmysql.h"
#include "zbot.h"

class ZServer : public ZCore
{
	public:
		ZServer();
		~ZServer();
		
		void SetMapName(string map_name_);
		void SetMapList(string map_list_name_);
		void SetSettingsFilename(string settings_filename_);
		void SetPerpetualSettingsFilename(string p_settings_filename_);
		void InitBot(int bot_team, bool do_init = true);
		void Setup();
		void Run();
	private:
		void SetupEHandler();
		void InitPerpetualServerSettings();
		void InitObjects();
		void InitZones();
		void InitSelectableMapList();
		void InitBots();
		void StartBot(int bot_team);
		void KillBot(int bot_team);
		void KillAllBots();
		void ProcessObjects();
		void ProcessPathFindingResults();
		void SendNews(int player, const char* message, char r=0, char g=0, char b=0);
		void BroadCastNews(const char* message, char r=0, char g=0, char b=0);
		void RelayObjectWayPoints(ZObject *obj);
		void RelayObjectRallyPoints(ZObject *obj, int player = -1);
		void RelayTeamMessage(team_type the_team, int pack_id, const char *data, int size);
		//void SetupFlagList();
		void CheckFlagCaptures();
		void CheckPlayerSuggestions();
		void AwardZone(OFlag *flag, ZObject *conquerer);
		void AwardZone(OFlag *flag, team_type new_team);
		void ResetObjectTeam(ZObject *obj, team_type new_team);
		void DeleteObject(ZObject *obj);
		bool ReadMapList();
		bool ReadSelectableMapList();
		bool ReadSelectableMapListFromFolder(string foldername = "");
		int NextInMapList();
		void ProcessEndGame();
		void CheckEndGame();
		void CheckResetGame();
		void DoResetGame(string map_name = "");
		bool EndGameRequirementsMet();
		void LoadNextMap(string map_name = "");
		void ResetGame();
		vector<ZObject*>::iterator DeleteObject(vector<ZObject*>::iterator obj);
		void ProcessMissiles();
		void ProcessMissileDamage(damage_missile &the_missile);
		void UpdateObjectHealth(ZObject *obj, int attacker_ref_id = -1);
		void UpdateObjectDriverHealth(ZObject *obj);
		void RelayObjectHealth(ZObject *obj, int player = -1);
		void RelayObjectAttackObject(ZObject *obj);
		void RelayBuildingState(ZObject *obj, int player = -1);
		void RelayBuildingBuiltCannons(ZObject *obj);
		ZObject *BuildingCreateUnit(ZObject *obj, unsigned char ot, unsigned char oid);
		ZObject *BuildingRepairUnit(ZObject *obj, unsigned char ot, unsigned char oid, int driver_type, vector<driver_info_s> &driver_info, vector<waypoint> &rwaypoint_list);
		void BuildingCreateCannon(ZObject *obj, unsigned char oid);
		ZObject *CreateObject(unsigned char ot, unsigned char oid, int x, int y, int owner, vector<ZObject*> *obj_list = NULL, int blevel = 0, unsigned short extra_links = 0, int health_percent = 100);
		ZObject *CreateRobotGroup(unsigned char oid, int x, int y, int owner, vector<ZObject*> *obj_list = NULL, int robot_amount = -1, int health_percent = 100);
		void RelayNewObject(ZObject *obj, int player = -1);
		void ProcessChangeObjectAmount();
		void ScuffleUnits();
		void CheckObjectWaypoints(ZObject *obj);
		void RelayObjectManufacturedSound(ZObject *building_obj, ZObject *obj);
		void RemoveObjectFromGroup(ZObject *obj);
		void CheckNoUnitsDestroyFort(int team);
		void CheckDestroyedFort(ZObject *obj);
		void CheckDestroyedBridge(ZObject *obj);
		void ResetZoneOwnagePercentages(bool notify_players = false);
		void RelayObjectLoc(ZObject *obj);
		void RelayObjectDeath(ZObject *obj, int killer_ref_id = -1);
		void RobotEnterObject(ZObject *robot_obj, ZObject *dest_obj);
		void UnitEnterRepairBuilding(ZObject *unit_obj, ZObject *rep_building_obj);
		bool CheckMapObject(map_object &m_obj);
		void ProcessPlayerCommand(int player, string command);
		bool LoginPlayer(int player, ZMysqlUser &user);
		bool LogoutPlayer(int player, bool connection_exists = true);
		void LogoutOthers(int db_id);
		void RelayLAdd(int player, int to_player = -1);
		void RelayLPlayerName(int player, int to_player = -1);
		void RelayLPlayerTeam(int player, int to_player = -1);
		void RelayLPlayerMode(int player, int to_player = -1);
		void RelayLPlayerIgnored(int player, int to_player = -1);
		void RelayLPlayerLoginInfo(int player, int to_player = -1);
		void RelayLPlayerVoteChoice(int player, int to_player = -1);
		bool LoginCheckDenied(int player);
		bool ActivationCheckPassed(int player);
		void UpdateUsersWinLoses();
		void RelayTeamsWonEnded();
		void InitOnlineHistory();
		void CheckUpdateOnlineHistory();
		void CheckOnlineHistoryPlayerCount();
		void CheckOnlineHistoryTrayPlayerCount();
		void AwardAffiliateCreation(string ip);
		void PauseGame();
		void ResumeGame();
		void RelayGamePaused(int player = -1);
		void RelayGameSpeed(int player = -1);
		bool VoteRequired();
		bool StartVote(int vote_type, int value = -1, int player = -1);
		bool VoteYes(int player);
		bool VoteNo(int player);
		bool VotePass(int player);
		void CheckVote();
		void CheckVoteExpired();
		void RelayVoteInfo(int player = -1);
		void ClearPlayerVotes();
		void KillVote();
		void ProcessVote(int vote_type, int value);
		bool CanVote(int player);
		void GivePlayerID(int player);
		void GivePlayerSelectableMapList(int player);
		bool TeamHasBot(int team, bool active_only = false);
		void SetTeamsBotsIgnored(int team, bool ignored);
		void AttemptPlayerLogin(int player, string loginname, string password);
		void AttemptCreateUser(int player, string username, string loginname, string password, string email);
		void SendPlayerLoginRequired(int player);
		void MakeAllFortTurretsUnEjectable();
		void RelayObjectGrenadeAmount(ZObject *obj, int player = -1);
		void RelayPortraitAnim(int ref_id, int anim_id);
		void RelayTeamEnded(int team, bool won);
		bool ChangePlayerTeam(int player, int team);
		void ReshuffleTeams();
		bool SuggestReshuffleTeams();
		void ChangeGameSpeed(float new_speed);
		void RelayObjectBuildingQueue(ZObject *obj, int player = -1);
		void RelayObjectGroupInfo(ZObject *obj, int player = -1);
		void RelayVersion(int player = -1);
		
		string map_name;
		string map_list_name;
		string settings_filename;
		string p_settings_filename;
		vector<string> map_list;
		bool load_maps_randomly;
		int current_map_i;

		double next_scuffle_time;
		double next_end_game_check_time;
		double next_reset_game_time;
		double next_make_suggestions_time;
		bool do_reset_game;

		//perpetual server settings
		ZPSettings psettings;
		ZMysql zmysql;

		//online history
		double next_online_history_time;
		int next_online_history_player_count;
		int next_online_history_tray_player_count;

		//variable used to let you know that the map etc is loaded
		//and the game is being played normally
		bool game_on;
		
		EventHandler<ZServer> ehandler;
		
		ServerSocket server_socket;

		ZPath_Finding_Engine path_finder;
		
		//vector<p_info> player_info;s
		//vector<ZObject*> flag_object_list;
		vector<damage_missile> damage_missile_list;
		vector<damage_missile> new_damage_missile_list;
		vector<ZObject*> new_object_list;

		//bots
		bool init_bot[MAX_TEAM_TYPES];
		SDL_Thread *bot_thread[MAX_TEAM_TYPES];
		ZBot bot[MAX_TEAM_TYPES];
		
		int next_ref_id;

		//player commands
		void PlayerCommand_NotFound(int player, string contents);
		void PlayerCommand_Help(int player, string contents);
		void PlayerCommand_ListCommands(int player, string contents);
		void PlayerCommand_Login(int player, string contents);
		void PlayerCommand_Logout(int player, string contents);
		void PlayerCommand_CreateUser(int player, string contents);
		void PlayerCommand_PauseGame(int player, string contents);
		void PlayerCommand_ResumeGame(int player, string contents);
		void PlayerCommand_ListMaps(int player, string contents);
		void PlayerCommand_ChangeMap(int player, string contents);
		void PlayerCommand_StartBot(int player, string contents);
		void PlayerCommand_StopBot(int player, string contents);
		void PlayerCommand_PlayerInfo(int player, string contents);
		void PlayerCommand_CurrentMap(int player, string contents);
		void PlayerCommand_ResetGame(int player, string contents);
		void PlayerCommand_ChangeTeam(int player, string contents);
		void PlayerCommand_ReshuffleTeams(int player, string contents);
		void PlayerCommand_BuyRegistration(int player, string contents);
		void PlayerCommand_ChangeSpeed(int player, string contents);
		void PlayerCommand_Version(int player, string contents);

		//events
		static void test_event(ZServer *p, char *data, int size, int player);
		static void connect_event(ZServer *p, char *data, int size, int player);
		static void disconnect_event(ZServer *p, char *data, int size, int player);
		static void request_map_event(ZServer *p, char *data, int size, int player);
		static void set_name_event(ZServer *p, char *data, int size, int player);
		static void send_object_list_event(ZServer *p, char *data, int size, int player);
		static void send_zone_info_list_event(ZServer *p, char *data, int size, int player);
		static void set_player_name_event(ZServer *p, char *data, int size, int player);
		static void set_player_team_event(ZServer *p, char *data, int size, int player);
		static void rcv_object_waypoints_event(ZServer *p, char *data, int size, int player);
		static void rcv_object_rallypoints_event(ZServer *p, char *data, int size, int player);
		static void start_building_event(ZServer *p, char *data, int size, int player);
		static void stop_building_event(ZServer *p, char *data, int size, int player);
		static void place_cannon_event(ZServer *p, char *data, int size, int player);
		static void relay_chat_event(ZServer *p, char *data, int size, int player);
		static void exit_vehicle_event(ZServer *p, char *data, int size, int player);
		static void request_settings_event(ZServer *p, char *data, int size, int player);
		static void set_player_mode_event(ZServer *p, char *data, int size, int player);
		static void request_player_list_event(ZServer *p, char *data, int size, int player);
		static void bot_login_bypass_event(ZServer *p, char *data, int size, int player);
		static void get_pause_game_event(ZServer *p, char *data, int size, int player);
		static void set_pause_game_event(ZServer *p, char *data, int size, int player);
		static void start_vote_event(ZServer *p, char *data, int size, int player);
		static void vote_yes_event(ZServer *p, char *data, int size, int player);
		static void vote_no_event(ZServer *p, char *data, int size, int player);
		static void vote_pass_event(ZServer *p, char *data, int size, int player);
		static void request_player_id_event(ZServer *p, char *data, int size, int player);
		static void request_selectable_map_list_event(ZServer *p, char *data, int size, int player);
		static void player_login_event(ZServer *p, char *data, int size, int player);
		static void request_login_required_event(ZServer *p, char *data, int size, int player);
		static void create_user_event(ZServer *p, char *data, int size, int player);
		static void buy_regkey_event(ZServer *p, char *data, int size, int player);
		static void get_game_speed_event(ZServer *p, char *data, int size, int player);
		static void set_game_speed_event(ZServer *p, char *data, int size, int player);
		static void add_building_queue_event(ZServer *p, char *data, int size, int player);
		static void cancel_building_queue_event(ZServer *p, char *data, int size, int player);
		static void reshuffle_teams_event(ZServer *p, char *data, int size, int player);
		static void start_bot_event(ZServer *p, char *data, int size, int player);
		static void stop_bot_event(ZServer *p, char *data, int size, int player);
		static void select_map_event(ZServer *p, char *data, int size, int player);
		static void reset_map_event(ZServer *p, char *data, int size, int player);
		static void request_version_event(ZServer *p, char *data, int size, int player);
};

#endif
