#include <iostream>
#include "Source/Window/Window.h"

#if defined(WIN32)
#include <Windows.h>
#include "resource.h"

#define IDI_PICKERLOGO "picker.ico"

HWND* hwnd;
Window* MainWindow = nullptr;

LRESULT CALLBACK Hookproc(int code, WPARAM wParam, LPARAM lParam)
{
    PKBDLLHOOKSTRUCT k = (PKBDLLHOOKSTRUCT)(lParam);

    POINT p;
    GetCursorPos(&p);

    if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN)
    {
        RECT rect = { };
        GetWindowRect(*hwnd, &rect);

        if (p.x >= rect.right || p.x <= rect.left ||
            p.y >= rect.bottom || p.y <= rect.top) 
        {
            ShowWindow(*hwnd, SW_HIDE);
        }
    }

    MainWindow->SendToClickListeners(wParam, p.x, p.y);

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

    HICON hIcon = static_cast<HICON>(LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)));

    if (hIcon == NULL)
    {
        return 0;
    }

    MainWindow = new Window(hIcon, hInstance, CLASS_NAME);
    MainWindow->StartWindow();
    hwnd = MainWindow->GetHWND();

    HHOOK GlobalClickHook;
    GlobalClickHook = SetWindowsHookEx(WH_MOUSE_LL, Hookproc, hInstance, 0);

    auto ret = MainWindow->LoopWindow();
    UnhookWindowsHookEx(GlobalClickHook);
    delete MainWindow;
    MainWindow = nullptr;
    return ret;
}

#else

int main() 
{

}
#endif
