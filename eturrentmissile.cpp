#include "eturrentmissile.h"
#include "edeathsparks.h"
#include "etoughmushroom.h"
#include "esideexplosion.h"

bool ETurrentMissile::finished_init = false;
ZSDL_Surface ETurrentMissile::light_turrent[8];
ZSDL_Surface ETurrentMissile::medium_turrent[8];
ZSDL_Surface ETurrentMissile::heavy_turrent[MAX_TEAM_TYPES][8];
ZSDL_Surface ETurrentMissile::gatling_wasted;
ZSDL_Surface ETurrentMissile::gun_wasted;
ZSDL_Surface ETurrentMissile::howitzer_wasted;
ZSDL_Surface ETurrentMissile::missile_wasted;
ZSDL_Surface ETurrentMissile::building_piece[2][12];
ZSDL_Surface ETurrentMissile::fort_building_piece[5][12];
ZSDL_Surface ETurrentMissile::grenade_img[4];


ETurrentMissile::ETurrentMissile(ZTime *ztime_, int start_x, int start_y, int dest_x, int dest_y, double offset_time, int object_, int team_) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	team = team_;
	object = object_;
	size = 1.0;
	rise = 1 + (0.01 * (rand() % 300));

	start_x += 5 - (rand() % 10);
	start_y += 5 - (rand() % 10);

	//set the dx / dy
	float mag;
	dx = dest_x - start_x;
	dy = dest_y - start_y;
	mag = (float)sqrt((dx * dx) + (dy * dy));

	init_time = the_time;
	final_time = init_time + offset_time;

	dx /= offset_time;
	dy /= offset_time;

	x = sx = start_x;
	y = sy = start_y;

	ex = dest_x;
	ey = dest_y;

	radius = 40;

	next_render_time = init_time + 0.1;
	render_i = 0;

	angle = 0;
	dangle = 240 - (rand() % 480);

	//dont bother if..
	if(object == ETURRENTMISSILE_HEAVY && team == NULL_TEAM)
		killme = true;

	switch(object)
	{
	case ETURRENTMISSILE_LIGHT:
	case ETURRENTMISSILE_MEDIUM:
	case ETURRENTMISSILE_HEAVY:
		max_render = 8;
		break;
	case ETURRENTMISSILE_BUILDING_PEICE0:
	case ETURRENTMISSILE_BUILDING_PEICE1:
	case ETURRENTMISSILE_FORT_BUILDING_PEICE0:
	case ETURRENTMISSILE_FORT_BUILDING_PEICE1:
	case ETURRENTMISSILE_FORT_BUILDING_PEICE2:
	case ETURRENTMISSILE_FORT_BUILDING_PEICE3:
	case ETURRENTMISSILE_FORT_BUILDING_PEICE4:
		max_render = 12;
		break;
	case ETURRENTMISSILE_GRENADE:
		max_render = 4;
		break;
	default:
		max_render = 0;
		break;
	}
}

void ETurrentMissile::Init()
{
	int i, j;
	char filename_c[500];

	for(i=0;i<4;i++)
	{
		sprintf(filename_c, "assets/other/grenades/grenade_n%02d.png", i);
		grenade_img[i].LoadBaseImage(filename_c);
	}

	for(i=0;i<8;i++)
	{
		sprintf(filename_c, "assets/units/vehicles/light/top_pop_n%02d.png", i);
		light_turrent[i].LoadBaseImage(filename_c);

		sprintf(filename_c, "assets/units/vehicles/medium/top_pop_n%02d.png", i);
		medium_turrent[i].LoadBaseImage(filename_c);

		for(j=1;j<MAX_TEAM_TYPES;j++)
		{
			sprintf(filename_c, "assets/units/vehicles/heavy/top_pop_%s_n%02d.png", team_type_string[j].c_str(), i);
			//heavy_turrent[j][i].LoadBaseImage(filename_c);
			ZTeam::LoadZSurface(j, heavy_turrent[ZTEAM_BASE_TEAM][i], heavy_turrent[j][i], filename_c);
		}
	}

	for(i=0;i<2;i++)
		for(j=0;j<12;j++)
		{
			sprintf(filename_c, "assets/buildings/death_effects/piece%d_n%02d.png", i, j);
			building_piece[i][j].LoadBaseImage(filename_c);
		}

	for(i=0;i<5;i++)
		for(j=0;j<12;j++)
		{
			sprintf(filename_c, "assets/buildings/death_effects/fort_piece%d_n%02d.png", i, j);
			fort_building_piece[i][j].LoadBaseImage(filename_c);
		}

	gatling_wasted.LoadBaseImage("assets/units/cannons/gatling/wasted.png");
	gun_wasted.LoadBaseImage("assets/units/cannons/gun/wasted.png");
	howitzer_wasted.LoadBaseImage("assets/units/cannons/howitzer/wasted.png");
	missile_wasted.LoadBaseImage("assets/units/cannons/missile_cannon/wasted.png");

	finished_init = true;
}

