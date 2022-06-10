#include "Window.h"

#include "../../resource.h"

NotifyIcon* Icon;

HBITMAP hArrowBmp, hBackgroundBmp;
BITMAP ArrowBmp, BackgroundBmp;
PAINTSTRUCT ps;
HDC hdc, hdcMem;
HINSTANCE hI;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        // Gather HINSTANCE
        hI = reinterpret_cast<HINSTANCE>(GetWindowLong(hwnd, GWL_HINSTANCE));
       
        // Gather resources' handles
        hArrowBmp = reinterpret_cast<HBITMAP>(::LoadImage(hI, MAKEINTRESOURCE(IDB_BITMAP2), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
        hBackgroundBmp = reinterpret_cast<HBITMAP>(::LoadImage(hI, MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));

        if (!hArrowBmp || !hBackgroundBmp)
        {
            if (Icon) Icon->RemoveIcon();
            exit(-1);
        }

        // Getting actual objects
        GetObject(hArrowBmp, sizeof(BITMAP), &ArrowBmp);
        GetObject(hBackgroundBmp, sizeof(BITMAP), &BackgroundBmp);

        break;
    case WM_CLOSE:
        ShowWindow(hwnd, SW_HIDE);
        return true;
        break;
    case WM_DESTROY:
        DeleteDC(hdcMem);
        DeleteObject(hArrowBmp);
        DeleteObject(hBackgroundBmp);

        if(Icon) Icon->RemoveIcon();
        exit(0);
        break;
    case 15666:
        if (lParam == WM_LBUTTONDOWN)
        {
            ShowWindow(hwnd, SW_SHOW);
            AlignWindowToNotify(hwnd);
            SetFocus(hwnd);
        }
        break;
    case WM_PAINT:
    {
        hdc = BeginPaint(hwnd, &ps);

        // Get rect
        RECT ClientRect;
        GetClientRect(hwnd, &ClientRect);

        // Draw background
        SetStretchBltMode(hdc, STRETCH_HALFTONE);
        StretchBlt(hdc, 0, 0, ClientRect.right, ClientRect.bottom,
            hdcMem, ClientRect.right / 2 - 36, 0, ArrowBmp.bmWidth, ArrowBmp.bmHeight, SRCCOPY);
        StretchBlt(hdc, 0, 0, ClientRect.right, ClientRect.bottom,
            hdcMem, 0, 0, BackgroundBmp.bmWidth, BackgroundBmp.bmHeight, SRCCOPY);

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
    }
    return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
        break;
    }
}

Window::Window(HICON Icon, HINSTANCE hInst, const wchar_t* ClassName)
	:hIcon(Icon), hInstance(hInst), CLASS_NAME(ClassName)
{
    winClass = {};
    winClass.lpfnWndProc = WindowProc;
    winClass.hInstance = hInstance;
    winClass.hIcon = hIcon;
    winClass.lpszClassName = CLASS_NAME;

    RegisterClass(&winClass);

    float Scale = 0.4f;

    hWnd = CreateWindowEx(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED,            // Window style
        CLASS_NAME,                     // Window class
        L"Color Picker",    // Window text
        WS_POPUP,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 700 * Scale, 516 * Scale,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hWnd == NULL)
    {
        exit(-1);
    }
}

Window::~Window()
{
    if (Icon)
    {
        Icon->RemoveIcon();
        delete Icon;
        Icon = nullptr;
    }
}

bool Window::StartWindow()
{
    ShowWindow(hWnd, SW_SHOW);

    Icon = new NotifyIcon(&hWnd, &hIcon);
    
    Icon->AddIcon();

    return true;
}

int Window::LoopWindow()
{
    MSG msg;
    while (GetMessage(&msg, hWnd, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

void AlignWindowToNotify(HWND _hwnd)
{
    POINT screen;
    screen.x = GetSystemMetrics(SM_CXSCREEN);
    screen.y = GetSystemMetrics(SM_CYSCREEN);

    RECT win = { };
    GetWindowRect(_hwnd, &win);
    POINT winSize = { win.right - win.left, win.bottom - win.top };

    RECT notify = Icon->GetRect();
    POINT notifySize = { notify.right - notify.left, notify.bottom - notify.top };
    int notifyCenter = (notify.left + notify.right)/2;

    RECT WinResult = {};
    WinResult.left = (notifyCenter - winSize.x/2);
    WinResult.top = (notify.top - winSize.y);
    WinResult.right = WinResult.left + winSize.x;
    WinResult.bottom = WinResult.top + winSize.y;
    
    MoveWindow(_hwnd, WinResult.left, WinResult.top, winSize.x, winSize.y, true);
}
