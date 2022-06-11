#include "Window.h"

#include "../../resource.h"
#include "Elements/basedef.h"

NotifyIcon* Icon;

HBITMAP hArrowBmp, hBackgroundBmp;
Gdiplus::Bitmap *ArrowBmp, *BackgroundBmp;
HPALETTE Palette;
PAINTSTRUCT ps;
HDC hdc, hdcMem;
HINSTANCE hI;

//GDI+
ULONG_PTR WinGDIToken;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static BITMAPINFOHEADER* pbmi = NULL;
    static BYTE* pBuffer = NULL;

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
        ArrowBmp = Gdiplus::Bitmap::FromHBITMAP(hArrowBmp, NULL);
        BackgroundBmp = Gdiplus::Bitmap::FromHBITMAP(hBackgroundBmp, NULL);

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
        else if (lParam == WM_RBUTTONDOWN)
        {
            HMENU hMenu = CreatePopupMenu();

            if (hMenu) {
                InsertMenu(hMenu, -1, MF_BYPOSITION, 15668, L"About");
                InsertMenu(hMenu, -1, MF_BYPOSITION | MF_CALLBACKS | MF_BYCOMMAND, 15667, L"Exit");

                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hwnd);
                TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
                PostMessage(hwnd, WM_NULL, 0, 0);
                DestroyMenu(hMenu);
            }
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
            case 15667:
                DeleteDC(hdcMem);
                DeleteObject(hArrowBmp);
                DeleteObject(hBackgroundBmp);

                if (Icon) Icon->RemoveIcon();
                exit(0);
                break;
            case 15668:
                MessageBox(hwnd, L"Created and managed by Sasisa\nGithub: SasisaDev/ColorPicker", L"About", MB_OK | MB_ICONINFORMATION);
                break;

        }
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
        break;
    }
}

