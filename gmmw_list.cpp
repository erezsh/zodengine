#include "zgui_main_menu_widgets.h"

bool GMMWList::finished_init = false;
ZSDL_Surface GMMWList::list_top_left_img;
ZSDL_Surface GMMWList::list_top_img;
ZSDL_Surface GMMWList::list_top_right_img;
ZSDL_Surface GMMWList::list_left_img;
ZSDL_Surface GMMWList::list_right_img;
ZSDL_Surface GMMWList::list_bottom_left_img;
ZSDL_Surface GMMWList::list_bottom_img;
ZSDL_Surface GMMWList::list_bottom_right_img;
ZSDL_Surface GMMWList::list_entry_top_img[MAX_MMLIST_STATES];
ZSDL_Surface GMMWList::list_entry_left_img[MAX_MMLIST_STATES];
ZSDL_Surface GMMWList::list_entry_center_img[MAX_MMLIST_STATES];
ZSDL_Surface GMMWList::list_entry_right_img[MAX_MMLIST_STATES];
ZSDL_Surface GMMWList::list_entry_bottom_img[MAX_MMLIST_STATES];
ZSDL_Surface GMMWList::list_button_up_img[MAX_MMLIST_STATES];
ZSDL_Surface GMMWList::list_button_down_img[MAX_MMLIST_STATES];
ZSDL_Surface GMMWList::list_scroller_img;

GMMWList::GMMWList() : ZGMMWidget()
{
	widget_type = MMLIST_WIDGET;

	view_i = 0;
	SetVisibleEntries(MMLIST_MIN_ENTRIES);
	up_button_state = MMLIST_NORMAL;
	down_button_state = MMLIST_NORMAL;
	button_down_time = 0;
}

void GMMWList::Init()
{
	list_top_left_img.LoadBaseImage("assets/other/main_menu_gui/list/list_top_left.png");
	list_top_img.LoadBaseImage("assets/other/main_menu_gui/list/list_top.png");
	list_top_right_img.LoadBaseImage("assets/other/main_menu_gui/list/list_top_right.png");
	list_left_img.LoadBaseImage("assets/other/main_menu_gui/list/list_left.png");
	list_right_img.LoadBaseImage("assets/other/main_menu_gui/list/list_right.png");
	list_bottom_left_img.LoadBaseImage("assets/other/main_menu_gui/list/list_bottom_left.png");
	list_bottom_img.LoadBaseImage("assets/other/main_menu_gui/list/list_bottom.png");
	list_bottom_right_img.LoadBaseImage("assets/other/main_menu_gui/list/list_bottom_right.png");

	list_scroller_img.LoadBaseImage("assets/other/main_menu_gui/list/list_scroller.png");

	for(int i=0;i<MAX_MMLIST_STATES;i++)
	{
		string token;

		switch(i)
		{
		case MMLIST_NORMAL: token = "normal"; break;
		case MMLIST_PRESSED: token = "pressed"; break;
		}

		list_entry_top_img[i].LoadBaseImage("assets/other/main_menu_gui/list/list_entry_top_" + token + ".png");
		list_entry_left_img[i].LoadBaseImage("assets/other/main_menu_gui/list/list_entry_left_" + token + ".png");
		list_entry_center_img[i].LoadBaseImage("assets/other/main_menu_gui/list/list_entry_center_" + token + ".png");
		list_entry_right_img[i].LoadBaseImage("assets/other/main_menu_gui/list/list_entry_right_" + token + ".png");
		list_entry_bottom_img[i].LoadBaseImage("assets/other/main_menu_gui/list/list_entry_bottom_" + token + ".png");

		list_button_up_img[i].LoadBaseImage("assets/other/main_menu_gui/list/list_button_up_" + token + ".png");
		list_button_down_img[i].LoadBaseImage("assets/other/main_menu_gui/list/list_button_down_" + token + ".png");
	}

	finished_init = true;
}

void GMMWList::Process()
{
	const int clicks_per_second = 30;
	int clicks_to_go;
	double time_dif;
	double the_time;

	if(up_button_state==MMLIST_PRESSED)
	{
		the_time = current_time();
		time_dif = the_time - button_down_time;
		if(time_dif < 0) return;
		clicks_to_go = (int)(time_dif * (double)clicks_per_second);

		if(clicks_to_go)
		{
			view_i -= clicks_to_go;
			if(view_i < 0) view_i = 0;
			button_down_time = the_time;
		}
	}
	
	if(down_button_state==MMLIST_PRESSED)
	{
		the_time = current_time();
		time_dif = the_time - button_down_time;
		if(time_dif < 0) return;
		clicks_to_go = (int)(time_dif * (double)clicks_per_second);

		if(clicks_to_go)
		{
			view_i += clicks_to_go;
			CheckViewI();
			button_down_time = the_time;
		}
	}
}

