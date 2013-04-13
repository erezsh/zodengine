#include "zplayer.h"
#include <math.h>

using namespace COMMON;

void ZPlayer::SetupEHandler()
{
	ehandler.SetParent(this);
	ehandler.AddFunction(TCP_EVENT, DEBUG_EVENT_, test_event);
	ehandler.AddFunction(TCP_EVENT, STORE_MAP, store_map_event);
	ehandler.AddFunction(TCP_EVENT, ADD_NEW_OBJECT, add_new_object_event);
	ehandler.AddFunction(TCP_EVENT, SET_ZONE_INFO, set_zone_info_event);
	ehandler.AddFunction(TCP_EVENT, NEWS_EVENT, display_news_event);
	ehandler.AddFunction(TCP_EVENT, SEND_WAYPOINTS, set_object_waypoints_event);
	ehandler.AddFunction(TCP_EVENT, SEND_RALLYPOINTS, set_object_rallypoints_event);
	ehandler.AddFunction(TCP_EVENT, SEND_LOC, set_object_loc_event);
	ehandler.AddFunction(TCP_EVENT, SET_OBJECT_TEAM, set_object_team_event);
	ehandler.AddFunction(TCP_EVENT, SET_ATTACK_OBJECT, set_object_attack_object_event);
	ehandler.AddFunction(TCP_EVENT, DELETE_OBJECT, delete_object_event);
	ehandler.AddFunction(TCP_EVENT, UPDATE_HEALTH, set_object_health_event);
	ehandler.AddFunction(TCP_EVENT, END_GAME, end_game_event);
	ehandler.AddFunction(TCP_EVENT, RESET_GAME, reset_game_event);
	ehandler.AddFunction(TCP_EVENT, FIRE_MISSILE, fire_object_missile_event);
	ehandler.AddFunction(TCP_EVENT, DESTROY_OBJECT, destroy_object_event);
	ehandler.AddFunction(TCP_EVENT, SET_BUILDING_STATE, set_building_state_event);
	ehandler.AddFunction(TCP_EVENT, SET_BUILT_CANNON_AMOUNT, set_building_cannon_list_event);
	ehandler.AddFunction(TCP_EVENT, COMP_MSG, set_computer_message_event);
	ehandler.AddFunction(TCP_EVENT, OBJECT_GROUP_INFO, set_object_group_info_event);
	ehandler.AddFunction(TCP_EVENT, DO_CRANE_ANIM, do_crane_anim_event);
	ehandler.AddFunction(TCP_EVENT, SET_REPAIR_ANIM, set_repair_building_anim_event);
	ehandler.AddFunction(TCP_EVENT, SET_SETTINGS, set_settings_event);
	ehandler.AddFunction(TCP_EVENT, SET_LID_OPEN, set_lid_open_event);
	ehandler.AddFunction(TCP_EVENT, SNIPE_OBJECT, snipe_object_event);
	ehandler.AddFunction(TCP_EVENT, DRIVER_HIT_EFFECT, driver_hit_effect_event);
	ehandler.AddFunction(TCP_EVENT, CLEAR_PLAYER_LIST, clear_player_list_event);
	ehandler.AddFunction(TCP_EVENT, ADD_LPLAYER, add_player_event);
	ehandler.AddFunction(TCP_EVENT, DELETE_LPLAYER, delete_player_event);
	ehandler.AddFunction(TCP_EVENT, SET_LPLAYER_NAME, set_player_name_event);
	ehandler.AddFunction(TCP_EVENT, SET_LPLAYER_TEAM, set_player_team_event);
	ehandler.AddFunction(TCP_EVENT, SET_LPLAYER_MODE, set_player_mode_event);
	ehandler.AddFunction(TCP_EVENT, SET_LPLAYER_IGNORED, set_player_ignored_event);
	ehandler.AddFunction(TCP_EVENT, SET_LPLAYER_LOGINFO, set_player_loginfo_event);
	ehandler.AddFunction(TCP_EVENT, SET_LPLAYER_VOTEINFO, set_player_voteinfo_event);
	ehandler.AddFunction(TCP_EVENT, UPDATE_GAME_PAUSED, update_game_paused_event);
	ehandler.AddFunction(TCP_EVENT, UPDATE_GAME_SPEED, update_game_speed_event);
	ehandler.AddFunction(TCP_EVENT, VOTE_INFO, set_vote_info_event);
	ehandler.AddFunction(TCP_EVENT, GIVE_PLAYER_ID, set_player_id_event);
	ehandler.AddFunction(TCP_EVENT, GIVE_SELECTABLE_MAP_LIST, set_selectable_map_list_event);
	ehandler.AddFunction(TCP_EVENT, GIVE_LOGINOFF, display_login_event);
	ehandler.AddFunction(TCP_EVENT, SET_GRENADE_AMOUNT, set_grenade_amount_event);
	ehandler.AddFunction(TCP_EVENT, PICKUP_GRENADE_ANIM, pickup_grenade_event);
	ehandler.AddFunction(TCP_EVENT, DO_PORTRAIT_ANIM, do_portrait_anim_event);
	ehandler.AddFunction(TCP_EVENT, TEAM_ENDED, team_ended_event);
	ehandler.AddFunction(TCP_EVENT, SET_TEAM, set_team_event);
	ehandler.AddFunction(TCP_EVENT, POLL_BUY_REGKEY, poll_buy_regkey);
	ehandler.AddFunction(TCP_EVENT, RETURN_REGKEY, set_regkey);
	ehandler.AddFunction(TCP_EVENT, SET_BUILDING_QUEUE_LIST, set_build_queue_list_event);
	ehandler.AddFunction(TCP_EVENT, REQUEST_VERSION, request_version_event);
	ehandler.AddFunction(TCP_EVENT, GIVE_VERSION, get_version_event);
	
	ehandler.AddFunction(OTHER_EVENT, CONNECT_EVENT, connect_event);
	ehandler.AddFunction(OTHER_EVENT, DISCONNECT_EVENT, disconnect_event);
	
	ehandler.AddFunction(SDL_EVENT, RESIZE_EVENT, resize_event);
	ehandler.AddFunction(SDL_EVENT, LCLICK_EVENT, lclick_event);
	ehandler.AddFunction(SDL_EVENT, LUNCLICK_EVENT, lunclick_event);
	ehandler.AddFunction(SDL_EVENT, RCLICK_EVENT, rclick_event);
	ehandler.AddFunction(SDL_EVENT, RUNCLICK_EVENT, runclick_event);
	ehandler.AddFunction(SDL_EVENT, MCLICK_EVENT, mclick_event);
	ehandler.AddFunction(SDL_EVENT, MUNCLICK_EVENT, munclick_event);
	ehandler.AddFunction(SDL_EVENT, WHEELUP_EVENT, wheelup_event);
	ehandler.AddFunction(SDL_EVENT, WHEELDOWN_EVENT, wheeldown_event);
	ehandler.AddFunction(SDL_EVENT, KEYDOWN_EVENT_, keydown_event);
	ehandler.AddFunction(SDL_EVENT, KEYUP_EVENT_, keyup_event);
	ehandler.AddFunction(SDL_EVENT, MOTION_EVENT, motion_event);
}

