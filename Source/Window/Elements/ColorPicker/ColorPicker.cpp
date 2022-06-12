#include "ColorPicker.h"

bool ColColorPicker::ClassRegistered = false;
Gdiplus::Bitmap* ColColorPicker::Background = nullptr;

LRESULT CALLBACK ColColorPickerProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

int ColColorPicker::Register(HINSTANCE hInstance, HWND hOwner, int x, int y, int cx, int cy)
{
    ColElement::Register(hInstance, hOwner, x, y, cx, cy);

    if (!ColColorPicker::ClassRegistered)
    {
        WNDCLASS Class = {};
        Class.lpfnWndProc = ColColorPickerProc;
        Class.hInstance = hInstance;
        Class.hCursor = LoadCursor(NULL, IDC_CROSS);
        Class.lpszClassName = EL_PICKER;

        RegisterClass(&Class);
        ColColorPicker::ClassRegistered = true;
    }

    handle = CreateWindow(EL_PICKER, L"", WS_VISIBLE | WS_CHILD, x, y, cx, cy, hOwner, 0, hInstance, 0);

	return 1;
}

void ColColorPicker::SetHue(int hue)
{
	Hue = hue;
}

int ColColorPicker::GetHue()
{
	return Hue;
}

void ColColorPicker::SetColor(int r, int g, int b)
{
}

Gdiplus::Color ColColorPicker::GetColor()
{
	return Gdiplus::Color();
}

int ColColorPicker::Paint(HDC* hdc, Gdiplus::Graphics* graphics)
{
    ColColorPicker::Background = new Gdiplus::Bitmap(GetRectOnCanvas().Width, GetRectOnCanvas().Height,
		PixelFormat24bppRGB);
	Gdiplus::Graphics* pGr = Gdiplus::Graphics::FromImage(ColColorPicker::Background);

    
	
	// Render to background
    // Make 2 gradients interpolation with multiply blending
    Gdiplus::SolidBrush brush(Gdiplus::Color(255, 0, 0));

    pGr->FillRectangle(&brush, Gdiplus::Rect(0, 0, GetRectOnCanvas().Width, GetRectOnCanvas().Height));


	// Render background to hdc
	graphics->DrawImage(ColColorPicker::Background, GetRectOnCanvas());

    
	return 1;
}

Gdiplus::Color HSVtoRGB(float H, float S, float V)
{
    if (H > 360 || H < 0 || S>100 || S < 0 || V>100 || V < 0) {
        exit(-1);
        return 0;
    }
    float s = S / 100;
    float v = V / 100;
    float C = s * v;
    float X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
    float m = v - C;
    float r, g, b;
    if (H >= 0 && H < 60) {
        r = C, g = X, b = 0;
    }
    else if (H >= 60 && H < 120) {
        r = X, g = C, b = 0;
    }
    else if (H >= 120 && H < 180) {
        r = 0, g = C, b = X;
    }
    else if (H >= 180 && H < 240) {
        r = 0, g = X, b = C;
    }
    else if (H >= 240 && H < 300) {
        r = X, g = 0, b = C;
    }
    else {
        r = C, g = 0, b = X;
    }
    int R = (r + m) * 255;
    int G = (g + m) * 255;
    int B = (b + m) * 255;

	return Gdiplus::Color(R, G, B);
}
