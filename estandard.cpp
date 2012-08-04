#include "estandard.h"

bool EStandard::finished_init = false;
ZSDL_Surface EStandard::edeath_big_smoke[4];
ZSDL_Surface EStandard::edeath_little_fire[4];
ZSDL_Surface EStandard::edeath_small_fire_smoke[4];
ZSDL_Surface EStandard::edeath_fire[4];

EStandard::EStandard(ZTime *ztime_, int x_, int y_, int object) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;
	int i;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	bx = x_;
	by = y_;

	max_render = 0;

	switch(object)
	{
	case EDEATH_BIG_SMOKE:
		max_render = 4;
		render_img = edeath_big_smoke;
		//for(i=0;i<max_render;i++) render_img[i] = edeath_big_smoke[i];
		x = x_ - 16;
		y = y_ - 32;
		break;
	case EDEATH_LITTLE_FIRE:
		max_render = 4;
		render_img = edeath_little_fire;
		//for(i=0;i<max_render;i++) render_img[i] = edeath_little_fire[i];
		x = x_ - 4;
		y = y_ - 8;
		break;
	case EDEATH_SMALL_FIRE_SMOKE:
		max_render = 4;
		render_img = edeath_small_fire_smoke;
		//for(i=0;i<max_render;i++) render_img[i] = edeath_small_fire_smoke[i];
		x = x_ - 8;
		y = y_ - 16;
		break;
	case EDEATH_FIRE:
		max_render = 4;
		render_img = edeath_fire;
		//for(i=0;i<max_render;i++) render_img[i] = edeath_fire[i];
		x = x_ - 4;
		y = y_ - 8;
		break;
	default:
		killme = true;
		break;
	}

	//be unique
	render_i = rand() % max_render;

	next_process_time = the_time + 0.1;
}

void EStandard::Init()
{
	int i;
	char filename_c[500];

	for(i=0;i<4;i++)
		{
			sprintf(filename_c, "assets/units/vehicles/death_effects/big_smoke_n%02d.png", i);
			edeath_big_smoke[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);

			sprintf(filename_c, "assets/units/vehicles/death_effects/little_fire_n%02d.png", i);
			edeath_little_fire[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);

			sprintf(filename_c, "assets/units/vehicles/death_effects/small_fire_smoke_n%02d.png", i);
			edeath_small_fire_smoke[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);

			sprintf(filename_c, "assets/units/vehicles/death_effects/fire_n%02d.png", i);
			edeath_fire[i].LoadBaseImage(filename_c);// = IMG_Load_Error(filename_c);
		}

	finished_init = true;
}

void EStandard::Process()
{
	double &the_time = ztime->ztime;

	if(killme) return;

	if(the_time >= next_process_time)
	{
		render_i++;
		if(render_i>=max_render) render_i=0;

		next_process_time = the_time + 0.15;
	}
}

void EStandard::DoRender(ZMap &zmap, SDL_Surface *dest)
{
	SDL_Rect from_rect, to_rect;

	if(killme) return;

	zmap.RenderZSurface(&render_img[render_i], x, y);
	//if(zmap.GetBlitInfo( render_img[render_i], x, y, from_rect, to_rect))
	//	SDL_BlitSurface( render_img[render_i], &from_rect, dest, &to_rect);
}

bool sort_estandards_func (EStandard *a, EStandard *b)
{
	return ((a->by) < (b->by));
}