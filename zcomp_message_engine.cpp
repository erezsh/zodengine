#include "zcomp_message_engine.h"
#include "zfont_engine.h"
#include "zobject.h"
#include "common.h"

using namespace COMMON;

ZSDL_Surface ZCompMessageEngine::robot_manufactured;
ZSDL_Surface ZCompMessageEngine::vehicle_manufactured;
ZSDL_Surface ZCompMessageEngine::gun_manufactured;
ZSDL_Surface ZCompMessageEngine::fort_under_attacked;
ZSDL_Surface ZCompMessageEngine::gun_img;
ZSDL_Surface ZCompMessageEngine::paused_img;
ZSDL_Surface ZCompMessageEngine::click_to_resume_img;
ZSDL_Surface ZCompMessageEngine::x_img[MAX_STORED_CANNONS];

ZCompMessageEngine::ZCompMessageEngine()
{
	ztime = NULL;

	final_time = 0;
	show_message = -1;
	flips_done = 0;
	ref_id = -1;
	object_list = NULL;
	rendered_guns = 0;

	render_box.x = 0;
	render_box.y = 0;
	render_box.w = 0;
	render_box.h = 0;

	show_message_img = NULL;
}

void ZCompMessageEngine::SetObjectList(vector<ZObject*> *object_list_)
{
	object_list = object_list_;
}

void ZCompMessageEngine::SetTeam(int our_team_)
{
	our_team = our_team_;
}

void ZCompMessageEngine::SetZTime(ZTime *ztime_)
{
	ztime = ztime_;
}

void ZCompMessageEngine::Init()
{
	int i;
	char message[50];

	robot_manufactured.LoadBaseImage("assets/other/comp_messages/robot_manufactured.png");
	vehicle_manufactured.LoadBaseImage("assets/other/comp_messages/vehicle_manufactured.png");
	gun_manufactured.LoadBaseImage("assets/other/comp_messages/gun_manufactured.png");
	fort_under_attacked.LoadBaseImage("assets/other/comp_messages/fort_under_attack.png");
	gun_img.LoadBaseImage("assets/other/comp_messages/gun.png");
	paused_img.LoadBaseImage("assets/other/comp_messages/paused.png");
	click_to_resume_img.LoadBaseImage("assets/other/comp_messages/click_to_resume.png");

	for(i=0;i<MAX_STORED_CANNONS;i++)
	{
		sprintf(message, "X%d", i+1);
		x_img[i].LoadBaseImage(ZFontEngine::GetFont(SMALL_WHITE_FONT).Render(message));
	}
}

bool ZCompMessageEngine::AbsorbedLClick(int x, int y, ZMap &the_map)
{
	int shift_x, shift_y, view_w, view_h;
	int img_x, img_y, img_w, img_h;
	int i;

	the_map.GetViewShiftFull(shift_x, shift_y, view_w, view_h);

	if(show_message != -1 && show_message_img && show_message_img->GetBaseSurface())
	{
		//">> 1" is a faster form of "/ 2" for non float type numbers
		img_x = (view_w >> 1) - (show_message_img->GetBaseSurface()->w >> 1);

		if((x >= img_x && x <= img_x + show_message_img->GetBaseSurface()->w) && 
			(y >= 20 && y <= 20 + show_message_img->GetBaseSurface()->h))
		{
			return true;
		}
	}

	img_y = 8;
	if(gun_img.GetBaseSurface())
	for(i=0;i<rendered_guns && i<MAX_RENDERABLE_STORED_GUNS; i++)
	{
		if((x >= 8 && x <= 8 + gun_img.GetBaseSurface()->w) && (y >= img_y && y <= img_y + gun_img.GetBaseSurface()->h))
		{
			return true;
		}

		//inc
		img_y += 2 + gun_img.GetBaseSurface()->h;
	}

	//resume game?
	if(ztime && ztime->IsPaused() && click_to_resume_img.GetBaseSurface())
	{
		img_x = (view_w - click_to_resume_img.GetBaseSurface()->w) >> 1;
		img_y = (view_h - click_to_resume_img.GetBaseSurface()->h) >> 1;
		img_w = click_to_resume_img.GetBaseSurface()->w;
		img_h = click_to_resume_img.GetBaseSurface()->h;

		if((x >= img_x && x <= img_x + img_w) && (y >= img_y && y <= img_y + img_h))
		{
			return true;
		}
	}

	return false;
}

