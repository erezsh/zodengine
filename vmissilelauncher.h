#ifndef _VMISSILELAUNCHER_H_
#define _VMISSILELAUNCHER_H_

#include "zvehicle.h"

class VMissileLauncher : public ZVehicle
{
	public:
		VMissileLauncher(ZTime *ztime_, ZSettings *zsettings_ = NULL);
		
		static void Init();
		
// 		SDL_Surface *GetRender();
		int Process();
		void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		void DoAfterEffects(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y);
		void FireMissile(int x_, int y_);
		void DoDeathEffect(bool do_fire_death, bool do_missile_death);
	private:
		static ZSDL_Surface base[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][3];
		static ZSDL_Surface top[MAX_TEAM_TYPES][MAX_ANGLE_TYPES];
		static ZSDL_Surface wasted[MAX_TEAM_TYPES];
		
		double next_turrent_time;
};

#endif
