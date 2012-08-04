#include "zhud.h"
#include "zteam.h"

HubButton::HubButton()
{
	name = "unknown_button";
	type = -1;
	state = B_ACTIVE;
	x = y = 0;
	shift_x = 0;
	shift_y = 0;
}

void HubButton::SetType(hud_buttons type_)
{
	type = type_;
	name = hub_buttons_string[type];
}

bool HubButton::WithinCords(int x_, int y_)
{
	int tx = x + shift_x;
	int ty = y + shift_y;

	if(x_ < tx) return false;
	if(y_ < ty) return false;
	if(x_ > tx + button_img[state].GetBaseSurface()->w) return false;
	if(y_ > ty + button_img[state].GetBaseSurface()->h) return false;

	return true;
}

hud_button_state HubButton::CurrentState()
{
	return (hud_button_state)state;
}

void HubButton::SetState(hud_button_state new_state)
{
	state = new_state;
}

void HubButton::SetShift(int shift_x_, int shift_y_)
{
	shift_x = shift_x_;
	shift_y = shift_y_;
}

void HubButton::Init()
{
	string filename;
	
	if(type == -1) return;

	filename = "assets/other/hud/" + name + "_active.bmp";
	button_img[B_ACTIVE].LoadBaseImage(filename);// = IMG_Load_Error(filename);
	filename = "assets/other/hud/" + name + "_inactive.bmp";
	button_img[B_INACTIVE].LoadBaseImage(filename);// = IMG_Load_Error(filename);
	filename = "assets/other/hud/" + name + "_pressed.bmp";
	button_img[B_PRESSED].LoadBaseImage(filename);// = IMG_Load_Error(filename);

	switch(type)
	{
	case A_BUTTON:
		x = 556;
		y = 8;
		state = B_INACTIVE;
		break;
	case B_BUTTON:
		x = 68;
		y = 458;
		state = B_INACTIVE;
		break;
	case D_BUTTON:
		x = 586;
		y = 264;
		break;
	case G_BUTTON:
		x = 98;
		y = 458;
		state = B_INACTIVE;
		break;
	case MENU_BUTTON:
		x = 482;
		y = 458;
		break;
	case R_BUTTON:
		x = 8;
		y = 458;
		state = B_INACTIVE;
		break;
	case T_BUTTON:
		x = 556;
		y = 264;
		break;
	case V_BUTTON:
		x = 38;
		y = 458;
		state = B_INACTIVE;
		break;
	case Z_BUTTON:
		x = 616;
		y = 264;
		break;
	}
}

void HubButton::Render(SDL_Surface *dest, int off_x, int off_y)
{
	SDL_Rect to_rect;

	to_rect.x = x + off_x + shift_x;
	to_rect.y = y + off_y + shift_y;

	button_img[state].BlitSurface(NULL, &to_rect);
	//button_img[state].RenderSurface(x + off_x, y + off_y);
	//SDL_BlitSurface(button_img[state], NULL, dest, &to_rect);
}

hud_buttons HubButton::GetType()
{
	return (hud_buttons)type;
}

ZHud::ZHud()
{
	//unit_amount_text = NULL;
	//chat_message_img = NULL;

	render_icon = NULL;
	render_label = NULL;
	render_unit_label = NULL;
	//render_backdrop = NULL;

	terrain = DESERT;
	team = NULL_TEAM;
	show_chat = false;
	ReRenderAll();
	ResetGame();
	max_units = DEFAULT_MAX_UNITS_PER_TEAM;

	chat_start_x = 0;
	chat_end_x = 0;

	aportrait.SetDoRandomAnims(false);
	a_ref_id = -1;

	next_a_check_time = 0;
	next_a_flash_time = 0;
	next_a_anim_time = 0;
	last_a_anim = -1;
	a_times_not_under_attack = 0;

	do_end_animations = false;
	next_end_anim_time = 0;
}

void ZHud::ReRenderAll()
{
	rerender_main = true;
	rerender_icon = true;
	rerender_backdrop = true;
	rerender_time = true;
	rerender_button = -1;
	rerender_health = true;
	rerender_unit_amount = true;
	rerender_chat = true;
}

void ZHud::ResetGame()
{
	do_end_animations = false;
	end_animations.clear();
	a_ref_id = -1;
	SetSelectedObject(NULL);
	unit_amount = -1;
	//ZSDL_FreeSurface(unit_amount_text);
	unit_amount_text.Unload();
}

void ZHud::StartEndAnimations(vector<zhud_end_unit> &end_animations_, bool won)
{
	do_end_animations = true;
	do_end_animations_won = won;
	next_end_anim_time = 0;
	end_animations = end_animations_;
}

void ZHud::DeleteObject(ZObject *obj)
{
	if(selected_object == obj)
	{
		SetSelectedObject(NULL);
	}
}

ZMiniMap &ZHud::GetMiniMap()
{
	return zminimap;
}

void ZHud::SetTerrainType(planet_type terrain_)
{
	terrain = terrain_;

	portrait.SetTerrainType(terrain);
	aportrait.SetTerrainType(terrain);
}

void ZHud::GiveSelectedCommand(bool no_way)
{
	//if(selected_object) selected_object->PlayAcknowledgeWav();
	if(selected_object) selected_object->PlayAcknowledgeAnim(portrait, no_way);
}

