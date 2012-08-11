#include "emomissilerockets.h"
#include "etoughmushroom.h"
#include "etoughsmoke.h"

bool EMoMissileRockets::finished_init = false;
ZSDL_Surface EMoMissileRockets::the_bullet;

EMoMissileRockets::EMoMissileRockets(ZTime *ztime_, int start_x, int start_y, int dest_x, int dest_y) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;
	//const double bullet_speed = MMISSILE_MISSILE_SPEED;
	const double bullet_speed = zsettings->vehicle_settings[MISSILE_LAUNCHER].attack_missile_speed;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	//set the dx / dy
	float mag;
	dx = dest_x - start_x;
	dy = dest_y - start_y;
	mag = (float)sqrt((dx * dx) + (dy * dy));

	udx = dx / mag;
	udy = dy / mag;

	left_x_shift = (int)(udy * 8);
	left_y_shift = (int)(udx * -8);

	right_x_shift = (int)(udy * -8);
	right_y_shift = (int)(udx * 8);

	init_time = the_time;
	last_smoke_time = init_time;
	final_time = init_time + (mag / bullet_speed);

	dx /= (final_time - init_time);
	dy /= (final_time - init_time);

	//find angle
	angle = 359 - AngleFromLoc((float)dx, (float)dy);
	//bullet_img = the_bullet.GetImage(angle);
	bullet_img = the_bullet;
	bullet_img.SetAngle((float)angle);

	//shift the x and y's
	//cx = (bullet_img.GetBaseSurface()->w >> 1);
	//cy = (bullet_img.GetBaseSurface()->h >> 1);
	//x = sx = start_x - cx;
	//y = sy = start_y - cy;
	x = sx = start_x;
	y = sy = start_y;

	ex = dest_x;
	ey = dest_y;

}

void EMoMissileRockets::Init()
{
	char filename_c[500];

	sprintf(filename_c, "assets/units/vehicles/missile_launcher/bullet.png");
	the_bullet.LoadBaseImage(filename_c);

	finished_init = true;
}

void EMoMissileRockets::Process()
{
	double &the_time = ztime->ztime;
	int i, mx, my;

	if(killme) return;

	//check
	if(the_time >= final_time)
	{
		killme = true;

		//extra xlarges
		for(i=0;i<3;i++)
		{
			mx = 9 -(rand() % 18);
			my = -(rand() % 18);

			if(effect_list) effect_list->push_back((ZEffect*)(new EToughMushroom(ztime, ex + mx, ey + my, 1.5)));
		}

		//extra smalls
		for(i=0;i<2;i++)
		{
			mx = 5 -(rand() % 10);
			my = -(rand() % 10);

			if(effect_list) effect_list->push_back((ZEffect*)(new EToughMushroom(ztime, ex + mx, ey + my)));
		}

		if(effect_list) effect_list->push_back((ZEffect*)(new EToughMushroom(ztime, ex, ey)));

		//particle neighboring objects
		eflags.unit_particles = true;
		//eflags.unit_particles_radius = MMISSILE_DAMAGE_RADIUS;
		eflags.unit_particles_radius = zsettings->vehicle_settings[MISSILE_LAUNCHER].attack_damage_radius;
		eflags.unit_particles_amount = 7 + (2 * 2) + (3 * 4);
		eflags.x = ex;
		eflags.y = ey;

		ZSoundEngine::PlayWavRestricted(RANDOM_EXPLOSION_SND, ex, ey);

		//place crater
		if(zmap) zmap->CreateCrater(ex, ey, true, 0.75);
	}
	else
	{
		//move
		x = sx + (int)(dx * (the_time - init_time));
		y = sy + (int)(dy * (the_time - init_time));

		left_x = x + left_x_shift;
		left_y = y + left_y_shift;
		right_x = x + right_x_shift;
		right_y = y + right_y_shift;

		PlaceSmoke(the_time);
	}
}

void EMoMissileRockets::DoRender(ZMap &zmap, SDL_Surface *dest)
{
//	SDL_Rect from_rect, to_rect;

	if(killme) return;

	zmap.RenderZSurface(&bullet_img, x, y, false, true);
	//if(zmap.GetBlitInfo(bullet_img, x, y, from_rect, to_rect))
	//	SDL_BlitSurface( bullet_img, &from_rect, dest, &to_rect);

	zmap.RenderZSurface(&bullet_img, left_x, left_y, false, true);
	//if(zmap.GetBlitInfo(bullet_img, left_x, left_y, from_rect, to_rect))
	//	SDL_BlitSurface( bullet_img, &from_rect, dest, &to_rect);

	zmap.RenderZSurface(&bullet_img, right_x, right_y, false, true);
	//if(zmap.GetBlitInfo(bullet_img, right_x, right_y, from_rect, to_rect))
	//	SDL_BlitSurface( bullet_img, &from_rect, dest, &to_rect);
}

void EMoMissileRockets::PlaceSmoke(double &the_time)
{
	//const double bullet_speed = MMISSILE_MISSILE_SPEED;
	const double bullet_speed = zsettings->vehicle_settings[MISSILE_LAUNCHER].attack_missile_speed;
	const double time_d = 6.0 / bullet_speed;
	const double time_d2 = 8.0 / bullet_speed;


	for(;the_time - last_smoke_time > time_d2; last_smoke_time += time_d2)
	{
		smx = sx + (int)(dx * ((last_smoke_time - time_d) - init_time));
		smy = sy + (int)(dy * ((last_smoke_time - time_d) - init_time));

		//smx += cx;
		//smy += cy;

		if(effect_list) effect_list->push_back((ZEffect*)(new EToughSmoke(ztime, smx, smy)));
		if(effect_list) effect_list->push_back((ZEffect*)(new EToughSmoke(ztime, smx + left_x_shift, smy + left_y_shift)));
		if(effect_list) effect_list->push_back((ZEffect*)(new EToughSmoke(ztime, smx + right_x_shift, smy + right_y_shift)));
	}
}
