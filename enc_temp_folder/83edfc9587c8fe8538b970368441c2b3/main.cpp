#include <iostream>
#include "Source/Window/Window.h"

#if defined(WIN32)
#include <Windows.h>
#include "resource.h"

#define IDI_PICKERLOGO "picker.ico"

HWND* hwnd;

LRESULT CALLBACK Hookproc(int code, WPARAM wParam, LPARAM lParam)
{
    PKBDLLHOOKSTRUCT k = (PKBDLLHOOKSTRUCT)(lParam);

    if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN)
    {
        POINT p;

        RECT rect = { };
        GetWindowRect(*hwnd, &rect);

        GetCursorPos(&p);
        if (p.x >= rect.right || p.x <= rect.left ||
            p.y >= rect.bottom || p.y <= rect.top) 
        {
            ShowWindow(*hwnd, SW_HIDE);
        }
        
    }

    return CallNextHookEx(NULL, code, wParam, lParam);
}

int WINAPI WinMain(
    HINSTANCE   hInstance,
    HINSTANCE   hPrevInstance,
    LPSTR       lpCmdLine,
    int         nCmdShow
)
{
	const wchar_t CLASS_NAME[] = L"ColorPickerClass";

    HICON hIcon = static_cast<HICON>(LoadIcon(GetModuleHandle(NULL),
                                            MAKEINTRESOURCE(IDI_ICON1)));

    if (hIcon == NULL)
    {
        return 0;
    }

    Window MainWindow(hIcon, hInstance, CLASS_NAME);
    MainWindow.StartWindow();
    hwnd = MainWindow.GetHWND();

    HHOOK GlobalClickHook;
    GlobalClickHook = SetWindowsHookEx(WH_MOUSE_LL, Hookproc, hInstance, 0);

    auto ret = MainWindow.LoopWindow();
    UnhookWindowsHookEx(GlobalClickHook);
    return ret;
}

#else

int main() 
{

}
#endif
