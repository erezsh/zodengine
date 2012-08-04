#ifndef _ZGMM_PLAYER_LIST_H_
#define _ZGMM_PLAYER_LIST_H_

#include "zgui_main_menu_base.h"

class GMMPlayerList : public ZGuiMainMenuBase
{
public:
	GMMPlayerList();

	void Process();
private:
	GMMWList player_list;
	GMMWLabel players_on_label;
	GMMWLabel players_onnum_label;

	void SetupLayout1();

	void HandleWidgetEvent(int event_type, ZGMMWidget *event_widget);

	void SetupList();
};

#endif
