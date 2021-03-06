#include "Slider.h"

#include "../../../../resource.h"

bool ColSlider::ClassRegistered = false;

Gdiplus::Bitmap* HueInsides = nullptr;
Gdiplus::Bitmap* AlphaInsides = nullptr;

LRESULT CALLBACK ColSliderProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_LBUTTONDOWN:

        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
        break;
    }
}

int ColSlider::Register(HINSTANCE hInstance, HWND hOwner, int x, int y, int cx, int cy)
{
    ColElement::Register(hInstance, hOwner, x, y, cx, cy);

    if (!ColSlider::ClassRegistered)
    {
        WNDCLASS Class = {};
        Class.lpfnWndProc = ColSliderProc;
        Class.hInstance = hInstance;
        Class.hCursor = LoadCursor(NULL, IDC_CROSS);
        Class.lpszClassName = EL_SLIDER;

        RegisterClass(&Class);
        ColSlider::ClassRegistered = true;
    }

    handle = CreateWindow(EL_SLIDER, L"", WS_VISIBLE | WS_CHILD, x, y, cx, cy, hOwner, 0, hInstance, 0);

    HandleBitmap = Gdiplus::Bitmap::FromHBITMAP(reinterpret_cast<HBITMAP>(LoadImage(hInstance,
        MAKEINTRESOURCE(IDB_BITMAP4), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION)), NULL);

    return 1;
}

void ColSlider::SetValue(int value)
{
    Value = value;
    if (OnChange)
        OnChange(value);
}

int ColSlider::GetValue()
{
    return Value;
}

void ColSlider::SetMaxValue(int value)
{
    MaxValue = value;
}

void ColSlider::OnClick(WPARAM e, int x, int y)
{
    POINT cur;
    RECT win;
    POINT WinCur;
    POINT EditCur;

    GetCursorPos(&cur);

    GetWindowRect(GetOwner(), &win);

    WinCur.x = cur.x - win.left;
    WinCur.y = cur.y - win.top;

    EditCur.x = WinCur.x - GetRectOnCanvas().X;
    EditCur.y = WinCur.y - GetRectOnCanvas().Y;

    switch (e)
    {
    case WM_LBUTTONDOWN:
        if (EditCur.x >= 0 && EditCur.x <= GetRectOnCanvas().Width
            && EditCur.y >= 0 && EditCur.y <= GetRectOnCanvas().Height)
        {
            TimerThread = new std::thread(&ColSlider::ThreadTimer, this);
            tickTimer = true;
        }
        else
        {
            tickTimer = false;
        }
        break;
    case WM_LBUTTONUP:
        tickTimer = false;
        break;
    }
}

void ColSlider::ThreadTimer()
{
    using namespace std;
    using namespace std::chrono;
timer:
    if (tickTimer)
    {
        POINT cur;
        RECT win;
        POINT WinCur;
        POINT EditCur;

        GetCursorPos(&cur);

        GetWindowRect(GetOwner(), &win);

        WinCur.x = cur.x - win.left;
        WinCur.y = cur.y - win.top;

        EditCur.x = WinCur.x - GetRectOnCanvas().X;
        EditCur.y = WinCur.y - GetRectOnCanvas().Y;

        if (EditCur.x >= 0 && EditCur.x <= GetRectOnCanvas().Width
            && EditCur.y >= 0 && EditCur.y <= GetRectOnCanvas().Height)
        {
            int res = (int)((float)((float)EditCur.x / (float)GetRectOnCanvas().Width) * MaxValue);
            SetValue(res);
        }
        else
        {
            int _x;

            _x = (EditCur.x < 0) ? 0 : MaxValue;

            SetValue(_x);
        }

        this_thread::sleep_for(10ms);
        goto timer;
    }
}

int ColSlider::Paint(HDC* hdc, Gdiplus::Graphics* graphics)
{
    graphics->FillRectangle(new Gdiplus::SolidBrush(Gdiplus::Color(254, 0, 0, 0)), GetRectOnCanvas());

    // 1 pixel padding 
    Gdiplus::Rect insides = { GetRectOnCanvas().X + 1, GetRectOnCanvas().Y + 1, GetRectOnCanvas().Width - 2, GetRectOnCanvas().Height - 2 };
    if (!isAlpha)
    {
        ColSliderPaintInsidesHue(insides, graphics);
    }
    else
    {
        ColSliderPaintInsidesAlpha(insides, graphics);
    }

    // Draw selector
    Gdiplus::ImageAttributes imAtt;
    imAtt.SetColorKey(Gdiplus::Color(0, 0, 0), Gdiplus::Color(200, 200, 200),
        Gdiplus::ColorAdjustTypeBitmap);

    int HeightFactor = (GetRectOnCanvas().Height + 3);
    int HandleX = (int)(((float)Value / (float)MaxValue) * (float)GetRectOnCanvas().Width);
    Gdiplus::Rect HandlePosition = {};
    HandlePosition.X = HandleX + GetRectOnCanvas().X - HeightFactor / 2;
    HandlePosition.X += HeightFactor/2 - 3;
    HandlePosition.Y = GetRectOnCanvas().Y - (HeightFactor/6)+2;
    HandlePosition.Width = HandlePosition.Height = HeightFactor;
    HandlePosition.Width /= 3;

    graphics->FillRectangle(new Gdiplus::SolidBrush(Gdiplus::Color(200, 120, 120, 120)), HandlePosition);

    /*auto res = graphics->DrawImage(HandleBitmap, HandlePosition, 0, 0, HandleBitmap->GetWidth(),
        HandleBitmap->GetHeight(), Gdiplus::Unit::UnitPixel, &imAtt);
    if (res != Gdiplus::Status::Ok)
    {
        return 0;
    }*/

    return 1;
}

int ColSliderPaintInsidesHue(Gdiplus::Rect insides, Gdiplus::Graphics* graphics)
{
    if (HueInsides == nullptr)
    {
        HueInsides = Gdiplus::Bitmap::FromHBITMAP(reinterpret_cast<HBITMAP>(LoadImage(GetModuleHandle(NULL),
            MAKEINTRESOURCE(IDB_BITMAP5), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION)), NULL);
    }

    graphics->DrawImage(HueInsides, insides);
    return 1;
}

int ColSliderPaintInsidesAlpha(Gdiplus::Rect insides, Gdiplus::Graphics* graphics)
{
    if (AlphaInsides == nullptr)
    {
        AlphaInsides = Gdiplus::Bitmap::FromHBITMAP(reinterpret_cast<HBITMAP>(LoadImage(GetModuleHandle(NULL),
            MAKEINTRESOURCE(IDB_BITMAP6), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION)), NULL);
    }

    graphics->DrawImage(AlphaInsides, insides);
    return 1;
}