void ZHud::SetSelectedObject(ZObject *selection)
{
	unsigned char ot, oid;

	selected_object = selection;
	rerender_icon = true;
	rerender_backdrop = true;
	rerender_health = true;

	render_icon = NULL;
	render_label = NULL;
	render_unit_label = NULL;
	//render_backdrop = NULL;

	portrait.SetObject(selected_object);
	//portrait.ClearRobotID();

	if(selected_object)
	{
		selected_object->GetObjectID(ot, oid);

		//also play the tune, this may not be the final location
		//selected_object->PlaySelectedWav();
		selected_object->PlaySelectedAnim(portrait);

		icon_shift_y = 0;

		switch(ot)
		{
		case ROBOT_OBJECT:
			render_icon = &robot_icon[oid][team];
			render_label = &robot_label[oid];
			render_unit_label = &robot_unit_label[oid][team];
			//render_backdrop = &backdrop[terrain];
			//portrait.SetInVehicle(false);
			//portrait.SetRobotID(oid);
			icon_shift_y = 3;
			break;
		case VEHICLE_OBJECT:
			render_icon = &vehicle_icon[oid][team];
			render_label = &vehicle_label[oid];
			render_unit_label = &robot_unit_label[selected_object->GetDriverType()][team];
			//portrait.SetRobotID(selected_object->GetDriverType());
			//portrait.SetInVehicle(true);
			//render_backdrop = &backdrop_vehicle;
			if(render_icon->GetBaseSurface())
				icon_shift_y = 30 - (render_icon->GetBaseSurface()->h >> 1);
			break;
		case CANNON_OBJECT:
			render_icon = &cannon_icon[oid][team];
			render_label = &cannon_label[oid];
			render_unit_label = &robot_unit_label[selected_object->GetDriverType()][team];
			//portrait.SetRobotID(selected_object->GetDriverType());
			//portrait.SetInVehicle(true);
			//render_backdrop = &backdrop[terrain];
			if(render_icon->GetBaseSurface())
				icon_shift_y = 30 - (render_icon->GetBaseSurface()->h >> 1);
			break;
		}
	}
}

void ZHud::MouseMotion(int x, int y, int screen_w, int screen_h, hud_click_response &response)
{
	response.clear();

	if((x >= screen_w - HUD_WIDTH) || (y >= screen_h - HUD_HEIGHT))
	{
		int off_x, off_y;
		int rx, ry;
		int i;
		
		off_x = screen_w - 648;
		off_y = screen_h - 484;
		rx = x - off_x;
		ry = y - off_y;

		response.used = true;

		if(zminimap.ClickedMap(rx - 555, ry - 299, response.mini_x, response.mini_y))
		{
			response.type = HUD_MINI_MAP;
			return;
		}
	}
}

bool ZHud::OverMiniMap(int x, int y, int screen_w, int screen_h, int &mini_x, int &mini_y)
{
	if(x >= screen_w - HUD_WIDTH)
	{
		int off_x, off_y;
		int rx, ry;
		int i;
		
		off_x = screen_w - 648;
		off_y = screen_h - 484;
		rx = x - off_x;
		ry = y - off_y;

		return zminimap.ClickedMap(rx - 555, ry - 299, mini_x, mini_y);
	}

	return false;
}

bool ZHud::OverPortrait(int rx, int ry)
{
	const int port_x = 556;
	const int port_y = 44;

	//to_rect.x = off_x + 556;
	//to_rect.y = off_y + 44;

	if(rx < port_x) return false;
	if(rx > port_x + ZPORTRAIT_BASE_WIDTH) return false;
	if(ry < port_y) return false;
	if(ry > port_y + ZPORTRAIT_BASE_HEIGHT) return false;

	return true;
}

void ZHud::LeftClick(int x, int y, int screen_w, int screen_h, hud_click_response &response)
{
	response.type = -1;

	if((x >= screen_w - HUD_WIDTH) || (y >= screen_h - HUD_HEIGHT))
	{
		int off_x, off_y;
		int rx, ry;
		int i;
		
		off_x = screen_w - 648;
		off_y = screen_h - 484;
		rx = x - off_x;
		ry = y - off_y;

		response.used = true;

		for(i=0;i<MAX_HUD_BUTTONS;i++)
			if(button[i].WithinCords(rx, ry))
			{
				if(button[i].CurrentState() == B_ACTIVE)
				{
					button[i].SetState(B_PRESSED);
					response.button = button[i].GetType();
					response.type = HUD_BUTTON;
					rerender_button = i;
				}

				//a button?
				if(i == A_BUTTON)
				{
					response.type = HUD_JUMP_TO_UNIT;
					response.jump_ref_id = a_ref_id;
				}
				
				return;
			}

		//not a button, so is it the mini map?
		if(zminimap.ClickedMap(rx - 555, ry - 299, response.mini_x, response.mini_y))
		{
			response.type = HUD_MINI_MAP;
			return;
		}

		//over the portrait?
		if(OverPortrait(rx, ry))
		{
			if(aportrait.DoingAnim())
			{
				response.type = HUD_JUMP_TO_UNIT;
				response.jump_ref_id = aportrait.GetRefID();
			}
			else
			{
				response.type = HUD_JUMP_TO_UNIT;
				response.jump_ref_id = portrait.GetRefID();
			}
			return;
		}
		
	}
	else
	{
		response.used = false;
	}
}

