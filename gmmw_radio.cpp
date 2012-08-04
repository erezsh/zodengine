#include "zgui_main_menu_widgets.h"

bool GMMWRadio::finished_init = false;
ZSDL_Surface GMMWRadio::radio_left_img;
ZSDL_Surface GMMWRadio::radio_center_img;
ZSDL_Surface GMMWRadio::radio_right_img;
ZSDL_Surface GMMWRadio::radio_selector_img;

GMMWRadio::GMMWRadio() : ZGMMWidget()
{
	widget_type = MMRADIO_WIDGET;

	si = 0;
	SetMaxSelections(MMRADIO_MIN_SELECTIONS);
	h = MMRADIO_HEIGHT;
}

void GMMWRadio::Init()
{
	radio_left_img.LoadBaseImage("assets/other/main_menu_gui/radio/radio_left.png");
	radio_center_img.LoadBaseImage("assets/other/main_menu_gui/radio/radio_center.png");
	radio_right_img.LoadBaseImage("assets/other/main_menu_gui/radio/radio_right.png");
	radio_selector_img.LoadBaseImage("assets/other/main_menu_gui/radio/radio_selector.png");

	finished_init = true;
}

void GMMWRadio::SetMaxSelections(int selections_)
{
	selections = selections_;

	if(selections<MMRADIO_MIN_SELECTIONS) selections = MMRADIO_MIN_SELECTIONS;

	w = MMRADIO_LEFT_WIDTH + ((selections-2) * MMRADIO_CENTER_WIDTH) + MMRADIO_RIGHT_WIDTH;

	CheckSI();
}

void GMMWRadio::CheckSI()
{
	if(si<0 || si>=selections) 
	{
		printf("GMMWRadio::CheckSI:bad si:%d max_selections:%d\n", si, selections);
		si = 0;
	}
}

bool GMMWRadio::Click(int x_, int y_)
{
	the_flags.clear();

	if(x_ < x) return false;
	if(x_ > x + w) return false;
	if(y_ < y) return false;
	if(y_ > y + h) return false;

	x_ -= x;

	if(x_ < MMRADIO_LEFT_WIDTH)
		the_flags.mmradio_si_selected = 0;
	else if(x_ > w - MMRADIO_RIGHT_WIDTH)
		the_flags.mmradio_si_selected = selections - 1;
	else
		the_flags.mmradio_si_selected = 1 + ((x_ - MMRADIO_LEFT_WIDTH) / MMRADIO_CENTER_WIDTH);

	return true;
}

void GMMWRadio::DoRender(ZMap &the_map, SDL_Surface *dest, int tx, int ty)
{
	if(!finished_init) return;
	if(!active) return;

	if(!radio_left_img.GetBaseSurface()) return;
	if(!radio_center_img.GetBaseSurface()) return;
	if(!radio_right_img.GetBaseSurface()) return;
	if(!radio_selector_img.GetBaseSurface()) return;

	tx += x;
	ty += y;

	//render backround
	{
		//left
		radio_left_img.BlitSurface(NULL, tx, ty);

		//right
		radio_right_img.BlitSurface(NULL, tx + (w - MMRADIO_RIGHT_WIDTH), ty);

		//center
		for(int i=0;i<selections-2;i++)
			radio_center_img.BlitSurface(NULL, tx + MMRADIO_LEFT_WIDTH + (i * MMRADIO_CENTER_WIDTH), ty);
	}

	//render selector
	{
		const int lx = 7;
		const int cx = 4;
		const int rx = 4;
		int ix;

		if(si==0) 
			ix = lx;
		else if(si==selections-1)
			ix = MMRADIO_LEFT_WIDTH + (selections-2)*MMRADIO_CENTER_WIDTH + rx;
		else 
			ix = MMRADIO_LEFT_WIDTH + (si-1)*MMRADIO_CENTER_WIDTH + cx;

		radio_selector_img.BlitSurface(NULL, tx + ix, ty + 1);
	}
}
