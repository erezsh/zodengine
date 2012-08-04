#include "gmm_player_list.h"

GMMPlayerList::GMMPlayerList() : ZGuiMainMenuBase()
{
	menu_type = GMM_PLAYER_LIST;
	title = "Player List";
	w = 112;
	h = 118;

	SetupLayout1();
}

void GMMPlayerList::SetupLayout1()
{
	players_on_label.SetText("Players Online:");
	players_on_label.SetCoords(GMM_SIDE_MARGIN + 4, GMM_TITLE_MARGIN);
	players_on_label.SetDimensions(w - (GMM_SIDE_MARGIN * 2), 0);
	players_on_label.SetJustification(MMLABEL_NORMAL);
	AddWidget(&players_on_label);

	players_onnum_label.SetText("50");
	players_onnum_label.SetCoords(GMM_SIDE_MARGIN, GMM_TITLE_MARGIN);
	players_onnum_label.SetDimensions(w - (GMM_SIDE_MARGIN * 2), 0);
	players_onnum_label.SetJustification(MMLABEL_RIGHT);
	AddWidget(&players_onnum_label);

	player_list.SetCoords(GMM_SIDE_MARGIN, GMM_TITLE_MARGIN + MMLABEL_HEIGHT + 2);
	player_list.SetDimensions(w - (GMM_SIDE_MARGIN * 2), 118);
	player_list.SetVisibleEntries(8);
	AddWidget(&player_list);

	h = GMM_TITLE_MARGIN + player_list.GetHeight() + GMM_BOTTOM_MARGIN + MMLABEL_HEIGHT + 2;

	//needed if w is ever changed / set
	UpdateDimensions();
}

void GMMPlayerList::Process()
{
	SetupList();

	ProcessWidgets();
}

void GMMPlayerList::SetupList()
{
	char num_c[500];

	if(!player_info) return;

	player_list.GetEntryList().clear();

	//add the players
	for(vector<p_info>::iterator i=player_info->begin(); i!=player_info->end(); ++i)
	{
		if(i->mode == PLAYER_MODE)
			player_list.GetEntryList().push_back(mmlist_entry(team_type_string[i->team] + ": " + i->name, i->p_id, i->team));
	}

	//set player online text
	sprintf(num_c, "%d", player_list.GetEntryList().size());
	players_onnum_label.SetText(num_c);

	//add the bots
	//for(vector<p_info>::iterator i=player_info->begin(); i!=player_info->end(); ++i)
	//{
	//	if(i->mode == BOT_MODE && !i->ignored)
	//		player_list.GetEntryList().push_back(mmlist_entry(team_type_string[i->team] + ": " + i->name, i->p_id, i->team + MAX_TEAM_TYPES));
	//}

	//sort the list
	sort(player_list.GetEntryList().begin(), player_list.GetEntryList().end(), sort_mmlist_entry_func);

	player_list.CheckViewI();
}

void GMMPlayerList::HandleWidgetEvent(int event_type, ZGMMWidget *event_widget)
{

}
