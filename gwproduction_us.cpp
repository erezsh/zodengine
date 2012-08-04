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

ZSDL_Surface GWPUnitSelector::selector_back_img;
ZSDL_Surface GWPUnitSelector::p_bar;
ZSDL_Surface GWPUnitSelector::p_bar_yellow;
bool GWPUnitSelector::finished_init = false;
int GWPUnitSelector::next_ref_id = 0;

GWPUnitSelector::GWPUnitSelector()
{
	ztime = NULL;
	zmap = NULL;
	building_obj = NULL;
	buildlist = NULL;
	draw_obj = NULL;
	px=0;
	py=0;
	x=0;
	y=0;
	w=61;
	h=55;
	is_active = true;
	is_only_selector = true;

	building_type = FORT_FRONT;
	select_i = 0;
	build_state = -1;
	percentage_produced = 0;

	up_button.SetType(UP_GUI_BUTTON);
	down_button.SetType(DOWN_GUI_BUTTON);

	up_button.SetOffsets(50 - 3, 21 - 19);
	down_button.SetOffsets(50 - 3, 64 - 19);

	ref_id = next_ref_id++;
}

GWPUnitSelector::~GWPUnitSelector()
{
	DeleteDrawObject();
}

void GWPUnitSelector::Init()
{
	//selector_back_img.LoadBaseImage("assets/other/production_gui/selector_back.png");
	p_bar.LoadBaseImage("assets/other/production_gui/percentage_bar.png");
	p_bar_yellow.LoadBaseImage("assets/other/production_gui/percentage_bar_yellow.png");
	finished_init = true;
}

void GWPUnitSelector::DeleteDrawObject()
{
	if(draw_obj)
	{	
		delete draw_obj;
		draw_obj = NULL;
	}
}

void GWPUnitSelector::SetDrawObject()
{
	unsigned char ot, oid;

	if(!building_obj) return;
	if(!buildlist) return;

	if(is_only_selector || build_state == BUILDING_SELECT)
	{
		int b_level = building_obj->GetLevel();

		//make sure our select_i is kosher
		if(!buildlist->GetBuildList(building_type, b_level).size()) return;

		if(select_i >= buildlist->GetBuildList(building_type, b_level).size())
			select_i = 0;

		ot = buildlist->GetBuildList(building_type, b_level)[select_i].ot;
		oid = buildlist->GetBuildList(building_type, b_level)[select_i].oid;

		ResetDrawObjectTo(ot, oid);
	}
	//from old code...
//	else if(state == BUILDING_SELECT && building_obj && building_obj->GetBuiltCannonList().size())
//	{
//		ot = CANNON_OBJECT;
//		oid = building_obj->GetBuiltCannonList()[0];
//
//		ResetDrawObjectTo(ot, oid);
//	}
	//dlo
	else
	{
		if(building_obj->GetBuiltCannonList().size())
		{
			ot = CANNON_OBJECT;
			oid = building_obj->GetBuiltCannonList()[0];

			ResetDrawObjectTo(ot, oid);
		}
		else
		{
			building_obj->GetBuildUnit(ot, oid);

			ResetDrawObjectTo(ot, oid);
		}
	}
}

void GWPUnitSelector::ResetDrawObjectTo(unsigned char ot, unsigned char oid)
{
	if(!ztime) return;
	if(!building_obj) return;

	//already have this loaded?
	if(draw_obj)
	{
		unsigned char cot, coid;

		draw_obj->GetObjectID(cot, coid);

		if(ot == cot && oid == coid) return;
	}

	//clear it
	DeleteDrawObject();

	//set it
	switch(ot)
	{
	case CANNON_OBJECT:
		switch(oid)
		{
		case GATLING:
			draw_obj = new CGatling(ztime);
			break;
		case GUN:
			draw_obj = new CGun(ztime);
			break;
		case HOWITZER:
			draw_obj = new CHowitzer(ztime);
			break;
		case MISSILE_CANNON:
			draw_obj = new CMissileCannon(ztime);
			break;
		}
		break;
	case VEHICLE_OBJECT:
		switch(oid)
		{
		case JEEP:
			draw_obj = new VJeep(ztime);
			break;
		case LIGHT:
			draw_obj = new VLight(ztime);
			break;
		case MEDIUM:
			draw_obj = new VMedium(ztime);
			break;
		case HEAVY:
			draw_obj = new VHeavy(ztime);
			break;
		case APC:
			draw_obj = new VAPC(ztime);
			break;
		case MISSILE_LAUNCHER:
			draw_obj = new VMissileLauncher(ztime);
			break;
		case CRANE:
			draw_obj = new VCrane(ztime);
			break;
		}

		//for fun
		if(draw_obj) draw_obj->SetDirection(rand()%MAX_ANGLE_TYPES);
		break;
	case ROBOT_OBJECT:
		switch(oid)
		{
		case GRUNT:
			draw_obj = new RGrunt(ztime);
			break;
		case PSYCHO:
			draw_obj = new RPsycho(ztime);
			break;
		case SNIPER:
			draw_obj = new RSniper(ztime);
			break;
		case TOUGH:
			draw_obj = new RTough(ztime);
			break;
		case PYRO:
			draw_obj = new RPyro(ztime);
			break;
		case LASER:
			draw_obj = new RLaser(ztime);
			break;
		}
		break;
	}

	if(draw_obj) draw_obj->SetOwner((team_type)building_obj->GetOwner());
}

