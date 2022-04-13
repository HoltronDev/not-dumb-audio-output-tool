#include "TrayIcon.h"

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const int IDM_EXIT = 200;
int cmd;

HINSTANCE hInst;
NOTIFYICONDATA nid;
HMENU audioOutputsMenu;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void CreateIcon(HWND hWnd);
void DeleteIcon(HWND hWnd);
void ModifyIcon(HWND hWnd);
void HandleOutputsMenu(HWND hWnd);
void HandleOutputsSelection(WPARAM wParam, LPARAM lParam);
void GetAudioEndpoints(std::vector<std::wstring> names);

int TrayIcon::RunApp(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,	int nCmdShow)
{
	wcscpy_s(szTitle, L"Something");
	wcscpy_s(szWindowClass, L"Some Window Class");

	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	CoInitialize(nullptr);

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

	CoUninitialize();

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
		case WM_LBUTTONUP:
			HandleOutputsMenu(hWnd);
			break;
		case WM_RBUTTONUP:
			ShowWindow(hWnd, TRUE);
			break;
		default: 
			return DefWindowProc(hWnd, msg, wParam, lParam);
		};
		break;
	case WM_COMMAND:
		HandleOutputsSelection(wParam, lParam);
	case WM_DESTROY:
		DeleteIcon(hWnd);
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	};
	return 0;
}

void HandleOutputsMenu(HWND hWnd)
{
	POINT pt;
	GetCursorPos(&pt);

	audioOutputsMenu = CreatePopupMenu();

	std::vector<std::wstring> deviceNames;

	GetAudioEndpoints(deviceNames);

	for (int i = 0; i < deviceNames.size(); i++)
	{
		InsertMenu(audioOutputsMenu, 0, MF_BYPOSITION | MF_STRING, IDM_EXIT + (i + 1), deviceNames.at(i).c_str());
	}
	InsertMenu(audioOutputsMenu, deviceNames.size() + 1, MF_BYPOSITION | MF_STRING, IDM_EXIT, L"Exit");

	SetForegroundWindow(hWnd);

	cmd = TrackPopupMenu(audioOutputsMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, pt.x, pt.y, 0, hWnd, NULL);

	PostMessage(hWnd, WM_NULL, 0, 0);
}

void HandleOutputsSelection(WPARAM wParam, LPARAM lParam)
{
	if (lParam != 0) 
	{
		return;
	}

	switch (LOWORD(wParam))
	{
	case IDM_EXIT:
		MessageBox(NULL, L"Exit clicked.", L"clicked", MB_OK);
		break;
	}
}

void GetAudioEndpoints(std::vector<std::wstring> names) 
{
	HRESULT hr = S_OK;
	IMMDeviceEnumerator* pEnumerator = NULL;
	IMMDeviceCollection* pCollection = NULL;
	IMMDevice* pEndpoint = NULL;
	IPropertyStore* pProps = NULL;
	LPWSTR pwszID = NULL;

	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator);

		hr = pEnumerator->EnumAudioEndpoints(
			eRender, DEVICE_STATE_ACTIVE,
			&pCollection);

		UINT  count;
	hr = pCollection->GetCount(&count);

		if (count == 0)
		{
			return;
		}

	// Each loop prints the name of an endpoint device.
	for (ULONG i = 0; i < count; i++)
	{
		// Get pointer to endpoint number i.
		hr = pCollection->Item(i, &pEndpoint);

			// Get the endpoint ID string.
			hr = pEndpoint->GetId(&pwszID);

			hr = pEndpoint->OpenPropertyStore(
				STGM_READ, &pProps);

			PROPVARIANT varName;
		// Initialize container for property value.
		PropVariantInit(&varName);

		// Get the endpoint's friendly-name property.
		hr = pProps->GetValue(
			PKEY_Device_FriendlyName, &varName);

		names.emplace_back(varName.pwszVal);

		CoTaskMemFree(pwszID);
		pwszID = NULL;
		PropVariantClear(&varName);
		SAFE_RELEASE(pProps)
		SAFE_RELEASE(pEndpoint)
	}
	SAFE_RELEASE(pEnumerator)
	SAFE_RELEASE(pCollection)
	return;
}