void ZHud::LeftUnclick(int x, int y, int screen_w, int screen_h, hud_click_response &response)
{
	int i;
	response.type = -1;

	if((x >= screen_w - HUD_WIDTH) || (y >= screen_h - HUD_HEIGHT))
	{
		int off_x, off_y;
		int rx, ry;
		int i;
		
		off_x = screen_w - 648;
		off_y = screen_h - 484;
		rx = x - off_x;
		ry = y - off_y;

		response.used = true;

		for(i=0;i<MAX_HUD_BUTTONS;i++)
			if(button[i].WithinCords(rx, ry))
			{
				if(button[i].CurrentState() == B_PRESSED)
				{
					response.button = button[i].GetType();
					response.type = HUD_BUTTON;
					rerender_button = i;
				}
				
				break;
			}

		//not a button, so is it the mini map?
		if(zminimap.ClickedMap(rx - 555, ry - 299, response.mini_x, response.mini_y))
		{
			response.type = HUD_MINI_MAP;
			return;
		}
	}
	else
	{
		response.used = false;
	}

	//clear all clicked
	for(i=0;i<MAX_HUD_BUTTONS;i++)
		if(button[i].CurrentState() == B_PRESSED)
		{
			button[i].SetState(B_ACTIVE);
			rerender_button = i;
		}
}

HubButton &ZHud::GetButton(int button_num)
{
	return button[button_num];
}

void ZHud::Process(double the_time, vector<ZObject*> &object_list)
{
	//rerender_backdrop = portrait.Process();

	rerender_backdrop = false;

	if(portrait.Process()) rerender_backdrop = true;
	if(aportrait.Process()) rerender_backdrop = true;

	ProcessA(the_time, object_list);

	if(do_end_animations)
	{
		if(end_animations.size())
		{
			if(the_time >= next_end_anim_time)
			{
				next_end_anim_time = the_time + 1.2;

				zhud_end_unit next_unit;

				next_unit = end_animations.back();

				end_animations.pop_back();

				switch(next_unit.ot)
				{
					case CANNON_OBJECT:
						aportrait.SetRefID(-1);
						aportrait.SetInVehicle(true);
						aportrait.SetRobotID(next_unit.roid);
						break;
					case VEHICLE_OBJECT:
						aportrait.SetRefID(-1);
						aportrait.SetInVehicle(true);
						aportrait.SetRobotID(next_unit.roid);
						break;
					case ROBOT_OBJECT:
						aportrait.SetRefID(-1);
						aportrait.SetInVehicle(false);
						aportrait.SetRobotID(next_unit.roid);
						break;
				}

				if(do_end_animations_won)
					aportrait.StartAnim(ENDW1_ANIM + (rand() % 3));
				else
					aportrait.StartAnim(ENDL1_ANIM + (rand() % 3));
			}
		}
		else
			do_end_animations = false;
	}
}

void ZHud::ProcessA(double the_time, vector<ZObject*> &object_list)
{
	if(a_ref_id == -1) return;

	ZObject *obj;

	obj = ZObject::GetObjectFromID(a_ref_id, object_list);

	if(!obj) 
	{
		a_ref_id = -1;
		button[A_BUTTON].SetState(B_INACTIVE);
		return;
	}

	//this guy still under attack (or alive?)
	if(the_time >= next_a_check_time)
	{
		next_a_check_time = the_time + 0.25;

		bool found = false;
		for(vector<ZObject*>::iterator i=object_list.begin(); i!=object_list.end(); i++)
			if((*i)->GetAttackObject() == obj)
			{
				found = true;
				break;
			}

		//exit if this has happened 10 times in a row
		//otherwise reset
		if(!found)
		{
			if(a_times_not_under_attack < 10)
				a_times_not_under_attack++;
			else
			{
				a_ref_id = -1;
				button[A_BUTTON].SetState(B_INACTIVE);
				return;
			}
		}
		else
			a_times_not_under_attack = 0;
	}

	//switch the flash?
	if(the_time >= next_a_flash_time)
	{
		next_a_flash_time = the_time + 0.15;

		if(button[A_BUTTON].CurrentState() == B_INACTIVE)
			button[A_BUTTON].SetState(B_ACTIVE);
		else
			button[A_BUTTON].SetState(B_INACTIVE);
	}

	//do another anim?
	if(the_time >= next_a_anim_time)
	{
		next_a_anim_time = the_time + 5 + (0.01 * (rand() % 300));

		int next_anim;

		next_anim = rand() % 6;
		while(next_anim == last_a_anim)
			next_anim = rand() % 6;

		if(!aportrait.DoingAnim())
		{
			last_a_anim = next_anim;

			aportrait.SetObject(obj);
			aportrait.StartAnim(I_SAID_WERE_UNDER_ATTACK_ANIM + next_anim);
		}
	}
}

void ZHud::SetARefID(int a_ref_id_)
{
	a_ref_id = a_ref_id_;

	if(ztime)
	{
		double &the_time = ztime->ztime;

		next_a_check_time = the_time + 0.25;
		next_a_flash_time = the_time + 0.15;
		next_a_anim_time = the_time + 5 + (0.01 * (rand() % 300));
	}
}