void ZPlayer::motion_event(ZPlayer *p, char *data, int size, int dummy)
{
	if(p->MainMenuMotion())
	{
		return;
	}

	//FocusCameraTo
	if(p->mbutton.down)
	{
		int change_x, change_y;

		change_x = p->mouse_x - p->mbutton.x;
		change_y = p->mouse_y - p->mbutton.y;

		if(!change_x && !change_y) return;

		p->mbutton.map_x += change_x;
		p->mbutton.map_y += change_y;

		p->FocusCameraTo(p->mbutton.map_x, p->mbutton.map_y);

		//move the mouse back
		SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
		SDL_WarpMouse(p->mbutton.x, p->mbutton.y);
		SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
		p->mouse_x = p->mbutton.x;
		p->mouse_y = p->mbutton.y;
		return;
	}

	p->hover_object = NULL;

	//are we over the map?
	//if(p->mouse_x < p->screen->w - HUD_WIDTH && p->mouse_y < p->screen->h - HUD_HEIGHT)
	if(p->mouse_x < p->init_w - HUD_WIDTH && p->mouse_y < p->init_h - HUD_HEIGHT)
	{
		int shift_x, shift_y;
		int map_x, map_y;
		vector<ZObject*>::iterator i;

		p->zmap.GetViewShift(shift_x, shift_y);

		map_x = p->mouse_x + shift_x;
		map_y = p->mouse_y + shift_y;

		//printf("motion:: mouse over map: %d %d \n", map_x, map_y);

		//find if we have an object under this cursor

		for(i=p->object_list.begin(); i!=p->object_list.end(); i++)
			if((*i)->UnderCursor(map_x, map_y))
		{
			p->hover_object_can_attack = (*i)->UnderCursorCanAttack(map_x, map_y);
			p->hover_object_can_enter_fort = (*i)->UnderCursorFortCanEnter(map_x, map_y);

			//if we can not attack or enter
			//then we aren't hovering over this object
			if(!p->hover_object_can_attack && !p->hover_object_can_enter_fort) continue;

			p->hover_object = (*i);
			//printf("object under cursor:%s\n", p->hover_object->GetObjectName().c_str());
		}
		
		if(!p->lbutton.started_over_hud && !p->lbutton.started_over_gui)
			p->CollectSelectables();
	}
	else
	{
		if(p->lbutton.down && p->lbutton.started_over_hud)
		{
			hud_click_response hud_response;

			p->zhud.MouseMotion(p->mouse_x, p->mouse_y, p->init_w, p->init_h, hud_response);

			if(hud_response.used)
			{
				if(hud_response.type == HUD_MINI_MAP)
				{
					p->FocusCameraTo(hud_response.mini_x, hud_response.mini_y);
				}
			}
		}
	}

	//a better place can be found
	p->DetermineCursor();
}

void ZPlayer::resize_event(ZPlayer *p, char *data, int size, int dummy)
{
	if(p->use_opengl)
	{
		if(p->is_windowed)
			SDL_SetVideoMode(p->init_w, p->init_h, 0, SDL_OPENGL | SDL_RESIZABLE);
		else
			SDL_SetVideoMode(p->init_w, p->init_h, 0, SDL_OPENGL | SDL_FULLSCREEN);

		ResetOpenGLViewPort(p->init_w, p->init_h);
	}
	else
	{
		if(p->is_windowed)
			SDL_SetVideoMode(p->init_w, p->init_h, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);
		else
			SDL_SetVideoMode(p->init_w, p->init_h, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE | SDL_FULLSCREEN);
	}

	ZSDL_Surface::SetScreenDimensions(p->init_w, p->init_h);
	
	p->zhud.ReRenderAll();
	
	p->zmap.SetViewingDimensions(p->init_w - HUD_WIDTH, p->init_h - HUD_HEIGHT);

	//move menus around
	if(p->prev_w && p->prev_h) p->MainMenuMove(1.0 * p->init_w / p->prev_w, 1.0 * p->init_h / p->prev_h);
	p->prev_w = p->init_w;
	p->prev_h = p->init_h;

// redraw_everything();
}

