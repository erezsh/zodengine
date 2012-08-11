#include "elaser.h"

bool ELaser::finished_init = false;
ZSDL_Surface ELaser::laser_bullet[2];

ELaser::ELaser(ZTime *ztime_, int start_x, int start_y, int dest_x, int dest_y) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;
	const double bullet_speed = 300;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	//which one to use this time?
	bullet_i = rand() % 2;

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
	//bullet_img = laser_bullet[bullet_i].GetImage(angle);
	bullet_img = laser_bullet[bullet_i];
	bullet_img.SetAngle((float)angle);

	if(!bullet_img.GetBaseSurface())
	{
		killme = true;
		return;
	}

	//shift the x and y's
	x = sx = start_x - (bullet_img.GetBaseSurface()->w >> 1);
	y = sy = start_y - (bullet_img.GetBaseSurface()->h >> 1);

}

void ELaser::Init()
{
	int i;
	char filename_c[500];

	for(i=0;i<2;i++)
	{
		sprintf(filename_c, "assets/units/robots/laser/bullet_n%02d.png", i);
		laser_bullet[i].LoadBaseImage(filename_c);
	}

	finished_init = true;
}

void ELaser::Process()
{
	double &the_time = ztime->ztime;

	//check
	if(the_time >= final_time) killme = true;
	else
	{
		//move
		x = sx + (int)(dx * (the_time - init_time));
		y = sy + (int)(dy * (the_time - init_time));
	}
}

void ELaser::DoRender(ZMap &zmap, SDL_Surface *dest)
{
//	SDL_Rect from_rect, to_rect;

	if(killme) return;

	zmap.RenderZSurface(&bullet_img, x, y);
	//if(zmap.GetBlitInfo(bullet_img, x, y, from_rect, to_rect))
	//	SDL_BlitSurface( bullet_img, &from_rect, dest, &to_rect);
}