void ZHud::DoRender(SDL_Surface *dest, int screen_w, int screen_h)
{
	SDL_Rect from_rect, to_rect;
	int off_x, off_y;
	int i;
	
	//off_x = dest->w - 648;
	//off_y = dest->h - 484;
	off_x = screen_w - 648;
	off_y = screen_h - 484;
	
	if(rerender_main)
	{
		//render bottom
		{
			//if(bottom_filler.GetBaseSurface())
			//{
			//	to_rect.x = 0;
			//	to_rect.y = 448 + off_y;
			//	for(;to_rect.x < off_x; to_rect.x += bottom_filler.GetBaseSurface()->w)
			//		bottom_filler.BlitSurface(NULL, &to_rect);
			//}

			//to_rect.x = off_x;
			//to_rect.y = off_y + 448;
			//
			//main_hud_bottom.BlitSurface(NULL, &to_rect);

			//left
			to_rect.x = 0;
			to_rect.y = off_y + 448;
			main_hud_bottom_left.BlitSurface(NULL, &to_rect);

			//best placed here, it goes over the left peice
			RenderUnitAmountBar(dest, off_x, off_y);

			//center
			if(main_hud_bottom_left.GetBaseSurface() && main_hud_bottom_center.GetBaseSurface() && main_hud_bottom_right.GetBaseSurface())
			{
				int start_x, end_x;

				start_x = main_hud_bottom_left.GetBaseSurface()->w;
				end_x = (548 + off_x) - main_hud_bottom_right.GetBaseSurface()->w;

				//set the chat start and stop here too
				chat_start_x = start_x;
				chat_end_x = end_x;

				while(start_x < end_x)
				{
					to_rect.x = start_x;
					to_rect.y = off_y + 448;

					from_rect.x = 0;
					from_rect.y = 0;
					from_rect.w = end_x - start_x;
					from_rect.h = main_hud_bottom_center.GetBaseSurface()->h;

					if(from_rect.w > main_hud_bottom_center.GetBaseSurface()->w)
						from_rect.w = main_hud_bottom_center.GetBaseSurface()->w;

					start_x += from_rect.w;

					main_hud_bottom_center.BlitSurface(&from_rect, &to_rect);
				}
			}

			//right
			if(main_hud_bottom_right.GetBaseSurface())
			{
				to_rect.x = (548 + off_x) - main_hud_bottom_right.GetBaseSurface()->w;
				to_rect.y = off_y + 448;
				main_hud_bottom_right.BlitSurface(NULL, &to_rect);
			}

			//replace button locations
			button[R_BUTTON].SetShift(-off_x);
			button[V_BUTTON].SetShift(-off_x);
			button[B_BUTTON].SetShift(-off_x);
			button[G_BUTTON].SetShift(-off_x);
		}

		//render right side
		{
			if(side_filler.GetBaseSurface())
			{
				to_rect.x = 548 + off_x;
				to_rect.y = 0;
				for(;to_rect.y < off_y; to_rect.y += side_filler.GetBaseSurface()->h)
					side_filler.BlitSurface(NULL, &to_rect);
			}
			
			to_rect.x = off_x + 548;
			to_rect.y = off_y;
			
			main_hud_side[team].BlitSurface(NULL, &to_rect);
		}
		
		//render buttons
		for(i=0;i<MAX_HUD_BUTTONS;i++)
			button[i].Render(dest, off_x, off_y);

		rerender_main = false;
		rerender_button = -1;
	}

	if(rerender_icon)
	{
		rerender_icon = false;

		//clear the render_icon spot always before the icon
		//because the previous one may be there
		{
			to_rect.x = off_x + 550;
			to_rect.y = off_y + 148;

			from_rect.x = 550 - 548;
			from_rect.y = 148;
			from_rect.w = 96;
			from_rect.h = 60;

			main_hud_side[team].BlitSurface(&from_rect, &to_rect);
			//SDL_BlitSurface(main_hud_side, &from_rect, dest, &to_rect);
		}

		if(render_icon)
		{
			to_rect.x = off_x + 550;
			to_rect.y = off_y + 148;

			to_rect.y += icon_shift_y;

			render_icon->BlitSurface(NULL, &to_rect);
			//render_icon->RenderSurface(to_rect.x, to_rect.y);
			//SDL_BlitSurface(render_icon, NULL, dest, &to_rect);
		}

		if(render_unit_label)
		{
			to_rect.x = off_x + 550;
			to_rect.y = off_y + 124;

			render_unit_label->BlitSurface(NULL, &to_rect);
			//render_unit_label->RenderSurface(to_rect.x, to_rect.y);
			//SDL_BlitSurface(render_unit_label, NULL, dest, &to_rect);
		}
		else
		{
			//clear
			to_rect.x = off_x + 550;
			to_rect.y = off_y + 124;

			from_rect.x = 2;
			from_rect.y = 124;
			from_rect.w = 100;
			from_rect.h = 18;

			main_hud_side[team].BlitSurface(&from_rect, &to_rect);
			//SDL_BlitSurface(main_hud_side, &from_rect, dest, &to_rect);
		}

		if(render_label)
		{
			to_rect.x = off_x + 550;
			to_rect.y = off_y + 230;

			render_label->BlitSurface(NULL, &to_rect);
			//render_label->RenderSurface(to_rect.x, to_rect.y);
			//SDL_BlitSurface(render_label, NULL, dest, &to_rect);
		}
		else
		{
			//clear
			to_rect.x = off_x + 550;
			to_rect.y = off_y + 230;

			from_rect.x = 2;
			from_rect.y = 230;
			from_rect.w = 100;
			from_rect.h = 18;

			main_hud_side[team].BlitSurface(&from_rect, &to_rect);
			//SDL_BlitSurface(main_hud_side, &from_rect, dest, &to_rect);
		}

		if(selected_object && selected_object->CanHaveGrenades())
		{
			static int last_grenade_amount = -1;
			static ZSDL_Surface grenade_amount_surface;

			if(last_grenade_amount != selected_object->GetGrenadeAmount())
			{
				char grenade_c[50];

				last_grenade_amount = selected_object->GetGrenadeAmount();

				sprintf(grenade_c, "%02d", last_grenade_amount);

				grenade_amount_surface.LoadBaseImage(ZFontEngine::GetFont(BIG_WHITE_FONT).Render(grenade_c));
			}

			to_rect.x = off_x + 575;
			to_rect.y = off_y + 185;

			grenade_img[selected_object->GetOwner()].BlitSurface(NULL, &to_rect);

			to_rect.x += 25;
			to_rect.y += 2;

			grenade_amount_surface.BlitSurface(NULL, &to_rect);
		}
	}

	RenderBackdrop(dest, off_x, off_y);

	zminimap.DoRender(dest, off_x + 555, off_y + 299);

	if(rerender_button != -1)
	{
		button[rerender_button].Render(dest, off_x, off_y);
		rerender_button = -1;
	}

	RenderTime(dest, off_x, off_y);

	RenderHealth(dest, off_x, off_y);

	RenderChatMessage(dest, off_x, off_y);
}

