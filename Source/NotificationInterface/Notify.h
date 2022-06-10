#if defined(WIN32) || defined(__MINGW32__)
#include <Windows.h>
#include <shellapi.h>
#include <assert.h>
#pragma comment(lib, "shell32.lib")

class NotifyIcon
{
protected:
	HWND* winHandler;
	NOTIFYICONDATAA Data;
	HICON* Icon;

public:
	NotifyIcon(HWND* hwnd, HICON* hIcon);

	bool AddIcon();
	bool RemoveIcon();

	RECT GetRect();
};

#elif defined(__APPLE__)

#elif defined(__linux__)

#endif