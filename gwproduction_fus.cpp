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

#define GWPFUS_MARGIN 2
#define GWPFUS_TOPH 20
#define GWPFUS_SIDE_SIZE 4
#define GWPFUS_OBJW 45
#define GWPFUS_OBJH 51
//filler color = 57,57,57

bool GWPFullUnitSelector::finished_init = false;
ZSDL_Surface GWPFullUnitSelector::fus_top_left_img;
ZSDL_Surface GWPFullUnitSelector::fus_top_right_img;
ZSDL_Surface GWPFullUnitSelector::fus_bottom_left_img;
ZSDL_Surface GWPFullUnitSelector::fus_bottom_right_img;
ZSDL_Surface GWPFullUnitSelector::fus_top_img;
ZSDL_Surface GWPFullUnitSelector::fus_bottom_img;
ZSDL_Surface GWPFullUnitSelector::fus_left_img;
ZSDL_Surface GWPFullUnitSelector::fus_right_img;
ZSDL_Surface GWPFullUnitSelector::object_back_img;

GWPFullUnitSelector::GWPFullUnitSelector()
{
	ztime = NULL;
	zmap = NULL;
	building_obj = NULL;
	buildlist = NULL;
	x=y=w=h=0;
	lists_building_type = -1;
	lists_building_level = -1;
	us_ref_id = -1;
	is_active = false;

	building_type = FORT_FRONT;

	ClearSelected();
}

GWPFullUnitSelector::~GWPFullUnitSelector()
{
	ClearLists();
}

void GWPFullUnitSelector::Init()
{
	fus_top_left_img.LoadBaseImage("assets/other/production_gui/fus_top_left.png");
	fus_top_right_img.LoadBaseImage("assets/other/production_gui/fus_top_right.png");
	fus_bottom_left_img.LoadBaseImage("assets/other/production_gui/fus_bottom_left.png");
	fus_bottom_right_img.LoadBaseImage("assets/other/production_gui/fus_bottom_right.png");
	fus_top_img.LoadBaseImage("assets/other/production_gui/fus_top.png");
	fus_bottom_img.LoadBaseImage("assets/other/production_gui/fus_bottom.png");
	fus_left_img.LoadBaseImage("assets/other/production_gui/fus_left.png");
	fus_right_img.LoadBaseImage("assets/other/production_gui/fus_right.png");

	object_back_img.LoadBaseImage("assets/other/production_gui/object_back.png");

	finished_init = true;
}

void GWPFullUnitSelector::CalculateXY()
{
	x = cx - (w>>1);
	y = cy - (h>>1);

	if(zmap)
	{
		int map_w, map_h;

		map_w = zmap->GetMapBasics().width * 16;
		map_h = zmap->GetMapBasics().height * 16;

		if(x + w + 16 > map_w) x = map_w - (w + 16);
		if(y + h + 16 > map_h) y = map_h - (h + 16);
	}

	if(x < 16) x = 16;
	if(y < 16) y = 16;
}