void ZHud::RenderChatMessage(SDL_Surface *dest, int off_x, int off_y)
{
	int clear_w = chat_end_x - chat_start_x;

	if(!rerender_chat) return;

	SDL_Rect from_rect, to_rect;

	//don't render if there is no space to
	if(clear_w <= 0) return;

	//clear area
	{
		//to_rect.x = off_x + 206;
		to_rect.x = chat_start_x;
		to_rect.y = off_y + 460;
		//to_rect.w = 419 - 206;
		to_rect.w = clear_w;
		to_rect.h = 478 - 460;
		//SDL_FillRect(dest, &to_rect, SDL_MapRGB(dest->format, 115,115,115));
		ZSDL_FillRect(&to_rect, 115, 115, 115);
	}

	if(show_chat && chat_message_img.GetBaseSurface() && (clear_w - 6 > 0))
	{
		//to_rect.x = off_x + 206 + 3;
		//to_rect.y = off_y + 460 + 5;
		to_rect.x = chat_start_x + 3;
		to_rect.y = off_y + 460 + 5;

		//if(chat_message_img.GetBaseSurface()->w < 419 - 206)
		if(chat_message_img.GetBaseSurface()->w < clear_w)
		{
			chat_message_img.BlitSurface(NULL, &to_rect);
			//chat_message_img.RenderSurface(to_rect.x, to_rect.y);
			//SDL_BlitSurface(chat_message_img, NULL, dest, &to_rect);
		}
		else
		{
			//from_rect.x = chat_message_img.GetBaseSurface()->w - (419 - 206 - 6);
			from_rect.x = chat_message_img.GetBaseSurface()->w - (clear_w - 6);
			from_rect.y = 0;
			//from_rect.w = 419 - 206 - 6;
			from_rect.w = clear_w - 6;
			from_rect.h = chat_message_img.GetBaseSurface()->h;

			chat_message_img.BlitSurface(&from_rect, &to_rect);
			//SDL_BlitSurface(chat_message_img, &from_rect, dest, &to_rect);
		}
	}
}

void ZHud::RenderUnitAmountBar(SDL_Surface *dest, int off_x, int off_y)
{
	SDL_Rect from_rect, to_rect;
	double percent_to_max;
	char message[50];

	if(!rerender_unit_amount) return;

	if(!unit_amount_bar[team].GetBaseSurface()) return;

	//clear the space
	{
		//to_rect.x = off_x + 132;
		to_rect.x = 132;
		to_rect.y = off_y + 460;
		to_rect.w = 62;
		to_rect.h = 16;
		//SDL_FillRect(dest, &to_rect, SDL_MapRGB(dest->format, 0,0,0));
		ZSDL_FillRect(&to_rect, 0, 0, 0);
	}

	percent_to_max = 1.0 * unit_amount / max_units;
	if(percent_to_max < 0) percent_to_max = 0;
	if(percent_to_max > 1) percent_to_max = 1;

	//to_rect.x = off_x + 132;
	to_rect.x = 132;
	to_rect.y = off_y + 460;

	from_rect.x = 0;
	from_rect.y = 0;
	from_rect.w = unit_amount_bar[team].GetBaseSurface()->w * percent_to_max;
	from_rect.h = unit_amount_bar[team].GetBaseSurface()->h;

	unit_amount_bar[team].BlitSurface(&from_rect, &to_rect);
	//SDL_BlitSurface(unit_amount_bar[team], &from_rect, dest, &to_rect);

	//render text
	{
		sprintf(message, "%d", unit_amount);

		//ZSDL_FreeSurface(unit_amount_text);
		//unit_amount_text.Unload();

		//unit_amount_text = ZFontEngine::GetFont(SMALL_WHITE_FONT).Render(message);
		unit_amount_text.LoadBaseImage(ZFontEngine::GetFont(SMALL_WHITE_FONT).Render(message));

		//to_rect.x = off_x + 132 + 3;
		to_rect.x = 132 + 3;
		to_rect.y = off_y + 460 + 5;

		//if(unit_amount_text) SDL_BlitSurface(unit_amount_text, NULL, dest, &to_rect);
		unit_amount_text.BlitSurface(NULL, &to_rect);
		//unit_amount_text.RenderSurface(to_rect.x, to_rect.y);
	}
}