bool ZCompMessageEngine::AbsorbedLUnClick(int x, int y, ZMap &the_map)
{
	int shift_x, shift_y, view_w, view_h;
	int img_x, img_y, img_w, img_h;
	int i;

	cmflags.Clear();

	the_map.GetViewShiftFull(shift_x, shift_y, view_w, view_h);

	if(show_message != -1 && show_message_img && show_message_img->GetBaseSurface())
	{
		img_x = (view_w >> 1) - (show_message_img->GetBaseSurface()->w >> 1);

		if((x >= img_x && x <= img_x + show_message_img->GetBaseSurface()->w) && 
			(y >= 20 && y <= 20 + show_message_img->GetBaseSurface()->h))
		{
			switch(show_message)
			{
			case ROBOT_MANUFACTURED_MSG:
				cmflags.ref_id = ref_id;
				cmflags.select_obj = true;
				break;
			case VEHICLE_MANUFACTURED_MSG:
				cmflags.ref_id = ref_id;
				cmflags.select_obj = true;
				break;
			case GUN_MANUFACTURED_MSG:
				cmflags.ref_id = ref_id;
				cmflags.open_gui = true;
				break;
			case FORT_MSG:
				cmflags.ref_id = ref_id;
				break;
			}

			return true;
		}
	}

	img_y = 8;
	if(gun_img.GetBaseSurface())
	for(i=0;i<rendered_guns && i<MAX_RENDERABLE_STORED_GUNS; i++)
	{
		if((x >= 8 && x <= 8 + gun_img.GetBaseSurface()->w) && (y >= img_y && y <= img_y + gun_img.GetBaseSurface()->h))
		{
			cmflags.ref_id = rendered_gun_ref_id[i];
			cmflags.open_gui = true;

			return true;
		}

		//inc
		img_y += 2 + gun_img.GetBaseSurface()->h;
	}

	//resume game?
	if(ztime && ztime->IsPaused() && click_to_resume_img.GetBaseSurface())
	{
		img_x = (view_w - click_to_resume_img.GetBaseSurface()->w) >> 1;
		img_y = (view_h - click_to_resume_img.GetBaseSurface()->h) >> 1;
		img_w = click_to_resume_img.GetBaseSurface()->w;
		img_h = click_to_resume_img.GetBaseSurface()->h;

		if((x >= img_x && x <= img_x + img_w) && (y >= img_y && y <= img_y + img_h))
		{
			cmflags.resume_game = true;

			return true;
		}
	}

	return false;
}

void ZCompMessageEngine::Process(double the_time)
{
	if(show_message != -1)
	{
		switch(show_message)
		{
		case ROBOT_MANUFACTURED_MSG: show_message_img = &robot_manufactured; break;
		case VEHICLE_MANUFACTURED_MSG: show_message_img = &vehicle_manufactured; break;
		case GUN_MANUFACTURED_MSG: show_message_img = &gun_manufactured; break;
		case FORT_MSG: show_message_img = &fort_under_attacked; break;
		default: show_message_img = NULL; break;
		}

		if(flips_done < 10)
		{
			if(the_time >= next_flip_time)
			{
				flips_done++;

				if(show_the_message)
					show_the_message = false;
				else
					show_the_message = true;

				next_flip_time = the_time + 0.3;

				if(flips_done == 10)
					final_time = the_time + 5;
			}
		}
		else if(the_time >= final_time)
		{
			show_message = -1;
			show_message_img = NULL;
			show_the_message = false;
		}
	}
}

