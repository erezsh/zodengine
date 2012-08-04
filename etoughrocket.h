#ifndef _ETOUGHROCKET_H_
#define _ETOUGHROCKET_H_

#include "zeffect.h"

class EToughRocket : ZEffect
{
	public:
		EToughRocket(ZTime *ztime_, int start_x, int start_y, int dest_x, int dest_y);

		static void Init();

		void Process();
		void DoRender(ZMap &zmap, SDL_Surface *dest);
	private:
		static bool finished_init;

		void PlaceSmoke(double &the_time);
		static ZSDL_Surface the_bullet[2];
		ZSDL_Surface bullet_img[2];

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
		
};

#endif