void ZHud::RenderHealth(SDL_Surface *dest, int off_x, int off_y)
{
	SDL_Rect from_rect, to_rect;

	if(!rerender_health) return;

	rerender_health = false;

	if(!selected_object || selected_object->IsDestroyed())
	{
		to_rect.x = off_x + 548 + 14;
		to_rect.y = off_y + 213;

		health_empty_img.BlitSurface(NULL, &to_rect);
		//health_empty_img.RenderSurface(to_rect.x, to_rect.y);
		//SDL_BlitSurface(health_empty_img, NULL, dest, &to_rect);
	}
	else
	{
		const int max_dist = 74;
		int green_dist, yellow_dist;

		//calculate
		green_dist = max_dist * (1.0 * selected_object->GetHealth() / MAX_UNIT_HEALTH);
		yellow_dist = max_dist * (1.0 * selected_object->GetMaxHealth() / MAX_UNIT_HEALTH);

		if(green_dist <= 0) green_dist = 1;
		if(yellow_dist <= 0) yellow_dist = 1;

		//green
		to_rect.x = off_x + 548 + 14;
		to_rect.y = off_y + 213;

		from_rect.x = 0;
		from_rect.y = 0;
		from_rect.w = green_dist;
		from_rect.h = 8;

		health_full_img.BlitSurface(&from_rect, &to_rect);
		//SDL_BlitSurface(health_full_img, &from_rect, dest, &to_rect);

		//yellow
		to_rect.x = off_x + 548 + 14 + green_dist;
		to_rect.y = off_y + 213;

		from_rect.x = green_dist;
		from_rect.y = 0;
		from_rect.w = yellow_dist - green_dist;
		from_rect.h = 8;

		if(from_rect.w)
			health_lost_img.BlitSurface(&from_rect, &to_rect);
			//SDL_BlitSurface(health_lost_img, &from_rect, dest, &to_rect);

		//empty
		to_rect.x = off_x + 548 + 14 + yellow_dist;
		to_rect.y = off_y + 213;

		from_rect.x = yellow_dist;
		from_rect.y = 0;
		from_rect.w = max_dist - yellow_dist;
		from_rect.h = 8;

		if(from_rect.w)
			health_empty_img.BlitSurface(&from_rect, &to_rect);
			//SDL_BlitSurface(health_empty_img, &from_rect, dest, &to_rect);
	}

}

void ZHud::RenderTime(SDL_Surface *dest, int off_x, int off_y)
{
	static int last_time = -1;
	static int start_time = 0;
	static int do_init = 1;
	int the_time;
	int seconds, minutes, hours;
	char message[50];
	SDL_Rect from_rect, to_rect;
	ZSDL_Surface surface;
	const int y_down_shift = 9;
	const int x_hours_shift = 38;
	const int x_minutes_shift = x_hours_shift + 14;
	const int x_seconds_shift = x_minutes_shift + 23;

	the_time = ztime->ztime - start_time;

	if(do_init)
	{
		do_init = 0;

		start_time = the_time;
		the_time = 0;
	}

	//only rerender if new or we are supposed to rerender everything
	if(the_time > last_time || rerender_time)
	{
		last_time = the_time;

		//setup these numbers
		seconds = the_time % 60;
		the_time /= 60;
		minutes = the_time % 60;
		the_time /= 60;
		hours = the_time % 60;

		//clear area
		{
			//clear
			to_rect.x = off_x + 548 + x_hours_shift;
			to_rect.y = off_y + y_down_shift;

			from_rect.x = x_hours_shift;
			from_rect.y = y_down_shift;
			from_rect.w = (x_seconds_shift + 20) - x_hours_shift;
			from_rect.h = 17;

			main_hud_side[team].BlitSurface(&from_rect, &to_rect);
			//SDL_BlitSurface(main_hud_side, &from_rect, dest, &to_rect);
		}

		//render test
		//sprintf(message, "%d:%02d:%02d", hours, minutes, seconds);
		//to_rect.x = off_x + 548;
		//to_rect.y = off_y;
		//surface = ZFontEngine::GetFont(BIG_WHITE_FONT).Render(message);
		//if(surface)
		//{
		//	SDL_BlitSurface(surface, NULL, dest, &to_rect);
		//	SDL_FreeSurface(surface);
		//}

		//render hours
		if(hours <= 9)
			sprintf(message, "%d", hours);
		else
			sprintf(message, "!");
		to_rect.x = off_x + 548 + x_hours_shift;
		to_rect.y = off_y + y_down_shift;
		//surface = ZFontEngine::GetFont(BIG_WHITE_FONT).Render(message);
		//if(surface)
		//{
		//	SDL_BlitSurface(surface, NULL, dest, &to_rect);
		//	SDL_FreeSurface(surface);
		//}
		surface.LoadBaseImage(ZFontEngine::GetFont(BIG_WHITE_FONT).Render(message));
		surface.BlitSurface(NULL, &to_rect);
		//surface.RenderSurface(to_rect.x, to_rect.y);
		surface.Unload();

		//render minutes
		sprintf(message, "%02d", minutes);
		to_rect.x = off_x + 548 + x_minutes_shift;
		to_rect.y = off_y + y_down_shift;
		//surface = ZFontEngine::GetFont(BIG_WHITE_FONT).Render(message);
		//if(surface)
		//{
		//	SDL_BlitSurface(surface, NULL, dest, &to_rect);
		//	SDL_FreeSurface(surface);
		//}
		surface.LoadBaseImage(ZFontEngine::GetFont(BIG_WHITE_FONT).Render(message));
		surface.BlitSurface(NULL, &to_rect);
		//surface.RenderSurface(to_rect.x, to_rect.y);
		surface.Unload();

		//render seconds
		sprintf(message, "%02d", seconds);
		to_rect.x = off_x + 548 + x_seconds_shift;
		to_rect.y = off_y + y_down_shift;
		//surface = ZFontEngine::GetFont(BIG_WHITE_FONT).Render(message);
		//if(surface)
		//{
		//	SDL_BlitSurface(surface, NULL, dest, &to_rect);
		//	SDL_FreeSurface(surface);
		//}
		surface.LoadBaseImage(ZFontEngine::GetFont(BIG_WHITE_FONT).Render(message));
		surface.BlitSurface(NULL, &to_rect);
		//surface.RenderSurface(to_rect.x, to_rect.y);
		surface.Unload();

		rerender_time = false;
	}
}

