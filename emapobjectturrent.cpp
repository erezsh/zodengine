#include "emapobjectturrent.h"
#include "etoughmushroom.h"
#include "eunitparticle.h"

ZSDL_Surface EMapObjectTurrent::object_img[MAP_ITEMS_AMOUNT];
bool EMapObjectTurrent::finished_init = false;

EMapObjectTurrent::EMapObjectTurrent(ZTime *ztime_, int start_x, int start_y, int dest_x_, int dest_y_, double offset_time, int object_) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;
	int mx, my;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	object = object_;
	dest_x = dest_x_;
	dest_y = dest_y_;
	
	if(object < 0) object = 0;
	if(object >= MAP_ITEMS_AMOUNT) object = MAP_ITEMS_AMOUNT-1;

	size = 1.0;
	rise = 0.5 + (0.01 * (rand() % 100));

	start_x += 5 - (rand() % 10);
	start_y += 5 - (rand() % 10);

	//set the dx / dy
	float mag;
	dx = dest_x - start_x;
	dy = dest_y - start_y;
	mag = sqrt((dx * dx) + (dy * dy));

	init_time = the_time;
	final_time = init_time + offset_time;

	dx /= offset_time;
	dy /= offset_time;

	{
		SDL_Surface *render_img;

		//render_img = object_img[object].GetImage(0, 1);
		render_img = object_img[object].GetBaseSurface();

		if(render_img)
		{
			//mx = (render_img->w >> 1);
			//my = (16 - render_img->h) + (render_img->h >> 1);
			mx = 0;
			my = (16 - render_img->h);
		}
		else
		{
			mx = 0;
			my = 0;
		}
	}

	x = sx = start_x + mx;
	y = sy = start_y + my;

	ex = dest_x + mx;
	ey = dest_y + my;

	angle = 0;
	dangle = 240 - (rand() % 480);

	if(dangle >= 0) dangle += 100;
	else dangle -= 100;
}

void EMapObjectTurrent::Init()
{
	char filename_c[500];
	int i;

	for(i=0;i<MAP_ITEMS_AMOUNT;i++)
	{
		sprintf(filename_c, "assets/other/map_items/no_shadow%d.png", i);
		object_img[i].LoadBaseImage(filename_c);
	}

	finished_init = true;
}

void EMapObjectTurrent::Process()
{
	double &the_time = ztime->ztime;
	int i, mx, my;

	if(killme) return;

	//check
	if(the_time >= final_time)
	{
		killme = true;
		EndExplosion();
		ZSoundEngine::PlayWavRestricted(TURRENT_EXPLOSION_SND, ex, ey);
		return;
	}

	{
		double time_dif = (the_time - init_time);

		//move
		x = sx + (dx * time_dif);
		y = sy + (dy * time_dif);

		size = -(rise / (final_time - init_time)) * (time_dif * time_dif) + rise * time_dif;
		y -= size * 30;

		size += 1.0;
		
		angle = dangle * time_dif;

		while(angle >= 360) angle -= 360;
		while(angle < 0) angle += 360;
	}
}

void EMapObjectTurrent::DoRender(ZMap &zmap, SDL_Surface *dest)
{
	SDL_Rect from_rect, to_rect;
	//SDL_Surface *render_img;

	if(killme) return;

	//render_img = object_img[object].GetImage(angle, size);
	object_img[object].SetAngle(angle);
	object_img[object].SetSize(size);

	//if(!render_img) return;

	zmap.RenderZSurface(&object_img[object], x, y, false, true);
	//if(zmap.GetBlitInfo(render_img, x - (render_img->w >> 1), y - (render_img->h >> 1), from_rect, to_rect))
	//	SDL_BlitSurface( render_img, &from_rect, dest, &to_rect);
}

void EMapObjectTurrent::EndExplosion()
{
	//SDL_Surface *render_img;
	int mx, my;
	int particles;

	//render_img = object_img[object].GetImage(angle, size);

	//if(!render_img) return;

	//mx = x;//(render_img->w >> 1);
	//my = y;//(render_img->h >> 1);

	if(effect_list) effect_list->push_back((ZEffect*)(new EToughMushroom(ztime, dest_x, dest_y, 1.0)));

	//particles
	particles = 10 + (rand() % 8);
	if(effect_list) 
	for(int i=0;i<particles;i++)
		effect_list->push_back((ZEffect*)(new EUnitParticle(ztime, dest_x, dest_y, 65, 55)));
}