void GWPFullUnitSelector::DoRender(ZMap &the_map, SDL_Surface *dest, int shift_x, int shift_y)
{
	int tx, ty;
	SDL_Rect the_box;
	SDL_Rect from_rect, to_rect;
	int fx, fy, fw, fh;
	int w_left, h_left;

	if(!is_active) return;

	CalculateXY();

	tx = x + shift_x;
	ty = y + shift_y;

	//corners
	the_map.RenderZSurface(&fus_top_left_img, tx, ty);
	the_map.RenderZSurface(&fus_top_right_img, (tx+w) - GWPFUS_SIDE_SIZE, ty);
	the_map.RenderZSurface(&fus_bottom_left_img, tx, (ty+h) - GWPFUS_SIDE_SIZE);
	the_map.RenderZSurface(&fus_bottom_right_img, (tx+w) - GWPFUS_SIDE_SIZE, (ty+h) - GWPFUS_SIDE_SIZE);

	//center filler
	{
		fx = tx + GWPFUS_SIDE_SIZE;
		fy = ty + GWPFUS_TOPH;
		fw = w - (GWPFUS_SIDE_SIZE + GWPFUS_SIDE_SIZE);
		fh = h - (GWPFUS_TOPH + GWPFUS_SIDE_SIZE);

		if(the_map.GetBlitInfo(fx, fy, fw, fh, from_rect, to_rect))
		{
			the_box.x = to_rect.x;
			the_box.y = to_rect.y;
			the_box.w = from_rect.w;
			the_box.h = from_rect.h;

			ZSDL_FillRect(&the_box, 57, 57, 57);
		}
	}

	//top filler
	if(fus_top_img.GetBaseSurface() && fus_top_left_img.GetBaseSurface())
	{
		fx = tx + fus_top_left_img.GetBaseSurface()->w;
		w_left = w - (fus_top_left_img.GetBaseSurface()->w + GWPFUS_SIDE_SIZE);

		the_map.RenderZSurfaceHorzRepeat(&fus_top_img, fx, ty, w_left);
	}

	//bottom filler
	if(fus_bottom_img.GetBaseSurface())
	{
		fx = tx + GWPFUS_SIDE_SIZE;
		fy = ty + h - GWPFUS_SIDE_SIZE;
		w_left = w - (GWPFUS_SIDE_SIZE + GWPFUS_SIDE_SIZE);

		the_map.RenderZSurfaceHorzRepeat(&fus_bottom_img, fx, fy, w_left);
	}

	//left filler
	if(fus_left_img.GetBaseSurface())
	{
		//fx = tx + GWPFUS_SIDE_SIZE;
		fy = ty + GWPFUS_TOPH;
		h_left = h - (GWPFUS_TOPH + GWPFUS_SIDE_SIZE);

		the_map.RenderZSurfaceVertRepeat(&fus_left_img, tx, fy, h_left);
	}

	//right filler
	if(fus_right_img.GetBaseSurface())
	{
		fx = tx + w - GWPFUS_SIDE_SIZE;
		fy = ty + GWPFUS_TOPH;
		h_left = h - (GWPFUS_TOPH + GWPFUS_SIDE_SIZE);

		the_map.RenderZSurfaceVertRepeat(&fus_right_img, fx, fy, h_left);
	}

	//render buttons
	for(vector<GWPFullUnitSelectorButton>::iterator i=button_list.begin(); i!=button_list.end(); ++i)
		i->object_button.DoRender(the_map, dest, tx, ty);

	//render lists
	{
		int lx, ly;

		lx = tx + GWPFUS_SIDE_SIZE + GWPFUS_MARGIN;
		ly = ty + GWPFUS_TOPH + GWPFUS_MARGIN;

		if(robot_list.size()) { DoRenderList(the_map, dest, lx, ly, robot_list); ly += GWPFUS_OBJH + GWPFUS_MARGIN; }
		if(vehicle_list.size()) { DoRenderList(the_map, dest, lx, ly, vehicle_list); ly += GWPFUS_OBJH + GWPFUS_MARGIN; }
		if(cannon_list.size()) { DoRenderList(the_map, dest, lx, ly, cannon_list); ly += GWPFUS_OBJH + GWPFUS_MARGIN; }
	}
}

void GWPFullUnitSelector::DoRenderList(ZMap &the_map, SDL_Surface *dest, int lx, int ly, vector<ZObject*> &the_list)
{
	for(vector<ZObject*>::iterator o=the_list.begin(); o!=the_list.end(); ++o)
	{
		int ow, oh;
		ZObject *draw_obj = *o;

		//it's back
		//the_map.RenderZSurface(&object_back_img, lx, ly);

		//the obj
		draw_obj->GetDimensionsPixel(ow, oh);
		draw_obj->SetCords(lx + (27-5) - (ow>>1), ly + (40-21) - (oh>>1));
		draw_obj->DoRender(the_map, dest);

		if(draw_obj->GetHoverNameImg().GetBaseSurface())
			the_map.RenderZSurface(&draw_obj->GetHoverNameImg(), lx + (28-5) - (draw_obj->GetHoverNameImg().GetBaseSurface()->w>>1), ly + (61-21));

		//the time
		//....

		lx += GWPFUS_OBJW + GWPFUS_MARGIN;
	}
}

