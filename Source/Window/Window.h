#include "../NotificationInterface/Notify.h"

#ifndef _WINDOW_H
#define _WINDOW_H

#if defined(WIN32) || defined(__MINGW32__)
#include <Windows.h>
#include <assert.h>
#include <objidl.h>
#include <atlimage.h>
#include <gdiplus.h>
#include <vector>

#include "Elements/Initializer.h"

#pragma comment (lib,"gdiplus.lib")

void AlignWindowToNotify(HWND _hwnd);

class Window
{
protected:
	HICON hIcon;
	WNDCLASS winClass;
	HWND hWnd;
	HWND hHostWnd;
	HINSTANCE hInstance;
	const wchar_t* CLASS_NAME;
	
	std::vector<ColElement*> elements;
public:
	Window(HICON Icon, HINSTANCE hInst, const wchar_t* ClassName);
	~Window();

	HWND* GetHWND() {return &hWnd;}

	void PopulateClientWithWindows(HWND hwnd);

	bool StartWindow();
	int LoopWindow();

	void RefreshWindow();

	void SendToClickListeners(WPARAM e, int x, int y);

	void OnPaint(HWND hwnd);
};

#elif defined(__APPLE__)

#elif defined(__linux__)

#endif

#endif