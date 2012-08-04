#include "esideexplosion.h"

bool ESideExplosion::finished_init = false;
ZSDL_Surface ESideExplosion::normal_img[7];

ESideExplosion::ESideExplosion(ZTime *ztime_, int x_, int y_, double size_, int type_) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;
	int speed;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	speed = 20 + (rand() % 10);

	ex = x_ + (20 - (rand() % 40));
	ey = y_ + (20 - (rand() % 40));

	float mag;
	dx = ex - x_;
	dy = ey - y_;
	mag = sqrt((dx * dx) + (dy * dy));

	init_time = the_time;

	dx /= mag;
	dy /= mag;

	dx *= speed;
	dy *= speed;

	render_i = 0;

	next_render_time = init_time + 0.13;

	x = sx = x_ - 16;
	y = sy = y_ - 16;

	size = size_;

	type = type_;

	switch(type)
	{
	case ESIDEEXPLOSION_NORMAL:
		render_img = normal_img;
		break;
	default:
		killme = true;
		break;
	}
}

void ESideExplosion::Init()
{
	int i;
	char filename_c[500];

	for(i=0;i<7;i++)
	{
		sprintf(filename_c, "assets/other/explosions/side_explosion_n%02d.png", i);
		normal_img[i].LoadBaseImage(filename_c);
	}

	finished_init = true;
}

void ESideExplosion::Process()
{
	double &the_time = ztime->ztime;

	if(killme) return;

	if(the_time >= next_render_time)
	{
		render_i++;
		if(render_i>=7)
		{
			killme = true;
			return;
		}

		next_render_time = the_time + 0.13;
	}

	//move
	{
		x = sx + (dx * (the_time - init_time));
		y = sy + (dy * (the_time - init_time));
	}
}

void ESideExplosion::DoRender(ZMap &zmap, SDL_Surface *dest)
{
	SDL_Rect from_rect, to_rect;
	//SDL_Surface *surface;

	if(killme) return;

	//surface = render_img[render_i].GetImage(size);
	render_img[render_i].SetSize(size);

	zmap.RenderZSurface(&render_img[render_i], x, y);
	//if(zmap.GetBlitInfo( surface, x, y, from_rect, to_rect))
	//	SDL_BlitSurface( surface, &from_rect, dest, &to_rect);
}
