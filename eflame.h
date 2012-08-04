#ifndef _EFLAME_H_
#define _EFLAME_H_

#include "zeffect.h"

class EFlame : ZEffect
{
	public:
		EFlame(ZTime *ztime_, int start_x, int start_y, int dest_x, int dest_y);

		static void Init();

		void Process();
		void DoRender(ZMap &zmap, SDL_Surface *dest);
	private:
		static bool finished_init;
		static ZSDL_Surface flame_bullet[4];
		ZSDL_Surface bullet_img[4];

		double init_time, final_time;
		int x, y;
		double dx, dy;
		int sx, sy;
		int ex, ey;

		int bullet_i;
		int angle;
		
};

#endif
