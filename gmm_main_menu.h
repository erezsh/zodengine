#ifndef _ZGMM_MAIN_MENU_H_
#define _ZGMM_MAIN_MENU_H_

#include "zgui_main_menu_base.h"

enum gmm_main_menu_button
{
	GMMMM_CHANGE_TEAMS_BUTTON, GMMMM_MANAGE_BOTS_BUTTON, GMMMM_PLAYER_LIST_BUTTON, 
	GMMMM_SELECT_MAP_BUTTON, GMMMM_MULTIPLAYER_BUTTON, GMMMM_OPTIONS_BUTTON, 
	GMMMM_QUIT_GAME_BUTTON,
	MAX_GMMMM_BUTTONS
};

const string gmm_main_menu_button_string[MAX_GMMMM_BUTTONS] = 
{
	"Change Teams", "Manage Bots", "Player List", 
	"Select Map", "Multiplayer", "Options", "Quit Game"
};

class GMMMainMenu : public ZGuiMainMenuBase
{
public:
	GMMMainMenu();
private:
	GMMWButton menu_button[MAX_GMMMM_BUTTONS];

	void HandleWidgetEvent(int event_type, ZGMMWidget *event_widget);
};

#endif