void ZHud::RenderBackdrop(SDL_Surface *dest, int off_x, int off_y)
{
	SDL_Rect to_rect;

	if(rerender_backdrop)
	{
		rerender_backdrop = false;

		to_rect.x = off_x + 556;
		to_rect.y = off_y + 44;

		if(aportrait.DoingAnim() || do_end_animations)
		{
			aportrait.SetCords(to_rect.x, to_rect.y);
			aportrait.DoRender();
		}
		else
		{
			portrait.SetCords(to_rect.x, to_rect.y);
			portrait.DoRender();
		}

		//if(render_backdrop)
		//{
		//	render_backdrop->BlitSurface(NULL, &to_rect);
		//	//render_backdrop->RenderSurface(to_rect.x, to_rect.y);
		//	//SDL_BlitSurface(render_backdrop, NULL, dest, &to_rect);
		//}
		//else
		//{
		//	//clear
		//	to_rect.w = 86;
		//	to_rect.h = 74;

		//	//SDL_FillRect(dest, &to_rect, SDL_MapRGB(dest->format, 0, 0, 0));
		//	ZSDL_FillRect(&to_rect, 0, 0, 0);
		//}
	}
}

ZObject *ZHud::GetSelectedObject()
{
	return selected_object;
}

void ZHud::SetMaxUnits(int max_units_)
{
	max_units = max_units_;
}

void ZHud::SetUnitAmount(int unit_amount_)
{
	if(unit_amount == unit_amount_) return;

	unit_amount = unit_amount_;
	//printf("zhud unit_amount:%d\n", unit_amount);

	rerender_unit_amount = true;
}

void ZHud::SetTeam(int team_)
{
	team = team_;

	portrait.SetTeam(team);
	aportrait.SetTeam(team);
}

void ZHud::ShowChatMessage(bool show_chat_)
{
	show_chat = show_chat_;
	rerender_chat = true;

	if(!show_chat)
	{
		chat_message.clear();
		//ZSDL_FreeSurface(chat_message_img);
		chat_message_img.Unload();
	}
	else
	{
		SetChatMessage("");
	}
}

void ZHud::SetChatMessage(string message)
{
	chat_message = "Say:: " + message;

	//ZSDL_FreeSurface(chat_message_img);
	//chat_message_img = ZFontEngine::GetFont(SMALL_WHITE_FONT).Render(chat_message.c_str());
	chat_message_img.LoadBaseImage(ZFontEngine::GetFont(SMALL_WHITE_FONT).Render(chat_message.c_str()));
	//chat_message_img.LoadBaseImage(ZFontEngine::GetFont(YELLOW_MENU_FONT).Render(chat_message.c_str()));
}

void ZHud::SetZTime(ZTime *ztime_)
{
	ztime = ztime_;
}

