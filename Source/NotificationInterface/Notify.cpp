#include "Notify.h"

#include <string>
#if defined(WIN32) || defined(__MINGW32__)

NotifyIcon::NotifyIcon(HWND *hwnd, HICON *hIcon)
	:winHandler(hwnd), Icon(hIcon)
{
	Data.hWnd = *hwnd;
	if(hIcon) Data.hIcon = *hIcon;
	CoCreateGuid(&Data.guidItem);
	Data.cbSize = sizeof(Data);
	Data.uID = 15;
	Data.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	Data.uCallbackMessage = 15666;
	std::string title = "Picker";
	strncpy_s(Data.szInfoTitle, title.c_str(), 64);
	strncpy_s(Data.szTip, title.c_str(), 64);
}

bool NotifyIcon::AddIcon()
{
	return Shell_NotifyIconA(NIM_ADD, &Data);
}

bool NotifyIcon::RemoveIcon()
{

	return Shell_NotifyIconA(NIM_DELETE, &Data);
}

RECT NotifyIcon::GetRect()
{
	RECT rect;

	NOTIFYICONIDENTIFIER ident = {};
	ident.cbSize = sizeof(ident);
	//ident.guidItem = Data.guidItem;
	ident.hWnd = Data.hWnd;
	ident.uID = Data.uID;

	HRESULT res = Shell_NotifyIconGetRect(&ident, &rect);
	if (res != S_OK)
	{
		printf("");
	}

	return rect;
}

#elif defined(__APPLE__)

#elif defined(__linux__)

#endif