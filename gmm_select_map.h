#ifndef _ZGMM_SELECT_MAP_H_
#define _ZGMM_SELECT_MAP_H_

#include "zgui_main_menu_base.h"

class GMMSelectMap : public ZGuiMainMenuBase
{
public:
	GMMSelectMap();

	void Process();
private:
	GMMWList map_list;
	GMMWButton select_button;
	GMMWButton reset_button;

	void SetupLayout1();

	void HandleWidgetEvent(int event_type, ZGMMWidget *event_widget);

	void SetupList();
};

#endif