void ZPlayer::lclick_event(ZPlayer *p, char *data, int size, int dummy)
{
	hud_click_response hud_response;

	p->lbutton.down = true;

	//taken by a main menu?
	if(p->MainMenuAbsorbLClick())
	{
		p->lbutton.started_over_gui = true;
		return;
	}

	//p->zhud.LeftClick(p->mouse_x, p->mouse_y, p->screen->w, p->screen->h, hud_response);
	p->zhud.LeftClick(p->mouse_x, p->mouse_y, p->init_w, p->init_h, hud_response);
		

	//did the hud absorb the click?
	if(!hud_response.used)
	{
		p->lbutton.started_over_hud = false;

		//see if gui absorbs it
		if(p->place_cannon)
		{

		}
		else if(p->GuiAbsorbLClick())
		{
			p->lbutton.started_over_gui = true;
		}
		else if(p->zcomp_msg.AbsorbedLClick(p->mouse_x, p->mouse_y, p->zmap))
		{
			p->lbutton.started_over_gui = true;
		}
		else
		{
			p->lbutton.started_over_gui = false;
			//normal click
			p->select_info.Clear();
			p->DetermineCursor();
			p->GiveHudSelected();
		}
	}
	else
	{
		p->lbutton.started_over_hud = true;
		p->lbutton.started_over_gui = false;

		switch(hud_response.type)
		{
		case HUD_BUTTON:
			break;
		case HUD_MINI_MAP:
			p->FocusCameraTo(hud_response.mini_x, hud_response.mini_y);
			break;
		case HUD_JUMP_TO_UNIT:
			{
				int x, y;
				ZObject *obj;

				obj = p->GetObjectFromID(hud_response.jump_ref_id, p->object_list);

				if(obj)
				{
					obj->GetCenterCords(x, y);
					p->FocusCameraTo(x, y);
				}

				if(!p->select_info.ObjectIsSelected(obj))
				{
					p->SelectZObject(obj);
				}
			}
			break;
		}
	}
}

void ZPlayer::lunclick_event(ZPlayer *p, char *data, int size, int dummy)
{
	hud_click_response hud_response;

	//taken by a main menu?
	if(p->MainMenuAbsorbLUnClick())
	{
		p->lbutton.down = false;
		return;
	}

	//p->zhud.LeftUnclick(p->mouse_x, p->mouse_y, p->screen->w, p->screen->h, hud_response);
	p->zhud.LeftUnclick(p->mouse_x, p->mouse_y, p->init_w, p->init_h, hud_response);

	//send it out
	if(!hud_response.used)
	{
		if(p->DoPlaceCannon())
		{

		}
		else if(p->GuiAbsorbLUnClick())
		{
			
		}
		else if(p->lbutton.started_over_gui && p->zcomp_msg.AbsorbedLUnClick(p->mouse_x, p->mouse_y, p->zmap))
		{
			if(p->zcomp_msg.GetFlags().resume_game)
			{
				p->SendSetPaused(false);
				//update_game_paused_packet packet;
				//packet.game_paused = false;
				//p->client_socket.SendMessage(SET_GAME_PAUSED, (const char*)&packet, sizeof(update_game_paused_packet));
			}
			else
			{
				ZObject *obj;
				obj = p->GetObjectFromID(p->zcomp_msg.GetFlags().ref_id, p->object_list);

				if(obj)
				{
					//select it?
					if(p->zcomp_msg.GetFlags().select_obj) p->SelectZObject(obj);
					//{
					//	//p->SelectZObject(obj);
					//	//p->DetermineCursor();
					//	//p->GiveHudSelected();
					//}

					//open a gui?
					if(p->zcomp_msg.GetFlags().open_gui)
					{
						p->ObjectMakeGuiWindow(obj);
						//p->DeleteCurrentGuiWindow();
						//if(obj->GetOwner() != NULL_TEAM && obj->GetOwner() == p->our_team && (p->gui_window = obj->MakeGuiWindow()))
						//{
						//	//give it the build list
						//	p->gui_window->SetBuildList(&p->buildlist);
						//}
					}

					//run to it
					int x, y;
					obj->GetCenterCords(x, y);
					p->FocusCameraTo(x, y);

					//printf("zcomp_msg.AbsorbedLUnClick ref_id:%d obj:%d\n", p->zcomp_msg.GetFlags().ref_id, obj);
				}
			}
		}
	}

	//p->GuiAbsorbLUnClick();

	//collect selectables
	if(!p->lbutton.started_over_hud && !p->lbutton.started_over_gui)
	{
		p->CollectSelectables();
		p->lbutton.down = false;
		p->DetermineCursor();
		p->GiveHudSelected();
	}
	else
		p->lbutton.down = false;

	//did the hud absorb the click?
	if(hud_response.used)
	{
	
	}

	//we have a hud event?
	switch(hud_response.type)
		{
		case HUD_BUTTON:
			p->HandleButton((hud_buttons)hud_response.button);
			break;
		case HUD_MINI_MAP: break;
		case HUD_JUMP_TO_UNIT: break;
		}
}

void ZPlayer::rclick_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->rbutton.down = true;
}

void ZPlayer::runclick_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->rbutton.down = false;

	if(!p->ShiftDown()) 
            p->ClearDevWayPointsOfSelected();

	p->AddDevWayPointToSelected();

	if(!p->ShiftDown()) 
	{
		//special case where a vehicle ejects?
		if(p->select_info.selected_list.size() == 1)
		{
			unsigned char ot, oid;
			int shift_x, shift_y;
			int mouse_x_map, mouse_y_map;
			vector<ZObject*>::iterator i;

			i = p->select_info.selected_list.begin();

			(*i)->GetObjectID(ot, oid);

			p->zmap.GetViewShift(shift_x, shift_y);

			mouse_x_map = p->mouse_x + shift_x;
			mouse_y_map = p->mouse_y + shift_y;

			//if(ot == CANNON_OBJECT && (*i)->UnderCursor(mouse_x_map, mouse_y_map))
			if((*i)->CanEjectDrivers() && (*i)->UnderCursor(mouse_x_map, mouse_y_map))
			{
				eject_vehicle_packet the_packet;

				the_packet.ref_id = (*i)->GetRefID();

				p->client_socket.SendMessage(EJECT_VEHICLE, (char*)&the_packet, sizeof(eject_vehicle_packet));

				p->ClearDevWayPointsOfSelected();
			}
		}

	}

        p->SendDevWayPointsOfSelected();
}