void GWPFullUnitSelector::Process()
{
	ZObject::ProcessList(robot_list);
	ZObject::ProcessList(vehicle_list);
	ZObject::ProcessList(cannon_list);

	if(is_active) LoadLists();
}

void GWPFullUnitSelector::ClearLists()
{
	ZObject::ClearAndDeleteList(robot_list);
	ZObject::ClearAndDeleteList(vehicle_list);
	ZObject::ClearAndDeleteList(cannon_list);

	lists_building_type = -1;
	lists_building_level = -1;

	button_list.clear();
}

void GWPFullUnitSelector::LoadButtonList()
{
	int lx, ly;

	button_list.clear();

	lx = GWPFUS_SIDE_SIZE + GWPFUS_MARGIN;
	ly = GWPFUS_TOPH + GWPFUS_MARGIN;

	if(robot_list.size()) { AppendButtonList(lx, ly, robot_list); ly += GWPFUS_OBJH + GWPFUS_MARGIN; }
	if(vehicle_list.size()) { AppendButtonList(lx, ly, vehicle_list); ly += GWPFUS_OBJH + GWPFUS_MARGIN; }
	if(cannon_list.size()) { AppendButtonList(lx, ly, cannon_list); ly += GWPFUS_OBJH + GWPFUS_MARGIN; }
}

void GWPFullUnitSelector::AppendButtonList(int lx, int ly, vector<ZObject*> &the_list)
{
	for(vector<ZObject*>::iterator o=the_list.begin(); o!=the_list.end(); ++o)
	{
		GWPFullUnitSelectorButton new_button;

		(*o)->GetObjectID(new_button.ot, new_button.oid);

		new_button.object_button.SetOffsets(lx, ly);

		button_list.push_back(new_button);

		lx += GWPFUS_OBJW + GWPFUS_MARGIN;
	}
}

void GWPFullUnitSelector::CalculateWH()
{
	int blocks_right;
	int blocks_down;

	blocks_right = 2;
	blocks_down = 0;

	if(robot_list.size()) blocks_down++;
	if(vehicle_list.size()) blocks_down++;
	if(cannon_list.size()) blocks_down++;

	if((int)robot_list.size() > blocks_right) blocks_right = robot_list.size();
	if((int)vehicle_list.size() > blocks_right) blocks_right = vehicle_list.size();
	if((int)cannon_list.size() > blocks_right) blocks_right = cannon_list.size();

	w = GWPFUS_SIDE_SIZE + ((GWPFUS_MARGIN + GWPFUS_OBJW) * blocks_right) + GWPFUS_MARGIN + GWPFUS_SIDE_SIZE;
	h = GWPFUS_TOPH + ((GWPFUS_MARGIN + GWPFUS_OBJH) * blocks_down) + GWPFUS_MARGIN + GWPFUS_SIDE_SIZE;
}

void GWPFullUnitSelector::LoadLists()
{
	int building_level;

	//checks
	if(!buildlist) return;
	if(!building_obj) return;

	//get bl
	building_level = building_obj->GetLevel();

	//already loaded this type/level?
	if(building_type == lists_building_type && lists_building_level == building_level) return;

	//clear the list
	ClearLists();

	vector<buildlist_object> &blist = buildlist->GetBuildList(building_type, building_level);

	//this building have things to build?
	if(!blist.size()) return;

	lists_building_type = building_type;
	lists_building_level = building_level;

	for(vector<buildlist_object>::iterator i=blist.begin(); i!=blist.end(); ++i)
		LoadUnitToLists(i->ot, i->oid);

	LoadButtonList();
	CalculateWH();
}

