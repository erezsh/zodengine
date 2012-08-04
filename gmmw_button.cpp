#include "zgui_main_menu_widgets.h"

bool GMMWButton::finished_init = false;
ZSDL_Surface GMMWButton::generic_top_left_img[MAX_MMBUTTON_STATES];
ZSDL_Surface GMMWButton::generic_top_img[MAX_MMBUTTON_STATES];
ZSDL_Surface GMMWButton::generic_top_right_img[MAX_MMBUTTON_STATES];
ZSDL_Surface GMMWButton::generic_left_img[MAX_MMBUTTON_STATES];
ZSDL_Surface GMMWButton::generic_center_img[MAX_MMBUTTON_STATES];
ZSDL_Surface GMMWButton::generic_right_img[MAX_MMBUTTON_STATES];
ZSDL_Surface GMMWButton::generic_bottom_left_img[MAX_MMBUTTON_STATES];
ZSDL_Surface GMMWButton::generic_bottom_img[MAX_MMBUTTON_STATES];
ZSDL_Surface GMMWButton::generic_bottom_right_img[MAX_MMBUTTON_STATES];
ZSDL_Surface GMMWButton::non_generic_img[MAX_MMBUTTON_TYPES][MAX_MMBUTTON_STATES];

GMMWButton::GMMWButton() : ZGMMWidget()
{
	widget_type = MMBUTTON_WIDGET;

	rerender_text = false;
	state = MMBUTTON_NORMAL;
	type = MMGENERIC_BUTTON;
	is_green = false;
}

void GMMWButton::Init()
{
	string filename;

	//normal button images
	for(int t=MMGENERIC_BUTTON+1;t<MAX_MMBUTTON_TYPES;t++)
		for(int i=0;i<MAX_MMBUTTON_STATES;i++)
		{
			string token;

			switch(i)
			{
			case MMBUTTON_NORMAL: token = "normal"; break;
			case MMBUTTON_PRESSED: token = "pressed"; break;
			case MMBUTTON_GREEN: token = "normal"; break;
			}

			non_generic_img[t][i].LoadBaseImage("assets/other/main_menu_gui/" + mmbutton_type_string[t] + "_button_" + token + ".png");
		}


	//generic button images
	for(int i=0;i<MAX_MMBUTTON_STATES;i++)
	{
		string token;

		switch(i)
		{
		case MMBUTTON_NORMAL: token = "normal"; break;
		case MMBUTTON_PRESSED: token = "pressed"; break;
		case MMBUTTON_GREEN: token = "green"; break;
		}

		generic_top_left_img[i].LoadBaseImage("assets/other/main_menu_gui/generic_button_" + token + "_top_left.png");
		generic_top_img[i].LoadBaseImage("assets/other/main_menu_gui/generic_button_" + token + "_top.png");
		generic_top_right_img[i].LoadBaseImage("assets/other/main_menu_gui/generic_button_" + token + "_top_right.png");

		generic_left_img[i].LoadBaseImage("assets/other/main_menu_gui/generic_button_" + token + "_left.png");
		generic_center_img[i].LoadBaseImage("assets/other/main_menu_gui/generic_button_" + token + "_center.png");
		generic_right_img[i].LoadBaseImage("assets/other/main_menu_gui/generic_button_" + token + "_right.png");

		generic_bottom_left_img[i].LoadBaseImage("assets/other/main_menu_gui/generic_button_" + token + "_bottom_left.png");
		generic_bottom_img[i].LoadBaseImage("assets/other/main_menu_gui/generic_button_" + token + "_bottom.png");
		generic_bottom_right_img[i].LoadBaseImage("assets/other/main_menu_gui/generic_button_" + token + "_bottom_right.png");
	}

	finished_init = true;
}

void GMMWButton::DoRender(ZMap &the_map, SDL_Surface *dest, int tx, int ty)
{
	if(!finished_init) return;
	if(!active) return;

	//shift to our rendering point
	tx+=x;
	ty+=y;

	if(type==MMGENERIC_BUTTON)
		RenderGeneric(the_map, dest, tx, ty);
	else
	{
		//specific backround images
		//non_generic_img[type][state].RenderSurface(tx, ty);
		non_generic_img[type][state].BlitSurface(NULL, tx, ty);
	}

	//text
	{
		MakeTextImage();

		if(text_img.GetBaseSurface())
		{
			int added_shift=0;

			if(state==MMBUTTON_PRESSED) added_shift=1;

			//text_img.RenderSurface((tx + (w >> 1)) - (text_img.GetBaseSurface()->w >> 1), ty + 3 + added_shift);
			text_img.BlitSurface(NULL, (tx + (w >> 1)) - (text_img.GetBaseSurface()->w >> 1), ty + 3 + added_shift);
		}
	}
}

