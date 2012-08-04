#ifndef _EROBOTDEATH_H_
#define _EROBOTDEATH_H_

#include "zeffect.h"

class ERobotDeath : ZEffect
{
	public:
		ERobotDeath(ZTime *ztime_, int x_, int y_, int team, bool do_melt_death);

		static void Init();

		void Process();
		void DoRender(ZMap &zmap, SDL_Surface *dest);
	private:
		static bool finished_init;
		static ZSDL_Surface die[4][MAX_TEAM_TYPES][10];
		static ZSDL_Surface melt[MAX_TEAM_TYPES][17];
		ZSDL_Surface *render_img;

		int x, y;
		double next_process_time;

		int owner;
		int die_i, render_i;
		int max_render_i;
};

#endif
