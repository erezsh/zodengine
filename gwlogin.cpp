#include "gwlogin.h"

int GWLogin::finished_init = false;

ZSDL_Surface GWLogin::base_img;

GWLogin::GWLogin(ZTime *ztime_) : ZGuiWindow(ztime_)
{
	width = 112;
	height = 100;

	login_button.SetType(LOGIN_MENU_BUTTON);
	create_button.SetType(CREATE_MENU_BUTTON);

	login_button.SetOffsets(8, 83);
	create_button.SetOffsets(66, 83);

	login_button.SetActive(true);
	create_button.SetActive(true);

	loginname_box.SetOffsets(6, 35);
	loginname_box.SetWidth(99);
	loginname_box.SetSelected(true);

	loginpass_box.SetOffsets(6, 64);
	loginpass_box.SetWidth(99);
	loginpass_box.SetPassworded(true);

	loginname_box.SetUseGoodChars(true);
	loginname_box.SetMaxText(MAX_PLAYER_NAME_SIZE);

	loginpass_box.SetUseGoodChars(true);
	loginpass_box.SetMaxText(MAX_PLAYER_NAME_SIZE);

}

void GWLogin::Init()
{
	base_img.LoadBaseImage("assets/other/menus/login_menu.png");

	finished_init = true;
}

void GWLogin::DoRender(ZMap &the_map, SDL_Surface *dest)
{
	if(!finished_init) return;

	loginname_box.CreateRenderIfNeeded();
	loginpass_box.CreateRenderIfNeeded();

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

		login_button.DoRender(the_map, dest, x, y);
		create_button.DoRender(the_map, dest, x, y);

		loginname_box.DoRender(the_map, dest, x, y);
		loginpass_box.DoRender(the_map, dest, x, y);
	}
}

bool GWLogin::Click(int x_, int y_)
{
	int x_local, y_local;

	x_local = x_ - x;
	y_local = y_ - y;

	login_button.Click(x_local, y_local);
	create_button.Click(x_local, y_local);

	if(loginname_box.Click(x_local, y_local))
	{
		loginname_box.SetSelected(true);
		loginpass_box.SetSelected(false);
	}
	if(loginpass_box.Click(x_local, y_local))
	{
		loginname_box.SetSelected(false);
		loginpass_box.SetSelected(true);
	}

	if(x_ < x) return false;
	if(y_ < y) return false;
	if(x_ >= x + width) return false;
	if(y_ >= y + height) return false;

	return true;
}

bool GWLogin::UnClick(int x_, int y_)
{
	int x_local, y_local;

	gflags.Clear();

	x_local = x_ - x;
	y_local = y_ - y;

	if(login_button.UnClick(x_local, y_local)) DoLogin();
	if(create_button.UnClick(x_local, y_local)) DoCreate();

	if(x_ < x) return false;
	if(y_ < y) return false;
	if(x_ >= x + width) return false;
	if(y_ >= y + height) return false;

	return true;
}

bool GWLogin::KeyPress(int c)
{
	gflags.Clear();

	if(c == 9) //tab
	{
		if(loginname_box.IsSelected())
		{
			loginname_box.SetSelected(false);
			loginpass_box.SetSelected(true);
		}
		else
		{
			loginname_box.SetSelected(true);
			loginpass_box.SetSelected(false);
		}

		return true;
	}
	else if(c == 13) //enter
	{
		DoLogin();

		return true;
	}

	if(loginname_box.IsSelected()) loginname_box.KeyPress(c);
	else if(loginpass_box.IsSelected()) loginpass_box.KeyPress(c);
	
	return true;
}

void GWLogin::DoLogin()
{
	gflags.do_login = true;
	gflags.login_name = loginname_box.GetText();
	gflags.login_password = loginpass_box.GetText();
}

void GWLogin::DoCreate()
{
	gflags.open_createuser = true;
}
