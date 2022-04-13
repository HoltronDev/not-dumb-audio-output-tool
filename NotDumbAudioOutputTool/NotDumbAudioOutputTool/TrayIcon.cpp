#include "TrayIcon.h"

HINSTANCE hInst;
NOTIFYICONDATA nid;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void CreateIcon(HWND hWnd);
void DeleteIcon(HWND hWnd);
void ModifyIcon(HWND hWnd);

int TrayIcon::RunApp(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,	int nCmdShow)
{
	wcscpy_s(szTitle, L"Something");
	wcscpy_s(szWindowClass, L"Some Window Class");

	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(100));

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

void CreateIcon(HWND hWnd) 
{
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = 100;
	nid.uVersion = NOTIFYICON_VERSION;
	nid.uCallbackMessage = WM_MYMESSAGE;
	nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	wcscpy_s(nid.szTip, L"Audio Outputs");
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

	auto success = !Shell_NotifyIcon(NIM_ADD, &nid);
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

BOOL TrayIcon::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	//ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	CreateIcon(hWnd);

	return TRUE;
}

ATOM TrayIcon::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRAYICON));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TRAYICON);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		CreateIcon(hWnd);
		break;
	case WM_MYMESSAGE:
		switch (lParam)
		{
		case WM_LBUTTONDBLCLK:
			MessageBox(NULL, L"Tray icon double clicked!", L"clicked", MB_OK);
			break;
		default: return DefWindowProc(hWnd, msg, wParam, lParam);
		};
		break;
	case WM_DESTROY:
		DeleteIcon(hWnd);
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	};
	return 0;
}