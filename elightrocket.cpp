#include "elightrocket.h"
#include "elightinitfire.h"
#include "etoughmushroom.h"

bool ELightRocket::finished_init = false;
ZSDL_Surface ELightRocket::the_bullet;

ELightRocket::ELightRocket(ZTime *ztime_, int start_x, int start_y, int dest_x, int dest_y, int speed, int extra_small_, int extra_large_, int xx_large_) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	bullet_speed = speed;

	extra_small = extra_small_;
	extra_large = extra_large_;
	xx_large = xx_large_;

	//set the dx / dy
	float mag;
	dx = dest_x - start_x;
	dy = dest_y - start_y;
	mag = sqrt((dx * dx) + (dy * dy));

	udx = dx / mag;
	udy = dy / mag;

	init_time = the_time;
	last_smoke_time = init_time;
	final_time = init_time + (mag / bullet_speed);

	dx /= (final_time - init_time);
	dy /= (final_time - init_time);

	//find angle
	angle = 359 - AngleFromLoc(dx, dy);
	//bullet_img = the_bullet.GetImage(angle);
	bullet_img = the_bullet;
	bullet_img.SetAngle(angle);

	//shift the x and y's
	cx = (bullet_img.GetBaseSurface()->w >> 1);
	cy = (bullet_img.GetBaseSurface()->h >> 1);
	x = sx = start_x - cx;
	y = sy = start_y - cy;

	ex = dest_x;
	ey = dest_y;

	//dont like putting this in a constructor but why not
	if(effect_list) effect_list->push_back((ZEffect*)(new ELightInitFire(ztime, start_x, start_y)));
}

void ELightRocket::Init()
{
	char filename_c[500];

	sprintf(filename_c, "assets/units/vehicles/light/bullet.png");
	the_bullet.LoadBaseImage(filename_c);

	finished_init = true;
}

void ELightRocket::Process()
{
	double &the_time = ztime->ztime;
	int i, mx, my;

	if(killme) return;

	//check
	if(the_time >= final_time)
	{
		killme = true;

		//extra larges
		for(i=0;i<xx_large;i++)
		{
			mx = 9 -(rand() % 18);
			my = -(rand() % 18);

			if(effect_list) effect_list->push_back((ZEffect*)(new EToughMushroom(ztime, ex + mx, ey + my, 1.5)));
		}

		//extra larges
		for(i=0;i<extra_large;i++)
		{
			mx = 7 -(rand() % 14);
			my = -(rand() % 14);

			if(effect_list) effect_list->push_back((ZEffect*)(new EToughMushroom(ztime, ex + mx, ey + my, 1.3)));
		}

		//extra smalls
		for(i=0;i<extra_small;i++)
		{
			mx = 5 -(rand() % 10);
			my = -(rand() % 10);

			if(effect_list) effect_list->push_back((ZEffect*)(new EToughMushroom(ztime, ex + mx, ey + my)));
		}

		if(effect_list) effect_list->push_back((ZEffect*)(new EToughMushroom(ztime, ex, ey)));

		//particle neighboring objects
		eflags.unit_particles = true;
		eflags.unit_particles_radius = 40;
		eflags.unit_particles_amount = 7 + (extra_small * 2) + (extra_large * 3) + (xx_large * 4);
		eflags.x = ex;
		eflags.y = ey;

		ZSoundEngine::PlayWavRestricted(RANDOM_EXPLOSION_SND, ex, ey);

		//place crater
		if(zmap) 
		{
			double chance = frand();

			if(xx_large && chance <= 0.35)
				zmap->CreateCrater(ex, ey, true, 0.75);
			else if(extra_large && chance <= 0.15)
				zmap->CreateCrater(ex, ey, true, 0.75);
			else
				zmap->CreateCrater(ex, ey, false, 0.75);
		}
	}
	else
	{
		//move
		x = sx + (dx * (the_time - init_time));
		y = sy + (dy * (the_time - init_time));
	}
}

void ELightRocket::DoRender(ZMap &zmap, SDL_Surface *dest)
{
	SDL_Rect from_rect, to_rect;

	if(killme) return;

	zmap.RenderZSurface(&bullet_img, x, y);
	//if(zmap.GetBlitInfo(bullet_img, x, y, from_rect, to_rect))
	//	SDL_BlitSurface( bullet_img, &from_rect, dest, &to_rect);
}