void ZPlayer::mclick_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->mbutton.down = true;
}

void ZPlayer::munclick_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->mbutton.down = false;
}

void ZPlayer::wheelup_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->MainMenuWheelUp();
	if(p->active_menu) p->active_menu->WheelUpButton();
	if(p->gui_window) p->gui_window->WheelUpButton();
	if(p->gui_factory_list) p->gui_factory_list->WheelUpButton();
}

void ZPlayer::wheeldown_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->MainMenuWheelDown();
	if(p->active_menu) p->active_menu->WheelDownButton();
	if(p->gui_window) p->gui_window->WheelDownButton();
	if(p->gui_factory_list) p->gui_factory_list->WheelDownButton();
}

void ZPlayer::keydown_event(ZPlayer *p, char *data, int size, int dummy)
{
	key_event *pi = (key_event*)data;
	int the_key = pi->the_key;
	int the_unicode = pi->the_unicode;
	
	if(the_unicode) p->ProcessUnicode(the_unicode);

	//printf("keydown:%d unicode:%d\n", the_key, the_unicode);

	p->SetAsciiState(the_key, true);

	switch(the_key)
	{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if(p->CtrlDown())
				p->select_info.SetGroup(the_key - '0');
			else
			{
				p->LoadControlGroup(the_key - '0');
			}
			break;
		case 27: //esc key
			exit(0);
			break;
		case 305: // rctrl
			p->rctrl_down = true;
			break;
		case 306: // lctrl
			p->lctrl_down = true;
			break;
		case 307: // ralt
			p->ralt_down = true;
			break;
		case 308: // lalt
			p->lalt_down = true;
			break;
		case 273: //up
			//p->zmap.ShiftViewUp();
			if(!p->up_down)
			{
				p->up_down = true;
				p->vert_scroll_over = 0;
				p->last_vert_scroll_time = current_time();
			}
			break;
		case 274: //down
			//p->zmap.ShiftViewDown();
			if(!p->down_down)
			{
				p->down_down = true;
				p->vert_scroll_over = 0;
				p->last_vert_scroll_time = current_time();
			}
			break;
		case 275: //right
			//p->zmap.ShiftViewRight();
			if(!p->right_down)
			{
				p->right_down = true;
				p->horz_scroll_over = 0;
				p->last_horz_scroll_time = current_time();
			}
			break;
		case 276: //left
			//p->zmap.ShiftViewLeft();
			if(!p->left_down)
			{
				p->left_down = true;
				p->horz_scroll_over = 0;
				p->last_horz_scroll_time = current_time();
			}
			break;
		case 304: //left shift
			p->lshift_down = true;
			break;
		case 303: //right shift
			p->rshift_down = true;
			break;
		case SDLK_F1:
			p->SendVoteYes();
			break;
		case SDLK_F2:
			p->SendVoteNo();
			break;
		case SDLK_F3:
			p->SendVotePass();
			break;
	}
}

void ZPlayer::keyup_event(ZPlayer *p, char *data, int size, int dummy)
{
	key_event *pi = (key_event*)data;
	int the_key = pi->the_key;
	//printf("keyup:%d\n", the_key);

	p->SetAsciiState(the_key, false);

	switch(the_key)
	{
		case 305: // rctrl
			p->rctrl_down = false;
			break;
		case 306: // lctrl
			p->lctrl_down = false;
			break;
		case 307: // ralt
			p->ralt_down = false;
			break;
		case 308: // lalt
			p->lalt_down = false;
			break;
		case 273: //up
			p->up_down = false;
			break;
		case 274: //down
			p->down_down = false;
			break;
		case 275: //right
			p->right_down = false;
			break;
		case 276: //left
			p->left_down = false;
			break;
		case 304: //left shift
			p->lshift_down = false;
			break;
		case 303: //right shift
			p->rshift_down = false;
			break;
	}
}

void ZPlayer::test_event(ZPlayer *p, char *data, int size, int dummy)
{
	if(size) printf("ZPlayer::test_event:%s...\n", data);
}

void ZPlayer::connect_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->ProcessConnect();

	p->SendLogin();
}

void ZPlayer::disconnect_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->ProcessDisconnect();
}

void ZPlayer::store_map_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->ProcessMapDownload(data, size);
	
	if(p->zmap.Loaded())
	{
		p->zmap.SetViewingDimensions(p->init_w - 100, p->init_h - 36);
		p->zhud.SetTerrainType((planet_type)p->zmap.GetMapBasics().terrain_type);
		p->zhud.GetMiniMap().Setup_Boundaries();
		p->InitAnimals();

		if(p->graphics_loaded)
			ZMusicEngine::PlayPlanetMusic(p->zmap.GetMapBasics().terrain_type);
	}
}

void ZPlayer::add_new_object_event(ZPlayer *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessNewObject(data, size);

	//is it our fort? or something else particular
	if(obj)
	{
		unsigned char ot, oid;

		obj->GetObjectID(ot, oid);

		if(ot == BUILDING_OBJECT && (oid == FORT_FRONT || oid == FORT_BACK) && obj->GetOwner() == p->our_team)
		{
			p->fort_ref_id = obj->GetRefID();
			p->FocusCameraToFort();
		}

		//if(ot == MAP_ITEM_OBJECT && oid == ROCK_ITEM) ORock::SetupRockRenders(p->zmap, p->object_list);
		if(ot == MAP_ITEM_OBJECT && oid == ROCK_ITEM) ORock::EditRockRender(p->zmap, p->object_list, obj, true);

		obj->CreationMapEffects(p->zmap);

		//set effect list
		obj->SetEffectList(&p->GetEffectList());
	}

	//make the buttons available that let you cycle through your units
	p->ReSetupButtons();

	p->ProcessChangeObjectAmount();
}

