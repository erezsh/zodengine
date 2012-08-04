#ifndef _EROCKPARTICLE_H_
#define _EROCKPARTICLE_H_

#include "zeffect.h"

enum rock_particle_type
{
	ROCK_SMALL_PARTICLE, ROCK_MID_PARTICLE
};

class ERockParticle : ZEffect
{
	public:
		ERockParticle(ZTime *ztime_, int x_, int y_, int palette_, int particle_type_, int max_horz, int max_vert);

		static void Init();

		void Process();
		void DoRender(ZMap &zmap, SDL_Surface *dest);
	private:
		static bool finished_init;
		static ZSDL_Surface debri_mid0_img[MAX_PLANET_TYPES][8];
		static ZSDL_Surface debri_mid1_img[MAX_PLANET_TYPES][8];
		static ZSDL_Surface debri_small_img[MAX_PLANET_TYPES][18];

		int palette;
		int particle_type;

		int x, y;
		int ex, ey;
		int sx, sy;
		double dx, dy;
		double next_process_time;
		double init_time, final_time;
		double rise;

		int render_i;
		int max_render_i;
		double size;
		ZSDL_Surface *render_img;
};

#endif
