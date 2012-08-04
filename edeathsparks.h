#ifndef _EDEATHSPARKS_H_
#define _EDEATHSPARKS_H_

#include "zeffect.h"

class EDeathSparks : ZEffect
{
	public:
		EDeathSparks(ZTime *ztime_, int x_, int y_);

		static void Init();

		void Process();
		void DoRender(ZMap &zmap, SDL_Surface *dest);
	private:
		static bool finished_init;
		static ZSDL_Surface base_img[6];

		int x, y;
		int ex, ey;
		int sx, sy;
		double dx, dy;
		double next_process_time;
		double init_time, final_time;
		int rise;

		int render_i;
		double size;
};

#endif
