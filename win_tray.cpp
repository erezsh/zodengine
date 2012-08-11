/* Smith's Tray Icon Example
** Creates an icon in the tray, left click on it for a message,
** right click to close
*/ 


//#include <windows.h>
#include "ztray.h"

#define WM_TRAYMESSAGE 101

void update_callback(ZTray *tray);

char szClassName[ ] = "TrayCallBack";
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
NOTIFYICONDATA empty, got_players;

int _stdcall WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR CmdLine,int nShowCmd)
{

	HWND hwnd;           
//    MSG messages;
	WNDCLASSEX wincl; 

	wincl.hInstance = hInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;           
    wincl.cbSize = sizeof (WNDCLASSEX);

	wincl.hIcon = NULL;
    wincl.hIconSm = NULL;
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
    
	if (!RegisterClassEx (&wincl))
        return 0;

	//Hidden window for tray callback
    hwnd = CreateWindowEx(WS_EX_LEFT|WS_EX_LTRREADING|WS_EX_WINDOWEDGE,
						  szClassName,
						  "TrayCallback",
						  WS_OVERLAPPED|WS_MINIMIZEBOX|WS_SYSMENU|WS_CLIPSIBLINGS|WS_CAPTION,
                          CW_USEDEFAULT,
						  CW_USEDEFAULT,
						  400,
						  430,
						  HWND_DESKTOP,
						  NULL,
                          hInstance,NULL);

	//nid.hIcon = LoadIcon(hInstance,IDI_WARNING);
	//nid.hIcon= (HICON)::LoadImage(hInstance,MAKEINTRESOURCE(IDI_INFORMATION),IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);

	//Tray Class Stuff
	empty.cbSize = sizeof(NOTIFYICONDATA);
	empty.hWnd = hwnd;
	empty.uID = 100;
	empty.uFlags = NIF_ICON|NIF_MESSAGE;
	empty.hIcon= (HICON)::LoadImage(0,"assets/tray_empty.ico",IMAGE_ICON,32,32,LR_LOADFROMFILE);
	empty.uCallbackMessage = WM_TRAYMESSAGE;

	//Load the tray icon up
	Shell_NotifyIcon(NIM_ADD,&empty);

	//Tray Class Stuff
	got_players.cbSize = sizeof(NOTIFYICONDATA);
	got_players.hWnd = hwnd;
	got_players.uID = 100;
	got_players.uFlags = NIF_ICON|NIF_MESSAGE;
	got_players.hIcon= (HICON)::LoadImage(0,"assets/tray_got_players.ico",IMAGE_ICON,32,32,LR_LOADFROMFILE);
	got_players.uCallbackMessage = WM_TRAYMESSAGE;

	//Load the tray icon up
	Shell_NotifyIcon(NIM_ADD,&got_players);

	//set to empty
	Shell_NotifyIcon(NIM_MODIFY, &empty);

	ZTray ztray;

	ztray.SetRemoteAddress("hestia.nighsoft.net");
	ztray.SetUpdateCallBack(update_callback);

	ztray.Setup();
	ztray.Run();

	return 1;

  //  while (GetMessage (&messages, NULL, 0, 0))
  //  {
		//static bool other = false;

		//if(other) 
		//	Shell_NotifyIcon(NIM_MODIFY, &nid);
		//else
		//	Shell_NotifyIcon(NIM_MODIFY, &nid2);
		//other = !other;

  //      TranslateMessage(&messages);
  //      DispatchMessage(&messages);
  //  }

  //  return messages.wParam;
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)             
    {
		case WM_TRAYMESSAGE:
		{
		switch(lParam)
		{
		case WM_LBUTTONUP:
			//MessageBox(hwnd,"You Clicked Me!","Smith's TrayIcon Example",MB_OK);
			break;

		case WM_RBUTTONUP:
			ExitProcess(0);
			break;
		}
		}

		default:                 
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

void update_callback(ZTray *tray)
{
	bool has_players = false;

	for(vector<p_info>::iterator i=tray->GetPlayerInfoList().begin(); i!=tray->GetPlayerInfoList().end(); i++)
		if(i->mode == PLAYER_MODE)
		{
			has_players = true;
			break;
		}

	if(has_players) 
		Shell_NotifyIcon(NIM_MODIFY, &got_players);
	else
		Shell_NotifyIcon(NIM_MODIFY, &empty);
}