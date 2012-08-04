#ifndef _BVEHICLE_H_
#define _BVEHICLE_H_

#include "zbuilding.h"

class BVehicle : public ZBuilding
{
	public:
		BVehicle(ZTime *ztime_, ZSettings *zsettings_ = NULL, planet_type palette_ = DESERT);
		
		static void Init();
		
// 		SDL_Surface *GetRender();
		void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		void DoAfterEffects(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y);
		int Process();
		void DoDeathEffect(bool do_fire_death, bool do_missile_death);
		ZGuiWindow *MakeGuiWindow();
		void SetMapImpassables(ZMap &tmap);
		bool CanSetRallypoints() { return true; }
		bool ProducesUnits() { return true; }

		bool GetCraneEntrance(int &x, int &y, int &x2, int &y2);
		bool GetCraneCenter(int &x, int &y);
	private:
		static ZSDL_Surface base[MAX_PLANET_TYPES][MAX_TEAM_TYPES];
		static ZSDL_Surface base_destroyed[MAX_PLANET_TYPES][MAX_TEAM_TYPES];
		static ZSDL_Surface base_color[MAX_TEAM_TYPES];
		static ZSDL_Surface base_color_destroyed[MAX_TEAM_TYPES];
		
		static ZSDL_Surface spin[8];
		static ZSDL_Surface vent[4];
		static ZSDL_Surface lights[2];
		static ZSDL_Surface bulb[2];
		static ZSDL_Surface tank[2];
		
		int spin_i;
		int vent_i;
		int lights_i;
		int bulb_i;
		int tank_i;
		int exhaust_i;
};

#endif