void ZPlayer::set_zone_info_event(ZPlayer *p, char *data, int size, int dummy)
{
   p->ProcessZoneInfo(data, size);
}

void ZPlayer::display_news_event(ZPlayer *p, char *data, int size, int dummy)
{
	news_entry new_entry;

	if(size <= 5) return;

	//extract
	memcpy(&new_entry.r, data, 1);
	memcpy(&new_entry.g, data+1, 1);
	memcpy(&new_entry.b, data+2, 1);
	new_entry.message = data+3;

	p->AddNewsEntry(new_entry.message, new_entry.r, new_entry.g, new_entry.b);

}

void ZPlayer::set_object_waypoints_event(ZPlayer *p, char *data, int size, int dummy)
{
	ZObject *our_object;

	our_object = p->ProcessWaypointData(data, size);

	//did any objects get their waypoint list updated?
	if(!our_object) return;

	//tell this object to render its waypoint list for some time
	our_object->ShowWaypoints();
}

void ZPlayer::set_object_rallypoints_event(ZPlayer *p, char *data, int size, int dummy)
{
	ZObject *our_object;

	our_object = p->ProcessRallypointData(data, size);

	//did any objects get their waypoint list updated?
	if(!our_object) return;
}

void ZPlayer::set_object_loc_event(ZPlayer *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessObjectLoc(data, size);
	//int ref_id;
	//object_location new_loc;

	//if(size != 4 + sizeof(object_location)) return;

	//ref_id = ((int*)data)[0];
	//memcpy(&new_loc, data+4, sizeof(object_location));

	//obj = p->GetObjectFromID(ref_id, p->object_list);

	//if(!obj) return;

	//obj->SetLoc(new_loc);
}

void ZPlayer::set_object_team_event(ZPlayer *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessObjectTeam(data, size);

	if(obj)
	{
		//deselect if does not belong to us anymore
		if(obj->GetOwner() != p->our_team)
			p->select_info.DeleteObject(obj);
	}
}

void ZPlayer::set_object_attack_object_event(ZPlayer *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessObjectAttackObject(data, size);

	if(!obj) return;

	//set the little flashy A in the hud?
	if(obj->GetAttackObject() && 
		obj->GetAttackObject()->GetOwner() == p->our_team &&
		p->zhud.GetARefID() == -1 &&
		!(rand() % 5))
	{
		p->zhud.SetARefID(obj->GetAttackObject()->GetRefID());

		if(!p->zhud.GetAPortrait().DoingAnim())
		{
			p->zhud.GetAPortrait().SetObject(obj->GetAttackObject());
			p->zhud.GetAPortrait().StartAnim(WERE_UNDER_ATTACK_ANIM);
		}

		//add this event to the spacebar event list
		p->AddSpaceBarEvent(SpaceBarEvent(obj->GetAttackObject()->GetRefID(), true));
	}

	//if(obj->GetAttackObject())
	//	printf("obj:%s now attacking %s\n", obj->GetObjectName().c_str(), obj->GetAttackObject()->GetObjectName().c_str());
	//else
	//	printf("obj:%s now attacking nothing\n", obj->GetObjectName().c_str());
}

void ZPlayer::delete_object_event(ZPlayer *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessDeleteObject(data, size);

	if(!obj) return;

	//this is now done inside of ProcessDeleteObject
	////now we have to make sure no other part of the client is using this pointer
	//p->DeleteObjectCleanUp(obj);
}

void ZPlayer::set_object_health_event(ZPlayer *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessObjectHealthTeam(data, size);

	if(!obj) return;

	obj->DoHitEffect();
}

void ZPlayer::end_game_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->ProcessEndGame();
}

void ZPlayer::reset_game_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->ProcessResetGame();
}

void ZPlayer::fire_object_missile_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->ProcessFireMissile(data, size);
}

