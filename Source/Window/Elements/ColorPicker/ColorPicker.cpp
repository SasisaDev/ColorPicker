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

    Gdiplus::Bitmap* gradient1 = new Gdiplus::Bitmap(GetRectOnCanvas().Width, GetRectOnCanvas().Height, PixelFormat24bppRGB);
    Gdiplus::Graphics* pGr1 = Gdiplus::Graphics::FromImage(gradient1);
    Gdiplus::Bitmap* gradient2 = new Gdiplus::Bitmap(GetRectOnCanvas().Width, GetRectOnCanvas().Height, PixelFormat24bppRGB);
    Gdiplus::Graphics* pGr2 = Gdiplus::Graphics::FromImage(gradient2);

    auto rect = Gdiplus::Rect(0, 0, GetRectOnCanvas().Width, GetRectOnCanvas().Height);

    Gdiplus::PointF a1(rect.Width/2, 0);
    Gdiplus::PointF b1(rect.Width/2, rect.Height);

    Gdiplus::LinearGradientBrush brush1(a1, b1, Gdiplus::Color(255, 255, 255, 255), Gdiplus::Color(255, 0, 0, 0));
    if (pGr1->FillRectangle(&brush1, rect) != Gdiplus::Ok)
    {
        return 0;
    }

    Gdiplus::PointF a2(0, rect.Height/2);
    Gdiplus::PointF b2(rect.Width, rect.Height/2);

    Gdiplus::LinearGradientBrush brush2(a2, b2, Gdiplus::Color(255, 255, 255, 255), HSVtoRGB(Hue, 100, 100));
    pGr2->FillRectangle(&brush2, rect);
	
	// Render to background
    // Make 2 gradients interpolation with multiply blending
    pGr->DrawImage(MultiplyImagePtr(gradient1, gradient2), rect);

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

Gdiplus::Bitmap* MultiplyImagePtr(Gdiplus::Bitmap* SrcBitmap1, Gdiplus::Bitmap* SrcBitmap2)
{
    int width;
    int height;

    if (SrcBitmap1->GetWidth() < SrcBitmap2->GetWidth())
        width = SrcBitmap1->GetWidth();
    else
        width = SrcBitmap2->GetWidth();

    if (SrcBitmap1->GetHeight() < SrcBitmap2->GetHeight())
        height = SrcBitmap1->GetHeight();
    else
        height = SrcBitmap2->GetHeight();

    Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(width, height);
    int clr1, clr2;

    try
    {
        Gdiplus::BitmapData Src1Data, Src2Data, DestData;
        SrcBitmap1->LockBits(new Gdiplus::Rect(0, 0, SrcBitmap1->GetWidth(), SrcBitmap1->GetHeight()), Gdiplus::ImageLockMode::ImageLockModeRead, PixelFormat24bppRGB, &Src1Data);

        SrcBitmap2->LockBits(new Gdiplus::Rect(0, 0, SrcBitmap2->GetWidth(), SrcBitmap2->GetHeight()), Gdiplus::ImageLockMode::ImageLockModeRead, PixelFormat24bppRGB, &Src2Data);

        bitmap->LockBits(new Gdiplus::Rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight()), Gdiplus::ImageLockMode::ImageLockModeWrite, PixelFormat24bppRGB, &DestData);

        int xOffset = 3;

        for (int col = 0; col < bitmap->GetHeight() - 1; col++)
        {
            byte* Src1Ptr = (byte*)Src1Data.Scan0 + col * Src1Data.Stride;
            byte* Src2Ptr = (byte*)Src2Data.Scan0 + col * Src2Data.Stride;
            byte* DestPtr = (byte*)DestData.Scan0 + col * DestData.Stride;

            for (int row = 0; row < bitmap->GetWidth() - 1; row++)
            {
                clr1 = (Src1Ptr[row * xOffset] + Src1Ptr[row * xOffset + 1] + Src1Ptr[row * xOffset + 2]) / 3;
                clr2 = (Src2Ptr[row * xOffset] + Src2Ptr[row * xOffset + 1] + Src2Ptr[row * xOffset + 2]) / 3;

                clr1 *= clr2;

                if (clr1 == 0)
                {
                    DestPtr[row * xOffset] = (byte)(0);
                    DestPtr[row * xOffset + 1] = (byte)(0);
                    DestPtr[row * xOffset + 2] = (byte)(0);
                }
                else
                {
                    DestPtr[row * xOffset] = (byte)(Src2Ptr[row * xOffset]);
                    DestPtr[row * xOffset + 1] = (byte)(Src2Ptr[row * xOffset + 1]);
                    DestPtr[row * xOffset + 2] = (byte)(Src2Ptr[row * xOffset + 2]);
                }
            }
        }

        bitmap->UnlockBits(&DestData);
        SrcBitmap1->UnlockBits(&Src1Data);
        SrcBitmap2->UnlockBits(&Src2Data);

    }
    catch (std::exception ex)
    {

    }

    return bitmap;
}