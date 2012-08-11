#include "zgui_main_menu_base.h"

bool ZGuiMainMenuBase::finished_init = false;
ZSDL_Surface ZGuiMainMenuBase::menu_top_left_img;
ZSDL_Surface ZGuiMainMenuBase::menu_top_right_img;
ZSDL_Surface ZGuiMainMenuBase::menu_top_img;
ZSDL_Surface ZGuiMainMenuBase::menu_left_img;
ZSDL_Surface ZGuiMainMenuBase::menu_right_img;
ZSDL_Surface ZGuiMainMenuBase::menu_bottom_img;
ZSDL_Surface ZGuiMainMenuBase::menu_center_img;
ZSDL_Surface ZGuiMainMenuBase::menu_warning_img;

ZGuiMainMenuBase::ZGuiMainMenuBase()
{
	killme = false;
	menu_type = -1;
	x=y=0;
	w=h=0;
	click_grabbed = false;
	player_info = NULL;
	selectable_map_list = NULL;
	player_team = NULL;
	sound_setting = NULL;
	ztime = NULL;

	close_button.SetType(MMCLOSE_BUTTON);
}

void ZGuiMainMenuBase::Init()
{
	menu_top_left_img.LoadBaseImage("assets/other/main_menu_gui/menu_top_left.png");
	menu_top_right_img.LoadBaseImage("assets/other/main_menu_gui/menu_top_right.png");
	menu_top_img.LoadBaseImage("assets/other/main_menu_gui/menu_top.png");
	menu_left_img.LoadBaseImage("assets/other/main_menu_gui/menu_left.png");
	menu_right_img.LoadBaseImage("assets/other/main_menu_gui/menu_right.png");
	menu_bottom_img.LoadBaseImage("assets/other/main_menu_gui/menu_bottom.png");
	menu_center_img.LoadBaseImage("assets/other/main_menu_gui/menu_center.png");
	menu_warning_img.LoadBaseImage("assets/other/main_menu_gui/menu_warning.png");

	finished_init = true;
}

void ZGuiMainMenuBase::UpdateDimensions()
{
	close_button.SetCoords(w-16, 4);
}

void ZGuiMainMenuBase::SetCenterCoords(int cx_, int cy_)
{
	x = cx_ - (w >> 1);
	y = cy_ - (h >> 1);
}

void ZGuiMainMenuBase::Process()
{
	ProcessWidgets();
}

void ZGuiMainMenuBase::ProcessWidgets()
{
	close_button.Process();

	//widgets
	for(vector<ZGMMWidget*>::iterator i=widget_list.begin(); i!=widget_list.end(); ++i)
		(*i)->Process();
}

bool ZGuiMainMenuBase::WithinDimensions(int x_, int y_)
{
	if(x_<x) return false;
	if(y_<y) return false;
	if(x_>x+w) return false;
	if(y_>y+h) return false;

	return true;
}

bool ZGuiMainMenuBase::IsOverHUD(int hud_left, int hud_top)
{
	if(x + w >= hud_left) return true;
	if(y + h >= hud_top) return true;

	return false;
}

void ZGuiMainMenuBase::Move(double px, double py)
{
	int cx, cy;

	cx = x + (w>>1);
	cy = y + (h>>1);

	cx = (int)((double)cx * px);
	cy = (int)((double)cy * py);

	x = cx - (w>>1);
	y = cy - (h>>1);
}

bool ZGuiMainMenuBase::Motion(int x_, int y_)
{
	bool action_taken = false;
	int tx, ty;

	gmm_flags.clear();

	//are we dragging the main window around?
	if(click_grabbed)
	{
		x = x_ - grab_x;
		y = y_ - grab_y;

		return true;
	}

	tx = x_ - x;
	ty = y_ - y;

	for(vector<ZGMMWidget*>::iterator i=widget_list.begin(); i!=widget_list.end(); ++i)
		if((*i)->Motion(tx, ty))
		{
			HandleWidgetEvent(GMM_MOTION_EVENT, *i);
			action_taken = true;
		}

	return action_taken;
}

bool ZGuiMainMenuBase::KeyPress(int c)
{
	bool action_taken = false;

	gmm_flags.clear();

	for(vector<ZGMMWidget*>::iterator i=widget_list.begin(); i!=widget_list.end(); ++i)
		if((*i)->KeyPress(c))
		{
			HandleWidgetEvent(GMM_KEYPRESS_EVENT, *i);
			action_taken = true;
		}

	return action_taken;
}

bool ZGuiMainMenuBase::WheelUpButton()
{
	bool action_taken = false;

	gmm_flags.clear();

	for(vector<ZGMMWidget*>::iterator i=widget_list.begin(); i!=widget_list.end(); ++i)
		if((*i)->WheelUpButton())
		{
			HandleWidgetEvent(GMM_WHEELUP_EVENT, *i);
			action_taken = true;
		}

	return action_taken;
}

bool ZGuiMainMenuBase::WheelDownButton()
{
	bool action_taken = false;

	gmm_flags.clear();

	for(vector<ZGMMWidget*>::iterator i=widget_list.begin(); i!=widget_list.end(); ++i)
		if((*i)->WheelDownButton())
		{
			HandleWidgetEvent(GMM_WHEELDOWN_EVENT, *i);
			action_taken = true;
		}

	return action_taken;
}

