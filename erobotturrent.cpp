#include "erobotturrent.h"

bool ERobotTurrent::finished_init;
ZSDL_Surface ERobotTurrent::robot_flip[MAX_TEAM_TYPES][33];

ERobotTurrent::ERobotTurrent(ZTime *ztime_, int x_, int y_, int owner_, int max_horz, int max_vert) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	if(owner_ == NULL_TEAM)
	{
		printf("ERobotTurrent::owner == NULL_TEAM\n");
		killme = true;
		return;
	}

	owner = owner_;

	//....
	double lifetime;

	lifetime = 3.5 + (0.1 * (rand() % 10));

	render_i = 0;
	size = 1.0;

	//16x10

	x = x_ + (2 - (rand() % 5));
	y = y_ + (2 - (rand() % 5));

	sx = x;
	sy = y;

	ex = x + (max_horz - (rand() % (max_horz + max_horz)));
	ey = y + (max_vert - (rand() % (max_vert + max_vert)));

	rise = (int)(1.3 + (0.01 * (rand() % 200)));

	double mag;
	dx = ex - sx;
	dy = ey - sy;
	mag = sqrt((dx * dx) + (dy * dy));

	init_time = the_time;
	final_time = init_time + lifetime;

	dx /= lifetime;
	dy /= lifetime;

	next_render_i_time = the_time + 0.05;
}

void ERobotTurrent::Init()
{
	int i, k;
	char filename_c[500];

	for(i=1;i<MAX_TEAM_TYPES;i++)
		for(k=0;k<33;k++)
		{
			sprintf(filename_c, "assets/units/robots/die5_%s_n%02d.png", team_type_string[i].c_str(), k);
			//robot_flip[i][k].LoadBaseImage(filename_c);
			ZTeam::LoadZSurface(i, robot_flip[ZTEAM_BASE_TEAM][k], robot_flip[i][k], filename_c);
		}

	finished_init = true;
}

void ERobotTurrent::Process()
{
	double &the_time = ztime->ztime;

	if(killme) return;

	if(the_time >= next_render_i_time)
	{
		render_i++;

		if(the_time < final_time)
		{
			next_render_i_time = the_time + 0.05;

			//keep us in the main loop if we are in the air
			if(render_i > 7) render_i = 0;
		}
		else
		{
			next_render_i_time = the_time + 0.15;

			//if we are past airtime go straight to
			//the final part of the animimation
			if(render_i < 8) render_i = 8;
		}

		if(render_i > 32)
		{
			killme = true;
			return;
		}
	}

	{
		if(the_time < final_time)
		{
			double time_dif = (the_time - init_time);
//			double up_amount;

			//move
			x = sx + (int)(dx * time_dif);
			y = sy + (int)(dy * time_dif);

			size = -(rise / (final_time - init_time)) * (time_dif * time_dif) + rise * time_dif;
			size += 1;
			y -= (int)((size-1) * 30);
		}
		else
		{
			//if there is a lot of lag
			//sometimes we can't properly get back
			//to size of 1 for the tail end of the animation
			size = 1;
		}
	}
}

void ERobotTurrent::DoRender(ZMap &zmap, SDL_Surface *dest)
{
//	SDL_Rect from_rect, to_rect;
	//SDL_Surface *surface;

	if(killme) return;

	//surface = robot_flip[owner][render_i].GetImage(size);
	robot_flip[owner][render_i].SetSize((float)size);

	//if(!surface) return;

	zmap.RenderZSurface(&robot_flip[owner][render_i], x, y, false, true);
	//if(zmap.GetBlitInfo( surface, x - (surface->w >> 1), y - (surface->h >> 1), from_rect, to_rect))
	//	SDL_BlitSurface( surface, &from_rect, dest, &to_rect);
}
