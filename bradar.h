#ifndef _BRADAR_H_
#define _BRADAR_H_

#include "zbuilding.h"

class BRadar : public ZBuilding
{
	public:
		BRadar(ZTime *ztime_, ZSettings *zsettings_ = NULL, planet_type palette_ = DESERT);
		
		static void Init();
		
// 		SDL_Surface *GetRender();
		void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		void DoAfterEffects(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y);
		int Process();
		void DoDeathEffect(bool do_fire_death, bool do_missile_death);
		void SetMapImpassables(ZMap &tmap);

		bool GetCraneEntrance(int &x, int &y, int &x2, int &y2);
		bool GetCraneCenter(int &x, int &y);
	private:
		static ZSDL_Surface base[MAX_PLANET_TYPES][MAX_TEAM_TYPES];
		static ZSDL_Surface base_destroyed[MAX_PLANET_TYPES];
		static ZSDL_Surface base_color[MAX_TEAM_TYPES];
		
		static ZSDL_Surface box_spinner[12];
		static ZSDL_Surface dish[8];
		static ZSDL_Surface front_light[2];
		static ZSDL_Surface side_light[2];
		
		int front_light_i;
		int side_light_i;
		int box_spinner_i;
		int dish_i;
};

#endif
