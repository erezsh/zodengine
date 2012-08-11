#include "eflame.h"
#include "epyrofire.h"

bool EFlame::finished_init = false;
ZSDL_Surface EFlame::flame_bullet[4];

EFlame::EFlame(ZTime *ztime_, int start_x, int start_y, int dest_x, int dest_y) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;
	const double bullet_speed = 300;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	//which one to use this time?
	bullet_i = rand() % 4;

	//set the dx / dy
	float mag;
	dx = dest_x - start_x;
	dy = dest_y - start_y;
	mag = (float)sqrt((dx * dx) + (dy * dy));

	init_time = the_time;
	final_time = init_time + (mag / bullet_speed);

	dx /= (final_time - init_time);
	dy /= (final_time - init_time);

	//find angle
	angle = 359 - AngleFromLoc((float)dx, (float)dy);
	for(int i=0;i<4;i++)
	{
		//bullet_img[i] = flame_bullet[i].GetImage(angle);
		bullet_img[i] = flame_bullet[i];
		bullet_img[i].SetAngle((float)angle);
	}

	if(!bullet_img[0].GetBaseSurface())
	{
		killme = true;
		return;
	}

	//shift the x and y's
	x = sx = start_x - (bullet_img[0].GetBaseSurface()->w >> 1);
	y = sy = start_y - (bullet_img[0].GetBaseSurface()->h >> 1);

	ex = dest_x;
	ey = dest_y;

}

void EFlame::Init()
{
	int i;
	char filename_c[500];

	for(i=0;i<4;i++)
	{
		sprintf(filename_c, "assets/units/robots/pyro/bullet_n%02d.png", i);
		flame_bullet[i].LoadBaseImage(filename_c);
	}

	finished_init = true;
}

void EFlame::Process()
{
	double &the_time = ztime->ztime;

	if(killme) return;

	//check
	if(the_time >= final_time)
	{
		killme = true;
		if(effect_list) effect_list->push_back((ZEffect*)(new EPyroFire(ztime, ex, ey)));
	}
	else
	{
		//move
		x = sx + (int)(dx * (the_time - init_time));
		y = sy + (int)(dy * (the_time - init_time));
	}
}

void EFlame::DoRender(ZMap &zmap, SDL_Surface *dest)
{
//	SDL_Rect from_rect, to_rect;

	if(killme) return;

	zmap.RenderZSurface(&bullet_img[bullet_i], x, y);
	//if(zmap.GetBlitInfo(bullet_img[0], x, y, from_rect, to_rect))
	//	SDL_BlitSurface( bullet_img[bullet_i], &from_rect, dest, &to_rect);

	//always new
	bullet_i = rand() % 4;
}
