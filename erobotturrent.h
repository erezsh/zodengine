#ifndef _EROBOTTURRENT_H_
#define _EROBOTTURRENT_H_

#include "zeffect.h"

class ERobotTurrent : ZEffect
{
	public:
		ERobotTurrent(ZTime *ztime_, int x_, int y_, int owner_, int max_horz = 100, int max_vert = 100);

		static void Init();

		void Process();
		void DoRender(ZMap &zmap, SDL_Surface *dest);
	private:
		static bool finished_init;
		static ZSDL_Surface robot_flip[MAX_TEAM_TYPES][33];

		int owner;

		int x, y;
		int ex, ey;
		int sx, sy;
		double dx, dy;
		double next_render_i_time;
		double init_time, final_time;
		int rise;

		int render_i;
		double size;

};

#endif