Window::Window(HICON Icon, HINSTANCE hInst, const wchar_t* ClassName)
	:hIcon(Icon), hInstance(hInst), CLASS_NAME(ClassName)
{
    RegisterAllElements(hInst);

    winClass = {};
    winClass.lpfnWndProc = WindowProc;
    winClass.hInstance = hInstance;
    winClass.hIcon = hIcon;
    winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    winClass.lpszClassName = CLASS_NAME;
    winClass.hbrBackground = (HBRUSH)0;

    winClass.lpszMenuName = NULL;
    winClass.cbClsExtra = 0;
    winClass.cbWndExtra = 0;

    RegisterClass(&winClass);

    Gdiplus::GdiplusStartupInput gdiinput = {};
    gdiinput.GdiplusVersion = 1;

    Gdiplus::GdiplusStartupOutput gdioutput = {};
    Gdiplus::GdiplusStartup(&WinGDIToken, &gdiinput, &gdioutput);

    /*hHostWnd = CreateWindow(CLASS_NAME, NULL, WS_POPUP,
        0, 0, 0, 0, NULL, NULL, hInstance, NULL);*/

    hWnd = CreateWindowEx(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED,            // Window style
        CLASS_NAME,                     // Window class
        L"Color Picker",    // Window text
        WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 700 * Scale, 516 * Scale,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    Window::PopulateClientWithWindows(hWnd);

    //SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
    //SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);

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

    Gdiplus::GdiplusShutdown(WinGDIToken);
}

bool Window::StartWindow()
{
    ShowWindow(hWnd, SW_HIDE);

    Icon = new NotifyIcon(&hWnd, &hIcon);
    
    Icon->AddIcon();

    RefreshWindow();

    UpdateWindow(hWnd);

    return true;
}

int Window::LoopWindow()
{
    MSG msg = {};
    ZeroMemory(&msg, sizeof(MSG));
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

void Window::RefreshWindow()
{
    OnPaint(hWnd);
    AlignWindowToNotify(hWnd);
}

void Window::OnPaint(HWND hwnd)
{
    RECT wndRect;
    ::GetWindowRect(hwnd, &wndRect);
    SIZE wndSize = { wndRect.right - wndRect.left,wndRect.bottom - wndRect.top };
    HDC hdc = ::GetDC(hwnd);
    HDC memDC = ::CreateCompatibleDC(hdc);
    HBITMAP memBitmap = ::CreateCompatibleBitmap(hdc, wndSize.cx, wndSize.cy);
    ::SelectObject(memDC, memBitmap);
    ::SetBkMode(memDC, TRANSPARENT);

    Gdiplus::Graphics graphics(memDC);

    graphics.Clear(Gdiplus::Color(0, 0, 0, 0));

    HDC screenDC = GetDC(NULL);
    POINT ptSrc = { 0,0 };

    BLENDFUNCTION blendFunction;
    blendFunction.AlphaFormat = AC_SRC_ALPHA;
    blendFunction.BlendFlags = 0;
    blendFunction.BlendOp = AC_SRC_OVER;
    blendFunction.SourceConstantAlpha = 255;

    //::AlphaBlend(screenDC, 0, 0, wndSize.cx, wndSize.cy, memDC, 0, 0, wndSize.cx, wndSize.cy, blendFunction);

    Gdiplus::ImageAttributes imAtt;
    imAtt.SetColorKey(Gdiplus::Color(0, 0, 0), Gdiplus::Color(128, 128, 128), Gdiplus::ColorAdjustTypeBitmap);

    //graphics.DrawImage(ArrowBmp, (INT)(((700) / 2 - (73) / 2) * Scale), (INT)((516 - 35) * Scale), (INT)(73 * Scale), (INT)(35 * Scale));
    //graphics.DrawImage(BackgroundBmp, 0, 0, (INT)(700 * Scale), (INT)((516 - 35) * Scale));

    Gdiplus::Rect arrowdest = { (INT)(((700) / 2 - (73) / 2) * Scale), (INT)((516 - 35) * Scale), (INT)(73 * Scale), (INT)(35 * Scale) };
    graphics.DrawImage(ArrowBmp, arrowdest, (INT)0, (INT)0, (INT)73, (INT)35, Gdiplus::Unit::UnitPixel, &imAtt);

    Gdiplus::Rect bodydest = { 0, 0, (INT)(700 * Scale), (INT)((516 - 35) * Scale) };
    graphics.DrawImage(BackgroundBmp, bodydest, (INT)0, (INT)0, (INT)700, (INT)516-35, Gdiplus::Unit::UnitPixel, &imAtt);

    // Render all elements
    for (ColElement* element : elements)
    {
        if (element->Paint(&memDC, &graphics) == 0)
        {
            printf("");
        }
    }

    if (::UpdateLayeredWindow(hwnd, screenDC, &ptSrc, &wndSize, memDC, &ptSrc, 0, &blendFunction, ULW_ALPHA) == 0)
    {
        exit(-1);
    }

    hdc = BeginPaint(hwnd, &ps);

    EndPaint(hwnd, &ps);

    ::DeleteDC(memDC);
    ::DeleteObject(memBitmap);
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


void Window::PopulateClientWithWindows(HWND hwnd)
{
    int MarginLR = 15;
    int MarginTB = 15;

    int InterEditboxMargin = 6;

    // Main colors RGBA
    ColEditBox* RGBEditBox = new ColEditBox();
    RGBEditBox->Register(hI, hwnd, (700 * Scale) - MarginLR - 72, MarginTB + InterEditboxMargin * 0, 72, 18);
    RGBEditBox->SetText(L"Test");
    elements.push_back(RGBEditBox);

    ColEditBox* HEXEditBox = new ColEditBox();
    HEXEditBox->Register(hI, hwnd, (700 * Scale) - MarginLR - 72, MarginTB * 2 + InterEditboxMargin * 1, 72, 18);
    elements.push_back(HEXEditBox);

    ColEditBox* CMYKEditBox = new ColEditBox();
    CMYKEditBox->Register(hI, hwnd, (700 * Scale) - MarginLR - 72, MarginTB * 3 + InterEditboxMargin * 2, 72, 18);
    elements.push_back(CMYKEditBox);

    ColEditBox* PMSEditBox = new ColEditBox();
    PMSEditBox->Register(hI, hwnd, (700 * Scale) - MarginLR - 72, MarginTB * 4 + InterEditboxMargin * 3, 72, 18);
    elements.push_back(PMSEditBox);

    // Alpha in 10 and 16
    ColEditBox* Alpha10EditBox = new ColEditBox();
    if(Alpha10EditBox->Register(hI, hwnd, (700 * Scale) - MarginLR - 72, MarginTB * 5 + InterEditboxMargin * 4, 72 / 2 - 1, 18)) elements.push_back(Alpha10EditBox);

    ColEditBox* Alpha16EditBox = new ColEditBox();
    if(Alpha16EditBox->Register(hI, hwnd, (700 * Scale) - MarginLR - 72 + 72/2 + 2, MarginTB * 5 + InterEditboxMargin * 4, 72 / 2 - 1, 18)) elements.push_back(Alpha16EditBox);
}