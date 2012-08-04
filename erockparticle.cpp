#include "erockparticle.h"

bool ERockParticle::finished_init = false;
ZSDL_Surface ERockParticle::debri_mid0_img[MAX_PLANET_TYPES][8];
ZSDL_Surface ERockParticle::debri_mid1_img[MAX_PLANET_TYPES][8];
ZSDL_Surface ERockParticle::debri_small_img[MAX_PLANET_TYPES][18];

ERockParticle::ERockParticle(ZTime *ztime_, int x_, int y_, int palette_, int particle_type_, int max_horz, int max_vert) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	palette = palette_;
	particle_type = particle_type_;

	switch(particle_type)
	{
	case ROCK_SMALL_PARTICLE:
		render_img = debri_small_img[palette];
		max_render_i = 16;
		break;
	case ROCK_MID_PARTICLE:
		if(rand() % 2)
			render_img = debri_mid0_img[palette];
		else
			render_img = debri_mid1_img[palette];
		max_render_i = 8;
		break;
	}
	

	//....
	double lifetime;

	lifetime = 1.1 + (0.1 * (rand() % 10));

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

	next_process_time = init_time + 0.07;
}

void ERockParticle::Init()
{
	char filename_c[500];
	int i, j;

	for(i=0;i<MAX_PLANET_TYPES;i++)
	{
		for(j=0;j<8;j++)
		{
			sprintf(filename_c, "assets/planets/rock_effects/debri_mid0_%s_n%02d.png", planet_type_string[i].c_str(), j);
			debri_mid0_img[i][j].LoadBaseImage(filename_c);

			sprintf(filename_c, "assets/planets/rock_effects/debri_mid1_%s_n%02d.png", planet_type_string[i].c_str(), j);
			debri_mid1_img[i][j].LoadBaseImage(filename_c);
		}

		for(j=0;j<16;j++)
		{
			sprintf(filename_c, "assets/planets/rock_effects/debri_small_%s_n%02d.png", planet_type_string[i].c_str(), j);
			debri_small_img[i][j].LoadBaseImage(filename_c);
		}
	}

	finished_init = true;
}

void ERockParticle::Process()
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
		if(render_i >=6) render_i = 0;

		next_process_time = the_time + 0.07;
	}

	{
		double time_dif = (the_time - init_time);
		double up_amount;

		//move
		x = sx + (dx * time_dif);
		y = sy + (dy * time_dif);

		size = -(rise / (final_time - init_time)) * (time_dif * time_dif) + rise * time_dif;
		size += 1;
		y -= (size-1) * 150;
	}
}

void ERockParticle::DoRender(ZMap &zmap, SDL_Surface *dest)
{
	SDL_Rect from_rect, to_rect;
	//SDL_Surface *surface;

	if(killme) return;

	//surface = render_img[render_i].GetImage(size);
	render_img[render_i].SetSize(size);

	//if(!surface) return;

	zmap.RenderZSurface(&render_img[render_i], x, y, false, true);
	//if(zmap.GetBlitInfo( surface, x - (surface->w >> 1), y - (surface->h >> 1), from_rect, to_rect))
	//	SDL_BlitSurface( surface, &from_rect, dest, &to_rect);
}
