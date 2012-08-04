#include "gwproduction.h"
#include "zfont_engine.h"
//#include "zbuilding.h"

#include "cgatling.h"
#include "cgun.h"
#include "chowitzer.h"
#include "cmissilecannon.h"

#include "vjeep.h"
#include "vlight.h"
#include "vmedium.h"
#include "vheavy.h"
#include "vapc.h"
#include "vmissilelauncher.h"
#include "vcrane.h"

#include "rgrunt.h"
#include "rpsycho.h"
#include "rsniper.h"
#include "rtough.h"
#include "rpyro.h"
#include "rlaser.h"

ZSDL_Surface GWProduction::base_img;
ZSDL_Surface GWProduction::base_expanded_img;
ZSDL_Surface GWProduction::name_label[GWPROD_TYPES_MAX];
ZSDL_Surface GWProduction::state_label[MAX_BUILDING_STATES][2];
ZSDL_Surface GWProduction::p_bar;
ZSDL_Surface GWProduction::p_bar_yellow;
//int GWProduction::is_expanded = false;

GWProduction::GWProduction(ZTime *ztime_) : ZGuiWindow(ztime_)
{
	double &the_time = ztime->ztime;

	ref_id = -1;
	building_obj = NULL;
	state_i = 0;
	show_time = 0;
	show_time_img = NULL;
	health_percent = -1;
	health_percent_img = NULL;
	is_expanded = false;

	//for defaults sake
	SetType(GWPROD_FORT);
	state = -1;
	SetState(BUILDING_SELECT);

	place_button.SetType(PLACE_GUI_BUTTON);
	ok_button.SetType(OK_GUI_BUTTON);
	cancel_button.SetType(CANCEL_GUI_BUTTON);
	small_plus_button.SetType(SMALL_PLUS_BUTTON);
	small_minus_button.SetType(SMALL_MINUS_BUTTON);
	queue_button.SetType(QUEUE_BUTTON);

	unit_selector.SetZTime(ztime);
	queue_selector.SetZTime(ztime);
	full_selector.SetZTime(ztime);
	
	unit_selector.SetCoords(3, 19);
	queue_selector.SetCoords(111, 19);

	unit_selector.SetIsOnlySelector(false);
	queue_selector.SetIsOnlySelector(true);

	ProcessSetExpanded();

	next_state_time = the_time + 0.3;
}

void GWProduction::Init()
{
	base_img.LoadBaseImage("assets/other/production_gui/base_image.png");
	base_expanded_img.LoadBaseImage("assets/other/production_gui/base_image_expanded.png");
	name_label[GWPROD_ROBOT].LoadBaseImage("assets/other/production_gui/robot_factory_label.png");
	name_label[GWPROD_FORT].LoadBaseImage("assets/other/production_gui/fort_factory_label.png");
	name_label[GWPROD_VEHICLE].LoadBaseImage("assets/other/production_gui/vehicle_factory_label.png");
	p_bar.LoadBaseImage("assets/other/production_gui/percentage_bar.png");
	p_bar_yellow.LoadBaseImage("assets/other/production_gui/percentage_bar_yellow.png");

	state_label[BUILDING_PLACE][0].LoadBaseImage("assets/other/production_gui/place_label.png");
	state_label[BUILDING_PLACE][1].LoadBaseImage("assets/other/production_gui/placeless_label.png");

	state_label[BUILDING_SELECT][0].LoadBaseImage("assets/other/production_gui/select_label.png");
	state_label[BUILDING_SELECT][1].LoadBaseImage("assets/other/production_gui/selectless_label.png");

	state_label[BUILDING_BUILDING][0].LoadBaseImage("assets/other/production_gui/building_label.png");
	state_label[BUILDING_BUILDING][1].LoadBaseImage("assets/other/production_gui/buildingless_label.png");

	state_label[BUILDING_PAUSED][0].LoadBaseImage("assets/other/production_gui/paused_label.png");
	state_label[BUILDING_PAUSED][1].LoadBaseImage("assets/other/production_gui/pausedless_label.png");

	//these classes are personal to GWProduction
	GWPUnitSelector::Init();
	GWPFullUnitSelector::Init();
}