void ZPlayer::destroy_object_event(ZPlayer *p, char *data, int size, int dummy)
{
	destroy_object_packet *pi = (destroy_object_packet*)data;
	ZObject *obj;
	int i;

	//good packet?
	if(size < sizeof(destroy_object_packet)) return;

	obj = p->GetObjectFromID(pi->ref_id, p->object_list);

	if(!obj) return;

	//good packet (double check)?
	if(size != sizeof(destroy_object_packet) + (sizeof(fire_missile_info) * pi->fire_missile_amount)) return;

	obj->SetHealth(0, p->zmap);
	obj->DoDeathEffect(pi->do_fire_death, pi->do_missile_death);

	//if(pi->fire_missile) obj->FireTurrentMissile(pi->missile_x, pi->missile_y, pi->missile_offset_time);
	for(i=0;i<pi->fire_missile_amount;i++)
	{
		fire_missile_info missile_info;

		memcpy((char*)&missile_info, data + sizeof(destroy_object_packet) + (sizeof(fire_missile_info) * i), sizeof(fire_missile_info));
		obj->FireTurrentMissile(missile_info.missile_x, missile_info.missile_y, missile_info.missile_offset_time);
	}

	//was it our fort, if so run to it
	if(obj->GetRefID() == p->fort_ref_id)
	{
		//run to it
		int x, y;
		obj->GetCenterCords(x, y);
		p->FocusCameraTo(x, y);
	}
	else
	//is it the last enemy fort?
	{
		unsigned char ot, oid;
		bool other_teams_exist = false;

		for(int i=RED_TEAM;i<MAX_TEAM_TYPES; i++)
			if(i != p->our_team && i != obj->GetOwner() && p->team_units_available[i] > 0)
			{
				other_teams_exist = true;
				break;
			}

		obj->GetObjectID(ot, oid);

		if((ot == BUILDING_OBJECT && (oid == FORT_FRONT || oid == FORT_BACK)) &&
			obj->GetOwner() != p->our_team &&
			p->team_units_available[p->our_team] > 0 &&
			!other_teams_exist)
		{
			int x, y;
			obj->GetCenterCords(x, y);
			p->FocusCameraTo(x, y);
		}
	}

	//was this the target of our "A" unit?
	if(pi->killer_ref_id != -1 && p->zhud.GetARefID() != -1 && !p->zhud.GetAPortrait().DoingAnim())
	{
		ZObject *aobj;
		ZObject *kobj;

		aobj = ZObject::GetObjectFromID(p->zhud.GetARefID(), p->object_list);
		kobj = ZObject::GetObjectFromID(pi->killer_ref_id, p->object_list);

		//is the killer one of us and near our "A" unit?
		if(aobj && kobj && (kobj->GetOwner() == p->our_team))
		{
			int ax, ay, kx, ky;

			aobj->GetCenterCords(ax, ay);
			kobj->GetCenterCords(kx, ky);

			if(aobj == kobj || points_within_distance(ax, ay, kx, ky, 100))
			{
				static int last_anim = -1;
				int next_anim;

				next_anim = rand() % 7;
				while(next_anim == last_anim)
					next_anim = rand() % 7;

				last_anim = next_anim;

				p->zhud.GetAPortrait().SetObject(kobj);
				p->zhud.GetAPortrait().StartAnim(GOOD_HIT_ANIM + next_anim);
			}
		}
	}

	//delete it?
	if(pi->destroy_object)
	{
		p->DeleteObjectCleanUp(obj);
		p->ols.DeleteObject(obj);
	}
}

void ZPlayer::set_building_state_event(ZPlayer *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessBuildingState(data, size);
}

void ZPlayer::set_building_cannon_list_event(ZPlayer *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessBuildingCannonList(data, size);
}

void ZPlayer::set_computer_message_event(ZPlayer *p, char *data, int size, int dummy)
{
	computer_msg_packet *pi = (computer_msg_packet*)data;

	//good packet?
	if(size != sizeof(computer_msg_packet)) return;

	//process
	switch(pi->sound)
	{
	case COMP_VEHICLE_SND:
		ZSoundEngine::PlayWav(pi->sound);
		p->zcomp_msg.DisplayMessage(VEHICLE_MANUFACTURED_MSG, pi->ref_id);
		p->AddSpaceBarEvent(SpaceBarEvent(pi->ref_id, true));
		break;
	case COMP_ROBOT_SND:
		ZSoundEngine::PlayWav(pi->sound);
		p->zcomp_msg.DisplayMessage(ROBOT_MANUFACTURED_MSG, pi->ref_id);
		p->AddSpaceBarEvent(SpaceBarEvent(pi->ref_id, true));
		break;
	case COMP_GUN_SND:
		ZSoundEngine::PlayWav(pi->sound);
		p->zcomp_msg.DisplayMessage(GUN_MANUFACTURED_MSG, pi->ref_id);
		p->AddSpaceBarEvent(SpaceBarEvent(pi->ref_id, false, true));
		break;
	case COMP_STARTING_MANUFACTURE_SND:
		ZSoundEngine::PlayWav(pi->sound);
		break;
	case COMP_MANUFACTURING_CANCELED_SND:
		ZSoundEngine::PlayWav(pi->sound);
		break;
	case COMP_TERRITORY_LOST_SND:
		ZSoundEngine::PlayWav(pi->sound);
		break;
	case COMP_RADAR_ACTIVATED_SND:
		ZSoundEngine::PlayWav(pi->sound);
		break;
	}
}

void ZPlayer::set_object_group_info_event(ZPlayer *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessObjectGroupInfo(data, size);
	//int ref_id;
	//ZObject *obj;

	////needs to atleast hold the ref_id at this point
	//if(size < 4) return;

	//ref_id = *(int*)data;

	//obj = p->GetObjectFromID(ref_id, p->object_list);

	//if(!obj) return;

	//obj->ProcessGroupInfoData(data, size, p->object_list);
}

void ZPlayer::do_crane_anim_event(ZPlayer *p, char *data, int size, int dummy)
{
	crane_anim_packet *pi = (crane_anim_packet*)data;
	ZObject *obj;
	ZObject *rep_obj;

	//good packet?
	if(size != sizeof(crane_anim_packet)) return;

	obj = p->GetObjectFromID(pi->ref_id, p->object_list);

	if(!obj) return;

	rep_obj = p->GetObjectFromID(pi->rep_ref_id, p->object_list);

	obj->DoCraneAnim(pi->on, rep_obj);
}

void ZPlayer::set_repair_building_anim_event(ZPlayer *p, char *data, int size, int dummy)
{
	repair_building_anim_packet *pi = (repair_building_anim_packet*)data;
	ZObject *obj;

	//good packet?
	if(size != sizeof(repair_building_anim_packet)) return;

	obj = p->GetObjectFromID(pi->ref_id, p->object_list);

	if(!obj) return;

	obj->DoRepairBuildingAnim(pi->on, pi->remaining_time);

	if(pi->play_sound && obj->GetOwner() == p->our_team)
	{
		if(pi->on)
			ZSoundEngine::PlayWav(COMP_STARTING_REPAIR_SND);
		else
		{
			ZSoundEngine::PlayWav(COMP_VEHICLE_REPAIRED_SND);

			//store space bar event
			p->AddSpaceBarEvent(SpaceBarEvent(pi->ref_id));
		}
	}
}

