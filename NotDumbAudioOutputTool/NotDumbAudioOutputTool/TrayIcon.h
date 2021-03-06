#pragma once

#include "PolicyConfig.h"

#define WM_MYMESSAGE (WM_USER + 1)
#define MAX_LOADSTRING 100
#define IDS_APP_TITLE 103
#define IDI_TRAYICON 107
#define IDI_SMALL 108
#define IDC_TRAYICON 109

class TrayIcon
{
	public:
		NOTIFYICONDATA nid;
		int RunApp(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
		ATOM MyRegisterClass(HINSTANCE hInstance);
		BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
};

