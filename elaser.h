#ifndef _ELASER_H_
#define _ELASER_H_

#include "zeffect.h"

class ELaser : ZEffect
{
	public:
		ELaser(ZTime *ztime_, int start_x, int start_y, int dest_x, int dest_y);

		static void Init();

		void Process();
		void DoRender(ZMap &zmap, SDL_Surface *dest);
	private:
		static bool finished_init;
		static ZSDL_Surface laser_bullet[2];
		ZSDL_Surface bullet_img;

		double init_time, final_time;
		int x, y;
		double dx, dy;
		int sx, sy;

		int bullet_i;
		int angle;
		
};

#endif
