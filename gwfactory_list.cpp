#include "gwfactory_list.h"
#include "zbuilding.h"

bool GWFactoryList::finished_init = false;

ZSDL_Surface GWFactoryList::main_top_img;
ZSDL_Surface GWFactoryList::main_right_img;
ZSDL_Surface GWFactoryList::main_entry_img;
ZSDL_Surface GWFactoryList::entry_bar_green_img;
ZSDL_Surface GWFactoryList::entry_bar_red_img;
ZSDL_Surface GWFactoryList::entry_bar_grey_img;
ZSDL_Surface GWFactoryList::entry_bar_white_i_img;

GWFactoryList::GWFactoryList(ZTime *ztime_) : ZGuiWindow(ztime_)
{
	up_button.SetType(FUP_GUI_BUTTON);
	down_button.SetType(FDOWN_GUI_BUTTON);
	up_button.SetActive(true);
	down_button.SetActive(true);

	ols = NULL;
	our_team = NULL_TEAM;

	width = 142;

	show_start_entry = 0;
	show_able_entries = 0;
}

void GWFactoryList::Init()
{
	main_top_img.LoadBaseImage("assets/other/factory_gui/main_top.png");
	main_right_img.LoadBaseImage("assets/other/factory_gui/main_right.png");
	main_entry_img.LoadBaseImage("assets/other/factory_gui/main_entry.png");
	entry_bar_green_img.LoadBaseImage("assets/other/factory_gui/entry_bar_green.png");
	entry_bar_red_img.LoadBaseImage("assets/other/factory_gui/entry_bar_red.png");
	entry_bar_grey_img.LoadBaseImage("assets/other/factory_gui/entry_bar_grey.png");
	entry_bar_white_i_img.LoadBaseImage("assets/other/factory_gui/entry_bar_white_i.png");

	//did they all load?
	if(!main_top_img.GetBaseSurface()) return;
	if(!main_right_img.GetBaseSurface()) return;
	if(!main_entry_img.GetBaseSurface()) return;
	if(!entry_bar_green_img.GetBaseSurface()) return;
	if(!entry_bar_red_img.GetBaseSurface()) return;
	if(!entry_bar_grey_img.GetBaseSurface()) return;
	if(!entry_bar_white_i_img.GetBaseSurface()) return;

	finished_init = true;
}

void GWFactoryList::Process()
{
	if(!show) return;
	if(!finished_init) return;
	if(!ols) return;
	if(our_team == NULL_TEAM) return;

	//collect entries
	CollectEntries();
}

void GWFactoryList::DoRender(ZMap &the_map, SDL_Surface *dest)
{
	int shift_x, shift_y, view_w, view_h;
	int tx, ty;

	if(!show) return;
	if(!finished_init) return;
	if(!ols) return;
	if(our_team == NULL_TEAM) return;

	the_map.GetViewShiftFull(shift_x, shift_y, view_w, view_h);

	x = shift_x;
	y = shift_y + view_h;

	DetermineHeight(view_h);

	y -= height;

	//top
	the_map.RenderZSurface(&main_top_img, x, y);

	//entries
	DoRenderEntries(the_map, dest);

	//right
	{
		tx = x + (main_top_img.GetBaseSurface()->w - main_right_img.GetBaseSurface()->w);
		ty = y + main_top_img.GetBaseSurface()->h;

		while(ty < shift_y + view_h)
		{
			the_map.RenderZSurface(&main_right_img, tx, ty);
			ty += main_right_img.GetBaseSurface()->h;
		}
	}

	if(entry_list.size())
	{
		int scroll_top;
		int scroll_bottom;
		int scroll_missing_entries;

		//buttons
		up_button.SetOffsets(123, main_top_img.GetBaseSurface()->h + 2);
		up_button.DoRender(the_map, dest, x, y);

		down_button.SetOffsets(123, height - 11);
		down_button.DoRender(the_map, dest, x, y);

		//scrollbar
		scroll_top = main_top_img.GetBaseSurface()->h + 14;
		scroll_bottom = height - 14;
		scroll_bar.SetOffsets(123, scroll_top);
		scroll_bar.SetHeight(scroll_bottom - scroll_top);

		scroll_missing_entries = entry_list.size() - show_able_entries;
		if(scroll_missing_entries > 0)
			scroll_bar.SetPercentDown(1.0 * show_start_entry / scroll_missing_entries);
		else
			scroll_bar.SetPercentDown(0);
		scroll_bar.SetPercentViewable(1.0 * show_able_entries / entry_list.size());

		scroll_bar.DoRender(the_map, dest, x, y);
	}
}