void GWPFullUnitSelector::LoadUnitToLists(unsigned char ot, unsigned char oid)
{
	vector<ZObject*> *the_list = NULL;
	ZObject* new_obj = NULL;

	if(!ztime) return;

	//choose list
	switch(ot)
	{
	case ROBOT_OBJECT: the_list = &robot_list; break;
	case VEHICLE_OBJECT: the_list = &vehicle_list; break;
	case CANNON_OBJECT: the_list = &cannon_list; break;
	default: printf("GWPFullUnitSelector::LoadUnitToLists::tried to add invalid object %d %d\n", ot, oid); break;
	}

	//heh double check
	if(!the_list) return;

	switch(ot)
	{
	case CANNON_OBJECT:
		switch(oid)
		{
		case GATLING:
			new_obj = new CGatling(ztime);
			break;
		case GUN:
			new_obj = new CGun(ztime);
			break;
		case HOWITZER:
			new_obj = new CHowitzer(ztime);
			break;
		case MISSILE_CANNON:
			new_obj = new CMissileCannon(ztime);
			break;
		}
		break;
	case VEHICLE_OBJECT:
		switch(oid)
		{
		case JEEP:
			new_obj = new VJeep(ztime);
			break;
		case LIGHT:
			new_obj = new VLight(ztime);
			break;
		case MEDIUM:
			new_obj = new VMedium(ztime);
			break;
		case HEAVY:
			new_obj = new VHeavy(ztime);
			break;
		case APC:
			new_obj = new VAPC(ztime);
			break;
		case MISSILE_LAUNCHER:
			new_obj = new VMissileLauncher(ztime);
			break;
		case CRANE:
			new_obj = new VCrane(ztime);
			break;
		}

		//for fun
		//if(new_obj) new_obj->SetDirection(rand()%MAX_ANGLE_TYPES);
		if(new_obj) new_obj->SetDirection(R270);
		//if(new_obj) new_obj->SetDirection(R225);
		break;
	case ROBOT_OBJECT:
		switch(oid)
		{
		case GRUNT:
			new_obj = new RGrunt(ztime);
			break;
		case PSYCHO:
			new_obj = new RPsycho(ztime);
			break;
		case SNIPER:
			new_obj = new RSniper(ztime);
			break;
		case TOUGH:
			new_obj = new RTough(ztime);
			break;
		case PYRO:
			new_obj = new RPyro(ztime);
			break;
		case LASER:
			new_obj = new RLaser(ztime);
			break;
		}
		break;
	}

	if(new_obj) 
	{
		new_obj->SetOwner((team_type)building_obj->GetOwner());

		the_list->push_back(new_obj);
	}
}

bool GWPFullUnitSelector::Click(int x_, int y_)
{
	int local_x, local_y;

	if(!is_active) return false;

	local_x = x_ - x;
	local_y = y_ - y;

	for(vector<GWPFullUnitSelectorButton>::iterator i=button_list.begin(); i!=button_list.end(); ++i)
		i->object_button.Click(local_x, local_y);

	if(x_ < x) return false;
	if(y_ < y) return false;
	if(x_ >= x + w) return false;
	if(y_ >= y + h) return false;

	return true;
}

bool GWPFullUnitSelector::UnClick(int x_, int y_)
{
	int local_x, local_y;

	if(!is_active) return false;

	local_x = x_ - x;
	local_y = y_ - y;

	for(vector<GWPFullUnitSelectorButton>::iterator i=button_list.begin(); i!=button_list.end(); ++i)
		if(i->object_button.UnClick(local_x, local_y))
		{
			object_selected = true;
			sot = i->ot;
			soid = i->oid;
		}

	if(x_ < x) return false;
	if(y_ < y) return false;
	if(x_ >= x + w) return false;
	if(y_ >= y + h) return false;

	return true;
}