void GMMWButton::RenderGeneric(ZMap &the_map, SDL_Surface *dest, int tx, int ty)
{
	int ix, iy, iw, ih;
	int state_to_use;

	if(state == MMBUTTON_NORMAL && is_green) 
		state_to_use = MMBUTTON_GREEN;
	else
		state_to_use = state;

	ZSDL_Surface &top_left = generic_top_left_img[state_to_use];
	ZSDL_Surface &top = generic_top_img[state_to_use];
	ZSDL_Surface &top_right = generic_top_right_img[state_to_use];
	ZSDL_Surface &left = generic_left_img[state_to_use];
	ZSDL_Surface &center = generic_center_img[state_to_use];
	ZSDL_Surface &right = generic_right_img[state_to_use];
	ZSDL_Surface &bottom_left = generic_bottom_left_img[state_to_use];
	ZSDL_Surface &bottom = generic_bottom_img[state_to_use];
	ZSDL_Surface &bottom_right = generic_bottom_right_img[state_to_use];

	if(!top_left.GetBaseSurface()) return;
	if(!top.GetBaseSurface()) return;
	if(!top_right.GetBaseSurface()) return;
	if(!left.GetBaseSurface()) return;
	if(!center.GetBaseSurface()) return;
	if(!right.GetBaseSurface()) return;
	if(!bottom_left.GetBaseSurface()) return;
	if(!bottom.GetBaseSurface()) return;
	if(!bottom_right.GetBaseSurface()) return;

	//top left
	//top_left.RenderSurface(tx, ty);
	top_left.BlitSurface(NULL, tx, ty);

	//top right
	{
		ix = (tx + w) - top_right.GetBaseSurface()->w;
		//top_right.RenderSurface(ix, ty);
		top_right.BlitSurface(NULL, ix, ty);
	}

	//top
	{
		ix = tx + top_left.GetBaseSurface()->w;
		iw = w - (top_left.GetBaseSurface()->w + top_right.GetBaseSurface()->w);
		top.RenderSurfaceHorzRepeat(ix, ty, iw);
	}

	//left
	{
		iy = ty + top_left.GetBaseSurface()->h;
		ih = h - (top_left.GetBaseSurface()->h + bottom_left.GetBaseSurface()->h);
		left.RenderSurfaceVertRepeat(tx, iy, ih);
	}

	//right
	{
		ix = (tx + w) - right.GetBaseSurface()->w;
		iy = ty + top_right.GetBaseSurface()->h;
		ih = h - (top_right.GetBaseSurface()->h + bottom_right.GetBaseSurface()->h);
		right.RenderSurfaceVertRepeat(ix, iy, ih);
	}

	//bottom left
	{
		iy = (ty + h) - bottom_left.GetBaseSurface()->h;
		//bottom_left.RenderSurface(tx, iy);
		bottom_left.BlitSurface(NULL, tx, iy);
	}

	//bottom right
	{
		ix = (tx + w) - bottom_right.GetBaseSurface()->w;
		iy = (ty + h) - bottom_right.GetBaseSurface()->h;
		//bottom_right.RenderSurface(ix, iy);
		bottom_right.BlitSurface(NULL, ix, iy);
	}

	//bottom
	{
		ix = tx + bottom_left.GetBaseSurface()->w;
		iy = (ty + h) - bottom.GetBaseSurface()->h;
		iw = w - (bottom_left.GetBaseSurface()->w + bottom_right.GetBaseSurface()->w);
		bottom.RenderSurfaceHorzRepeat(ix, iy, iw);
	}

	//main area
	{
		ix = tx + left.GetBaseSurface()->w;
		iy = ty + top_left.GetBaseSurface()->h;
		iw = w - (left.GetBaseSurface()->w + right.GetBaseSurface()->w);
		ih = h - (top_left.GetBaseSurface()->h + bottom.GetBaseSurface()->h);
		center.RenderSurfaceAreaRepeat(ix, iy, iw, ih);
	}
}

void GMMWButton::MakeTextImage()
{
	if(!rerender_text) return;

	if(text.length())
		text_img.LoadBaseImage(ZFontEngine::GetFont(YELLOW_MENU_FONT).Render(text.c_str()));
	else
		text_img.Unload();

	rerender_text = false;
}

void GMMWButton::DetermineDimensions()
{
	switch(type)
	{
	case MMCLOSE_BUTTON:
		w=12;
		h=12;
		break;
	}
}

bool GMMWButton::Click(int x_, int y_)
{
	if(!active) return false;
	if(!WithinDimensions(x_, y_)) return false;

	state = MMBUTTON_PRESSED;

	return true;
}

bool GMMWButton::UnClick(int x_, int y_)
{
	int previous_state = state;

	state = MMBUTTON_NORMAL;

	if(!active) return false;
	if(!WithinDimensions(x_, y_)) return false;

	//call an unclick event if we were previously pressed
	//and the unclick is over the button
	return (previous_state == MMBUTTON_PRESSED);
}