bool ZGuiMainMenuBase::Click(int x_, int y_)
{
	bool action_taken = false;
	int tx, ty;
	bool within_dimensions;

	gmm_flags.clear();

	tx = x_ - x;
	ty = y_ - y;

	//close button
	if(close_button.Click(tx, ty)) action_taken = true;

	//normal widgets
	for(vector<ZGMMWidget*>::iterator i=widget_list.begin(); i!=widget_list.end(); ++i)
		if((*i)->Click(tx, ty))
		{
			HandleWidgetEvent(GMM_CLICK_EVENT, *i);
			action_taken = true;
		}

	within_dimensions = WithinDimensions(x_, y_);

	//grab the window for dragging?
	if(!action_taken && within_dimensions)
	{
		click_grabbed = true;
		grab_x = tx;
		grab_y = ty;
	}

	return action_taken || within_dimensions;
}

bool ZGuiMainMenuBase::UnClick(int x_, int y_)
{
	bool action_taken = false;
	int tx, ty;

	gmm_flags.clear();

	//can't be true at this point
	click_grabbed = false;

	tx = x_ - x;
	ty = y_ - y;

	//close button
	if(close_button.UnClick(tx, ty))
	{
		killme = true;
		return true;
	}

	//normal widgets
	for(vector<ZGMMWidget*>::iterator i=widget_list.begin(); i!=widget_list.end(); ++i)
		if((*i)->UnClick(tx, ty))
		{
			HandleWidgetEvent(GMM_UNCLICK_EVENT, *i);
			action_taken = true;
		}

	return action_taken || WithinDimensions(x_, y_);
}

void ZGuiMainMenuBase::HandleWidgetEvent(int event_type, ZGMMWidget *event_widget)
{
	if(!event_widget) return;
	if(event_type<0) return;
	if(event_type>=MAC_GMM_EVENTS) return;

	printf("ZGuiMainMenuBase::HandleWidgetEvent::%d:%s widget_type:%d ref_id:%d\n", event_type, gmm_event_type_string[event_type].c_str(), event_widget->GetWidgetType(), event_widget->GetRefID());
}

void ZGuiMainMenuBase::DoRender(ZMap &the_map, SDL_Surface *dest)
{
	if(!finished_init) return;

	RenderBase(the_map, dest, x, y);

	//title
	{
		MakeTitle();

		//title_img.RenderSurface(x + 8, y + 6);
		title_img.BlitSurface(NULL, x + 8, y + 6);
	}

	//close button
	close_button.DoRender(the_map, dest, x, y);

	//widgets
	RenderWidgets(the_map, dest);
}

void ZGuiMainMenuBase::RenderWidgets(ZMap &the_map, SDL_Surface *dest)
{
	//widgets
	for(vector<ZGMMWidget*>::iterator i=widget_list.begin(); i!=widget_list.end(); ++i)
		(*i)->DoRender(the_map, dest, x, y);
}

void ZGuiMainMenuBase::RenderBase(ZMap &the_map, SDL_Surface *dest, int tx, int ty)
{
	int ix, iy, iw, ih;

	if(!menu_top_left_img.GetBaseSurface()) return;
	if(!menu_top_right_img.GetBaseSurface()) return;
	if(!menu_top_img.GetBaseSurface()) return;
	if(!menu_left_img.GetBaseSurface()) return;
	if(!menu_right_img.GetBaseSurface()) return;
	if(!menu_bottom_img.GetBaseSurface()) return;
	if(!menu_center_img.GetBaseSurface()) return;

	//top left
	//menu_top_left_img.RenderSurface(tx, ty);
	menu_top_left_img.BlitSurface(NULL, tx, ty);

	//top right
	{
		ix = (tx + w) - menu_top_right_img.GetBaseSurface()->w;
		//menu_top_right_img.RenderSurface(ix, ty);
		menu_top_right_img.BlitSurface(NULL, ix, ty);
	}

	//top
	{
		ix = tx + menu_top_left_img.GetBaseSurface()->w;
		iw = w - (menu_top_left_img.GetBaseSurface()->w + menu_top_right_img.GetBaseSurface()->w);
		menu_top_img.RenderSurfaceHorzRepeat(ix, ty, iw);
	}

	//left
	{
		iy = ty + menu_top_left_img.GetBaseSurface()->h;
		ih = h - menu_top_left_img.GetBaseSurface()->h;
		menu_left_img.RenderSurfaceVertRepeat(tx, iy, ih);
	}

	//right
	{
		ix = (tx + w) - menu_right_img.GetBaseSurface()->w;
		iy = ty + menu_top_right_img.GetBaseSurface()->h;
		ih = h - menu_top_right_img.GetBaseSurface()->h;
		menu_right_img.RenderSurfaceVertRepeat(ix, iy, ih);
	}

	//bottom
	{
		ix = tx + menu_left_img.GetBaseSurface()->w;
		iy = (ty + h) - menu_bottom_img.GetBaseSurface()->h;
		iw = w - (menu_left_img.GetBaseSurface()->w + menu_right_img.GetBaseSurface()->w);
		menu_bottom_img.RenderSurfaceHorzRepeat(ix, iy, iw);
	}

	//main area
	{
		ix = tx + menu_left_img.GetBaseSurface()->w;
		iy = ty + menu_top_left_img.GetBaseSurface()->h;
		iw = w - (menu_left_img.GetBaseSurface()->w + menu_right_img.GetBaseSurface()->w);
		ih = h - (menu_top_left_img.GetBaseSurface()->h + menu_bottom_img.GetBaseSurface()->h);
		menu_center_img.RenderSurfaceAreaRepeat(ix, iy, iw, ih);
	}
}

void ZGuiMainMenuBase::MakeTitle()
{
	if(!title.length()) return;
	if(title_img.GetBaseSurface()) return;

	title_img.LoadBaseImage(ZFontEngine::GetFont(YELLOW_MENU_FONT).Render(title.c_str()));
}