void GWProduction::Process()
{
	double &the_time = ztime->ztime;

	if(killme) return;

	//do not process if we are not attached to a building object
	if(!building_obj)
	{
		killme = true;
		return;
	}

	//reset queue button list?
	CheckQueueButtonList();

	//recheck building state
	if(building_obj) SetState(building_obj->GetBuildState());

	//selectors
	unit_selector.Process();
	queue_selector.Process();
	full_selector.Process();

	//set show time
	RecalcShowTime();

	if(the_time >= next_state_time)
	{
		state_i++;
		if(state_i>=2) state_i = 0;
		next_state_time = the_time + 0.3;
	}

	//health percent
	{
		int new_percent = 0;

		if(building_obj->GetMaxHealth())
			new_percent = 100 * building_obj->GetHealth() / building_obj->GetMaxHealth();

		if(new_percent < 0) new_percent = 0;
		if(new_percent > 100) new_percent = 100;

		if(new_percent != health_percent)
		{
			char message[50];

			health_percent = new_percent;

			//ZSDL_FreeSurface(health_percent_img);
			sprintf(message, "%d%c", health_percent, '%');
			//printf("%s\n", message);
			//health_percent_img = ZFontEngine::GetFont(SMALL_WHITE_FONT).Render(message);
			health_percent_img.LoadBaseImage(ZFontEngine::GetFont(SMALL_WHITE_FONT).Render(message));
		}

	}
}

void GWProduction::CheckQueueButtonList()
{
	if(!building_obj) return;

	//sizes different?
	if(queue_button_list.size() != building_obj->GetQueueList().size())
	{
		MakeQueueButtonList();
		return;
	}

	//items different?
	{
		vector<ZBProductionUnit> &bqlist = building_obj->GetQueueList();
		vector<ZBProductionUnit>::iterator bqi = bqlist.begin();
		vector<GWPQueueItem>::iterator gqi = queue_button_list.begin();

		for(; gqi!=queue_button_list.end() && bqi!=bqlist.end(); ++bqi, ++gqi)
		{
			if(gqi->ot != bqi->ot || gqi->oid != bqi->oid)
			{
				MakeQueueButtonList();
				return;
			}
		}
	}
}

void GWProduction::MakeQueueButtonList()
{
	const int button_h = 13;
	const int button_margin = 1;
	int lx, ly;

	lx = 177;
	ly = 22;

	//check
	if(!building_obj) return;

	//speed things up
	vector<ZBProductionUnit> &bqlist = building_obj->GetQueueList();

	//clear
	queue_button_list.clear();

	//create
	for(vector<ZBProductionUnit>::iterator bqi=bqlist.begin(); bqi!=bqlist.end(); ++bqi)
	{
		queue_button_list.push_back(GWPQueueItem(lx, ly, bqi->ot, bqi->oid));
		ly += button_h + button_margin;
	}
}

void GWProduction::ProcessSetExpanded()
{
	if(is_expanded)
	{
		width = 228;
		height = 96;

		small_plus_button.SetActive(false);
		small_minus_button.SetActive(true);
		queue_button.SetActive(true);

		queue_selector.SetActive(true);
	}
	else
	{
		width = 112;
		height = 80;

		small_plus_button.SetActive(true);
		small_minus_button.SetActive(false);
		queue_button.SetActive(false);

		queue_selector.SetActive(false);
	}
}

void GWProduction::RecalcShowTime()
{
	double &the_time = ztime->ztime;
	double new_time = -1;

	//setup ticker
	if(state == BUILDING_SELECT)
	{
		if(!building_obj) return;
		if(!buildlist) return;

		unsigned char ot, oid;

		if(unit_selector.GetSelectedID(ot, oid))
			new_time = building_obj->BuildTimeModified(buildlist->UnitBuildTime(ot, oid));
	}
	else
	{
		new_time = building_obj->ProductionTimeLeft(the_time);
	}

	if(show_time != (int)new_time)
		ResetShowTime((int)new_time);
}

void GWProduction::ResetShowTime(int new_time)
{
	int minutes, seconds;
	char message[50];

	//if(show_time_img)
	//{
	//	SDL_FreeSurface(show_time_img);
	//	show_time_img = NULL;
	//}

	show_time = new_time;

	//setup these numbers
	seconds = new_time % 60;
	new_time /= 60;
	minutes = new_time % 60;

	sprintf(message, "%d:%02d", minutes, seconds);
	//show_time_img = ZFontEngine::GetFont(SMALL_WHITE_FONT).Render(message);
	show_time_img.LoadBaseImage(ZFontEngine::GetFont(SMALL_WHITE_FONT).Render(message));
}

