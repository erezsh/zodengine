#include "gmm_warning.h"

GMMWarning::GMMWarning(gmm_warning_flag warning_flags_) : ZGuiMainMenuBase()
{
	warning_flags = warning_flags_;

	menu_type = GMM_WARNING;
	title = "";
	w = 112;
	h = 60;

	close_button.SetActive(false);

	SetupLayout1();
}

void GMMWarning::SetupLayout1()
{
	int label_y;

	cancel_button.SetType(MMGENERIC_BUTTON);
	cancel_button.SetText("Cancel");
	cancel_button.SetCoords(8, 41);
	cancel_button.SetDimensions(38, 14);
	AddWidget(&cancel_button);

	ok_button.SetType(MMGENERIC_BUTTON);
	ok_button.SetText("Ok");
	ok_button.SetCoords(66, 41);
	ok_button.SetDimensions(38, 14);
	AddWidget(&ok_button);

	label_y = 19;
	text1_label.SetFont(SMALL_WHITE_FONT);
	text1_label.SetText(warning_flags.text1);
	text1_label.SetCoords(8, label_y);
	text1_label.SetDimensions(w-16, MMLABEL_HEIGHT);
	text1_label.SetJustification(MMLABEL_CENTER);
	AddWidget(&text1_label);
	label_y += MMLABEL_HEIGHT + 2;

	text2_label.SetFont(SMALL_WHITE_FONT);
	text2_label.SetText(warning_flags.text2);
	text2_label.SetCoords(8, label_y);
	text2_label.SetDimensions(w-16, MMLABEL_HEIGHT);
	text2_label.SetJustification(MMLABEL_CENTER);
	AddWidget(&text2_label);

	//needed if w is ever changed / set
	UpdateDimensions();
}

void GMMWarning::DoRender(ZMap &the_map, SDL_Surface *dest)
{
	if(!finished_init) return;

	//render base
	menu_warning_img.BlitSurface(NULL, x, y);

	//widgets
	RenderWidgets(the_map, dest);
}

void GMMWarning::HandleWidgetEvent(int event_type, ZGMMWidget *event_widget)
{
	int w_ref_id;

	if(!event_widget) return;

	w_ref_id = event_widget->GetRefID();

	switch(event_type)
	{
	case GMM_UNCLICK_EVENT:
		if(w_ref_id == ok_button.GetRefID())
		{
			killme = true;
			gmm_flags.quit_game = warning_flags.quit_game;
			gmm_flags.reset_map = warning_flags.reset_map;
		}
		else if(w_ref_id == cancel_button.GetRefID())
		{
			killme = true;
		}
		break;
	}
}

