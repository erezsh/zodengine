#include "gwcreateuser.h"

int GWCreateUser::finished_init = false;

ZSDL_Surface GWCreateUser::base_img;

GWCreateUser::GWCreateUser(ZTime *ztime_) : ZGuiWindow(ztime_)
{
	width = 112;
	height = 157;

	ok_button.SetType(OK_MENU_BUTTON);
	cancel_button.SetType(CANCEL_MENU_BUTTON);

	ok_button.SetOffsets(8, 140);
	cancel_button.SetOffsets(66, 140);

	ok_button.SetActive(true);
	cancel_button.SetActive(true);

	username_box.SetOffsets(6, 35);
	loginname_box.SetOffsets(6, 64);
	loginpass_box.SetOffsets(6, 94);
	email_box.SetOffsets(6, 122);

	username_box.SetSelected(true);

	username_box.SetWidth(99);
	loginname_box.SetWidth(99);
	loginpass_box.SetWidth(99);
	email_box.SetWidth(99);

	username_box.SetUseGoodChars(true);
	loginname_box.SetUseGoodChars(true);
	loginpass_box.SetUseGoodChars(true);
	email_box.SetUseGoodChars(true);

	username_box.SetMaxText(MAX_PLAYER_NAME_SIZE);
	loginname_box.SetMaxText(MAX_PLAYER_NAME_SIZE);
	loginpass_box.SetMaxText(MAX_PLAYER_NAME_SIZE);
	email_box.SetMaxText(250);
}

void GWCreateUser::Init()
{
	base_img.LoadBaseImage("assets/other/menus/create_user_menu.png");

	finished_init = true;
}

void GWCreateUser::DoRender(ZMap &the_map, SDL_Surface *dest)
{
	if(!finished_init) return;

	loginname_box.CreateRenderIfNeeded();
	loginpass_box.CreateRenderIfNeeded();
	username_box.CreateRenderIfNeeded();
	email_box.CreateRenderIfNeeded();

	if(base_img.GetBaseSurface())
	{
		int shift_x, shift_y, view_w, view_h;

		//int x, y;

		the_map.GetViewShiftFull(shift_x, shift_y, view_w, view_h);

		x = (view_w - base_img.GetBaseSurface()->w) >> 1;
		y = (view_h - base_img.GetBaseSurface()->h) >> 1;

		x += shift_x;
		y += shift_y;

		the_map.RenderZSurface(&base_img, x, y);

		ok_button.DoRender(the_map, dest, x, y);
		cancel_button.DoRender(the_map, dest, x, y);

		loginname_box.DoRender(the_map, dest, x, y);
		loginpass_box.DoRender(the_map, dest, x, y);
		username_box.DoRender(the_map, dest, x, y);
		email_box.DoRender(the_map, dest, x, y);
	}
}

bool GWCreateUser::Click(int x_, int y_)
{
	int x_local, y_local;

	x_local = x_ - x;
	y_local = y_ - y;

	ok_button.Click(x_local, y_local);
	cancel_button.Click(x_local, y_local);

	if(loginname_box.Click(x_local, y_local))
	{
		RemoveSelections();
		loginname_box.SetSelected(true);
	}
	if(loginpass_box.Click(x_local, y_local))
	{
		RemoveSelections();
		loginpass_box.SetSelected(true);
	}
	if(username_box.Click(x_local, y_local))
	{
		RemoveSelections();
		username_box.SetSelected(true);
	}
	if(email_box.Click(x_local, y_local))
	{
		RemoveSelections();
		email_box.SetSelected(true);
	}

	if(x_ < x) return false;
	if(y_ < y) return false;
	if(x_ >= x + width) return false;
	if(y_ >= y + height) return false;

	return true;
}

bool GWCreateUser::UnClick(int x_, int y_)
{
	int x_local, y_local;

	gflags.Clear();

	x_local = x_ - x;
	y_local = y_ - y;

	if(ok_button.UnClick(x_local, y_local)) DoOk();
	if(cancel_button.UnClick(x_local, y_local)) DoCancel();

	if(x_ < x) return false;
	if(y_ < y) return false;
	if(x_ >= x + width) return false;
	if(y_ >= y + height) return false;

	return true;
}

bool GWCreateUser::KeyPress(int c)
{
	gflags.Clear();

	if(c == 9) //tab
	{
		if(username_box.IsSelected())
		{
			RemoveSelections();
			loginname_box.SetSelected(true);
		}
		else if(loginname_box.IsSelected())
		{
			RemoveSelections();
			loginpass_box.SetSelected(true);
		}
		else if(loginpass_box.IsSelected())
		{
			RemoveSelections();
			email_box.SetSelected(true);
		}
		else if(email_box.IsSelected())
		{
			RemoveSelections();
			username_box.SetSelected(true);
		}

		return true;
	}
	else if(c == 13) //enter
	{
		DoOk();

		return true;
	}

	if(loginname_box.IsSelected()) loginname_box.KeyPress(c);
	else if(loginpass_box.IsSelected()) loginpass_box.KeyPress(c);
	else if(username_box.IsSelected()) username_box.KeyPress(c);
	else if(email_box.IsSelected()) email_box.KeyPress(c);

	return true;
}

void GWCreateUser::RemoveSelections()
{
	loginname_box.SetSelected(false);
	loginpass_box.SetSelected(false);
	username_box.SetSelected(false);
	email_box.SetSelected(false);
}

void GWCreateUser::DoOk()
{
	printf("ok\n");
	gflags.do_createuser = true;
	gflags.user_name = username_box.GetText();
	gflags.login_name = loginname_box.GetText();
	gflags.login_password = loginpass_box.GetText();
	gflags.email = email_box.GetText();
}

void GWCreateUser::DoCancel()
{
	printf("cancel\n");
	gflags.open_login = true;
}