void GWProduction::DoRender(ZMap &the_map, SDL_Surface *dest)
{
	int lx, ly;
	SDL_Rect from_rect, to_rect;

	if(killme) return;

	if(!building_obj)
	{
		killme = true;
		return;
	}

	if(!is_expanded)
		the_map.RenderZSurface(&base_img, x, y);
	else
		the_map.RenderZSurface(&base_expanded_img, x, y);

	lx = x + 9;
	ly = y + 6;

	the_map.RenderZSurface(&name_label[type], lx, ly);
	//if(the_map.GetBlitInfo(name_label[type], x + lx, y + ly, from_rect, to_rect))
	//	SDL_BlitSurface( name_label[type], &from_rect, dest, &to_rect);

	lx = x + 64;
	ly = y + 19;

	the_map.RenderZSurface(&state_label[state][state_i], lx, ly);
	//if(the_map.GetBlitInfo( state_label[state][state_i], x + lx, y + ly, from_rect, to_rect))
	//	SDL_BlitSurface( state_label[state][state_i], &from_rect, dest, &to_rect);

	//buttons
	ok_button.DoRender(the_map, dest, x, y);
	cancel_button.DoRender(the_map, dest, x, y);
	place_button.DoRender(the_map, dest, x, y);
	small_plus_button.DoRender(the_map, dest, x, y);
	small_minus_button.DoRender(the_map, dest, x, y);
	queue_button.DoRender(the_map, dest, x, y);

	//queue_list buttons
	RenderQueueButtonList(the_map, dest);

	lx = x + 90;
	ly = y + 35;

	the_map.RenderZSurface(&show_time_img, lx, ly);
	//if(show_time_img)
	//	if(the_map.GetBlitInfo( show_time_img, x + 90, y + 35, from_rect, to_rect))
	//		SDL_BlitSurface( show_time_img, &from_rect, dest, &to_rect);

	if(health_percent_img.GetBaseSurface())
	{
		lx = x + 86 - (health_percent_img.GetBaseSurface()->w >> 1);
		ly = y + 6;

		the_map.RenderZSurface(&health_percent_img, lx, ly);
	}
		//if(the_map.GetBlitInfo( health_percent_img, x + 97 - (health_percent_img->w/2), y + 7, from_rect, to_rect))
		//	SDL_BlitSurface( health_percent_img, &from_rect, dest, &to_rect);

	//selectors
	unit_selector.DoRender(the_map, dest, x, y);
	queue_selector.DoRender(the_map, dest, x, y);

	//full selector goes over everything
	full_selector.DoRender(the_map, dest);
}

void GWProduction::RenderQueueButtonList(ZMap &the_map, SDL_Surface *dest)
{
	if(!is_expanded) return;

	for(vector<GWPQueueItem>::iterator qb=queue_button_list.begin(); qb!=queue_button_list.end(); ++qb)
	{
		ZSDL_Surface *name_render;

		qb->obj_name_button.DoRender(the_map, dest, x, y);

		name_render = &ZObject::GetHoverNameImgStatic(qb->ot, qb->oid);

		if(name_render && name_render->GetBaseSurface())
			the_map.RenderZSurface(name_render, (qb->x_off + x + 23) - (name_render->GetBaseSurface()->w>>1), qb->y_off + y + 2);
	}
}

void GWProduction::SetBuildList(ZBuildList *buildlist_)
{
	buildlist = buildlist_;

	unit_selector.SetBuildList(buildlist_);
	queue_selector.SetBuildList(buildlist_);
	full_selector.SetBuildList(buildlist_);
}

void GWProduction::SetState(int state_)
{
	if(building_obj && building_obj->GetBuiltCannonList().size()) state_ = BUILDING_PLACE;
	if(building_obj && building_obj->IsDestroyed()) state_ = BUILDING_PAUSED;

	if(state == state_) return;

	state = state_;

	switch(state)
	{
	case BUILDING_PLACE:
		ok_button.SetActive(false);
		cancel_button.SetActive(true);
		place_button.SetActive(true);
		break;
	case BUILDING_SELECT:
		ok_button.SetActive(true);
		cancel_button.SetActive(true);
		place_button.SetActive(false);
		break;
	case BUILDING_BUILDING:
		ok_button.SetActive(true);
		cancel_button.SetActive(true);
		place_button.SetActive(false);
		break;
	case BUILDING_PAUSED:
		ok_button.SetActive(true);
		cancel_button.SetActive(true);
		place_button.SetActive(false);
		break;
	}
}

