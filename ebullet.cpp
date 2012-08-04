#include "ebullet.h"
#include "eunitparticle.h"

EBullet::EBullet(ZTime *ztime_, team_type team, int start_x, int start_y, int dest_x, int dest_y) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;
	const double bullet_speed = 300;

	//rgb_map = 0;

	owner = team;
	x = start_x;
	y = start_y;
	sx = start_x;
	sy = start_y;
	ex = dest_x;
	ey = dest_y;

	//set the dx / dy
	float mag;
	dx = dest_x - start_x;
	dy = dest_y - start_y;
	mag = sqrt((dx * dx) + (dy * dy));

	init_time = the_time;
	final_time = init_time + (mag / bullet_speed);

	dx /= (final_time - init_time);
	dy /= (final_time - init_time);
}

void EBullet::Process()
{
	double &the_time = ztime->ztime;

	//check
	if(the_time >= final_time)
	{
		killme = true;

		int particle_amount = rand() % 3;
		for(int i=0;i<particle_amount;i++)
		{
			if(effect_list) effect_list->push_back((ZEffect*)(new EUnitParticle(ztime, ex, ey)));
		}

		ZSoundEngine::PlayWavRestricted(RIOCHET_SND, ex, ey);
	}
	else
	{
		//move
		x = sx + (dx * (the_time - init_time));
		y = sy + (dy * (the_time - init_time));
	}
}

void EBullet::DoRender(ZMap &zmap, SDL_Surface *dest)
{
	SDL_Rect the_box;
	int shift_x, shift_y, view_w, view_h;

	if(killme) return;

	zmap.GetViewShiftFull(shift_x, shift_y, view_w, view_h);

	//out of bounds?
	if(x < shift_x) return;
	if(y < shift_y) return;
	if(x > view_w + shift_x) return;
	if(y > view_h + shift_y) return;

	the_box.x = x - shift_x;
	the_box.y = y - shift_y;
	the_box.w = 2;
	the_box.h = 2;

	////set color
	//if(!rgb_map)
	//{
	//	team_type &t = owner;
	//	rgb_map = SDL_MapRGB(dest->format, team_color[t].r, team_color[t].g, team_color[t].b);
	//}

	//SDL_FillRect(dest, &the_box, rgb_map);
	team_type &t = owner;
	ZSDL_FillRect(&the_box, team_color[t].r, team_color[t].g, team_color[t].b);
}
