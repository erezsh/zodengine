#ifndef _VCRANE_H_
#define _VCRANE_H_

#include "zvehicle.h"

class VCrane : public ZVehicle
{
	public:
		VCrane(ZTime *ztime_, ZSettings *zsettings_ = NULL);
		~VCrane();
		
		static void Init();
		
// 		SDL_Surface *GetRender();
		int Process();
		void DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);
		void DoAfterEffects(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y);
		void DoDeathEffect(bool do_fire_death, bool do_missile_death);

		void DoCraneAnim(bool on_, ZObject *rep_obj = NULL);
	private:
		void RenderCrane(ZMap &the_map, SDL_Surface *dest, int shift_x = 0, int shift_y = 0);

		static ZSDL_Surface base[MAX_TEAM_TYPES][MAX_ANGLE_TYPES][3];
		static ZSDL_Surface crane[MAX_ANGLE_TYPES];
		static ZSDL_Surface hook[16];
		static ZSDL_Surface wasted[MAX_TEAM_TYPES];
		
		int hook_i;
		
		double next_turrent_time;
		double next_hook_time;

		bool anim_on;
		ECraneConco *conco_anim;
};

#endif
