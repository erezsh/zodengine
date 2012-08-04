#ifndef _ZGMM_CHANGE_TEAMS_H_
#define _ZGMM_CHANGE_TEAMS_H_

#include "zgui_main_menu_base.h"

class GMMChangeTeams : public ZGuiMainMenuBase
{
public:
	GMMChangeTeams();

	void Process();
private:
	GMMWLabel your_label;
	GMMWButton team_button[MAX_TEAM_TYPES];
	GMMWTeamColor team_color[MAX_TEAM_TYPES];
	GMMWLabel team_label[MAX_TEAM_TYPES];
	GMMWLabel all_label;
	GMMWButton reshuffle_button;

	void SetupLayout1();
	void SetupLayout2();
	void SetupLayout3();
	void SetupLayout4();

	void HighlightOurTeam();

	void HandleWidgetEvent(int event_type, ZGMMWidget *event_widget);
};

#endif
