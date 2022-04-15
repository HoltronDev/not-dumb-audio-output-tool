#include "TrayIcon.h"

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != nullptr)  \
                { (punk)->Release(); (punk) = nullptr; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const int IDM_EXIT = 199;
int cmd;

HINSTANCE hInst;
NOTIFYICONDATA nid;
HMENU audioOutputsMenu;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void CreateTrayIcon(HWND hWnd);
void RemoveTrayIcon(HWND hWnd);
void ModifyTrayIcon(HWND hWnd);
void HandleOutputsMenu(HWND hWnd);
void HandleOutputsSelection(HWND hWnd, WPARAM wParam, LPARAM lParam);
std::vector<std::wstring> GetAudioEndpoints(int* activeDevice);
void SetDefaultAudioPlaybackDevice(int device);

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

void CreateTrayIcon(HWND hWnd) 
{
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = 100;
	nid.uVersion = NOTIFYICON_VERSION;
	nid.uCallbackMessage = WM_MYMESSAGE;
	nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	wcscpy_s(nid.szTip, L"Audio Outputs");
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

	Shell_NotifyIcon(NIM_ADD, &nid);
}

void RemoveTrayIcon(HWND hWnd)
{
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = 100;
	Shell_NotifyIcon(NIM_DELETE, &nid);
}

void ModifyTrayIcon(HWND hWnd)
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

	UpdateWindow(hWnd);

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
		CreateTrayIcon(hWnd);
		break;
	case WM_MYMESSAGE:
		switch (lParam)
		{
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			HandleOutputsMenu(hWnd);
			break;
		default: 
			return DefWindowProc(hWnd, msg, wParam, lParam);
		};
		break;
	case WM_COMMAND:
		HandleOutputsSelection(hWnd, wParam, lParam);
		break;
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

	int activeDevice;
	auto deviceNames = GetAudioEndpoints(&activeDevice);

	for (auto i = 0; i < deviceNames.size(); i++)
	{
		if (i == activeDevice)
		{
			InsertMenu(audioOutputsMenu, 0, MF_BYPOSITION | MF_STRING | MF_CHECKED, IDM_EXIT + (i + 1), deviceNames.at(i).c_str());
		}
		else
		{
			InsertMenu(audioOutputsMenu, 0, MF_BYPOSITION | MF_STRING, IDM_EXIT + (i + 1), deviceNames.at(i).c_str());
		}
	}
	InsertMenu(audioOutputsMenu, deviceNames.size() + 1, MF_BYPOSITION | MF_STRING, IDM_EXIT, L"Exit");

	SetForegroundWindow(hWnd);

	cmd = TrackPopupMenu(audioOutputsMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, pt.x, pt.y, 0, hWnd, NULL);

	PostMessage(hWnd, WM_NULL, 0, 0);
}

void HandleOutputsSelection(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (lParam != 0) 
	{
		return;
	}

	switch (LOWORD(wParam))
	{
	case IDM_EXIT:
		RemoveTrayIcon(hWnd);
		PostMessage(hWnd, WM_QUIT, 0, 0);
		break;
	default:
		if (wParam - 200 < 0) break;
		SetDefaultAudioPlaybackDevice(wParam - 200);
		break;
	}
}

