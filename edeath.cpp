#include <algorithm>
#include "edeath.h"
#include "edeathsparks.h"

bool EDeath::finished_init = false;
ZSDL_Surface EDeath::jeep_wasted;
ZSDL_Surface EDeath::momissile_wasted;
ZSDL_Surface EDeath::apc_wasted;
ZSDL_Surface EDeath::crane_wasted;

EDeath::EDeath(ZTime *ztime_, int x_, int y_, int object, ZSDL_Surface *prefered_surface) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;
	int i, max;
	int xs, ys, ws, hs;
	wasted_img = NULL;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	x = x_;
	y = y_;
	final_time = the_time + 5 + (rand() % 3);

	switch(object)
	{
	case EDEATH_JEEP:
		wasted_img = &jeep_wasted;

		xs = 5;
		ys = 14;
		ws = 22;
		hs = 10;
		break;
	case EDEATH_MOMISSILE:
		wasted_img = &momissile_wasted;

		xs = 5;
		ys = 10;
		ws = 21;
		hs = 19;
		break;
	case EDEATH_APC:
		wasted_img = &apc_wasted;

		xs = 5;
		ys = 8;
		ws = 18;
		hs = 20;
		break;
	case EDEATH_CRANE:
		wasted_img = &crane_wasted;

		xs = 4;
		ys = 9;
		ws = 23;
		hs = 19;
		break;
	case EDEATH_TANK:
		if(!prefered_surface)
		{
			killme = true;
			return;
		}
		wasted_img = prefered_surface;

		xs = 8;
		ys = 8;
		ws = 16;
		hs = 16;
		break;
	default:
		killme = true;
		return;
		break;
	}

	max = 3 + (rand() % 3); //little fires
	for(i=0;i<max;i++) extra_effects.push_back(new EStandard(ztime, x + xs + (rand() % ws), y + ys + (rand() % hs), EDEATH_LITTLE_FIRE));
	max = 1 + (rand() % 2); //big smokes
	for(i=0;i<max;i++) extra_effects.push_back(new EStandard(ztime, x + xs + (rand() % ws), y + ys + (rand() % hs), EDEATH_BIG_SMOKE));
	max = (rand() % 2); //small fire smokes
	for(i=0;i<max;i++) extra_effects.push_back(new EStandard(ztime, x + xs + (rand() % ws), y + ys + (rand() % hs), EDEATH_SMALL_FIRE_SMOKE));

	//sort effects
	sort (extra_effects.begin(), extra_effects.end(), sort_estandards_func);
}

EDeath::~EDeath()
{
	//no memory leaks
	for(vector<EStandard*>::iterator i=extra_effects.begin(); i!=extra_effects.begin(); i++)
		delete *i;
}

void EDeath::Init()
{
	jeep_wasted.LoadBaseImage("assets/units/vehicles/jeep/wasted.png");
	momissile_wasted.LoadBaseImage("assets/units/vehicles/missile_launcher/wasted.png");
	apc_wasted.LoadBaseImage("assets/units/vehicles/apc/wasted.png");
	crane_wasted.LoadBaseImage("assets/units/vehicles/crane/wasted_null.png");

	finished_init = true;
}

void EDeath::Process()
{
	double &the_time = ztime->ztime;

	if(killme) return;

	if(the_time >= final_time) 
	{	
		killme = true;
		DoSparks();
		return;
	}

	for(vector<EStandard*>::iterator i=extra_effects.begin(); i!=extra_effects.end(); i++)
		(*i)->Process();
}

void EDeath::DoRender(ZMap &zmap, SDL_Surface *dest)
{
	SDL_Rect from_rect, to_rect;

	if(killme) return;

	zmap.RenderZSurface(wasted_img, x, y);
	//if(zmap.GetBlitInfo( wasted_img, x, y, from_rect, to_rect))
	//	SDL_BlitSurface( wasted_img, &from_rect, dest, &to_rect);

	for(vector<EStandard*>::iterator i=extra_effects.begin(); i!=extra_effects.end(); i++)
		(*i)->DoRender(zmap, dest);
}

void EDeath::DoSparks()
{
	int sparks_amt;
	int spark_x, spark_y;
	int i;

	sparks_amt = 40 + (rand() % 30);

	spark_x = x + 16;
	spark_y = y + 16;

	if(effect_list) 
	for(i=0;i<sparks_amt;i++)
		effect_list->push_back((ZEffect*)(new EDeathSparks(ztime, spark_x, spark_y)));

}
