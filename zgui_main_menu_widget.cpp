#include "zgui_main_menu_widgets.h"

int ZGMMWidget::next_ref_id=0;

ZGMMWidget::ZGMMWidget() 
{
	x=y=w=h=0;
	active = true;
	widget_type = MMUNKNOWN_WIDGET;

	//give each widget a unique id
	ref_id=next_ref_id++;
}

bool ZGMMWidget::WithinDimensions(int x_, int y_)
{
	if(x_<x) return false;
	if(y_<y) return false;
	if(x_>x+w) return false;
	if(y_>y+h) return false;

	return true;
}