void GMMWList::SetVisibleEntries(int visible_entries_)
{
	visible_entries = visible_entries_;

	if(visible_entries < MMLIST_MIN_ENTRIES) visible_entries = MMLIST_MIN_ENTRIES;

	SetHeight();
}

void GMMWList::SetHeight()
{
	h = 3 + (visible_entries * MMLIST_ENTRY_HEIGHT) + 2;
}

int GMMWList::GetFirstSelected()
{
	for(int i=0;i<(int)entry_list.size();i++)
		if(entry_list[i].state == MMLIST_PRESSED)
			return i;

	return -1;
}

void GMMWList::UnSelectAll(int except_entry)
{
	for(int i=0;i<(int)entry_list.size();i++)
		if(i != except_entry)
			entry_list[i].state = MMLIST_NORMAL;
}

void GMMWList::CheckViewI()
{
	int available_slots;

	available_slots = entry_list.size() - visible_entries;

	if(view_i > available_slots) view_i = available_slots;
	if(view_i < 0) view_i = 0;
}

bool GMMWList::Click(int x_, int y_)
{
	int entry_clicked;

	the_flags.clear();

	x_-=x;
	y_-=y;

	//buttons?
	if(WithinUpButton(x_, y_)) 
	{ 
		up_button_state=MMLIST_PRESSED; 
		button_down_time=current_time()+0.2;
		return true; 
	}
	if(WithinDownButton(x_, y_)) 
	{ 
		down_button_state=MMLIST_PRESSED; 
		button_down_time=current_time()+0.2;
		return true; 
	}

	//entry?
	entry_clicked = WithinEntry(x_, y_);
	if((entry_clicked != -1) && (entry_clicked < (int)entry_list.size()))
	{
		if(entry_list[entry_clicked].state == MMLIST_NORMAL)
			entry_list[entry_clicked].state = MMLIST_PRESSED;
		else
			entry_list[entry_clicked].state = MMLIST_NORMAL;

		//flag stuff
		the_flags.mmlist_entry_selected = entry_clicked;

		return true;
	}

	return false;
}

bool GMMWList::UnClick(int x_, int y_)
{
	int prev_up_button_state;
	int prev_down_button_state;

	x_-=x;
	y_-=y;

	prev_up_button_state = up_button_state;
	prev_down_button_state = down_button_state;

	up_button_state = MMLIST_NORMAL;
	down_button_state = MMLIST_NORMAL;

	if(prev_up_button_state==MMLIST_PRESSED && WithinUpButton(x_, y_)) { return MoveUp(); }
	if(prev_down_button_state==MMLIST_PRESSED && WithinDownButton(x_, y_)) { return MoveDown(); }

	return false;
}

int GMMWList::WithinEntry(int x_, int y_)
{
	int entry_found;

	if(!list_top_img.GetBaseSurface()) return -1;
	if(!list_left_img.GetBaseSurface()) return -1;

	if(x_ < list_left_img.GetBaseSurface()->w) return -1;
	if(x_ > w - (list_left_img.GetBaseSurface()->w + list_right_img.GetBaseSurface()->w)) return -1;
	if(y_ < list_top_img.GetBaseSurface()->h) return -1;
	if(y_ > list_top_img.GetBaseSurface()->h + (visible_entries * MMLIST_ENTRY_HEIGHT)) return -1;

	entry_found = (y_ - list_top_img.GetBaseSurface()->h) / 13;

	if(entry_found < 0) return -1;
	if(entry_found >= visible_entries) return -1;

	entry_found += view_i;

	if(entry_found < 0) return -1;
	if(entry_found >= (int)entry_list.size()) return -1;

	return entry_found;
}

bool GMMWList::WithinUpButton(int x_, int y_)
{
	const int iw = 11;
	const int ih = 8;
	int ix, iy;

	ix = (w - MMLIST_UP_BUTTON_FROM_RIGHT);
	iy = MMLIST_UP_BUTTON_FROM_TOP;
	
	if(x_ < ix) return false;
	if(y_ < iy) return false;
	if(x_ > ix + iw) return false;
	if(y_ > iy + ih) return false;

	return true;
}

