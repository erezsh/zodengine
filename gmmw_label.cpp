#include "zgui_main_menu_widgets.h"

GMMWLabel::GMMWLabel() : ZGMMWidget()
{
	widget_type = MMRADIO_WIDGET;

	rerender_text = false;
	justify = MMLABEL_NORMAL;
	h = MMLABEL_HEIGHT;
	font = YELLOW_MENU_FONT;
}

void GMMWLabel::SetText(string text_)
{
	if(text_ == rendered_text) return;

	text = text_; 
	rerender_text = true;
}

void GMMWLabel::DoRender(ZMap &the_map, SDL_Surface *dest, int tx, int ty)
{
	if(!active) return;

	//shift to our rendering point
	tx+=x;
	ty+=y;

	MakeTextImage();

	if(text_img.GetBaseSurface())
	{
		int added_shift=0;

		switch(justify)
		{
		case MMLABEL_CENTER: added_shift = (w >> 1) - (text_img.GetBaseSurface()->w >> 1); break;
		case MMLABEL_RIGHT: added_shift = w - text_img.GetBaseSurface()->w; break;
		}

		text_img.BlitSurface(NULL, tx + added_shift, ty);
	}
}

void GMMWLabel::MakeTextImage()
{
	if(!rerender_text) return;

	if(text.length())
		text_img.LoadBaseImage(ZFontEngine::GetFont(font).Render(text.c_str()));
	else
		text_img.Unload();

	rendered_text = text;
	rerender_text = false;
}
