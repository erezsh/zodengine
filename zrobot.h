#ifndef _ZROBOT_H_
#define _ZROBOT_H_

#include "zobject.h"

class ZRobot : public ZObject
{
	public:
		ZRobot(ZTime *ztime_, ZSettings *zsettings_ = NULL);
		
		static void Init();
		virtual void SetAttackObject(ZObject *obj);
		virtual void DoDeathEffect(bool do_fire_death, bool do_missile_death);
		virtual void FireMissile(int x_, int y_);
		virtual bool CanSetWaypoints() { return true; }
		virtual bool CanPickupGrenades() { return grenade_amount <= 0; }
		virtual bool CanHaveGrenades() { return true; }
		virtual void SetGrenadeAmount(int grenade_amount_);
		virtual int GetGrenadeAmount() { return grenade_amount; }
		virtual bool CanThrowGrenades();
		virtual void DoPickupGrenadeAnim();
	protected:
		void RecalcDirection();
		void Common_Process(double the_time);
		int action_i;
		int grenade_i;
		bool throw_grenade;

		double next_grenade_time;

		int submerge_amount;

		int grenade_amount;
		
		static ZSDL_Surface null_img;
		static ZSDL_Surface dodge[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][2];
		static ZSDL_Surface stand[MAX_TEAM_TYPES][MAX_ANGLE_TYPES];
		static ZSDL_Surface throw_something[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][4];
		static ZSDL_Surface enter_apc[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][5];
		static ZSDL_Surface walk[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][4];
		static ZSDL_Surface look_around[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][3];
		static ZSDL_Surface point[MAX_TEAM_TYPES][MAX_ANGLE_TYPES];
		static ZSDL_Surface beat_ground[MAX_TEAM_TYPES][9];
		static ZSDL_Surface beer[MAX_TEAM_TYPES][10];
		static ZSDL_Surface celebrate[MAX_TEAM_TYPES][4];
		static ZSDL_Surface cigarette[MAX_TEAM_TYPES][11];
		static ZSDL_Surface confused[MAX_TEAM_TYPES][2];
		static ZSDL_Surface escape_tank[MAX_TEAM_TYPES][8];
		static ZSDL_Surface full_area_scan[MAX_TEAM_TYPES][12];
		static ZSDL_Surface head_stretch[MAX_TEAM_TYPES][11];
		static ZSDL_Surface jump_up[MAX_TEAM_TYPES][5];
		static ZSDL_Surface jump_down[MAX_TEAM_TYPES][5];
		static ZSDL_Surface jump_left[MAX_TEAM_TYPES][5];
		static ZSDL_Surface jump_right[MAX_TEAM_TYPES][5];
		static ZSDL_Surface pickup_down[MAX_TEAM_TYPES][4];
		static ZSDL_Surface pickup_up[MAX_TEAM_TYPES][4];
		static ZSDL_Surface pope[MAX_TEAM_TYPES][17];
		static ZSDL_Surface praise_the_lord[MAX_TEAM_TYPES][5];
		
};

#endif
