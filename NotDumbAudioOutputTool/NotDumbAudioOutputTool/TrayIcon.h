#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
//#include <tchar.h>

#define WM_MYMESSAGE (WM_USER + 1)

class TrayIcon
{
	public:
		NOTIFYICONDATA nid;
		void CreateIcon(HWND hWnd);
		void DeleteIcon(HWND hWnd);
		void ModifyIcon(HWND hWnd);
		LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		//int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow);
};

