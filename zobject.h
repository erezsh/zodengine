#ifndef _ZOBJECT_H_
#define _ZOBJECT_H_

#include "constants.h"
#include "common.h"
#include "zsdl.h"
#include "zsdl_opengl.h"
#include "zmap.h"
#include "cursor.h"
#include "event_handler.h"
#include "zeffect.h"
#include "zdamagemissile.h"
#include "zbuildlist.h"
#include "zsound_engine.h"
#include "zpath_finding.h"
#include "zsettings.h"
#include "zportrait.h"
#include "zteam.h"
#include "ztime.h"
#include "zolists.h"

//effects
#include "ebullet.h"
#include "elaser.h"
#include "eflame.h"
#include "epyrofire.h"
#include "etoughrocket.h"
#include "etoughmushroom.h"
#include "etoughsmoke.h"
#include "elightrocket.h"
#include "elightinitfire.h"
#include "emomissilerockets.h"
#include "emissilecrockets.h"
#include "erobotdeath.h"
#include "erobotturrent.h"
#include "eunitparticle.h"
#include "edeath.h"
#include "estandard.h"
#include "edeathsparks.h"
#include "eturrentmissile.h"
#include "esideexplosion.h"
#include "ecannondeath.h"
#include "erockparticle.h"
#include "erockturrent.h"
#include "emapobjectturrent.h"
#include "ebridgeturrent.h"
#include "ecraneconco.h"
#include "etrack.h"
#include "etankdirt.h"
#include "etanksmoke.h"
#include "etankoil.h"
#include "etankspark.h"

//gui
//#include "zgui_window.h"
//#include "gwproduction.h"
class ZGuiWindow;
class ZPortrait;

#include <string>
#include <vector>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

using namespace std;

enum object_mode
{
	NULL_MODE, JUST_PLACED_MODE, ROTATING_MODE, 
	STATIONARY_MODE, R_WALKING, R_STANDING, R_CIGARETTE, R_FULLSCAN, 
	R_HEADSTRETCH, R_BEER, R_ATTACKING, R_PICKUP_UP_GRENADES, R_PICKUP_DOWN_GRENADES, 
	C_ATTACKING, 
	MAX_OBJECT_MODES
};

enum waypoint_mode
{
	MOVE_WP, ENTER_WP, ATTACK_WP, FORCE_MOVE_WP, CRANE_REPAIR_WP, 
	UNIT_REPAIR_WP, AGRO_WP, ENTER_FORT_WP, DODGE_WP, PICKUP_GRENADES_WP,
	MAX_WAYPOINT_MODES
};

enum crane_repair_wp_stage
{
	GOTO_ENTRANCE_CRWS, ENTER_BUILDING_CRWS, EXIT_BUILDING_CRWS
};

enum unit_repair_wp_stage
{
	GOTO_ENTRANCE_URWS, ENTER_BUILDING_URWS, EXIT_BUILDING_URWS, WAIT_URWS
};

enum agro_wp_stage
{
	ATTACK_AWS, RETURN_URWS
};

enum enter_fort_wp_stage
{
	GOTO_ENTRANCE_EFWS, ENTER_BUILDING_EFWS, EXIT_BUILDING_EFWS
};

class waypoint_information
{
public:
	int stage;
	int x, y;

	int sx, sy;
	int adx, ady;

	int crane_exit_x, crane_exit_y;
	int agro_center_x, agro_center_y;
	int fort_exit_x, fort_exit_y;
	int init_attack_x, init_attack_y;

	//pathfinding stuff
	int path_finding_id;
	bool got_pf_response;
	vector<ZPath_Finding_AStar::pf_point> pf_point_list;

	waypoint_information() {clear();}

	void clear()
	{
		stage = 0;
		x = 0;
		y = 0;
		sx = 0;
		sy = 0;
		adx = 0;
		ady = 0;
		crane_exit_x = 0;
		crane_exit_y = 0;
		agro_center_x = 0;
		agro_center_y = 0;
		fort_exit_x = 0;
		fort_exit_y = 0;
		init_attack_x = 0;
		init_attack_y = 0;
		path_finding_id = 0;
		got_pf_response = false;
		pf_point_list.clear();
	}

