#include "gmm_change_teams.h"

GMMChangeTeams::GMMChangeTeams() : ZGuiMainMenuBase()
{
	menu_type = GMM_CHANGE_TEAMS;
	title = "Change Teams";
	w = 112;
	h = 118;

	//SetupLayout1();
	//SetupLayout2();
	//SetupLayout3();
	SetupLayout4();
}

void GMMChangeTeams::SetupLayout1()
{
	int i;
	int last_team_button_y;

	//setup widgets
	your_label.SetText("Change Team To");
	your_label.SetCoords(GMM_SIDE_MARGIN, GMM_TITLE_MARGIN);
	your_label.SetDimensions(w - (GMM_SIDE_MARGIN * 2), 0);
	your_label.SetJustification(MMLABEL_CENTER);
	AddWidget(&your_label);

	for(i=0; i<MAX_TEAM_TYPES; i++)
	{
		team_button[i].SetType(MMGENERIC_BUTTON);
		team_button[i].SetText(team_type_string[i]);
		team_button[i].SetCoords(GMM_SIDE_MARGIN, MMLABEL_HEIGHT + 2 + GMM_TITLE_MARGIN + i*(GMMWBUTTON_HEIGHT+1));
		team_button[i].SetDimensions(w - (GMM_SIDE_MARGIN * 2), GMMWBUTTON_HEIGHT);
		AddWidget(&team_button[i]);
	}

	last_team_button_y = (MMLABEL_HEIGHT + 2 + GMM_TITLE_MARGIN + i*(GMMWBUTTON_HEIGHT+1));

	all_label.SetText("---");
	all_label.SetCoords(GMM_SIDE_MARGIN, last_team_button_y + 2);
	all_label.SetDimensions(w - (GMM_SIDE_MARGIN * 2), 0);
	all_label.SetJustification(MMLABEL_CENTER);
	AddWidget(&all_label);

	reshuffle_button.SetType(MMGENERIC_BUTTON);
	reshuffle_button.SetText("Reshuffle Teams");
	reshuffle_button.SetCoords(GMM_SIDE_MARGIN, last_team_button_y + MMLABEL_HEIGHT + 4);
	reshuffle_button.SetDimensions(w - (GMM_SIDE_MARGIN * 2), GMMWBUTTON_HEIGHT);
	AddWidget(&reshuffle_button);

	//height hack
	h = last_team_button_y + MMLABEL_HEIGHT + 4 + GMMWBUTTON_HEIGHT + GMM_BOTTOM_MARGIN;

	//needed if w is ever changed / set
	UpdateDimensions();
}

void GMMChangeTeams::SetupLayout2()
{
	int i;
	int temp_y;

	temp_y = GMM_TITLE_MARGIN;
	reshuffle_button.SetType(MMGENERIC_BUTTON);
	reshuffle_button.SetText("Reshuffle Teams");
	reshuffle_button.SetCoords(GMM_SIDE_MARGIN, temp_y);
	reshuffle_button.SetDimensions(w - (GMM_SIDE_MARGIN * 2), GMMWBUTTON_HEIGHT);
	AddWidget(&reshuffle_button);
	temp_y += GMMWBUTTON_HEIGHT;

	temp_y += 2;
	your_label.SetText("Change Team To");
	your_label.SetCoords(GMM_SIDE_MARGIN, temp_y);
	your_label.SetDimensions(w - (GMM_SIDE_MARGIN * 2), 0);
	your_label.SetJustification(MMLABEL_CENTER);
	AddWidget(&your_label);
	temp_y += MMLABEL_HEIGHT;

	temp_y += 2;
	for(i=0; i<MAX_TEAM_TYPES; i++)
	{
		team_button[i].SetType(MMGENERIC_BUTTON);
		team_button[i].SetText(team_type_string[i]);
		team_button[i].SetCoords(GMM_SIDE_MARGIN, temp_y + i*(GMMWBUTTON_HEIGHT+1));
		team_button[i].SetDimensions(w - (GMM_SIDE_MARGIN * 2), GMMWBUTTON_HEIGHT);
		AddWidget(&team_button[i]);
	}
	temp_y += MAX_TEAM_TYPES * (GMMWBUTTON_HEIGHT+1);

	//fit h to widgets
	h = temp_y + GMM_BOTTOM_MARGIN;

	//needed if w is ever changed / set
	UpdateDimensions();
}