std::vector<std::wstring> GetAudioEndpoints(int* activeDevice)
{
	HRESULT hr = S_OK;
	IMMDeviceEnumerator* pEnumerator = nullptr;
	IMMDeviceCollection* pCollection = nullptr;
	IMMDevice* pEndpoint = nullptr;
	IPropertyStore* pProps = nullptr;
	DWORD pdwState = NULL;
	LPWSTR pwszID = nullptr;
	LPWSTR defaultDeviceId = nullptr;
	std::vector<std::wstring> names;
	std::wstring defaultDeviceIdReadable;

	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, nullptr,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator);
	EXIT_ON_ERROR(hr)

	hr = pEnumerator->EnumAudioEndpoints(
		eRender, DEVICE_STATE_ACTIVE,
		&pCollection);
	EXIT_ON_ERROR(hr)

	UINT  count;
	hr = pCollection->GetCount(&count);
	EXIT_ON_ERROR(hr)

	if (count == 0)
	{
		return names;
	}

	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pEndpoint);
	EXIT_ON_ERROR(hr)

	hr = pEndpoint->GetId(&defaultDeviceId);
	EXIT_ON_ERROR(hr)

	defaultDeviceIdReadable = std::wstring(defaultDeviceId);

	// Each loop prints the name of an endpoint device.
	for (ULONG i = 0; i < count; i++)
	{
		// Get pointer to endpoint number i.
		hr = pCollection->Item(i, &pEndpoint);
		EXIT_ON_ERROR(hr)

		// Get the endpoint ID string.
		hr = pEndpoint->GetId(&pwszID);
		EXIT_ON_ERROR(hr)

		auto currentAudioDeviceId = std::wstring(pwszID);

		if (defaultDeviceIdReadable == currentAudioDeviceId)
		{
			*activeDevice = static_cast<int>(i);
		}

		hr = pEndpoint->OpenPropertyStore(
			STGM_READ, &pProps);
		EXIT_ON_ERROR(hr)

		PROPVARIANT varName;
		// Initialize container for property value.
		PropVariantInit(&varName);

		// Get the endpoint's friendly-name property.
		hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
		EXIT_ON_ERROR(hr)

		names.emplace_back(varName.pwszVal);

		CoTaskMemFree(pwszID);
		pwszID = nullptr;
		PropVariantClear(&varName);
		SAFE_RELEASE(pProps)
		SAFE_RELEASE(pEndpoint)
	}
	SAFE_RELEASE(pEnumerator)
	SAFE_RELEASE(pCollection)
	return names;

Exit:
	MessageBox(nullptr, L"Something went wrong trying to get your audio devices!.", L"Error!", MB_OK);
	CoTaskMemFree(pwszID);
	SAFE_RELEASE(pEnumerator)
	SAFE_RELEASE(pCollection)
	SAFE_RELEASE(pEndpoint)
	SAFE_RELEASE(pProps)
	return names;
}

void SetDefaultAudioPlaybackDevice(int device)
{
	HRESULT hr = S_OK;
	IMMDeviceEnumerator* pEnumerator = nullptr;
	IMMDeviceCollection* pCollection = nullptr;
	IMMDevice* pEndpoint = nullptr;
	LPWSTR devID;
	IPolicyConfigVista* pPolicyConfig;
	ERole reserved = eConsole;

	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, nullptr,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator);
	EXIT_ON_ERROR(hr)

	hr = pEnumerator->EnumAudioEndpoints(
		 eRender, DEVICE_STATE_ACTIVE,
		 &pCollection);
	EXIT_ON_ERROR(hr)

	UINT  count;
	hr = pCollection->GetCount(&count);
	EXIT_ON_ERROR(hr)

	if (device > count)
	{
		return;
	}

	hr = pCollection->Item(device, &pEndpoint);
	EXIT_ON_ERROR(hr)

	// Get the endpoint ID string.
	hr = pEndpoint->GetId(&devID);
	EXIT_ON_ERROR(hr)

	hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient),
		NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID*)&pPolicyConfig);
	EXIT_ON_ERROR(hr)
	
	hr = pPolicyConfig->SetDefaultEndpoint(devID, reserved);
	pPolicyConfig->Release();

	SAFE_RELEASE(pEnumerator)
	SAFE_RELEASE(pCollection)
	SAFE_RELEASE(pEndpoint)
	return;

Exit:
	MessageBox(nullptr, L"Something went wrong while setting your active audio device!.", L"Error!", MB_OK);
	SAFE_RELEASE(pEnumerator)
	SAFE_RELEASE(pCollection)
	SAFE_RELEASE(pEndpoint)
	return;
}