void GWFactoryList::DoRenderEntries(ZMap &the_map, SDL_Surface *dest)
{
	int ty;

	ty = y + main_top_img.GetBaseSurface()->h;

	for(int ei=show_start_entry; ((ei<(int)entry_list.size()) && ((ei-show_start_entry)<show_able_entries)); ei++)
	{
		gwfl_render_entry *e = &entry_list[ei];
		int bx, by;

		//since it rendered put in its render locations
		e->x = 0;
		e->y = ty - y;
		e->h = main_entry_img.GetBaseSurface()->h;
		e->w = 120;

		//render its main peice
		the_map.RenderZSurface(&main_entry_img, x, ty);

		//begin rendering the labels
		bx = x + 12;
		by = ty + 7;

		for(int i=0;i<3;i++)
		{
			if(!e->colored[i])
				the_map.RenderZSurface(&entry_bar_grey_img, bx, by);
			else
			{
				if(e->percent[i] > 0.99)
					the_map.RenderZSurface(&entry_bar_green_img, bx, by);
				else
				{
					int gw;
					int wx;
					SDL_Rect from_rect, to_rect;

					gw = (int)((double)entry_bar_green_img.GetBaseSurface()->w * e->percent[i]);

					if(gw <= 0) gw = 0;

					if(the_map.GetBlitInfo(entry_bar_green_img.GetBaseSurface(), bx, by, from_rect, to_rect))
					{
						if(from_rect.w > gw) from_rect.w = gw;

						entry_bar_green_img.BlitSurface(&from_rect, &to_rect);
					}

					//the white line
					wx = bx + gw - 1;
					if(wx < bx) wx = bx;
					the_map.RenderZSurface(&entry_bar_white_i_img, wx, by);
				}
			}

			//left text
			if(e->message_left[i].size())
			{
				ZSDL_Surface message_img;

				message_img.LoadBaseImage(ZFontEngine::GetFont(SMALL_WHITE_FONT).Render(e->message_left[i].c_str()));

				the_map.RenderZSurface(&message_img, bx + 2, by + 3);
			}

			//right text
			if(e->message_right[i].size())
			{
				ZSDL_Surface message_img;

				message_img.LoadBaseImage(ZFontEngine::GetFont(SMALL_WHITE_FONT).Render(e->message_right[i].c_str()));

				if(message_img.GetBaseSurface())
					the_map.RenderZSurface(&message_img, bx + 101 - message_img.GetBaseSurface()->w, by + 3);
			}

			by += 17;
		}

		ty += main_entry_img.GetBaseSurface()->h;
	}
}

void GWFactoryList::DetermineHeight(int max_h)
{
	int min_h;
	int free_space;
	int max_start_entry;

	height = main_top_img.GetBaseSurface()->h;

	if(!entry_list.size())
	{
		show_start_entry = 0;
		show_able_entries = 0;	
		return;
	}

	min_h = height + main_entry_img.GetBaseSurface()->h;

	free_space = max_h - min_h;

	show_able_entries = 1;

	if(free_space > 0) show_able_entries += free_space / main_entry_img.GetBaseSurface()->h;

	if(show_able_entries > (int)entry_list.size()) show_able_entries = entry_list.size();

	max_start_entry = entry_list.size() - show_able_entries;
	if(show_start_entry > max_start_entry) show_start_entry = max_start_entry;

	height += show_able_entries * main_entry_img.GetBaseSurface()->h;
}

bool GWFactoryList::Click(int x_, int y_)
{
	int x_local, y_local;

	if(!show) return false;

	gflags.Clear();

	x_local = x_ - x;
	y_local = y_ - y;

	up_button.Click(x_local, y_local);
	down_button.Click(x_local, y_local);

	//test
	for(vector<gwfl_render_entry>::iterator e=entry_list.begin(); e!=entry_list.end(); e++)
	{
		if(x_local < e->x) continue;
		if(y_local < e->y) continue;
		if(x_local > e->x + e->w) continue;
		if(y_local > e->y + e->h) continue;

		gflags.jump_to_building = true;
		gflags.bref_id = e->ref_id;

		break;
	}

	if(x_ < x) return false;
	if(y_ < y) return false;
	if(x_ >= x + width) return false;
	if(y_ >= y + height) return false;

	return true;
}

bool GWFactoryList::UnClick(int x_, int y_)
{
	int x_local, y_local;

	if(!show) return false;

	gflags.Clear();

	x_local = x_ - x;
	y_local = y_ - y;

	if(up_button.UnClick(x_local, y_local)) DoUpButton();
	if(down_button.UnClick(x_local, y_local)) DoDownButton();

	if(x_ < x) return false;
	if(y_ < y) return false;
	if(x_ >= x + width) return false;
	if(y_ >= y + height) return false;

	return true;
}

bool GWFactoryList::WheelUpButton()
{
	if(!show) return false;

	DoUpButton();

	return true;
}

bool GWFactoryList::WheelDownButton()
{
	if(!show) return false;

	DoDownButton();

	return true;
}

void GWFactoryList::DoUpButton()
{
	show_start_entry--;

	if(show_start_entry < 0) show_start_entry = 0;
}

void GWFactoryList::DoDownButton()
{
	show_start_entry++;
}