void ETurrentMissile::Process()
{
	double &the_time = ztime->ztime;
//	int i, mx, my;

	if(killme) return;

	//check
	if(the_time >= final_time)
	{
		killme = true;
		EndExplosion();
		ZSoundEngine::PlayWavRestricted(TURRENT_EXPLOSION_SND, ex, ey);

		//place crater
		if(zmap) zmap->CreateCrater(ex, ey, false, 0.35);

		return;
	}

	if(the_time >= next_render_time)
	{
		render_i++;
		if(render_i>=max_render) render_i = 0;

		next_render_time = the_time + 0.1;
	}

	{
		double time_dif = (the_time - init_time);

		//move
		x = sx + (int)(dx * time_dif);
		y = sy + (int)(dy * time_dif);

		size = -(rise / (final_time - init_time)) * (time_dif * time_dif) + rise * time_dif;
		size += 1.0;
		y -= (int)(size * 30);
		y += 30;

		angle = (int)(dangle * time_dif);

		while(angle >= 360) angle -= 360;
		while(angle < 0) angle += 360;
	}
}

void ETurrentMissile::DoRender(ZMap &zmap, SDL_Surface *dest)
{
//	SDL_Rect from_rect, to_rect;

	if(killme) return;

	switch(object)
	{
	case ETURRENTMISSILE_LIGHT:
		render_img = &light_turrent[render_i];
		break;
	case ETURRENTMISSILE_MEDIUM:
		render_img = &medium_turrent[render_i];
		break;
	case ETURRENTMISSILE_HEAVY:
		render_img = &heavy_turrent[team][render_i];
		break;
	case ETURRENTMISSILE_GATLING:
		render_img = &gatling_wasted;
		break;
	case ETURRENTMISSILE_GUN:
		render_img = &gun_wasted;
		break;
	case ETURRENTMISSILE_HOWITZER:
		render_img = &howitzer_wasted;
		break;
	case ETURRENTMISSILE_MISSILE_CANNON:
		render_img = &missile_wasted;
		break;
	case ETURRENTMISSILE_BUILDING_PEICE0:
		render_img = &building_piece[0][render_i];
		break;
	case ETURRENTMISSILE_BUILDING_PEICE1:
		render_img = &building_piece[1][render_i];
		break;
	case ETURRENTMISSILE_FORT_BUILDING_PEICE0:
		render_img = &fort_building_piece[0][render_i];
		break;
	case ETURRENTMISSILE_FORT_BUILDING_PEICE1:
		render_img = &fort_building_piece[1][render_i];
		break;
	case ETURRENTMISSILE_FORT_BUILDING_PEICE2:
		render_img = &fort_building_piece[2][render_i];
		break;
	case ETURRENTMISSILE_FORT_BUILDING_PEICE3:
		render_img = &fort_building_piece[3][render_i];
		break;
	case ETURRENTMISSILE_FORT_BUILDING_PEICE4:
		render_img = &fort_building_piece[4][render_i];
		break;
	case ETURRENTMISSILE_GRENADE:
		render_img = &grenade_img[render_i];
		break;
	default:
		return;
		break;
	}

	if(!render_img) return;

	render_img->SetAngle((float)angle);
	render_img->SetSize((float)size);

	zmap.RenderZSurface(render_img, x, y, false, true);
	//if(zmap.GetBlitInfo(render_img, x - (render_img->w >> 1), y - (render_img->h >> 1), from_rect, to_rect))
	//	SDL_BlitSurface( render_img, &from_rect, dest, &to_rect);
}

void ETurrentMissile::EndExplosion()
{
	int sparks_amt;
	int spark_x, spark_y;
	int i;
	int mx, my;

	//mushroom
	{
		mx = 7 -(rand() % 14);
		my = -(rand() % 14);

		if(effect_list) effect_list->push_back((ZEffect*)(new EToughMushroom(ztime, ex + mx, ey + my, 1.3)));
	}

	//side explosion
	{
		mx = 24 -(rand() % 48);
		my = 24 -(rand() % 48);

		if(effect_list) effect_list->push_back((ZEffect*)(new ESideExplosion(ztime, ex + mx, ey + my)));
	}

	//sparks
	{
		sparks_amt = 30 + (rand() % 30);

		spark_x = x + 16;
		spark_y = y + 16;

		if(effect_list)
		for(i=0;i<sparks_amt;i++)
			effect_list->push_back((ZEffect*)(new EDeathSparks(ztime, spark_x, spark_y)));
	}

}
