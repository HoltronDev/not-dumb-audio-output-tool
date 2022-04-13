#include "TrayIcon.h"

class TrayIcon 
{
	NOTIFYICONDATA nid = {};

	void CreateIcon(HWND hWnd) {
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hWnd;
		nid.uID = 100;
		nid.uVersion = NOTIFYICON_VERSION;
		nid.uCallbackMessage = WM_MYMESSAGE;
		nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wcscpy_s(nid.szTip, L"Tray Icon");
		nid.uFlags = NIF_MESSAGE & NIF_ICON & NIF_TIP;

		Shell_NotifyIcon(NIM_ADD, &nid);
	}

	void DeleteIcon(HWND hWnd)
	{
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hWnd;
		nid.uID = 100;
		Shell_NotifyIcon(NIM_DELETE, &nid);
	}

	void ModifyIcon(HWND hWnd)
	{
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hWnd;
		nid.uID = 100;
		wcscpy_s(nid.szTip, L"Modified Tip");
		nid.uFlags = NIF_TIP;
		Shell_NotifyIcon(NIM_MODIFY, &nid);
	}

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CREATE:
			break;
		case WM_MYMESSAGE:
			switch (lParam)
			{
			case WM_LBUTTONDBLCLK:
				MessageBox(NULL, L"Tray icon duble clicked!", L"clicked", MB_OK);
				break;
			default: return DefWindowProc(hWnd, msg, wParam, lParam);
			};
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		};
		return 0;
	}
};