void GMMChangeTeams::SetupLayout3()
{
	int i;
	int temp_y;

	temp_y = GMM_TITLE_MARGIN;
	your_label.SetText("Change Team To");
	your_label.SetCoords(GMM_SIDE_MARGIN, temp_y);
	your_label.SetDimensions(w - (GMM_SIDE_MARGIN * 2), 0);
	your_label.SetJustification(MMLABEL_CENTER);
	AddWidget(&your_label);
	temp_y += MMLABEL_HEIGHT;

	temp_y += 2;
	for(i=0; i<MAX_TEAM_TYPES; i++)
	{
		team_button[i].SetType(MMGENERIC_BUTTON);
		team_button[i].SetText(team_type_string[i]);
		team_button[i].SetCoords(GMM_SIDE_MARGIN, temp_y + i*(GMMWBUTTON_HEIGHT+1));
		team_button[i].SetDimensions(w - (GMM_SIDE_MARGIN * 2), GMMWBUTTON_HEIGHT);
		AddWidget(&team_button[i]);

		team_color[i].SetCoords(GMM_SIDE_MARGIN, temp_y + i*(GMMWBUTTON_HEIGHT+1));
		team_color[i].SetDimensions(w - (GMM_SIDE_MARGIN * 2), GMMWBUTTON_HEIGHT);
		team_color[i].SetTeam(i);
		AddWidget(&team_color[i]);
	}
	temp_y += MAX_TEAM_TYPES * (GMMWBUTTON_HEIGHT+1);

	//reshuffle_button.SetType(MMGENERIC_BUTTON);
	//reshuffle_button.SetText("Reshuffle Teams");
	//reshuffle_button.SetCoords(GMM_SIDE_MARGIN, temp_y);
	//reshuffle_button.SetDimensions(w - (GMM_SIDE_MARGIN * 2), GMMWBUTTON_HEIGHT);
	//AddWidget(&reshuffle_button);
	//temp_y += GMMWBUTTON_HEIGHT;

	//fit h to widgets
	h = temp_y + GMM_BOTTOM_MARGIN;

	//needed if w is ever changed / set
	UpdateDimensions();
}

void GMMChangeTeams::SetupLayout4()
{
	int i;
	int temp_y, temp_x;
	const int button_width = 40;

	temp_y = GMM_TITLE_MARGIN;
	your_label.SetText("Change Team To:");
	your_label.SetCoords(GMM_SIDE_MARGIN, temp_y);
	your_label.SetDimensions(w - (GMM_SIDE_MARGIN * 2), 0);
	your_label.SetJustification(MMLABEL_NORMAL);
	AddWidget(&your_label);
	temp_y += MMLABEL_HEIGHT;

	temp_y += 2;
	for(i=0; i<MAX_TEAM_TYPES; i++)
	{
		int cur_y;

		cur_y = temp_y + i*(GMMWBUTTON_HEIGHT+1);

		temp_x = w - (GMM_SIDE_MARGIN + button_width);
		team_button[i].SetType(MMGENERIC_BUTTON);
		team_button[i].SetText("Join");
		team_button[i].SetCoords(temp_x, cur_y);
		team_button[i].SetDimensions(button_width, GMMWBUTTON_HEIGHT);
		AddWidget(&team_button[i]);

		temp_x -= 2 + MMTEAM_COLOR_WIDTH;
		team_color[i].SetCoords(temp_x, cur_y + ((GMMWBUTTON_HEIGHT - MMTEAM_COLOR_HEIGHT) / 2));
		team_color[i].SetTeam(i);
		AddWidget(&team_color[i]);

		temp_x -= 2;
		team_label[i].SetText(team_type_string[i]);
		team_label[i].SetCoords(GMM_SIDE_MARGIN, cur_y + ((GMMWBUTTON_HEIGHT - MMLABEL_HEIGHT) / 2));
		team_label[i].SetDimensions(((temp_x - 2) - x) - GMM_SIDE_MARGIN, 0);
		team_label[i].SetJustification(MMLABEL_RIGHT);
		AddWidget(&team_label[i]);
	}
	temp_y += MAX_TEAM_TYPES * (GMMWBUTTON_HEIGHT+1);

	//fit h to widgets
	h = temp_y + GMM_BOTTOM_MARGIN;

	//needed if w is ever changed / set
	UpdateDimensions();
}

void GMMChangeTeams::Process()
{
	HighlightOurTeam();

	ProcessWidgets();
}

void GMMChangeTeams::HighlightOurTeam()
{
	//check
	if(!player_team) return;
	if(*player_team < 0) return;
	if(*player_team >= MAX_TEAM_TYPES) return;

	for(int i=0;i<MAX_TEAM_TYPES;i++)
		team_button[i].SetGreen(false);

	team_button[*player_team].SetGreen(true);
}

void GMMChangeTeams::HandleWidgetEvent(int event_type, ZGMMWidget *event_widget)
{
	int w_ref_id;

	if(!event_widget) return;

	w_ref_id = event_widget->GetRefID();

	switch(event_type)
	{
	case GMM_UNCLICK_EVENT:
		if(w_ref_id == reshuffle_button.GetRefID())
		{
			gmm_flags.reshuffle_teams = true;
		}
		else
		{
			for(int i=0;i<MAX_TEAM_TYPES;i++)
				if(w_ref_id == team_button[i].GetRefID())
				{
					gmm_flags.change_team = true;
					gmm_flags.change_team_type = i;
					break;
				}
		}
		break;
	}
}
