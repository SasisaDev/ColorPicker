#include "Slider.h"

#include "../../../../resource.h"

bool ColSlider::ClassRegistered = false;

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
    if(OnChange) OnChange(value);
}

int ColSlider::GetValue()
{
    return 0;
}

void ColSlider::SetMaxValue(int value)
{
    MaxValue = value;
}

void ColSlider::OnClick(WPARAM e, int x, int y)
{
}

void ColSlider::ThreadTimer()
{
}

int ColSlider::Paint(HDC* hdc, Gdiplus::Graphics* graphics)
{
    graphics->FillRectangle(new Gdiplus::SolidBrush(Gdiplus::Color(254, 0, 0, 0)), GetRectOnCanvas());

    // 1 pixel padding 
    Gdiplus::Rect insides = { GetRectOnCanvas().X + 2, GetRectOnCanvas().Y + 1, GetRectOnCanvas().Width - 1, GetRectOnCanvas().Height - 1 };
    if(PaintInsidesProc) PaintInsidesProc(insides, graphics);

    // Draw selector
    Gdiplus::ImageAttributes imAtt;
    imAtt.SetColorKey(Gdiplus::Color(0, 0, 0), Gdiplus::Color(200, 200, 200),
        Gdiplus::ColorAdjustTypeBitmap);

    int HeightFactor = (GetRectOnCanvas().Height + 3);
    Gdiplus::Rect HandlePosition = {};
    HandlePosition.X = ((Value / MaxValue) * GetRectOnCanvas().Width) - HeightFactor/2 + GetRectOnCanvas().X;
    HandlePosition.Y = GetRectOnCanvas().Y - (HeightFactor/6)+2;
    HandlePosition.Width = HandlePosition.Height = HeightFactor;

    auto res = graphics->DrawImage(HandleBitmap, HandlePosition, 0, 0, HandleBitmap->GetWidth(),
        HandleBitmap->GetHeight(), Gdiplus::Unit::UnitPixel, &imAtt);
    if (res != Gdiplus::Status::Ok)
    {
        return 0;
    }

    return 1;
}

int ColSliderPaintInsidesHue(Gdiplus::Rect insides, Gdiplus::Graphics* graphics)
{
    return 1;
}

int ColSliderPaintInsidesAlpha(Gdiplus::Rect insides, Gdiplus::Graphics* graphics)
{
    return 1;
}