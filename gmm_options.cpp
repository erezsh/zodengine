#include "gmm_options.h"

GMMOptions::GMMOptions() : ZGuiMainMenuBase()
{
	menu_type = GMM_OPTIONS;
	title = "Options";
	w = 112;
	h = 118;

	SetupLayout1();
}

void GMMOptions::SetupLayout1()
{
	int next_y;

	next_y = GMM_TITLE_MARGIN;

	volume_label.SetText("Set Volume:");
	volume_label.SetCoords(GMM_SIDE_MARGIN, next_y);
	volume_label.SetDimensions(w - (GMM_SIDE_MARGIN * 2), 0);
	volume_label.SetJustification(MMLABEL_NORMAL);
	AddWidget(&volume_label);
	next_y += MMLABEL_HEIGHT + 1;

	volume_radio.SetCoords(GMM_SIDE_MARGIN, next_y);
	volume_radio.SetMaxSelections(MAX_SOUND_SETTINGS);
	AddWidget(&volume_radio);
	next_y += MMRADIO_HEIGHT + 2;

	speed_label.SetText("Set Game Speed:");
	speed_label.SetCoords(GMM_SIDE_MARGIN, next_y);
	speed_label.SetDimensions(w - (GMM_SIDE_MARGIN * 2), 0);
	speed_label.SetJustification(MMLABEL_NORMAL);
	AddWidget(&speed_label);
	next_y += MMLABEL_HEIGHT + 1;

	speed_radio.SetCoords(GMM_SIDE_MARGIN, next_y);
	speed_radio.SetMaxSelections(MAX_GMMOPTIONS_SPEED_SETTINGS);
	AddWidget(&speed_radio);
	next_y += MMRADIO_HEIGHT + 7;

	pause_button.SetType(MMGENERIC_BUTTON);
	pause_button.SetText("Pause Game");
	pause_button.SetCoords(GMM_SIDE_MARGIN, next_y);
	pause_button.SetDimensions(w - (GMM_SIDE_MARGIN * 2), GMMWBUTTON_HEIGHT);
	AddWidget(&pause_button);
	next_y += GMMWBUTTON_HEIGHT + 1;

	reshuffle_button.SetType(MMGENERIC_BUTTON);
	reshuffle_button.SetText("Reshuffle Teams");
	reshuffle_button.SetCoords(GMM_SIDE_MARGIN, next_y);
	reshuffle_button.SetDimensions(w - (GMM_SIDE_MARGIN * 2), GMMWBUTTON_HEIGHT);
	AddWidget(&reshuffle_button);
	next_y += GMMWBUTTON_HEIGHT + 1;

	reset_button.SetType(MMGENERIC_BUTTON);
	reset_button.SetText("Reset Map");
	reset_button.SetCoords(GMM_SIDE_MARGIN, next_y);
	reset_button.SetDimensions(w - (GMM_SIDE_MARGIN * 2), GMMWBUTTON_HEIGHT);
	AddWidget(&reset_button);
	next_y += GMMWBUTTON_HEIGHT + 1;

	h = next_y + GMM_BOTTOM_MARGIN;

	//needed if w is ever changed / set
	UpdateDimensions();
}

void GMMOptions::Process()
{
	SetVolumeStatus();

	SetTimeStatuses();

	ProcessWidgets();
}

void GMMOptions::SetTimeStatuses()
{
	if(!ztime) return;

	pause_button.SetGreen(ztime->IsPaused());

	for(int i=0;i<MAX_GMMOPTIONS_SPEED_SETTINGS;i++)
		if(ztime->GameSpeed() <= gmmoption_speed_setting_value[i])
		{
			char speed_str[500];

			sprintf(speed_str, "Set Game Speed: %.0f%%\n", 100 * ztime->GameSpeed());
			speed_label.SetText(speed_str);
			speed_radio.SetSelected(i);
			break;
		}
}

void GMMOptions::SetVolumeStatus()
{
	int si;

	if(!sound_setting) return;

	si = *sound_setting;

	if(si < 0) return;
	if(si >= MAX_SOUND_SETTINGS) return;

	if(si != volume_radio.GetSelected())
	{
		volume_label.SetText("Set Volume: " + sound_setting_string[si]);
		volume_radio.SetSelected(si);
	}
}

void GMMOptions::HandleWidgetEvent(int event_type, ZGMMWidget *event_widget)
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
		else if(w_ref_id == reset_button.GetRefID())
		{
			//gmm_flags.reset_map = true;
			gmm_flags.open_main_menu = true;
			gmm_flags.open_main_menu_type = GMM_WARNING;
			gmm_flags.warning_flags.text1 = "Are you sure you want";
			gmm_flags.warning_flags.text2 = "to reset the map?";
			gmm_flags.warning_flags.reset_map = true;
		}
		else if(w_ref_id == pause_button.GetRefID())
		{
			gmm_flags.pause_game = true;
		}
		break;
	case GMM_CLICK_EVENT:
		if(w_ref_id == volume_radio.GetRefID())
		{
			//testing
			//volume_radio.SetSelected(volume_radio.GetGMMWFlags().mmradio_si_selected);

			gmm_flags.set_volume = true;
			gmm_flags.set_volume_value = volume_radio.GetGMMWFlags().mmradio_si_selected;
		}
		else if(w_ref_id == speed_radio.GetRefID())
		{
			int si;

			si = speed_radio.GetGMMWFlags().mmradio_si_selected;

			if(si >=0 && si < MAX_GMMOPTIONS_SPEED_SETTINGS)
			{
				gmm_flags.set_game_speed = true;
				gmm_flags.set_game_speed_value = gmmoption_speed_setting_value[si];
			}
		}
		break;
	}
}
