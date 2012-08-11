#include "gmm_select_map.h"

GMMSelectMap::GMMSelectMap() : ZGuiMainMenuBase()
{
	menu_type = GMM_SELECT_MAP;
	title = "Select Map";
	w = 112 + 56;
	h = 118;

	SetupLayout1();
}

void GMMSelectMap::SetupLayout1()
{
	int next_y;

	next_y = GMM_TITLE_MARGIN;
	map_list.SetCoords(GMM_SIDE_MARGIN, next_y);
	map_list.SetDimensions(w - (GMM_SIDE_MARGIN * 2), 118);
	map_list.SetVisibleEntries(8);
	AddWidget(&map_list);
	next_y += map_list.GetHeight();

	next_y += 2;
	select_button.SetType(MMGENERIC_BUTTON);
	select_button.SetText("Select Map");
	select_button.SetCoords(GMM_SIDE_MARGIN, next_y);
	select_button.SetDimensions(w - (GMM_SIDE_MARGIN * 2), GMMWBUTTON_HEIGHT);
	AddWidget(&select_button);
	next_y += GMMWBUTTON_HEIGHT;

	//next_y += 1;
	//reset_button.SetType(MMGENERIC_BUTTON);
	//reset_button.SetText("Reset Current Map");
	//reset_button.SetCoords(GMM_SIDE_MARGIN, next_y);
	//reset_button.SetDimensions(w - (GMM_SIDE_MARGIN * 2), GMMWBUTTON_HEIGHT);
	//AddWidget(&reset_button);
	//next_y += GMMWBUTTON_HEIGHT;

	h = next_y + 1 + GMM_BOTTOM_MARGIN;

	//needed if w is ever changed / set
	UpdateDimensions();
}

void GMMSelectMap::Process()
{
	SetupList();

	ProcessWidgets();
}

void GMMSelectMap::HandleWidgetEvent(int event_type, ZGMMWidget *event_widget)
{
	int w_ref_id;

	if(!event_widget) return;

	w_ref_id = event_widget->GetRefID();

	switch(event_type)
	{
	case GMM_CLICK_EVENT:
		if(w_ref_id == map_list.GetRefID())
		{
			if(map_list.GetGMMWFlags().mmlist_entry_selected != -1)
			{
				map_list.UnSelectAll(map_list.GetGMMWFlags().mmlist_entry_selected);
			}
		}
		break;
	case GMM_UNCLICK_EVENT:
		if(w_ref_id == select_button.GetRefID())
		{
			int selected_map = map_list.GetFirstSelected();

			if(selected_map != -1)
			{
				gmm_flags.change_map = true;
				gmm_flags.change_map_number = map_list.GetEntryList()[selected_map].ref_id;
			}
		}
		break;
	}
}

void GMMSelectMap::SetupList()
{
	if(!selectable_map_list) return;

	//only update list if a change in size is detected
	if(selectable_map_list->size() == map_list.GetEntryList().size()) return;

	map_list.GetEntryList().clear();

	for(int i=0;i<(int)selectable_map_list->size();i++)
		map_list.GetEntryList().push_back(mmlist_entry((*selectable_map_list)[i], i, i));

	map_list.CheckViewI();
}
