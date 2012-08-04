#ifndef _ZGMM_OPTIONS_H_
#define _ZGMM_OPTIONS_H_

#include "zgui_main_menu_base.h"

#define MAX_GMMOPTIONS_SPEED_SETTINGS 7

const double gmmoption_speed_setting_value[MAX_GMMOPTIONS_SPEED_SETTINGS] =
{
	0.25, 0.5, 0.75, 1.0, 1.5, 2.0, 4.0
};

class GMMOptions : public ZGuiMainMenuBase
{
public:
	GMMOptions();

	void Process();
private:
	GMMWButton reshuffle_button;
	GMMWButton reset_button;
	GMMWButton pause_button;
	GMMWLabel volume_label;
	GMMWRadio volume_radio;
	GMMWLabel speed_label;
	GMMWRadio speed_radio;

	void SetupLayout1();

	void HandleWidgetEvent(int event_type, ZGMMWidget *event_widget);

	void SetVolumeStatus();
	void SetTimeStatuses();
};

#endif