void GWProduction::SetCords(int center_x, int center_y)
{
	//x = center_x - (width >> 1);
	//y = center_y - (height >> 1);

	//initial placement based on unexpanded
	x = center_x - (112 >> 1);
	y = center_y - (80 >> 1);

	if(zmap)
	{
		int map_w, map_h;

		map_w = zmap->GetMapBasics().width * 16;
		map_h = zmap->GetMapBasics().height * 16;

		//if(x + width + 16 > view_w) x = view_w - (width + 16);
		//if(y + height + 16 > view_h) y = view_h - (height + 16);

		//check if fits on map based on expanded size
		if(x + 228 + 16 > map_w) x = map_w - (228 + 16);
		if(y + 96 + 16 > map_h) y = map_h - (96 + 16);
	}

	if(x < 16) x = 16;
	if(y < 16) y = 16;

	full_selector.SetCenterCoords(center_x, center_y);
}

void GWProduction::SetType(int type_)
{
	type = type_;

	switch(type)
	{
	case GWPROD_FORT:
		building_type = FORT_FRONT;
		break;
	case GWPROD_VEHICLE:
		building_type = VEHICLE_FACTORY;
		break;
	case GWPROD_ROBOT:
		building_type = ROBOT_FACTORY;
		break;
	}

	unit_selector.SetBuildingType(building_type);
	queue_selector.SetBuildingType(building_type);
	full_selector.SetBuildingType(building_type);
}

void GWProduction::SetBuildingObj(ZBuilding *building_obj_)
{
	building_obj = building_obj_;

	unit_selector.SetBuildingObj(building_obj);
	queue_selector.SetBuildingObj(building_obj);
	full_selector.SetBuildingObj(building_obj);
}

void GWProduction::DoOkButton()
{
	switch(state)
	{
	case BUILDING_PLACE:
		//the ok button doesnt show during the place state
		break;
	case BUILDING_SELECT:
		{
			unsigned char ot, oid;

			if(unit_selector.GetSelectedID(ot, oid))
			{
				gflags.send_new_production = true;
				gflags.pot = ot;
				gflags.poid = oid;
				gflags.pref_id = building_obj->GetRefID();
			}
		}
		break;
	case BUILDING_PAUSED:
	case BUILDING_BUILDING:
		killme = true;
		break;
	}
}

void GWProduction::DoCancelButton()
{
	switch(state)
	{
	case BUILDING_PAUSED:
	case BUILDING_PLACE:
	case BUILDING_SELECT:
		killme = true;
		break;
	case BUILDING_BUILDING:
		//tell server to cancel this production
		gflags.send_stop_production = true;
		gflags.pref_id = building_obj->GetRefID();
		break;
	}
}

void GWProduction::DoPlaceButton()
{
	if(!building_obj) return;
	if(!building_obj->GetBuiltCannonList().size()) return;
	if(!building_obj->GetConnectedZone()) return;

	gflags.place_cannon = true;
	gflags.coid = building_obj->GetBuiltCannonList()[0];
	gflags.cref_id = building_obj->GetRefID();
	gflags.cleft = building_obj->GetConnectedZone()->x;
	gflags.cright = gflags.cleft + building_obj->GetConnectedZone()->w;
	gflags.ctop = building_obj->GetConnectedZone()->y;
	gflags.cbottom = gflags.ctop + building_obj->GetConnectedZone()->h;

	//also good bye
	killme = true;
}

void GWProduction::DoQueueButton()
{
	unsigned char ot, oid;

	if(queue_selector.GetSelectedID(ot, oid))
	{
		gflags.send_new_queue_item = true;
		gflags.qot = ot;
		gflags.qoid = oid;
		gflags.qref_id = building_obj->GetRefID();
	}
}

void GWProduction::DoCancelQueueItem(int i)
{
	if(i<0) return;
	if(i>=queue_button_list.size()) return;

	gflags.send_cancel_queue_item = true;
	gflags.qcref_id = building_obj->GetRefID();
	gflags.qc_i = i;
	gflags.qcot = queue_button_list[i].ot;
	gflags.qcoid = queue_button_list[i].oid;
}

