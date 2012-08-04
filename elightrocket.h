#ifndef _ELIGHTROCKET_H_
#define _ELIGHTROCKET_H_

#include "zeffect.h"

class ELightRocket : ZEffect
{
	public:
		ELightRocket(ZTime *ztime_, int start_x, int start_y, int dest_x, int dest_y, int speed/* = LIGHT_MISSILE_SPEED*/, int extra_small_ = 0, int extra_large_ = 0, int xx_large_ = 0);

		static void Init();

		void Process();
		void DoRender(ZMap &zmap, SDL_Surface *dest);
	private:
		static bool finished_init;
		static ZSDL_Surface the_bullet;
		ZSDL_Surface bullet_img;

		double init_time, final_time;
		int x, y;
		double dx, dy;
		double udx, udy;
		int cx, cy;
		int smx, smy;
		double last_smoke_time;
		int sx, sy;
		int ex, ey;

		int bullet_i;
		int angle;
		int bullet_speed;
		int extra_small, extra_large, xx_large;
		
};

#endif