	void clear_and_kill(ZMap &tmap)
	{
		if(path_finding_id) tmap.GetPathFinder().Kill_Thread_Id(path_finding_id);
		clear();
	}
};

//these transfer packets must have a uniform makeup / size
#pragma pack(1)

class waypoint
{
public:
	char mode;
	int ref_id;
	int x, y;
	bool attack_to;
	bool player_given;

	waypoint() { clear(); }

	void clear()
	{
		mode = -1;
		ref_id = -1;
		x = y = 0;
		attack_to = false;
		player_given = false;
	}

	bool operator==(const waypoint &other) const 
	{
		if(this == &other) return true;

		if(other.mode != mode) return false;
		if(other.ref_id != ref_id) return false;
		if(other.x != x) return false;
		if(other.y != y) return false;
		if(other.attack_to != attack_to) return false;
		if(other.player_given != player_given) return false;

		return true;
	}

	bool operator!=(const waypoint &other) const 
	{
		return !(*this == other);
	}
};

struct object_location
{
	int x, y;
	float dx, dy;
};

struct fire_missile_info
{
	double missile_offset_time;
	int missile_x, missile_y;
};

struct driver_info_s
{
	int driver_health;
	double next_attack_time;
};

#pragma pack()

class ZObject;

class server_flag
{
public:
	server_flag() {clear();}
	
	bool updated_location;
	bool updated_velocity;
	bool updated_waypoints;
	bool updated_attack_object;
	bool updated_attack_object_health;
	bool updated_attack_object_driver_health;
	bool updated_open_lid;
	bool fired_missile;
	int missile_x, missile_y;
	//ZObject* entered_target;
	int entered_target_ref_id;
	bool build_unit;
	unsigned char bot, boid;
	bool auto_repair;
	bool set_crane_anim;
	bool crane_anim_on;
	int crane_rep_ref_id;
	//ZObject* entered_repair_building;
	int entered_repair_building_ref_id;
	bool repair_unit;
	unsigned char rot, roid;
	int rdriver_type;
	vector<driver_info_s> rdriver_info;
	vector<waypoint> rwaypoint_list;
	bool recheck_lid_status;
	//ZObject* destroy_fort_building;
	int destroy_fort_building_ref_id;
	bool updated_grenade_amount;
	bool updated_leader_grenade_amount;
	//ZObject* delete_grenade_box;
	int delete_grenade_box_ref_id;
	bool do_pickup_grenade_anim;
	int portrait_anim_ref_id;
	int portrait_anim_value;

	void clear()
	{
		updated_location = false;
		updated_velocity = false;
		updated_waypoints = false;
		updated_attack_object = false;
		updated_attack_object_health = false;
		updated_attack_object_driver_health = false;
		updated_open_lid = false;
		fired_missile = false;
		build_unit = false;
		//entered_target = NULL;
		entered_target_ref_id = -1;
		auto_repair = false;
		set_crane_anim = false;
		crane_anim_on = false;
		crane_rep_ref_id = -1;
		//entered_repair_building = NULL;
		entered_repair_building_ref_id = -1;
		repair_unit = false;
		//destroy_fort_building = NULL;
		destroy_fort_building_ref_id = -1;
		updated_grenade_amount = false;
		updated_leader_grenade_amount = false;
		//delete_grenade_box = NULL;
		delete_grenade_box_ref_id = -1;
		do_pickup_grenade_anim = false;
		portrait_anim_ref_id = -1;
		portrait_anim_value = -1;
	}
};

class ZObject
{
	public:
		ZObject(ZTime *ztime_, ZSettings *zsettings_ = NULL);
		virtual ~ZObject();

		static void Init(TTF_Font *ttf_font);
		static void SetDamageMissileList(vector<damage_missile> *damage_missile_list_) { damage_missile_list = damage_missile_list_; }
		
