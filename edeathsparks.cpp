#include "edeathsparks.h"

bool EDeathSparks::finished_init = false;
ZSDL_Surface EDeathSparks::base_img[6];

EDeathSparks::EDeathSparks(ZTime *ztime_, int x_, int y_) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;
	double lifetime;
	const int max_up = 70;
	const int max_down = 150;
	const int max_left = 180;
	const int max_right = 180;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	lifetime = 1.5 + (0.1 * (rand() % 3));

	render_i = 0;
	size = 1.0;

	//16x10

	x = x_ + (2 - (rand() % 5));
	y = y_ + (2 - (rand() % 5));

	sx = x - 8;
	sy = y - 5;

	ex = x + (max_right - (rand() % (max_right + max_left)));
	ey = y + (max_down - (rand() % (max_down + max_up)));

	rise = 3 + (0.01 * (rand() % 300));

	double mag;
	dx = ex - sx;
	dy = ey - sy;
	mag = sqrt((dx * dx) + (dy * dy));

	init_time = the_time;
	final_time = init_time + lifetime;

	dx /= lifetime;
	dy /= lifetime;

	next_process_time = init_time + 0.1;
}

void EDeathSparks::Init()
{
	int i;
	char filename_c[500];

	for(i=0;i<6;i++)
		{
			sprintf(filename_c, "assets/units/vehicles/death_effects/spark_n%02d.png", i);
			base_img[i].LoadBaseImage(filename_c);
		}

	finished_init = true;
}

void EDeathSparks::Process()
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

		next_process_time = the_time + 0.1;
	}

	{
		double time_dif = (the_time - init_time);
		double up_amount;

		//move
		x = sx + (dx * time_dif);
		y = sy + (dy * time_dif);

		//double percent = (time_dif / (final_time - init_time));
		//if(percent >= 0.5)
		//	size = 2.0 - percent;
		//else
		//	size = 1.0 + percent;

		//size *= 0.2;
		//size += 0.8;
		
		//y -= -(rise / (final_time - init_time)) * (time_dif * time_dif) + rise * time_dif;

		size = -(rise / (final_time - init_time)) * (time_dif * time_dif) + rise * time_dif;
		y -= size * 30;
		//printf("size:%lf\n", size);

		//y -= (rise * (size - 1.0));

		//y -= -20 * (time_dif * time_dif) + 50 * time_dif;
	}
}

void EDeathSparks::DoRender(ZMap &zmap, SDL_Surface *dest)
{
	SDL_Rect from_rect, to_rect;
	//SDL_Surface *render_img;

	if(killme) return;

	//render_img = base_img[render_i].GetImage(size);
	base_img[render_i].SetSize(size);

	zmap.RenderZSurface(&base_img[render_i], x, y, false, true);
	//if(zmap.GetBlitInfo( render_img, x, y, from_rect, to_rect))
	//	SDL_BlitSurface( render_img, &from_rect, dest, &to_rect);
}
