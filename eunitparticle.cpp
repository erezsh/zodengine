#include "eunitparticle.h"

ZSDL_Surface EUnitParticle::base_img[20];
bool EUnitParticle::finished_init = false;

EUnitParticle::EUnitParticle(ZTime *ztime_, int x_, int y_, int max_horz, int max_vert) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;
	double lifetime;

	lifetime = 1.4 + (0.1 * (rand() % 10));

	render_i = 0;
	size = 1.0;

	//16x10

	x = x_ + (rand() % 8);
	y = y_ + (rand() % 24);

	sx = x - 8;
	sy = y - 5;

	ex = x + (max_horz - (rand() % (max_horz + max_horz)));
	ey = y + (max_vert - (rand() % (max_vert + max_vert)));

	rise = 1.1 + (0.01 * (rand() % 30));

	double mag;
	dx = ex - sx;
	dy = ey - sy;
	mag = sqrt((dx * dx) + (dy * dy));

	init_time = the_time;
	final_time = init_time + lifetime;

	dx /= lifetime;
	dy /= lifetime;

	next_process_time = init_time + 0.03;
}

void EUnitParticle::Init()
{
	int i;
	char filename_c[500];

	for(i=0;i<20;i++)
		{
			sprintf(filename_c, "assets/other/particles/unit_particle_n%02d.png", i);
			base_img[i].LoadBaseImage(filename_c);
		}
}

void EUnitParticle::Process()
{
	double &the_time = ztime->ztime;

	if(killme) return;

	if(the_time >= final_time)
	{
		killme = true;
		return;
	}

	if(the_time >= next_process_time)
	{
		render_i++;
		if(render_i >=20) render_i = 0;

		next_process_time = the_time + 0.03;
	}

	{
		double time_dif = (the_time - init_time);
		double up_amount;

		//move
		x = sx + (dx * time_dif);
		y = sy + (dy * time_dif);

		size = -(rise / (final_time - init_time)) * (time_dif * time_dif) + rise * time_dif;
		size += 1;
		y -= (size-1) * 65;
		size = 1;
	}
}

void EUnitParticle::DoRender(ZMap &zmap, SDL_Surface *dest)
{
	SDL_Rect from_rect, to_rect;
	//SDL_Surface *surface;

	if(killme) return;

	//surface = base_img[render_i].GetImage(size);
	base_img[render_i].SetSize(size);

	//if(!surface) return;

	zmap.RenderZSurface(&base_img[render_i], x, y);
	//if(zmap.GetBlitInfo( surface, x - (surface->w >> 1), y - (surface->h >> 1), from_rect, to_rect))
	//	SDL_BlitSurface( surface, &from_rect, dest, &to_rect);
}
