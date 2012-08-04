#include "gmm_manage_bots.h"

GMMManageBots::GMMManageBots() : ZGuiMainMenuBase()
{
	menu_type = GMM_MANAGE_BOTS;
	title = "Manage Bots";
	w = 112;
	h = 118;

	SetupLayout1();
}

void GMMManageBots::SetupLayout1()
{
	int i, rows, cols;
	const int max_rows=4;
	const int label_width = 40;
	const int start_width = 25;
	const int stop_width = 25;
	const int button_spacer = 2;

	rows = MAX_TEAM_TYPES-1;
	if(rows > max_rows) rows = max_rows;
	cols = (MAX_TEAM_TYPES-1)/max_rows;
	if((MAX_TEAM_TYPES-1)%max_rows) cols++;
	
	for(i=NULL_TEAM+1; i<MAX_TEAM_TYPES; i++)
	{
		int button_y;
		int button_x;

		button_x = GMM_SIDE_MARGIN + ((i-1)/max_rows)*(label_width + button_spacer + start_width + button_spacer + stop_width);
		button_y = GMM_TITLE_MARGIN + ((i-1)%max_rows)*(GMMWBUTTON_HEIGHT+1);

		//left
		{
			team_label[i].SetText(team_type_string[i]);
			team_label[i].SetCoords(button_x, button_y + ((GMMWBUTTON_HEIGHT-MMLABEL_HEIGHT)/2));
			team_label[i].SetDimensions(label_width, 0);
			team_label[i].SetJustification(MMLABEL_RIGHT);
			AddWidget(&team_label[i]);
			button_x += label_width + button_spacer;

			start_button[i].SetType(MMGENERIC_BUTTON);
			start_button[i].SetText("On");
			start_button[i].SetCoords(button_x, button_y);
			start_button[i].SetDimensions(start_width, GMMWBUTTON_HEIGHT);
			AddWidget(&start_button[i]);
			button_x += start_width + button_spacer;

			stop_button[i].SetType(MMGENERIC_BUTTON);
			stop_button[i].SetText("Off");
			stop_button[i].SetCoords(button_x, button_y);
			stop_button[i].SetDimensions(stop_width, GMMWBUTTON_HEIGHT);
			AddWidget(&stop_button[i]);
			button_x += stop_width;
		}
	}

	w = (GMM_SIDE_MARGIN * 2) + cols*(label_width + button_spacer + start_width + button_spacer + stop_width);
	h = GMM_TITLE_MARGIN + GMM_BOTTOM_MARGIN + (rows)*(GMMWBUTTON_HEIGHT+1);

	//needed if w is ever changed / set
	UpdateDimensions();
}

void GMMManageBots::Process()
{
	CheckBots();

	ProcessWidgets();
}

void GMMManageBots::CheckBots()
{
	bool team_has_bot[MAX_TEAM_TYPES];

	//check
	if(!player_info) return;

	//clear
	for(int i=0;i<MAX_TEAM_TYPES;i++) team_has_bot[i] = false;

	//populate
	for(vector<p_info>::iterator i=player_info->begin(); i!=player_info->end(); ++i)
		if(i->mode == BOT_MODE && !i->ignored)
			team_has_bot[i->team] = true;

	//set
	for(int i=0;i<MAX_TEAM_TYPES;i++)
	{
		if(team_has_bot[i])
		{
			start_button[i].SetGreen(true);
			stop_button[i].SetGreen(false);
		}
		else
		{
			start_button[i].SetGreen(false);
			stop_button[i].SetGreen(true);
		}
	}
}

void GMMManageBots::HandleWidgetEvent(int event_type, ZGMMWidget *event_widget)
{
	int w_ref_id;

	if(!event_widget) return;

	w_ref_id = event_widget->GetRefID();

	switch(event_type)
	{
	case GMM_UNCLICK_EVENT:
		for(int i=0;i<MAX_TEAM_TYPES;i++)
		{
			if(w_ref_id == start_button[i].GetRefID())
			{
				gmm_flags.start_bot = true;
				gmm_flags.start_bot_team = i;
				break;
			}
			else if(w_ref_id == stop_button[i].GetRefID())
			{
				gmm_flags.stop_bot = true;
				gmm_flags.stop_bot_team = i;
				break;
			}
		}
		break;
	}
}