bool GMMWList::WithinDownButton(int x_, int y_)
{
	const int iw = 11;
	const int ih = 8;
	int ix, iy;

	ix = (w - MMLIST_DOWN_BUTTON_FROM_RIGHT);
	iy = h - MMLIST_DOWN_BUTTON_FROM_BOTTOM;
	
	if(x_ < ix) return false;
	if(y_ < iy) return false;
	if(x_ > ix + iw) return false;
	if(y_ > iy + ih) return false;

	return true;
}

bool GMMWList::MoveUp()
{
	view_i--;

	if(view_i<0)
	{
		view_i = 0;
		return false;
	}

	return true;
}

bool GMMWList::MoveDown()
{
	int available_slots;

	view_i++;

	available_slots =  entry_list.size() - visible_entries;

	if(view_i > available_slots)
	{
		view_i = available_slots;

		if(view_i<0) view_i = 0;

		return false;
	}

	return true;
}

void GMMWList::DoRender(ZMap &the_map, SDL_Surface *dest, int tx, int ty)
{
	if(!finished_init) return;
	if(!active) return;

	tx += x;
	ty += y;

	RenderBackround(the_map, dest, tx, ty);
	RenderEntries(the_map, dest, tx, ty);
	RenderControls(the_map, dest, tx, ty);
}

void GMMWList::RenderBackround(ZMap &the_map, SDL_Surface *dest, int tx, int ty)
{
	int ix, iy, iw, ih;

	if(!list_top_left_img.GetBaseSurface()) return;
	if(!list_top_img.GetBaseSurface()) return;
	if(!list_top_right_img.GetBaseSurface()) return;
	if(!list_left_img.GetBaseSurface()) return;
	if(!list_right_img.GetBaseSurface()) return;
	if(!list_bottom_left_img.GetBaseSurface()) return;
	if(!list_bottom_img.GetBaseSurface()) return;
	if(!list_bottom_right_img.GetBaseSurface()) return;

	//top left
	list_top_left_img.BlitSurface(NULL, tx, ty);

	//top right
	{
		ix = tx + (w - list_top_right_img.GetBaseSurface()->w);
		list_top_right_img.BlitSurface(NULL, ix, ty);
	}

	//top
	{
		ix = tx + list_top_left_img.GetBaseSurface()->w;
		iw = w - (list_top_left_img.GetBaseSurface()->w + list_top_right_img.GetBaseSurface()->w);
		list_top_img.RenderSurfaceHorzRepeat(ix, ty, iw);
	}

	//left
	{
		iy = ty + list_top_left_img.GetBaseSurface()->h;
		ih = h - (list_top_left_img.GetBaseSurface()->h + list_bottom_left_img.GetBaseSurface()->h);
		list_left_img.RenderSurfaceVertRepeat(tx, iy, ih);
	}

	//right
	{
		ix = tx + (w - list_top_right_img.GetBaseSurface()->w);
		iy = ty + list_top_right_img.GetBaseSurface()->h;
		ih = h - (list_top_right_img.GetBaseSurface()->h + list_bottom_right_img.GetBaseSurface()->h);
		list_right_img.RenderSurfaceVertRepeat(ix, iy, ih);
	}

	//bottom left
	{
		iy = ty + (h - list_bottom_left_img.GetBaseSurface()->h);
		list_bottom_left_img.BlitSurface(NULL, tx, iy);
	}

	//bottom right
	{
		ix = tx + (w - list_bottom_right_img.GetBaseSurface()->w);
		iy = ty + (h - list_bottom_right_img.GetBaseSurface()->h);
		list_bottom_right_img.BlitSurface(NULL, ix, iy);
	}

	//bottom
	{
		ix = tx + list_bottom_left_img.GetBaseSurface()->w;
		iy = ty + (h - list_bottom_img.GetBaseSurface()->h);
		iw = w - (list_bottom_left_img.GetBaseSurface()->w + list_bottom_right_img.GetBaseSurface()->w);
		list_bottom_img.RenderSurfaceHorzRepeat(ix, iy, iw);
	}
}