void ZHud::Init()
{
	int i, j;
	string filename;
	//char filename_c[500];
	
	for(i=0;i<MAX_HUD_BUTTONS;i++)
	{
		button[i].SetType((hud_buttons)i);
		button[i].Init();
	}
	
	//filename = "assets/other/hud/main_hud.png";
	//main_hud = IMG_Load_Error(filename);
	
	//filename = "assets/other/hud/main_hud_side.bmp";
	//main_hud_side.LoadBaseImage(filename);// = IMG_Load_Error(filename);

	filename = "assets/other/hud/main_hud_bottom.bmp";
	main_hud_bottom.LoadBaseImage(filename);// = IMG_Load_Error(filename);

	filename = "assets/other/hud/main_hud_bottom_left.bmp";
	main_hud_bottom_left.LoadBaseImage(filename);

	filename = "assets/other/hud/main_hud_bottom_center.bmp";
	main_hud_bottom_center.LoadBaseImage(filename);

	filename = "assets/other/hud/main_hud_bottom_right.bmp";
	main_hud_bottom_right.LoadBaseImage(filename);

	filename = "assets/other/hud/bottom_filler.bmp";
	bottom_filler.LoadBaseImage(filename);// = IMG_Load_Error(filename);
	
	filename = "assets/other/hud/side_filler.bmp";
	side_filler.LoadBaseImage(filename);// = IMG_Load_Error(filename);

	//filename = "assets/other/hud/backdrop_vehicle.bmp";
	//backdrop_vehicle.LoadBaseImage(filename);// = IMG_Load_Error(filename);

	filename = "assets/other/hud/health_full.png";
	health_full_img.LoadBaseImage(filename);// = IMG_Load_Error(filename);

	filename = "assets/other/hud/health_lost.png";
	health_lost_img.LoadBaseImage(filename);// = IMG_Load_Error(filename);

	filename = "assets/other/hud/health_empty.png";
	health_empty_img.LoadBaseImage(filename);// = IMG_Load_Error(filename);

	for(i=0;i<MAX_TEAM_TYPES;i++)
	{
		filename = "assets/other/hud/unit_amount_bar_" + team_type_string[i] + ".bmp";
		//unit_amount_bar[i].LoadBaseImage(filename);// = IMG_Load_Error(filename);
		ZTeam::LoadZSurface(i, unit_amount_bar[ZTEAM_BASE_TEAM], unit_amount_bar[i], filename);

		filename = "assets/other/hud/main_hud_side_" + team_type_string[i] + ".png";
		//main_hud_side[i].LoadBaseImage(filename);// = IMG_Load_Error(filename);
		ZTeam::LoadZSurface(i, main_hud_side[ZTEAM_BASE_TEAM], main_hud_side[i], filename);

		filename = "assets/other/hud/icon_grenade_" + team_type_string[i] + ".png";
		//main_hud_side[i].LoadBaseImage(filename);// = IMG_Load_Error(filename);
		ZTeam::LoadZSurface(i, grenade_img[ZTEAM_BASE_TEAM], grenade_img[i], filename);
	}

	//for(i=0;i<MAX_PLANET_TYPES;i++)
	//{
	//	filename = "assets/other/hud/backdrop_" + planet_type_string[i] + ".bmp";
	//	backdrop[i].LoadBaseImage(filename);// = IMG_Load_Error(filename);
	//}

	for(i=0;i<MAX_ROBOT_TYPES;i++)
	{
		for(j=0;j<MAX_TEAM_TYPES;j++)
		{
			filename = "assets/other/hud/unit_label_" + robot_type_string[i] + "_" + team_type_string[j] + ".png";
			//robot_unit_label[i][j].LoadBaseImage(filename);// = IMG_Load_Error(filename);
			ZTeam::LoadZSurface(j, robot_unit_label[i][ZTEAM_BASE_TEAM], robot_unit_label[i][j], filename);

			if(j != NULL_TEAM)
			{
				filename = "assets/other/hud/icon_" + robot_type_string[i] + "_" + team_type_string[j] + ".png";
				//robot_icon[i][j].LoadBaseImage(filename);// = IMG_Load_Error(filename);
				ZTeam::LoadZSurface(j, robot_icon[i][ZTEAM_BASE_TEAM], robot_icon[i][j], filename);
			}
		}

		//filename = "assets/other/hud/" + robot_type_string[i] + "_label.bmp";
		filename = "assets/other/hud/label_" + robot_type_string[i] + ".png";
		robot_label[i].LoadBaseImage(filename);// = IMG_Load_Error(filename);
	}

	for(i=0;i<MAX_CANNON_TYPES;i++)
	{
		for(j=1;j<MAX_TEAM_TYPES;j++)
		{
			filename = "assets/other/hud/icon_" + cannon_type_string[i] + "_" + team_type_string[j] + ".png";
			//cannon_icon[i][j].LoadBaseImage(filename);// = IMG_Load_Error(filename);
			ZTeam::LoadZSurface(j, cannon_icon[i][ZTEAM_BASE_TEAM], cannon_icon[i][j], filename);
		}

		//filename = "assets/other/hud/" + cannon_type_string[i] + "_label.bmp";
		filename = "assets/other/hud/label_" + cannon_type_string[i] + ".png";
		cannon_label[i].LoadBaseImage(filename);// = IMG_Load_Error(filename);
	}

	for(i=0;i<MAX_VEHICLE_TYPES;i++)
	{
		for(j=1;j<MAX_TEAM_TYPES;j++)
		{
			filename = "assets/other/hud/icon_" + vehicle_type_string[i] + "_" + team_type_string[j] + ".png";
			//vehicle_icon[i][j].LoadBaseImage(filename);// = IMG_Load_Error(filename);
			ZTeam::LoadZSurface(j, vehicle_icon[i][ZTEAM_BASE_TEAM], vehicle_icon[i][j], filename);
		}

		//filename = "assets/other/hud/" + vehicle_type_string[i] + "_label.bmp";
		filename = "assets/other/hud/label_" + vehicle_type_string[i] + ".png";
		vehicle_label[i].LoadBaseImage(filename);// = IMG_Load_Error(filename);
	}
}
