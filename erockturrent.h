#ifndef _EROCKTURRENT_H_
#define _EROCKTURRENT_H_

#include "zeffect.h"

class ERockTurrent : ZEffect
{
	public:
		ERockTurrent(ZTime *ztime_, int x_, int y_, int palette_, int max_horz, int max_vert);

		static void Init();

		void Process();
		void DoRender(ZMap &zmap, SDL_Surface *dest);
	private:
		static bool finished_init;
		static ZSDL_Surface debri_large_img[2][MAX_PLANET_TYPES][12];

		void EndExplosion();

		int palette;
		int large_i;

		int x, y;
		int ex, ey;
		int sx, sy;
		double dx, dy;
		double next_process_time;
		double init_time, final_time;
		int rise;

		int render_i;
		double size;
		double dangle;
		int angle;
};

#endif