void GWProduction::LoadFullSelector(int us_ref_id)
{
	int fus_cx, fus_cy;

	full_selector.SetActive(true);
	full_selector.SetUnitSelectorRefID(us_ref_id);
	full_selector.ClearSelected();

	if(us_ref_id == unit_selector.GetRefID())
		unit_selector.GetCoords(fus_cx, fus_cy);
	else if(us_ref_id == queue_selector.GetRefID())
		queue_selector.GetCoords(fus_cx, fus_cy);
	else
	{
		fus_cx = width>>1;
		fus_cy = height>>1;
	}

	full_selector.SetCenterCoords(x + fus_cx + GWP_SELECTOR_CENTER_X, y + fus_cy + GWP_SELECTOR_CENTER_Y);
}

bool GWProduction::Click(int x_, int y_)
{
	int x_local, y_local;

	//full selector uses map coords
	if(full_selector.Click(x_, y_)) return true;

	x_local = x_ - x;
	y_local = y_ - y;

	//buttons
	ok_button.Click(x_local, y_local);
	cancel_button.Click(x_local, y_local);
	place_button.Click(x_local, y_local);
	small_plus_button.Click(x_local, y_local);
	small_minus_button.Click(x_local, y_local);
	queue_button.Click(x_local, y_local);

	//queue_list buttons
	if(is_expanded)
	{
		for(vector<GWPQueueItem>::iterator qb=queue_button_list.begin(); qb!=queue_button_list.end(); ++qb)
			qb->obj_name_button.Click(x_local, y_local);
	}

	//selectors
	unit_selector.Click(x_local, y_local);
	queue_selector.Click(x_local, y_local);

	if(x_ < x) return false;
	if(y_ < y) return false;
	if(x_ >= x + width) return false;
	if(y_ >= y + height) return false;

	return true;
}

bool GWProduction::UnClick(int x_, int y_)
{
	int x_local, y_local;

	gflags.Clear();

	//full selector uses map coords
	if(full_selector.IsActive())
	{
		if(!full_selector.UnClick(x_, y_))
		{
			//it was clicked outside so kill it
			full_selector.SetActive(false);
		}
		else
		{
			unsigned char ot, oid;

			//it took a click but do we have a selection?
			if(full_selector.GetSelected(ot, oid))
			{
				//has a selection so load it into the unit selector
				full_selector.SetActive(false);
				if(unit_selector.GetRefID() == full_selector.GetUnitSelectorRefID())
				{
					unit_selector.SetSelection(ot, oid);
					DoOkButton();
				}
				if(queue_selector.GetRefID() == full_selector.GetUnitSelectorRefID())
				{
					queue_selector.SetSelection(ot, oid);
					DoQueueButton();
				}
			}

			//took the click but no selection
			//so do not allow any other gwprod
			//gui to process
			return true;
		}
	}

	x_local = x_ - x;
	y_local = y_ - y;

	//buttons
	if(ok_button.UnClick(x_local, y_local)) DoOkButton();
	if(cancel_button.UnClick(x_local, y_local)) DoCancelButton();
	if(place_button.UnClick(x_local, y_local)) DoPlaceButton();
	if(small_plus_button.UnClick(x_local, y_local)) DoPlusButton();
	if(small_minus_button.UnClick(x_local, y_local)) DoMinusButton();
	if(queue_button.UnClick(x_local, y_local)) DoQueueButton();

	//queue_list buttons
	if(is_expanded)
	{
		for(int i=0; i<queue_button_list.size(); i++)
		{
			if(queue_button_list[i].obj_name_button.UnClick(x_local, y_local))
				DoCancelQueueItem(i);
		}
	}

	//selectors
	if(unit_selector.UnClick(x_local, y_local) && unit_selector.LoadFullSelector()) LoadFullSelector(unit_selector.GetRefID());
	if(queue_selector.UnClick(x_local, y_local) && queue_selector.LoadFullSelector()) LoadFullSelector(queue_selector.GetRefID());

	if(x_ < x) return false;
	if(y_ < y) return false;
	if(x_ >= x + width) return false;
	if(y_ >= y + height) return false;

	return true;
}

bool GWProduction::WheelUpButton()
{
	if(!unit_selector.WheelUpButton())
		return queue_selector.WheelUpButton();

	return true;
}

bool GWProduction::WheelDownButton()
{
	if(!unit_selector.WheelDownButton())
		return queue_selector.WheelDownButton();

	return true;
}