void ZCompMessageEngine::DoRender(ZMap &the_map, SDL_Surface *dest)
{
	int shift_x, shift_y, view_w, view_h;
	SDL_Rect from_rect, to_rect;

	if(show_message_img && show_the_message && show_message_img->GetBaseSurface())
	{
		int x, y;

		the_map.GetViewShiftFull(shift_x, shift_y, view_w, view_h);

		//x = (view_w >> 1) - (show_message_img->GetBaseSurface()->w >> 1);
		x = (view_w - show_message_img->GetBaseSurface()->w) >> 1;

		x += shift_x;
		y = 20 + shift_y;

		the_map.RenderZSurface(show_message_img, x, y);
		//if(the_map.GetBlitInfo(show_message_img, x, y, from_rect, to_rect))
		//	SDL_BlitSurface( show_message_img, &from_rect, dest, &to_rect);
	}

	//render these too
	RenderGuns(the_map, dest);

	//piggy back the resume image here
	RenderResume(the_map, dest);
}

void ZCompMessageEngine::RenderResume(ZMap &the_map, SDL_Surface *dest)
{
	if(ztime && ztime->IsPaused() && click_to_resume_img.GetBaseSurface())
	{
		int shift_x, shift_y, view_w, view_h;

		int x, y;

		the_map.GetViewShiftFull(shift_x, shift_y, view_w, view_h);

		x = (view_w - click_to_resume_img.GetBaseSurface()->w) >> 1;
		y = (view_h - click_to_resume_img.GetBaseSurface()->h) >> 1;

		x += shift_x;
		y += shift_y;

		the_map.RenderZSurface(&click_to_resume_img, x, y);
	}
}

void ZCompMessageEngine::RenderGuns(ZMap &the_map, SDL_Surface *dest)
{
	SDL_Rect from_rect, to_rect;
	int shift_x, shift_y, view_w, view_h;
	int x, y;

	if(!object_list) return;
	if(!gun_img.GetBaseSurface()) return;

	the_map.GetViewShiftFull(shift_x, shift_y, view_w, view_h);

	x = 8 + shift_x;
	y = 8 + shift_y;

	rendered_guns = 0;

	for(vector<ZObject*>::iterator i=object_list->begin(); i!=object_list->end(); i++)
	{
		unsigned char ot, oid;

		if((*i)->GetOwner() != our_team) continue;
		if((*i)->IsDestroyed()) continue;

		(*i)->GetObjectID(ot, oid);

		if(ot != BUILDING_OBJECT) continue;
		if(!(oid == FORT_FRONT || oid == FORT_BACK || oid == ROBOT_FACTORY || oid == VEHICLE_FACTORY)) continue;

		int cannon_amount;
		cannon_amount = (*i)->GetBuiltCannonList().size();
		if(cannon_amount)
		{
			//were drawing yo!
			the_map.RenderZSurface(&gun_img, x, y);
			//if(the_map.GetBlitInfo(gun_img, x, y, from_rect, to_rect))
			//	SDL_BlitSurface( gun_img, &from_rect, dest, &to_rect);

			//more then one?
			if(cannon_amount > 1 && cannon_amount <= MAX_STORED_CANNONS)
			{
				//draw the multiplier
				the_map.RenderZSurface(&x_img[cannon_amount-1], x + gun_img.GetBaseSurface()->w + 4, y + 3);
				//if(the_map.GetBlitInfo(x_img[cannon_amount-1], x + gun_img->w + 4, y + 3, from_rect, to_rect))
				//	SDL_BlitSurface( x_img[cannon_amount-1], &from_rect, dest, &to_rect);
			}

			//set some store info
			rendered_gun_ref_id[rendered_guns] = (*i)->GetRefID();

			//inc
			rendered_guns++;

			//we render our limit?
			if(rendered_guns >= MAX_RENDERABLE_STORED_GUNS) break;

			//set for next
			y += 2 + gun_img.GetBaseSurface()->h;
		}
	}
}

void ZCompMessageEngine::DisplayMessage(int comp_message_, int ref_id_)
{
	double the_time;

	the_time = current_time();

	show_message = comp_message_;
	next_flip_time = the_time + 0.3;
	show_the_message = true;
	flips_done = 0;
	ref_id = ref_id_;
}

comp_msg_flags &ZCompMessageEngine::GetFlags()
{
	return cmflags;
}