void GWPUnitSelector::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int tx, ty;

	if(!finished_init) return;
	if(!is_active) return;

	tx = x + shift_x;
	ty = y + shift_y;

	//the_map.RenderZSurface(&selector_back_img, tx, ty);

	up_button.DoRender(the_map, dest, tx, ty);
	down_button.DoRender(the_map, dest, tx, ty);
	DrawPercentageBar(the_map, dest, tx, ty);

	if(draw_obj)
	{
		int ow, oh;

		draw_obj->GetDimensionsPixel(ow, oh);
		draw_obj->SetCords(tx + (27-3) - (ow>>1), ty + (40-19) - (oh>>1));
		draw_obj->DoRender(the_map, dest);

		if(draw_obj->GetHoverNameImg().GetBaseSurface())
			the_map.RenderZSurface(&draw_obj->GetHoverNameImg(), tx + (28-3) - (draw_obj->GetHoverNameImg().GetBaseSurface()->w>>1), ty + (61-19));
	}
}

void GWPUnitSelector::DrawPercentageBar(ZMap &the_map, SDL_Surface *dest, int tx, int ty)
{
	SDL_Rect from_rect, to_rect;

	if(!building_obj) return;
	if(build_state == BUILDING_SELECT) return;
	if(is_only_selector) return;

	the_map.RenderZSurface(&p_bar, tx + (53-3), ty + (21-19));

	if(p_bar_yellow.GetBaseSurface())
	if(the_map.GetBlitInfo( p_bar_yellow.GetBaseSurface(), tx + (53-3), ty + (21-19), from_rect, to_rect))
	{
		int max_h;

		max_h = (1.0 - percentage_produced) * p_bar_yellow.GetBaseSurface()->h;

		if(from_rect.h > max_h)
			from_rect.h = max_h;

		p_bar_yellow.BlitSurface(&from_rect, &to_rect);
		//SDL_BlitSurface( p_bar_yellow, &from_rect, dest, &to_rect);
	}
}

void GWPUnitSelector::Process()
{
	if(!ztime) return;

	double &the_time = ztime->ztime;

	if(building_obj) 
	{
		build_state = building_obj->GetBuildState();

		if(building_obj->GetBuiltCannonList().size()) build_state = BUILDING_PLACE;
		if(building_obj->IsDestroyed()) build_state = BUILDING_PAUSED;

		if(build_state != BUILDING_SELECT) percentage_produced = building_obj->PercentageProduced(the_time);

		//show up buttons or percentage bar?
		if(is_only_selector || build_state == BUILDING_SELECT)
		{
			up_button.SetActive(true);
			down_button.SetActive(true);
		}
		else
		{
			up_button.SetActive(false);
			down_button.SetActive(false);
		}
	}

	SetDrawObject();

	//process
	if(draw_obj) draw_obj->Process();
}

void GWPUnitSelector::SetSelection(unsigned char ot, unsigned char oid)
{
	int building_level;

	//checks
	if(!buildlist) return;
	if(!building_obj) return;

	//get bl
	building_level = building_obj->GetLevel();

	vector<buildlist_object> &blist = buildlist->GetBuildList(building_type, building_level);

	for(int i=0; i<blist.size(); ++i)
		if(blist[i].ot == ot && blist[i].oid == oid)
		{
			select_i = i;
			break;
		}

	SetDrawObject();
}

bool GWPUnitSelector::WheelUpButton()
{
	if(up_button.IsActive())
	{
		DoUpButton();
		return true;
	}

	return false;
}

bool GWPUnitSelector::WheelDownButton()
{
	if(down_button.IsActive())
	{
		DoDownButton();
		return true;
	}

	return false;
}

bool GWPUnitSelector::Click(int x_, int y_)
{
	int local_x, local_y;

	if(!is_active) return false;

	local_x = x_ - x;
	local_y = y_ - y;

	up_button.Click(local_x, local_y);
	down_button.Click(local_x, local_y);

	if(x_ < x) return false;
	if(y_ < y) return false;
	if(x_ >= x + w) return false;
	if(y_ >= y + h) return false;

	return true;
}

bool GWPUnitSelector::UnClick(int x_, int y_)
{
	int local_x, local_y;

	load_fus = false;

	if(!is_active) return false;

	local_x = x_ - x;
	local_y = y_ - y;

	if(up_button.UnClick(local_x, local_y)) DoUpButton();
	if(down_button.UnClick(local_x, local_y)) DoDownButton();

	//load fus?
	if(is_only_selector || build_state == BUILDING_SELECT)
		load_fus = WithinPortrait(local_x, local_y);

	if(x_ < x) return false;
	if(y_ < y) return false;
	if(x_ >= x + w) return false;
	if(y_ >= y + h) return false;

	return true;
}

bool GWPUnitSelector::WithinPortrait(int x, int y)
{
	if(x < 2) return false;
	if(y < 2) return false;
	if(x > 46) return false;
	if(y > 52) return false;

	return true;
}

void GWPUnitSelector::DoUpButton()
{
	if(!building_obj) return;
	if(!buildlist) return;

	int b_level = building_obj->GetLevel();

	//make sure our select_i is kosher
	if(!buildlist->GetBuildList(building_type, b_level).size()) return;

	select_i++;

	if(select_i >= buildlist->GetBuildList(building_type, b_level).size())
		select_i = 0;
}

void GWPUnitSelector::DoDownButton()
{
	if(!building_obj) return;
	if(!buildlist) return;

	int b_level = building_obj->GetLevel();

	//make sure our select_i is kosher
	if(!buildlist->GetBuildList(building_type, b_level).size()) return;

	select_i--;

	if(select_i < 0)
		select_i = buildlist->GetBuildList(building_type, b_level).size() - 1;
}

bool GWPUnitSelector::GetSelectedID(unsigned char &ot, unsigned char &oid)
{
	if(draw_obj)
	{
		draw_obj->GetObjectID(ot, oid);
		return true;
	}

	return false;
}
