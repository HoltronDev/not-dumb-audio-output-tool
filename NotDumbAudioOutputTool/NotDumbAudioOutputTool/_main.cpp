#include "platform.h"
#include "TrayIcon.h"

int APIENTRY WinMain(_In_ HINSTANCE hInstance, 
	                 _In_opt_ HINSTANCE hPrevInstance, 
	                 _In_ LPSTR lpCmdLine, 
	                 _In_ int nCmdShow)
{
	auto inst = TrayIcon();
	
	auto result = inst.RunApp(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	return result;
}