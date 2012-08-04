#include "etoughmushroom.h"

bool EToughMushroom::finished_init = false;
ZSDL_Surface EToughMushroom::base_img[12];
const int etoughmushroom_shift_y[12] =
{
	14,
	9,
	2,
	0,
	0,
	0,
	1,
	2,
	3,
	4,
	5,
	6
};

EToughMushroom::EToughMushroom(ZTime *ztime_, int x_, int y_, double size) : ZEffect(ztime_)
{
	double &the_time = ztime->ztime;

	if(!finished_init)
	{
		killme = true;
		return;
	}

	render_i = 0;

	//for(int i=0;i<12;i++)
	//	render_img[i] = base_img[i].GetImage(size);

	zoom_size = size;
	//size *= 10;
	//zoom_size = (int)size;
	//zoom_size /= 10;

	x = x_ - (int)(16 * zoom_size);//(render_img[0]->w / 2);
	y = y_ - (int)(32 * zoom_size);//(render_img[0]->h / 2);

	next_process_time = the_time + 0.08;
}

void EToughMushroom::Init()
{
	int i;
	char filename_c[500];

	for(i=0;i<12;i++)
		{
			sprintf(filename_c, "assets/units/robots/tough/mushroom_n%02d.png", i);
			//render_img[i] = IMG_Load_Error(filename_c);
			base_img[i].LoadBaseImage(filename_c);
		}

	finished_init = true;
}

void EToughMushroom::Process()
{
	double &the_time = ztime->ztime;

	if(killme) return;

	if(the_time >= next_process_time)
	{
		next_process_time = the_time + 0.08;

		render_i++;
		if(render_i>=12)
			killme = true;
	}

}

void EToughMushroom::DoRender(ZMap &zmap, SDL_Surface *dest)
{
	SDL_Rect from_rect, to_rect;

	if(killme) return;

	base_img[render_i].SetSize(zoom_size);

	zmap.RenderZSurface(&base_img[render_i], x, y + (etoughmushroom_shift_y[render_i] * zoom_size));

	//if(zmap.GetBlitInfo( render_img[render_i], x, y + (etoughmushroom_shift_y[render_i] * zoom_size), from_rect, to_rect))
	//	SDL_BlitSurface( render_img[render_i], &from_rect, dest, &to_rect);
}
