#ifndef _EMISSILECROCKETS_H_
#define _EMISSILECROCKETS_H_

#include "zeffect.h"

class EMissileCRockets : ZEffect
{
	public:
		EMissileCRockets(ZTime *ztime_, int start_x, int start_y, int dest_x, int dest_y);

		static void Init();

		void Process();
		void DoRender(ZMap &zmap, SDL_Surface *dest);
	private:
		static bool finished_init;

		void PlaceSmoke(double &the_time);
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

		int other_x_shift, other_y_shift;

		int angle;
		
};

#endif