		string GetObjectName();
		void SetMap(ZMap *zmap_);
		void SetEffectList(vector<ZEffect*> *effect_list_) { effect_list = effect_list_; }
		virtual void SetOwner(team_type owner_);
		inline team_type GetOwner() { return owner; }
		inline void GetDimensions(int &w, int &h) { w = width; h = height; }
		inline void GetDimensionsPixel(int &w_pix, int &h_pix) { w_pix = width_pix; h_pix = height_pix; }
		virtual int Process();
		int ProcessObject();
// 		virtual SDL_Surface *GetRender();
		void RenderWaypointLine(int sx, int sy, int ex, int ey, int view_h, int view_w);
		virtual void DoRenderWaypoints(ZMap &the_map, SDL_Surface *dest, vector<ZObject*> &object_list, bool is_rally_points = false, int shift_x = 0, int shift_y = 0);
		virtual void DoPreRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		virtual void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		virtual void DoAfterEffects(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		void SetDirection(int direction_);
		virtual void PlaySelectedWav();
		virtual void PlaySelectedAnim(ZPortrait &portrait);
		virtual void PlayAcknowledgeWav();
		virtual void PlayAcknowledgeAnim(ZPortrait &portrait, bool no_way = false);
		bool IntersectsObject(ZObject &obj);
		double DistanceFromObject(ZObject &obj);
		double DistanceFromCoords(int x, int y);
		ZObject* NearestObjectFromList(vector<ZObject*> &the_list);
		static void RemoveObjectFromList(ZObject* the_object, vector<ZObject*> &the_list);
		static ZObject* NextSelectableObjectAboveID(vector<ZObject*> &the_list, int unit_type, int only_team, int min_ref_id);
		static ZObject* NearestSelectableObject(vector<ZObject*> &the_list, int unit_type, int only_team, int x, int y);
		static ZObject* NearestObjectToCoords(vector<ZObject*> &the_list, int x, int y);
		static void ClearAndDeleteList(vector<ZObject*> &the_list);
		static void ProcessList(vector<ZObject*> &the_list);
		virtual void FireMissile(int x_, int y_);
		virtual void FireTurrentMissile(int x_, int y_, double offset_time);
		virtual ZGuiWindow *MakeGuiWindow();
		void SetBuildList(ZBuildList *buildlist_);
		void SetUnitLimitReachedList(bool *unit_limit_reached_);
		virtual void SetLevel(int level_);
		virtual int GetLevel();
		virtual int GetBuildState();
		virtual void AddGroupMinion(ZObject *obj);
		virtual void RemoveGroupMinion(ZObject *obj);
		virtual void SetGroupLeader(ZObject *obj);
		virtual ZObject* GetGroupLeader();
		virtual vector<ZObject*> &GetMinionList();
		virtual void ClearGroupInfo();
		virtual void CreateGroupInfoData(char *&data, int &size);
		virtual void ProcessGroupInfoData(char *data, int size, vector<ZObject*> &object_list);
		virtual void CloneMinionWayPoints();
		virtual bool IsMinion();
		virtual bool IsApartOfAGroup();
		virtual bool CanAttack();
		virtual bool HasExplosives();
		virtual bool AttackedOnlyByExplosives();
		virtual bool CanBeRepairedByCrane(int repairers_team);
		virtual bool GetCraneEntrance(int &x, int &y, int &x2, int &y2);
		virtual bool GetCraneCenter(int &x, int &y);
		virtual void SetDriverType(int driver_type_);
		virtual void AddDriver(int driver_health_);
		virtual void AddDriver(driver_info_s new_driver);
		virtual vector<driver_info_s> &GetDrivers();
		virtual void ClearDrivers();
		virtual int GetDriverType();
		virtual int GetDriverHealth();
		virtual bool CanEjectDrivers();
		virtual void SetInitialDrivers();
		virtual void SetEjectableCannon(bool ejectable_);
		virtual void ResetDamageInfo();
		virtual unsigned short GetExtraLinks();
		void DoHitEffect();
		virtual void DoCraneAnim(bool on_, ZObject *rep_obj = NULL);
		virtual void DoRepairBuildingAnim(bool on_, double remaining_time_);
		virtual bool CanBeRepaired();
		virtual bool CanRepairUnit(int units_team);
		virtual bool RepairingAUnit();
		virtual bool GetRepairEntrance(int &x, int &y);
		virtual bool GetRepairCenter(int &x, int &y);
		virtual bool SetRepairUnit(ZObject *unit_obj);
		virtual void SetLidState(bool lid_open_);
		virtual bool GetLidState();
		virtual bool CanSnipe();
		virtual bool CanBeSniped();
		virtual bool HasLid();
		bool HasProcessedDeath();
		void SetHasProcessedDeath(bool processed_death_);
		void DoDriverHitEffect();
		virtual bool CanBeEntered();
		void SetDamagedByFireTime(double the_time);
		void SetDamagedByMissileTime(double the_time);
		double GetDamagedByFireTime();
		double GetDamagedByMissileTime();
		virtual bool CanEnterFort(int team);
		virtual bool CanSetWaypoints();
		virtual bool CanSetRallypoints() { return false; }
		virtual bool CanMove();
		virtual bool CanPickupGrenades() { return false; }
		virtual bool CanHaveGrenades() { return false; }
		virtual void SetGrenadeAmount(int grenade_amount_) { return; }
		virtual int GetGrenadeAmount() { return 0; }
		virtual bool CanThrowGrenades() { return false; }
		virtual void DoPickupGrenadeAnim() { }
		bool CanAttackObject(ZObject *obj);
		virtual void TryDropTracks() {}
		virtual bool AddBuildingQueue(unsigned char ot, unsigned char oid, bool push_to_front = true) { return false; }
		virtual bool CancelBuildingQueue(int list_i, unsigned char ot, unsigned char oid) { return false; }
		virtual bool ProducesUnits() { return false; }
		
		static int DirectionFromLoc(float dx, float dy);
		void SetCords(int x, int y);
		inline void GetCords(int &x, int &y) { x = loc.x; y = loc.y; }
		inline void GetCenterCords(int &x, int &y) { x = center_x; y = center_y; }
		virtual void SetDestroyed(bool is_destroyed);
		void GetObjectID(unsigned char &object_type_, unsigned char &object_id_);
		bool Selectable();
		void SetRefID(int id);
		void ShowWaypoints();
		int GetRefID();
		vector<waypoint> &GetWayPointList();
		vector<waypoint> &GetWayPointDevList();
		vector<waypoint> &GetRallyPointList() { return rallypoint_list; }
		void RenderHover(ZMap &zmap, SDL_Surface *dest, team_type viewers_team);
		void RenderHealth(ZMap &zmap, SDL_Surface *dest);
		void RenderAttackRadius(ZMap &zmap, SDL_Surface *dest, vector<ZObject*> &avoid_list);
		virtual void RenderSelection(ZMap &zmap, SDL_Surface *dest);
		virtual bool UnderCursor(int &map_x, int &map_y);
		virtual bool UnderCursorCanAttack(int &map_x, int &map_y) { return true; }
		virtual bool UnderCursorFortCanEnter(int &map_x, int &map_y) { return false; }
		virtual bool WithinSelection(int &map_left, int &map_right, int &map_top, int &map_bottom);
		virtual bool CannonNotPlacable(int &map_left, int &map_right, int &map_top, int &map_bottom);
		void SetLoc(object_location new_loc);
		void SetGroup(int group_num_);
		int GetHealth();
		int GetMaxHealth();
		void SetHealth(int new_health, ZMap &tmap);
		void SetHealthPercent(int health_percent, ZMap &tmap);
		int GetInitialHealthPercent() { return initial_health_percent; }
		int DamageHealth(int damage_amount, ZMap &tmap);
		int DamageDriverHealth(int damage_amount);
		virtual void DoDeathEffect(bool do_fire_death, bool do_missile_death);
		virtual void DoReviveEffect();
		virtual bool IsDestroyed();
		bool WithinAgroRadius(int ox, int oy);
		bool WithinAgroRadius(ZObject *obj);
		bool WithinAutoEnterRadius(int ox, int oy);
		bool WithinAutoGrabFlagRadius(int ox, int oy);
		bool WithinAttackRadius(int ox, int oy);
		bool WithinAttackRadius(ZObject *obj);
		bool WithinAttackRadiusOf(vector<ZObject*> &avoid_list, int ox, int oy);
		static ZObject* GetObjectFromID_BS(int ref_id, vector<ZObject*> &the_list);
		static ZObject* GetObjectFromID(int ref_id, vector<ZObject*> &the_list);
		bool StopMove();
		virtual void SetAttackObject(ZObject *obj);
		ZObject* GetAttackObject();
		void RemoveObject(ZObject *obj);
		void SmoothMove(double &the_time);
		ZSDL_Surface &GetHoverNameImg();
		static string GetHoverName(unsigned char ot, unsigned char oid);
		static ZSDL_Surface &GetHoverNameImgStatic(unsigned char ot, unsigned char oid);
		virtual void ProcessSetBuildingStateData(char *data, int size);
		virtual void ProcessSetBuiltCannonData(char *data, int size);
		virtual bool GetBuildingCreationPoint(int &x, int &y);
		virtual bool GetBuildingCreationMovePoint(int &x, int &y);
		virtual bool GetBuildUnit(unsigned char &ot, unsigned char &oid);
		virtual map_zone_info *GetConnectedZone();
		virtual void SetConnectedZone(map_zone_info *connected_zone_);
		virtual void SetConnectedZone(ZMap &the_map);
		void DontStamp(bool dont_stamp_);
		virtual void SetMapImpassables(ZMap &tmap);
		virtual void UnSetMapImpassables(ZMap &tmap);
		virtual void SetDestroyMapImpassables(ZMap &tmap);
		virtual void UnSetDestroyMapImpassables(ZMap &tmap);
		virtual void SetupRockRender(bool **rock_list, int map_w, int map_h);
		virtual bool CanBeDestroyed();
		virtual void CreationMapEffects(ZMap &tmap);
		virtual void DeathMapEffects(ZMap &tmap);
		virtual bool ResetBuildTime(float zone_ownage);
		virtual void SetZoneOwnage(float zone_ownage_) {;}
		virtual bool RecalcBuildTime();
		double SpeedOffsetPercent();
		double SpeedOffsetPercentInv() { return 1.0 / SpeedOffsetPercent(); }
		

		//server only stuff
		virtual int ProcessServer(ZMap &tmap, ZOLists &ols);//vector<ZObject*> &object_list);
		virtual void ProcessServerLid();
		virtual void ProcessAttackDamage(ZMap &tmap, bool attack_player_given);
		virtual void ProcessRunStamina(double time_dif);
		server_flag &GetSFlags();
		void SetVelocity(ZObject *target_object = NULL);
		void CreateLocationData(char *&data, int &size);
		void CreateAttackObjectData(char *&data, int &size);
		void ProcessMoveWP(vector<waypoint>::iterator &wp, double time_dif, bool is_new, ZOLists &ols, ZMap &tmap, bool stoppable);
		void ProcessDodgeWP(vector<waypoint>::iterator &wp, double time_dif, bool is_new, ZOLists &ols, ZMap &tmap);
		void ProcessAttackWP(vector<waypoint>::iterator &wp, double time_dif, bool is_new, ZOLists &ols, ZMap &tmap);
		void ProcessAgroWP(vector<waypoint>::iterator &wp, double time_dif, bool is_new, ZOLists &ols, ZMap &tmap);
		void ProcessPickupWP(vector<waypoint>::iterator &wp, double time_dif, bool is_new, ZOLists &ols, ZMap &tmap);
		void ProcessEnterWP(vector<waypoint>::iterator &wp, double time_dif, bool is_new, ZOLists &ols, ZMap &tmap);
		void ProcessEnterFortWP(vector<waypoint>::iterator &wp, double time_dif, bool is_new, ZOLists &ols, ZMap &tmap);
		void ProcessCraneRepairWP(vector<waypoint>::iterator &wp, double time_dif, bool is_new, ZOLists &ols, ZMap &tmap);
		void ProcessUnitRepairWP(vector<waypoint>::iterator &wp, double time_dif, bool is_new, ZOLists &ols, ZMap &tmap);
		bool CheckAttackTo(vector<waypoint>::iterator &wp, ZOLists &ols);
		void KillWP(vector<waypoint>::iterator &wp);
		bool CanOverwriteWP();
		bool DoAttackImpassableAtCoords(ZOLists &ols, int x, int y);
		bool DodgeMissile(int tx, int ty, double time_till_explode);
		bool ProcessMoveOrKillWP(double time_dif, ZMap &tmap, vector<waypoint>::iterator &wp, ZOLists &ols, bool stoppable = true);
		bool ProcessMove(double time_dif, ZMap &tmap, bool stoppable = true) { int sx, sy; return ProcessMove(time_dif, tmap, sx, sy, stoppable); }
		bool ProcessMove(double time_dif, ZMap &tmap, int &stop_x, int &stop_y, bool stoppable = true);
		bool ReachedTarget();
		void SetTarget(int x, int y);
		void SetTarget() { SetTarget(cur_wp_info.x, cur_wp_info.y); }
		bool KillMe(double the_time);
		void DoKillMe(double killtime = 0);
		void Engage(ZObject *attack_object_);
		bool Disengage();
		void CheckPassiveEngage(double &the_time, ZOLists &ols);
		bool IsMoving();
		//virtual bool ServerFireTurrentMissile(int &x_, int &y_, int &damage, int &radius, double &offset_time);
		virtual vector<fire_missile_info> ServerFireTurrentMissile(int &damage, int &radius);
		virtual bool SetBuildingDefaultProduction() { return false; }
		virtual bool SetBuildingProduction(unsigned char ot, unsigned char oid) { return false; }
		virtual void CreateBuildingStateData(char *&data, int &size) { data = NULL; size = 0; }
		virtual void CreateBuildingQueueData(char *&data, int &size) { data = NULL; size = 0; }
		virtual void ProcessBuildingQueueData(char *data, int size) {}
		virtual bool StopBuildingProduction(bool clear_queue_list = true) { return false; }
		virtual bool BuildUnit(double &the_time, unsigned char &ot, unsigned char &oid);
		virtual bool RepairUnit(double &the_time, unsigned char &ot, unsigned char &oid, int &driver_type_, vector<driver_info_s> &driver_info_, vector<waypoint> &rwaypoint_list_);
		virtual bool StoreBuiltCannon(unsigned char oid);
		virtual void CreateBuiltCannonData(char *&data, int &size) { data = NULL; size = 0; }
		virtual void CreateTeamData(char *&data, int &size);
		virtual void CreateRepairAnimData(char *&data, int &size, bool play_sound = true) { data = NULL; size = 0; }
		virtual void ResetProduction();
		virtual bool RemoveStoredCannon(unsigned char oid);
		virtual bool HaveStoredCannon(unsigned char oid);
		virtual int CannonsInZone(ZOLists &ols);
		virtual vector<unsigned char> &GetBuiltCannonList();
		bool DoAutoRepair(ZMap &tmap, ZOLists &ols);
		void StopAutoRepair();
		bool HasDestroyedFortInZone(ZOLists &ols);
		virtual void ProcessKillObject();
		void InitRealMoveSpeed(ZMap &tmap);
		void PostPathFindingResult(ZPath_Finding_Response* response);
		virtual void SignalLidShouldOpen();
		virtual void SignalLidShouldClose();
		bool EstimateMissileTarget(ZObject *target, int &x, int &y);
		void SetJustLeftCannon(bool just_left_cannon_) { just_left_cannon = just_left_cannon_; }
		virtual bool NearestAttackLoc(int sx, int sy, int &ex, int &ey, int aa_radius, bool is_robot, ZMap &tmap);
		virtual bool IsDestroyableImpass() { return false; }
		virtual bool CausesImpassAtCoord(int x, int y) { return false; }
		double DamagedSpeed();
		double RunSpeed();
		bool CanReachTargetRunning(int x, int y);
		virtual bool ShowDamaged() { return false; }
		virtual bool ShowPartiallyDamaged() { return false; }
		void AttemptStartRun(int x, int y) { if(CanReachTargetRunning(x,y)) AttemptStartRun(); }
		void AttemptStartRun();

		//AI stuff
		vector<ZObject*> &GetAIList() { return ai_list; }
		vector<ZObject*> &GetAIOrigList() { return ai_orig_list; }
		void AddToAIList(ZObject *obj) { ai_list.push_back(obj); obj->GetAIList().push_back(this); }
	private:
		
	protected:
		void InitTypeId(unsigned char ot, unsigned char oid);
		virtual void RecalcDirection();

		static ZSettings default_zsettings;

		static ZSDL_Surface group_tag[10];
		//static Mix_Chunk *selected_wav[6];
		//static Mix_Chunk *selected_robot_wav[MAX_ROBOT_TYPES];
		//static Mix_Chunk *acknowledge_wav[12];

		static vector<damage_missile> *damage_missile_list;

		ZBuildList *buildlist;
		ZSettings *zsettings;
		ZMap *zmap;
		ZTime *ztime;
		vector<ZEffect*> *effect_list;

		bool can_be_destroyed;
		bool destroyed;
		double last_process_time;
		double next_process_time;
		double process_time_int;
		double next_waypoint_time;
		double render_death_time;
		double last_process_server_time;
		double next_move_time;
		double last_radius_time;
		double next_attack_time;
		double last_loc_set_time;
		double next_check_passive_attack_time;
		double next_loc_update_time;
		double loc_update_int;
		object_location loc;
		object_location last_loc;
		int center_x, center_y;
		float xover, yover;
		server_flag sflags;
		int move_speed;
		double real_move_speed;
		int width, height;
		int width_pix, height_pix;
		team_type owner;
		string object_name;
		int mode;
		int direction;
		unsigned char object_type;
		unsigned char object_id;
		bool selectable;
		int ref_id;
		int waypoint_i;
		int move_i;
		int attack_i;
		bool show_waypoints;
		vector<waypoint> waypoint_list;
		vector<waypoint> waypoint_dev_list;
		vector<waypoint> rallypoint_list;
		ZCursor waypoint_cursor;
		waypoint_information cur_wp_info;
		int group_num;
		string hover_name;
		ZSDL_Surface hover_name_img;
		ZSDL_Surface hover_name_star_img;
		int hover_name_x_shift;
		int hover_name_star_x_shift;
		double radius_i;
		int max_health;
		int health;
		map_zone_info *connected_zone;
		bool *unit_limit_reached;
		bool dont_stamp;
		bool has_explosives;
		bool attacked_by_explosives;
		bool do_hit_effect;
		bool do_driver_hit_effect;
		bool can_snipe;
		bool has_lid;
		bool processed_death;
		bool can_be_sniped;
		double last_damaged_by_fire_time;
		double last_damaged_by_missile_time;
		bool just_left_cannon;
		int initial_health_percent;
		double next_drop_track_time;
		double max_stamina;
		double stamina;
		bool is_running;

		//driver
		int driver_type;
		vector<driver_info_s> driver_info;

		//attacking stuff
		ZObject *attack_object;
		double next_damage_time;
		double damage_int_time;
		double damage_chance;
		int damage;
		bool damage_is_missile;
		int damage_radius;
		int missile_speed;
		int attack_radius;
		double snipe_chance;

		//server only stuff
		bool killme;
		double killme_time;
		waypoint last_wp;

		//group stuff
		vector<ZObject*> minion_list;
		ZObject *leader_obj;

		//AI Stuff
		vector<ZObject*> ai_list;
		vector<ZObject*> ai_orig_list;

		bool do_auto_repair;
		double next_auto_repair_time;

		friend bool sort_objects_func (ZObject *a, ZObject *b);
};

extern bool sort_objects_func (ZObject *a, ZObject *b);

#endif
