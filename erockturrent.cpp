#include "erockturrent.h"
#include "erockparticle.h"

bool ERockTurrent::finished_init = false;
ZSDL_Surface ERockTurrent::debri_large_img[2][MAX_PLANET_TYPES][12];

ERockTurrent::ERockTurrent(ZTime *ztime_, int x_, int y_, int palette_, int max_horz, int max_vert) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	palette = palette_;

	switch(palette)
	{
	case CITY:
	case DESERT:
		large_i = 0;
		break;
	default:
		large_i = rand() % 2;
		break;
	}

	//....
	double lifetime;

	lifetime = 1.5 + (0.1 * (rand() % 10));

	render_i = 0;
	size = 1.0;

	//16x10

	x = x_ + (rand() % 8);
	y = y_ + (rand() % 24);

	sx = x - 8;
	sy = y - 5;

	ex = x + (max_horz - (rand() % (max_horz + max_horz)));
	ey = y + (max_vert - (rand() % (max_vert + max_vert)));

	rise = (int)(1.1 + (0.01 * (rand() % 200)));

	double mag;
	dx = ex - sx;
	dy = ey - sy;
	mag = sqrt((dx * dx) + (dy * dy));

	init_time = the_time;
	final_time = init_time + lifetime;

	dx /= lifetime;
	dy /= lifetime;

	next_process_time = init_time + 0.07;

	//angle
	angle = 0;
	dangle = 240 - (rand() % 480);
}

void ERockTurrent::Init()
{
	char filename_c[500];
	int i, j, k;

	for(k=0;k<2;k++)
	for(i=0;i<MAX_PLANET_TYPES;i++)
	{
		if(i==DESERT && k==1) continue;
		if(i==CITY && k==1) continue;

		for(j=0;j<12;j++)
		{
			sprintf(filename_c, "assets/planets/rock_effects/debri_large%d_%s_n%02d.png", k, planet_type_string[i].c_str(), j);
			debri_large_img[k][i][j].LoadBaseImage(filename_c);
		}
	}

	finished_init = true;
}

void ERockTurrent::Process()
{
	double &the_time = ztime->ztime;

	if(killme) return;

	if(the_time >= final_time)
	{
		EndExplosion();
		killme = true;
		return;
	}

	if(the_time >= next_process_time)
	{
		render_i++;
		if(render_i >=12) render_i = 0;

		next_process_time = the_time + 0.07;
	}

	{
		double time_dif = (the_time - init_time);
//		double up_amount;

		//move
		x = sx + (int)(dx * time_dif);
		y = sy + (int)(dy * time_dif);

		size = -(rise / (final_time - init_time)) * (time_dif * time_dif) + rise * time_dif;
		size += 1;
		y -= (int)((size-1) * 30);

		angle = (int)(dangle * time_dif);

		while(angle >= 360) angle -= 360;
		while(angle < 0) angle += 360;
	}
}

void ERockTurrent::DoRender(ZMap &zmap, SDL_Surface *dest)
{
//	SDL_Rect from_rect, to_rect;
	//SDL_Surface *surface;

	if(killme) return;

	//surface = debri_large_img[large_i][palette][render_i].GetImage(angle, size);
	debri_large_img[large_i][palette][render_i].SetAngle((float)angle);
	debri_large_img[large_i][palette][render_i].SetSize((float)size);

	//if(!surface) return;

	zmap.RenderZSurface(&debri_large_img[large_i][palette][render_i], x, y, false, true);
	//if(zmap.GetBlitInfo( surface, x - (surface->w >> 1), y - (surface->h >> 1), from_rect, to_rect))
	//	SDL_BlitSurface( surface, &from_rect, dest, &to_rect);
}

void ERockTurrent::EndExplosion()
{
	int small_particles;
	int i;

	small_particles = 12 + (rand() % 6);

	if(effect_list) 
	for(i=0;i<small_particles;i++)
		effect_list->push_back((ZEffect*)(new ERockParticle(ztime, x, y, palette, ROCK_SMALL_PARTICLE, 80, 60)));
}
