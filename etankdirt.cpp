#include "etankdirt.h"

void tank_dirt_graphics::LoadGraphics(int palette)
{
	char filename[500];

	dirts = 2;
	i_max = 5;

	switch(palette)
	{
	case JUNGLE:
		dirts = 1;
		i_max = 6;
		break;
	case CITY:
		dirts = 0;
		i_max = 0;
		break;
	}

	for(int d=0;d<dirts;d++)
		for(int i=0;i<i_max;i++)
		{
			sprintf(filename, "assets/units/vehicles/tank_dirt/tank_dirt_%d_%s_n%02d.png", d, planet_type_string[palette].c_str(), i);
			tank_dirt[d][i].LoadBaseImage(filename);
		}
}

bool ETankDirt::finished_init = false;
tank_dirt_graphics ETankDirt::td_graphics[MAX_PLANET_TYPES];

#define ETANKDIRT_TIME 0.15

ETankDirt::ETankDirt(ZTime *ztime_, int cx_, int cy_, int palette_, bool do_pre_render_) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	palette = palette_;
	cx = cx_;
	cy = cy_;
	do_pre_render = do_pre_render_;

	do_pre_render = true;

	if(!td_graphics[palette].dirts || !td_graphics[palette].i_max)
	{
		killme = true;
		return;
	}

	ni = 0;
	dirt_i = rand() % td_graphics[palette].dirts;

	next_ni_time = the_time + ETANKDIRT_TIME;
}

void ETankDirt::Init()
{
	for(int i=0;i<MAX_PLANET_TYPES;i++)
		td_graphics[i].LoadGraphics(i);

	finished_init = true;
}

void ETankDirt::Process()
{
	double &the_time = ztime->ztime;

	if(killme) return;

	if(the_time >= next_ni_time)
	{
		next_ni_time = the_time + ETANKDIRT_TIME;

		ni++;

		if(ni>=td_graphics[palette].i_max)
		{
			ni = 0;
			killme = true;
		}
	}
}

void ETankDirt::TheRender(ZMap &zmap, SDL_Surface *dest)
{
	int x, y;

	ZSDL_Surface *render_img;

	if(killme) return;

	render_img = &td_graphics[palette].tank_dirt[dirt_i][ni];

	if(!render_img) return;
	if(!render_img->GetBaseSurface()) return;

	x = cx - (render_img->GetBaseSurface()->w >> 1);
	y = cy - render_img->GetBaseSurface()->h;

	zmap.RenderZSurface(render_img, x, y);
}
