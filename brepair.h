#ifndef _BREPAIR_H_
#define _BREPAIR_H_

#include "zbuilding.h"

class BRepair : public ZBuilding
{
	public:
		BRepair(ZTime *ztime_, ZSettings *zsettings_ = NULL, planet_type palette_ = DESERT);
		
		static void Init();
		
// 		SDL_Surface *GetRender();
		void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		void DoAfterEffects(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y);
		int Process();
		void DoDeathEffect(bool do_fire_death, bool do_missile_death);
		void SetMapImpassables(ZMap &tmap);

		bool GetCraneEntrance(int &x, int &y, int &x2, int &y2);
		bool GetCraneCenter(int &x, int &y);
		bool GetRepairEntrance(int &x, int &y);
		bool GetRepairCenter(int &x, int &y);
		bool CanRepairUnit(int units_team);
		bool RepairingAUnit();
		bool SetRepairUnit(ZObject *unit_obj);
		void CreateRepairAnimData(char *&data, int &size, bool play_sound = true);
		void DoRepairBuildingAnim(bool on_, double remaining_time_);
		bool RepairUnit(double &the_time, unsigned char &ot, unsigned char &oid, int &driver_type_, vector<driver_info_s> &driver_info_, vector<waypoint> &rwaypoint_list_);
	private:
		static ZSDL_Surface base[MAX_PLANET_TYPES][MAX_TEAM_TYPES];
		static ZSDL_Surface base_destroyed[MAX_PLANET_TYPES];
		static ZSDL_Surface base_color[MAX_TEAM_TYPES];
		
		static ZSDL_Surface smoke_stack[5];
		static ZSDL_Surface text_box[3];
		static ZSDL_Surface side_light[2];
		static ZSDL_Surface front_light[2];
		static ZSDL_Surface bulb[2];
		
		int smoke_stack_i;
		int text_box_i;
		int side_light_i;
		int front_light_i;
		int bulb_i;

		//repair info
		bool repairing_unit;
		double repair_time;
		unsigned char rot, roid;
		vector<waypoint> rwaypoint_list;
};

#endif