void ZPlayer::set_settings_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->ProcessZSettings(data, size);
	////good packet?
	//if(size != sizeof(ZSettings)) return;

	////stuff it
	//memcpy(&p->zsettings, data, sizeof(ZSettings));
}

void ZPlayer::set_lid_open_event(ZPlayer *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessObjectLidState(data, size);
	//set_lid_state_packet *pi = (set_lid_state_packet*)data;
	//ZObject *obj;

	////good packet?
	//if(size != sizeof(set_lid_state_packet)) return;

	//obj = p->GetObjectFromID(pi->ref_id, p->object_list);

	//if(!obj) return;

	//obj->SetLidState(pi->lid_open);
}

void ZPlayer::snipe_object_event(ZPlayer *p, char *data, int size, int dummy)
{
	snipe_object_packet *pi = (snipe_object_packet*)data;
	ZObject *obj;

	//good packet?
	if(size != sizeof(snipe_object_packet)) return;

	obj = p->GetObjectFromID(pi->ref_id, p->object_list);

	if(!obj) return;

	int cx, cy;

	obj->GetCenterCords(cx, cy);

	p->effect_list.push_back((ZEffect*)(new ERobotTurrent(&(p->ztime), cx, cy-4, obj->GetOwner())));
}

void ZPlayer::driver_hit_effect_event(ZPlayer *p, char *data, int size, int dummy)
{
	driver_hit_packet *pi = (driver_hit_packet*)data;
	ZObject *obj;

	//good packet?
	if(size != sizeof(driver_hit_packet)) return;

	obj = p->GetObjectFromID(pi->ref_id, p->object_list);

	if(!obj) return;

	obj->DoDriverHitEffect();
}

void ZPlayer::clear_player_list_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->player_info.clear();
}

void ZPlayer::add_player_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->ProcessAddLPlayer(data, size);
}

void ZPlayer::delete_player_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->ProcessDeleteLPlayer(data, size);
}

void ZPlayer::set_player_name_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->ProcessSetLPlayerName(data, size);
}

void ZPlayer::set_player_team_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->ProcessSetLPlayerTeam(data, size);
} 

void ZPlayer::set_player_mode_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->ProcessSetLPlayerMode(data, size);
}

void ZPlayer::set_player_ignored_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->ProcessSetLPlayerIgnored(data, size);
}

void ZPlayer::set_player_loginfo_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->ProcessSetLPlayerLogInfo(data, size);
}

void ZPlayer::set_player_voteinfo_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->ProcessSetLPlayerVoteInfo(data, size);

	//update vote numbers maybe
	if(p->zvote.VoteInProgress())
		p->zvote.SetupImages(p->OurPInfo().real_voting_power(), p->VotesNeeded(), p->VotesFor(), p->VotesAgainst(), p->VoteAppendDescription());
		//p->zvote.SetupImages(p->OurPInfo().voting_power, p->VotesNeeded(), p->VotesFor(), p->VotesAgainst(), p->VoteAppendDescription());
}

void ZPlayer::update_game_paused_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->ProcessUpdateGamePaused(data, size);
}

void ZPlayer::update_game_speed_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->ProcessUpdateGameSpeed(data, size);
}

void ZPlayer::set_vote_info_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->ProcessVoteInfo(data, size);

	//setup images if we'll be rendering them
	if(p->zvote.VoteInProgress())
		p->zvote.SetupImages(p->OurPInfo().real_voting_power(), p->VotesNeeded(), p->VotesFor(), p->VotesAgainst(), p->VoteAppendDescription());
		//p->zvote.SetupImages(p->OurPInfo().voting_power, p->VotesNeeded(), p->VotesFor(), p->VotesAgainst(), p->VoteAppendDescription());
}

void ZPlayer::set_player_id_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->ProcessPlayerID(data, size);
}

void ZPlayer::set_selectable_map_list_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->ProcessSelectableMapList(data, size);
}

void ZPlayer::display_login_event(ZPlayer *p, char *data, int size, int dummy)
{
	loginoff_packet *pi = (loginoff_packet*)data;

	//good packet?
	if(size != sizeof(loginoff_packet)) return;

	if(pi->show_login)
	{
		//already shown?
		if(p->active_menu == p->login_menu) return;
		if(p->active_menu == p->create_user_menu) return;

		//then show it
		p->active_menu = p->login_menu;
	}
	else
	{
		//hide it if shown
		if(p->active_menu == p->login_menu) p->active_menu = NULL;
		if(p->active_menu == p->create_user_menu) p->active_menu = NULL;
	}
}

void ZPlayer::set_grenade_amount_event(ZPlayer *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessSetGrenadeState(data, size);

	if(!obj) return;

	if(p->zhud.GetSelectedObject() == obj)
		p->zhud.ReRenderAll();

	if(p->select_info.UpdateGroupMember(obj))
		p->DetermineCursor();
}

void ZPlayer::pickup_grenade_event(ZPlayer *p, char *data, int size, int dummy)
{
	int_packet *pi = (int_packet*)data;
	ZObject *obj;

	//good packet?
	if(size != sizeof(int_packet)) return;

	obj = GetObjectFromID(pi->ref_id, p->object_list);

	if(!obj) return;

	obj->DoPickupGrenadeAnim();

	if(obj->GetOwner() == p->our_team && !p->zhud.GetAPortrait().DoingAnim())
	{
		p->zhud.GetAPortrait().SetObject(obj);
		p->zhud.GetAPortrait().StartAnim(GRENADES_COLLECTED_ANIM);

		//save space bar event
		p->AddSpaceBarEvent(SpaceBarEvent(pi->ref_id, true));
	}
}

