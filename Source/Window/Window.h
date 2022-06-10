#include "../NotificationInterface/Notify.h"

#if defined(WIN32) || defined(__MINGW32__)
#include <Windows.h>
#include <assert.h>
#include <objidl.h>
#include <atlimage.h>
#include <gdiplus.h>

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
	
public:
	Window(HICON Icon, HINSTANCE hInst, const wchar_t* ClassName);
	~Window();

	HWND* GetHWND() {return &hWnd;}

	static void PopulateClientWithWindows(HWND hwnd);

	bool StartWindow();
	int LoopWindow();
};

#elif defined(__APPLE__)

#elif defined(__linux__)

#endif