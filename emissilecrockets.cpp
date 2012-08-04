#include "emissilecrockets.h"
#include "etoughmushroom.h"
#include "etoughsmoke.h"

bool EMissileCRockets::finished_init = false;
ZSDL_Surface EMissileCRockets::the_bullet;

EMissileCRockets::EMissileCRockets(ZTime *ztime_, int start_x, int start_y, int dest_x, int dest_y) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;
	//const double bullet_speed = MISSILE_CANNON_MISSILE_SPEED;
	const double bullet_speed = zsettings->cannon_settings[MISSILE_CANNON].attack_missile_speed;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	//set the dx / dy
	float mag;
	dx = dest_x - start_x;
	dy = dest_y - start_y;
	mag = sqrt((dx * dx) + (dy * dy));

	udx = dx / mag;
	udy = dy / mag;

	other_x_shift = udy * 8;
	other_y_shift = udx * -8;

	//right_x_shift = udy * -8;
	//right_y_shift = udx * 8;

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
	//cx = (bullet_img.GetBaseSurface()->w >> 1);
	//cy = (bullet_img.GetBaseSurface()->h >> 1);
	//x = sx = (start_x - cx) + (udy * -4);
	//y = sy = (start_y - cy) + (udx * 8);
	x = sx = start_x + (udy * -4);
	y = sy = start_y + (udx * 8);

	ex = dest_x;
	ey = dest_y;

}

void EMissileCRockets::Init()
{
	char filename_c[500];

	sprintf(filename_c, "assets/units/cannons/missile_cannon/bullet.png");
	the_bullet.LoadBaseImage(filename_c);

	finished_init = true;
}

void EMissileCRockets::Process()
{
	double &the_time = ztime->ztime;
	int i, mx, my;

	if(killme) return;

	//check
	if(the_time >= final_time)
	{
		killme = true;

		//extra larges
		for(i=0;i<3;i++)
		{
			mx = 7 -(rand() % 14);
			my = -(rand() % 14);

			if(effect_list) effect_list->push_back((ZEffect*)(new EToughMushroom(ztime, ex + mx, ey + my, 1.3)));
		}

		//extra smalls
		for(i=0;i<1;i++)
		{
			mx = 5 -(rand() % 10);
			my = -(rand() % 10);

			if(effect_list) effect_list->push_back((ZEffect*)(new EToughMushroom(ztime, ex + mx, ey + my)));
		}

		if(effect_list) effect_list->push_back((ZEffect*)(new EToughMushroom(ztime, ex, ey)));

		//particle neighboring objects
		eflags.unit_particles = true;
		//eflags.unit_particles_radius = MISSILE_CANNON_DAMAGE_RADIUS;
		eflags.unit_particles_radius = zsettings->cannon_settings[MISSILE_CANNON].attack_damage_radius;
		eflags.unit_particles_amount = 7 + (1 * 2) + (3 * 3);
		eflags.x = ex;
		eflags.y = ey;

		ZSoundEngine::PlayWavRestricted(RANDOM_EXPLOSION_SND, ex, ey);

		//place crater
		if(zmap) zmap->CreateCrater(ex, ey, false);
	}
	else
	{
		//move
		x = sx + (dx * (the_time - init_time));
		y = sy + (dy * (the_time - init_time));

		PlaceSmoke(the_time);
	}
}

void EMissileCRockets::DoRender(ZMap &zmap, SDL_Surface *dest)
{
	SDL_Rect from_rect, to_rect;

	if(killme) return;

	zmap.RenderZSurface(&bullet_img, x, y, false, true);
	//if(zmap.GetBlitInfo(bullet_img, x, y, from_rect, to_rect))
	//	SDL_BlitSurface( bullet_img, &from_rect, dest, &to_rect);

	zmap.RenderZSurface(&bullet_img, x + other_x_shift, y + other_y_shift, false, true);
	//if(zmap.GetBlitInfo(bullet_img, x + other_x_shift, y + other_y_shift, from_rect, to_rect))
	//	SDL_BlitSurface( bullet_img, &from_rect, dest, &to_rect);
}

void EMissileCRockets::PlaceSmoke(double &the_time)
{
	//const double bullet_speed = MISSILE_CANNON_MISSILE_SPEED;
	const double bullet_speed = zsettings->cannon_settings[MISSILE_CANNON].attack_missile_speed;
	const double time_d = 6.0 / bullet_speed;
	const double time_d2 = 8.0 / bullet_speed;

	for(;the_time - last_smoke_time > time_d2; last_smoke_time += time_d2)
	{
		smx = sx + (dx * ((last_smoke_time - time_d) - init_time));
		smy = sy + (dy * ((last_smoke_time - time_d) - init_time));

		//smx += cx;
		//smy += cy;

		if(effect_list) effect_list->push_back((ZEffect*)(new EToughSmoke(ztime, smx, smy)));
		if(effect_list) effect_list->push_back((ZEffect*)(new EToughSmoke(ztime, smx + other_x_shift, smy + other_y_shift)));
	}
}