void GMMWList::RenderEntries(ZMap &the_map, SDL_Surface *dest, int tx, int ty)
{
	int ix, iy, iw;
	int i, j;

	if(!list_top_img.GetBaseSurface()) return;
	if(!list_left_img.GetBaseSurface()) return;

	ix = tx + list_left_img.GetBaseSurface()->w;
	iy = ty + list_top_img.GetBaseSurface()->h;
	iw = w - (list_left_img.GetBaseSurface()->w + list_right_img.GetBaseSurface()->w);

	for(i=0,j=view_i;i<visible_entries;i++,j++)
	{
		if(j < (int)entry_list.size())
		{
			ZSDL_Surface text_img;

			text_img.LoadBaseImage(ZFontEngine::GetFont(SMALL_WHITE_FONT).Render(entry_list[j].text.c_str()));

			RenderEntry(the_map, dest, ix, iy + (i*MMLIST_ENTRY_HEIGHT), iw, &text_img, entry_list[j].state);
		}
		else
			RenderEntry(the_map, dest, ix, iy + (i*MMLIST_ENTRY_HEIGHT), iw, NULL, MMLIST_NORMAL);
	}
}

void GMMWList::RenderEntry(ZMap &the_map, SDL_Surface *dest, int tx, int ty, int tw, ZSDL_Surface *text, int state)
{
	int ix, iy, iw;//, ih;

	ZSDL_Surface &top = list_entry_top_img[state];
	ZSDL_Surface &left = list_entry_left_img[state];
	ZSDL_Surface &center = list_entry_center_img[state];
	ZSDL_Surface &right = list_entry_right_img[state];
	ZSDL_Surface &bottom = list_entry_bottom_img[state];

	if(!top.GetBaseSurface()) return;
	if(!left.GetBaseSurface()) return;
	if(!center.GetBaseSurface()) return;
	if(!right.GetBaseSurface()) return;
	if(!bottom.GetBaseSurface()) return;

	//left
	left.BlitSurface(NULL, tx, ty);

	//right
	{
		ix = tx + (tw - right.GetBaseSurface()->w);
		right.BlitSurface(NULL, ix, ty);
	}

	//top
	{
		ix = tx + left.GetBaseSurface()->w;
		iw = tw - (left.GetBaseSurface()->w + right.GetBaseSurface()->w);
		top.RenderSurfaceHorzRepeat(ix, ty, iw);
	}

	//bottom
	{
		ix = tx + left.GetBaseSurface()->w;
		iy = ty + (MMLIST_ENTRY_HEIGHT - bottom.GetBaseSurface()->h);
		iw = tw - (left.GetBaseSurface()->w + right.GetBaseSurface()->w);
		bottom.RenderSurfaceHorzRepeat(ix, iy, iw);
	}

	//center
	{
		ix = tx + left.GetBaseSurface()->w;
		iy = ty + top.GetBaseSurface()->h;
		iw = tw - (left.GetBaseSurface()->w + right.GetBaseSurface()->w);
		center.RenderSurfaceHorzRepeat(ix, iy, iw);
	}

	if(text && text->GetBaseSurface())
		text->BlitSurface(0, 0, tw - 8, text->GetBaseSurface()->h, NULL, tx + 4, ty + 4);
}

void GMMWList::RenderControls(ZMap &the_map, SDL_Surface *dest, int tx, int ty)
{
	int ix, iy;

	//buttons
	{
		ix = tx + (w - MMLIST_UP_BUTTON_FROM_RIGHT);
		iy = ty + MMLIST_UP_BUTTON_FROM_TOP;
		list_button_up_img[up_button_state].BlitSurface(NULL, ix, iy);

		//ix = tx + (w - MMLIST_DOWN_BUTTON_FROM_RIGHT);
		iy = ty + (h - MMLIST_DOWN_BUTTON_FROM_BOTTOM);
		list_button_down_img[down_button_state].BlitSurface(NULL, ix, iy);
	}

	//scroller
	if(list_top_right_img.GetBaseSurface() && list_bottom_right_img.GetBaseSurface())
	{
		int total_space;
		int available_slots;
		double percent_down;

		total_space = h - (list_top_right_img.GetBaseSurface()->h + list_bottom_right_img.GetBaseSurface()->h);

		available_slots = entry_list.size() - visible_entries;
		if(available_slots < 0) available_slots = 0;

		percent_down = 1.0 * (view_i+1) / (available_slots+2);
		if(percent_down<0) percent_down=0;
		if(percent_down>1) percent_down=1;

		ix = tx + (w - MMLIST_SCROLLER_FROM_RIGHT);
		iy = ty + list_top_right_img.GetBaseSurface()->h + (int)((double)total_space * percent_down) - 2;

		list_scroller_img.BlitSurface(NULL, ix, iy);
	}
}

bool sort_mmlist_entry_func (const mmlist_entry &a, const mmlist_entry &b)
{
	return a.sort_number < b.sort_number;
}
