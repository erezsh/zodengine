#include "etoughrocket.h"
#include "etoughmushroom.h"
#include "etoughsmoke.h"

bool EToughRocket::finished_init = false;
ZSDL_Surface EToughRocket::the_bullet[2];

EToughRocket::EToughRocket(ZTime *ztime_, int start_x, int start_y, int dest_x, int dest_y) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;
	double bullet_speed = zsettings->GetUnitSettings(ROBOT_OBJECT, TOUGH).attack_missile_speed;

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

	udx = dx / mag;
	udy = dy / mag;

	init_time = the_time;
	last_smoke_time = init_time;
	final_time = init_time + (mag / bullet_speed);

	dx /= (final_time - init_time);
	dy /= (final_time - init_time);

	//find angle
	angle = 359 - AngleFromLoc((float)dx, (float)dy);
	for(int i=0;i<2;i++)
	{
		//bullet_img[i] = the_bullet[i].GetImage(angle);
		bullet_img[i] = the_bullet[i];
		bullet_img[i].SetAngle((float)angle);
	}

	//shift the x and y's
	//cx = (bullet_img[0].GetBaseSurface()->w >> 1);
	//cy = (bullet_img[0].GetBaseSurface()->h >> 1);
	//x = sx = start_x - cx;
	//y = sy = start_y - cy;
	x = sx = start_x;
	y = sy = start_y;

	ex = dest_x;
	ey = dest_y;

}

void EToughRocket::Init()
{
	int i;
	char filename_c[500];

	for(i=0;i<2;i++)
	{
		sprintf(filename_c, "assets/units/robots/tough/bullet_n%02d.png", i);
		the_bullet[i].LoadBaseImage(filename_c);
	}

	finished_init = true;
}

void EToughRocket::Process()
{
	double &the_time = ztime->ztime;

	if(killme) return;

	//check
	if(the_time >= final_time)
	{
		killme = true;
		if(effect_list) effect_list->push_back((ZEffect*)(new EToughMushroom(ztime, ex, ey)));
		ZSoundEngine::PlayWavRestricted(RANDOM_EXPLOSION_SND, ex, ey);

		//place crater
		if(zmap) zmap->CreateCrater(ex, ey, false, 0.35);
	}
	else
	{
		//move
		x = sx + (int)(dx * (the_time - init_time));
		y = sy + (int)(dy * (the_time - init_time));

		PlaceSmoke(the_time);
	}
}

void EToughRocket::DoRender(ZMap &zmap, SDL_Surface *dest)
{
//	SDL_Rect from_rect, to_rect;

	if(killme) return;

	zmap.RenderZSurface(&bullet_img[0], x, y, false, true);
	//if(zmap.GetBlitInfo(bullet_img[0], x, y, from_rect, to_rect))
	//	SDL_BlitSurface( bullet_img[bullet_i], &from_rect, dest, &to_rect);

	//always new
	bullet_i++;
	if(bullet_i) bullet_i = 0;
}

void EToughRocket::PlaceSmoke(double &the_time)
{
	double bullet_speed = zsettings->GetUnitSettings(ROBOT_OBJECT, TOUGH).attack_missile_speed;
	const double time_d = 6.0 / bullet_speed;
	const double time_d2 = 8.0 / bullet_speed;


	for(;the_time - last_smoke_time > time_d2; last_smoke_time += time_d2)
	{
		smx = sx + (int)(dx * ((last_smoke_time - time_d) - init_time));
		smy = sy + (int)(dy * ((last_smoke_time - time_d) - init_time));

		//smx += cx;
		//smy += cy;

		effect_list->push_back((ZEffect*)(new EToughSmoke(ztime, smx, smy)));
	}
}
