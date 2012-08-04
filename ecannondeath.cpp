#include <algorithm>
#include "ecannondeath.h"
#include "edeathsparks.h"
#include "eturrentmissile.h"

bool ECannonDeath::finished_init = false;
ZSDL_Surface ECannonDeath::gatling_wasted;
ZSDL_Surface ECannonDeath::gun_wasted;
ZSDL_Surface ECannonDeath::howitzer_wasted;
ZSDL_Surface ECannonDeath::missile_wasted;

ECannonDeath::ECannonDeath(ZTime *ztime_, int x_, int y_, int object_, int ex_, int ey_, double offset_time_) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;
	int i, max;
	int xs, ys, ws, hs;
	//wasted_img = NULL;
	double the_random;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	x = x_;
	y = y_;
	the_random = 2 + (rand() % 3);
	final_time = the_time + the_random;

	ex = ex_;
	ey = ey_;
	offset_time = offset_time_ - the_random;
	object = object_;

	switch(object)
	{
	case ECANNONDEATH_GATLING:
		wasted_img = gatling_wasted;
		break;
	case ECANNONDEATH_GUN:
		wasted_img = gun_wasted;
		break;
	case ECANNONDEATH_HOWITZER:
		wasted_img = howitzer_wasted;
		break;
	case ECANNONDEATH_MISSILE:
		wasted_img = missile_wasted;
		break;
	default:
		killme = true;
		return;
		break;
	}

	//max = 3 + (rand() % 3); //little fires
	//for(i=0;i<max;i++) extra_effects.push_back(new EStandard(x + xs + (rand() % ws), y + ys + (rand() % hs), EDEATH_LITTLE_FIRE));
	//max = 1 + (rand() % 2); //big smokes
	//for(i=0;i<max;i++) extra_effects.push_back(new EStandard(x + xs + (rand() % ws), y + ys + (rand() % hs), EDEATH_BIG_SMOKE));
	//max = (rand() % 2); //small fire smokes
	//for(i=0;i<max;i++) extra_effects.push_back(new EStandard(x + xs + (rand() % ws), y + ys + (rand() % hs), EDEATH_SMALL_FIRE_SMOKE));

	////sort effects
	//sort (extra_effects.begin(), extra_effects.end(), sort_estandards_func);
}

ECannonDeath::~ECannonDeath()
{
	//no memory leaks
	for(vector<EStandard*>::iterator i=extra_effects.begin(); i!=extra_effects.begin(); i++)
		delete *i;
}

void ECannonDeath::Init()
{
	gatling_wasted.LoadBaseImage("assets/units/cannons/gatling/wasted.png");
	gun_wasted.LoadBaseImage("assets/units/cannons/gun/wasted.png");
	howitzer_wasted.LoadBaseImage("assets/units/cannons/howitzer/wasted.png");
	missile_wasted.LoadBaseImage("assets/units/cannons/missile_cannon/wasted.png");

	finished_init = true;
}

void ECannonDeath::Process()
{
	double &the_time = ztime->ztime;

	if(killme) return;

	if(the_time >= final_time) 
	{	
		killme = true;
		DoSparks();

		switch(object)
		{
		case ECANNONDEATH_GATLING:
			if(effect_list) effect_list->push_back((ZEffect*)(new ETurrentMissile(ztime, x, y, ex, ey, offset_time, ETURRENTMISSILE_GATLING)));
			break;
		case ECANNONDEATH_GUN:
			if(effect_list) effect_list->push_back((ZEffect*)(new ETurrentMissile(ztime, x, y, ex, ey, offset_time, ETURRENTMISSILE_GUN)));
			break;
		case ECANNONDEATH_HOWITZER:
			if(effect_list) effect_list->push_back((ZEffect*)(new ETurrentMissile(ztime, x, y, ex, ey, offset_time, ETURRENTMISSILE_HOWITZER)));
			break;
		case ECANNONDEATH_MISSILE:
			if(effect_list) effect_list->push_back((ZEffect*)(new ETurrentMissile(ztime, x, y, ex, ey, offset_time, ETURRENTMISSILE_MISSILE_CANNON)));
			break;
		default:
			killme = true;
			return;
			break;
		}
		return;
	}

	for(vector<EStandard*>::iterator i=extra_effects.begin(); i!=extra_effects.end(); i++)
		(*i)->Process();
}

void ECannonDeath::DoRender(ZMap &zmap, SDL_Surface *dest)
{
	SDL_Rect from_rect, to_rect;

	if(killme) return;

	zmap.RenderZSurface(&wasted_img, x, y);
	//if(zmap.GetBlitInfo( wasted_img, x, y, from_rect, to_rect))
	//	SDL_BlitSurface( wasted_img, &from_rect, dest, &to_rect);

	for(vector<EStandard*>::iterator i=extra_effects.begin(); i!=extra_effects.end(); i++)
		(*i)->DoRender(zmap, dest);
}

void ECannonDeath::DoSparks()
{
	int sparks_amt;
	int spark_x, spark_y;
	int i;

	sparks_amt = 20 + (rand() % 15);

	spark_x = x + 16;
	spark_y = y + 16;

	if(effect_list) 
	for(i=0;i<sparks_amt;i++)
		effect_list->push_back((ZEffect*)(new EDeathSparks(ztime, spark_x, spark_y)));

}