void GWFactoryList::CollectEntries()
{
	vector<ZObject*> fort_list;
	vector<ZObject*> robot_list;
	vector<ZObject*> vehicle_list;

	if(!ols) return;

	entry_list.clear();

	for(vector<ZObject*>::iterator i=ols->building_olist.begin(); i!=ols->building_olist.end(); i++)
	{
		if((*i)->GetOwner() != our_team) continue;

		unsigned char ot, oid;

		(*i)->GetObjectID(ot, oid);

		if(ot != BUILDING_OBJECT) continue;

		switch(oid)
		{
		case FORT_FRONT:
		case FORT_BACK:
			fort_list.push_back(*i);
			break;
		case ROBOT_FACTORY: robot_list.push_back(*i); break;
		case VEHICLE_FACTORY: vehicle_list.push_back(*i); break;
		}
	}

	for(vector<ZObject*>::iterator i=fort_list.begin(); i!=fort_list.end(); i++) AddEntry(*i);
	for(vector<ZObject*>::iterator i=robot_list.begin(); i!=robot_list.end(); i++) AddEntry(*i);
	for(vector<ZObject*>::iterator i=vehicle_list.begin(); i!=vehicle_list.end(); i++) AddEntry(*i);
}

void GWFactoryList::AddEntry(ZObject *obj)
{
	double &the_time = ztime->ztime;
	gwfl_render_entry new_entry;
	unsigned char ot, oid;
	unsigned char fot, foid;
	char message[500];

	obj->GetObjectID(fot, foid);
	obj->GetBuildUnit(ot, oid);

	new_entry.ref_id = obj->GetRefID();

	//name bar
	{
		new_entry.colored[0] = true;
		new_entry.percent[0] = 1;

		if(obj->GetMaxHealth()) new_entry.percent[0] = 1.0 * obj->GetHealth() / obj->GetMaxHealth();

		if(new_entry.percent[0] > 1) new_entry.percent[0] = 1;
		if(new_entry.percent[0] < 0) new_entry.percent[0] = 0;

		sprintf(message, "%d%c", (int)(new_entry.percent[0] * 100), '%');
		new_entry.message_right[0] = message;

		switch(foid)
		{
		case FORT_FRONT:
		case FORT_BACK: new_entry.message_left[0] = "Fort Factory"; break;
		case ROBOT_FACTORY: new_entry.message_left[0] = "Robot Factory"; break;
		case VEHICLE_FACTORY: new_entry.message_left[0] = "Vehicle Factory"; break;
		}
	}

	if(!obj->IsDestroyed())
	{
		//production bar
		{
			switch(obj->GetBuildState())
			{
			case BUILDING_PLACE:
				new_entry.colored[1] = false;
				new_entry.message_left[1] = "Placing Cannon";
				break;
			case BUILDING_SELECT:
				new_entry.colored[1] = false;
				new_entry.message_left[1] = "No Production";
				break;
			case BUILDING_BUILDING:
				new_entry.colored[1] = true;
				new_entry.percent[1] = ((ZBuilding*)obj)->PercentageProduced(the_time);

				switch(ot)
				{
				case ROBOT_OBJECT: if(oid >= 0 && oid < MAX_ROBOT_TYPES) new_entry.message_left[1] = robot_production_string[oid]; break;
				case VEHICLE_OBJECT: if(oid >= 0 && oid < MAX_VEHICLE_TYPES) new_entry.message_left[1] = vehicle_production_string[oid]; break;
				case CANNON_OBJECT: if(oid >= 0 && oid < MAX_CANNON_TYPES) new_entry.message_left[1] = cannon_production_string[oid]; break;
				default: new_entry.message_left[1] = "???";
				}

				//prod time
				{
					int minutes, seconds;
					int prod_time;

					prod_time = (int)((ZBuilding*)obj)->ProductionTimeLeft(the_time);

					seconds = prod_time % 60;
					prod_time /= 60;
					minutes = prod_time % 60;

					sprintf(message, "%d:%02d", minutes, seconds);
					new_entry.message_right[1] = message;
				}
				break;
			case BUILDING_PAUSED:
				new_entry.colored[1] = false;
				new_entry.message_left[1] = "Paused";
				break;
			}

			if(new_entry.percent[1] > 1) new_entry.percent[1] = 1;
			if(new_entry.percent[1] < 0) new_entry.percent[1] = 0;
		}

		//tech bar
		{
			new_entry.colored[2] = false;

			sprintf(message, "%d", obj->GetLevel() + 1);
			new_entry.message_left[2] = "Tech Level ";
			new_entry.message_left[2] += message;
		}
	}
	else
	{
		new_entry.colored[1] = true;
		new_entry.colored[2] = true;
		new_entry.percent[1] = 0;
		new_entry.percent[2] = 0;

		new_entry.message_left[1] = "Destroyed";

		//tech bar
		sprintf(message, "%d", obj->GetLevel() + 1);
		new_entry.message_left[2] = "Tech Level ";
		new_entry.message_left[2] += message;
	}

	entry_list.push_back(new_entry);
}
