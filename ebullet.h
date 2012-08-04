#ifndef _EBULLET_H_
#define _EBULLET_H_

#include "zeffect.h"

class EBullet : ZEffect
{
	public:
		EBullet(ZTime *ztime_, team_type team, int start_x, int start_y, int dest_x, int dest_y);

		void Process();
		void DoRender(ZMap &zmap, SDL_Surface *dest);
	private:
		double init_time, final_time;
		int x, y;
		double dx, dy;
		int sx, sy;
		int ex, ey;
		team_type owner;
		//int rgb_map;
};

#endif
