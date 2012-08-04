#ifndef _EMOMISSILEROCKETS_H_
#define _EMOMISSILEROCKETS_H_

#include "zeffect.h"

class EMoMissileRockets : ZEffect
{
	public:
		EMoMissileRockets(ZTime *ztime_, int start_x, int start_y, int dest_x, int dest_y);

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

		int left_x_shift, left_y_shift;
		int right_x_shift, right_y_shift;
		int left_x, left_y;
		int right_x, right_y;

		int angle;
		
};

#endif