void ZPlayer::do_portrait_anim_event(ZPlayer *p, char *data, int size, int dummy)
{
	do_portrait_anim_packet *pi = (do_portrait_anim_packet*)data;
	ZObject *obj;

	//good packet?
	if(size != sizeof(do_portrait_anim_packet)) return;

	obj = GetObjectFromID(pi->ref_id, p->object_list);

	if(!obj) return;

	if(obj->GetOwner() != p->our_team) return;
	if(p->zhud.GetAPortrait().DoingAnim()) return;

	p->zhud.GetAPortrait().SetObject(obj);

	//do the anim
	switch(pi->anim_id)
	{
		case VEHICLE_CAPTURED_ANIM: p->zhud.GetAPortrait().StartAnim(pi->anim_id); break;
		case GUN_CAPTURED_ANIM: p->zhud.GetAPortrait().StartAnim(pi->anim_id); break;
		case TERRITORY_TAKEN_ANIM: p->zhud.GetAPortrait().StartAnim(pi->anim_id); break;
		case TARGET_DESTROYED_ANIM: p->zhud.GetAPortrait().StartAnim(pi->anim_id); break;
	}

	//store space bar event
	p->AddSpaceBarEvent(SpaceBarEvent(pi->ref_id, true));
}

void ZPlayer::team_ended_event(ZPlayer *p, char *data, int size, int dummy)
{
	team_ended_packet *pi = (team_ended_packet*)data;

	//good packet?
	if(size != sizeof(team_ended_packet)) return;

	if(pi->team != p->our_team) return;

	vector<zhud_end_unit> end_animations;

	for(vector<ZObject*>::iterator i=p->object_list.begin(); i!=p->object_list.end(); i++)
		if((*i)->GetOwner() == p->our_team)
	{
		unsigned char ot, oid;

		(*i)->GetObjectID(ot, oid);

		switch(ot)
		{
		case CANNON_OBJECT:
			end_animations.push_back(zhud_end_unit(ot, oid, (*i)->GetDriverType()));
			break;
		case VEHICLE_OBJECT:
			end_animations.push_back(zhud_end_unit(ot, oid, (*i)->GetDriverType()));
			break;
		case ROBOT_OBJECT:
			end_animations.push_back(zhud_end_unit(ot, oid, oid));
			break;
		}
	}

	p->zhud.StartEndAnimations(end_animations, pi->won);
}

void ZPlayer::set_team_event(ZPlayer *p, char *data, int size, int dummy)
{
	p->ProcessSetTeam(data, size);
}

void ZPlayer::poll_buy_regkey(ZPlayer *p, char *data, int size, int dummy)
{
	if(size) return;

	if(p->is_registered)
	{
		p->AddNewsEntry("the zod engine is already registered on this computer");
	}
	else
	{
		if(!file_can_be_written("registration.zkey"))
		{
			p->AddNewsEntry("can not open registration file for writing, please visit www.nighsoft.com for troubleshooting");
			return;
		}

		buy_registration_packet packet;

		memset(packet.buf, 0, 16);
		SocketHandler::GetMAC(packet.buf);

		p->client_socket.SendMessage(BUY_REGKEY, (const char*)&packet, sizeof(buy_registration_packet));
	}
}

void ZPlayer::set_regkey(ZPlayer *p, char *data, int size, int dummy)
{
	buy_registration_packet *pi = (buy_registration_packet*)data;

	//good packet?
	if(size != sizeof(buy_registration_packet)) return;

	if(p->is_registered)
	{
		p->AddNewsEntry("the zod engine is already registered on this computer");
	}
	else
	{
		FILE *fp;
		int ret;

		fp = fopen("registration.zkey", "w");

		if(!fp)
		{
			p->AddNewsEntry("new registration key bought but could not save to file! please visit www.nighsoft.com");
			return;
		}

		ret = fwrite(pi->buf, 16, 1, fp);
	
		if(!ret)
		{
			p->AddNewsEntry("new registration key bought but could not save to file! please visit www.nighsoft.com");
			return;
		}

		fclose(fp);

		//are we registered now?
		if(!p->CheckRegistration())
		{
			p->AddNewsEntry("new registration key bought but we still are not registered! please visit www.nighsoft.com");
			return;
		}
		else
		{
			p->AddNewsEntry("congratulations the zod engine is now registered");
		}
	}
}

void ZPlayer::set_build_queue_list_event(ZPlayer *p, char *data, int size, int dummy)
{
	ZObject *obj;

	obj = p->ProcessBuildingQueueList(data, size);
}

void ZPlayer::request_version_event(ZPlayer *p, char *data, int size, int dummy)
{
	version_packet packet;

	//check
	if(strlen(GAME_VERSION) + 1 >= MAX_VERSION_PACKET_CHARS)
	{
		printf("ZPlayer::request_version_event: '%s' too large to fit in a %d char array\n", GAME_VERSION, MAX_VERSION_PACKET_CHARS);
		return;
	}

	strcpy(packet.version, GAME_VERSION);

	p->client_socket.SendMessage(GIVE_VERSION, (const char*)&packet, sizeof(version_packet));
}

void ZPlayer::get_version_event(ZPlayer *p, char *data, int size, int dummy)
{
	version_packet *pi = (version_packet*)data;
	char temp_version[MAX_VERSION_PACKET_CHARS];
	string message;

	//good packet?
	if(size != sizeof(version_packet)) return;

	memcpy(temp_version, pi->version, MAX_VERSION_PACKET_CHARS);
	temp_version[MAX_VERSION_PACKET_CHARS-1] = 0;

	//if not the same as our version
	if(strcmp(temp_version, GAME_VERSION))
	{
		message = "the server version is ";
		message += temp_version;
		message += ", which mismatches our version ";
		message += GAME_VERSION;
		message += "";

		p->AddNewsEntry(message);
	}
	else
	{
		message = "the server version is ";
		message += temp_version;

		p->AddNewsEntry(message);
	}

}
