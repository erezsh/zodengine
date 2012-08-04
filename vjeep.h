#ifndef _VJEEP_H_
#define _VJEEP_H_

#include "zvehicle.h"

class VJeep : public ZVehicle
{
	public:
		VJeep(ZTime *ztime_, ZSettings *zsettings_ = NULL);
		
		static void Init();
		
// 		SDL_Surface *GetRender();
		int Process();
		void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		void DoAfterEffects(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y);
		void DoDeathEffect(bool do_fire_death, bool do_missile_death);
	private:
		void SetAttackObject(ZObject *obj);
		
		static ZSDL_Surface turrent[MAX_ANGLE_TYPES];
		static ZSDL_Surface fire[MAX_ANGLE_TYPES];
		static ZSDL_Surface base[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][2];
		static ZSDL_Surface under[MAX_ANGLE_TYPES][4];
		static ZSDL_Surface wasted;
		
		int base_i;
		bool renderfire;
		
		double next_base_time;
		double next_turrent_time;
};

#endif
