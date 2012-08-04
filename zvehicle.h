#ifndef _ZVEHICLE_H_
#define _ZVEHICLE_H_

#include "zobject.h"

class ZVehicle : public ZObject
{
	public:
		ZVehicle(ZTime *ztime_, ZSettings *zsettings_ = NULL);
		
		static void Init();
		virtual void SetAttackObject(ZObject *obj);
		virtual void SetInitialDrivers();
		virtual bool CanBeRepaired();
		virtual void SetLidState(bool lid_open_);
		virtual bool GetLidState();
		virtual void ProcessServerLid();
		virtual void SignalLidShouldOpen();
		virtual void SignalLidShouldClose();
		virtual bool CanBeSniped();
		virtual bool CanSetWaypoints()  { return true; }
		virtual void TryDropTracks();
	protected:
		virtual void RecalcDirection();
		static ZSDL_Surface lid[MAX_ANGLE_TYPES][3];
		static ZSDL_Surface tank_robot[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][2];
		bool ShowDamaged();
		bool ShowPartiallyDamaged();
		virtual void ProcessLid();
		virtual void RenderLid(ZMap &the_map, SDL_Surface *dest, int &lx, int &ly, int &shift_x, int &shift_y);
		
		int t_direction;
		int lid_i;
		int robot_i;
		double next_lid_time;
		bool moving;
		bool lid_open;
		bool show_robot;
		int track_type;

		bool do_open_lid;
		double next_open_lid_time;
		bool do_close_lid;
		double next_close_lid_time;

};

#endif
