#include "ColorPicker.h"

#include <atomic>

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
    Rerender();
}

int ColColorPicker::GetHue()
{
	return Hue;
}

void ColColorPicker::SetColor(int r, int g, int b)
{
    Color = Gdiplus::Color(r, g, b);
}

Gdiplus::Color ColColorPicker::GetColor()
{
	return Color;
}

void ColColorPicker::OnClick(WPARAM e, int x, int y)
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
            TimerThread = new std::thread(&ColColorPicker::ThreadTimer, this);
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

void ColColorPicker::ThreadTimer()
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
            Selection = { EditCur.x, EditCur.y };
        }
        else
        {
            int _x, _y;

            _x = (EditCur.x < 0) ? 0 : 
                (EditCur.x > GetRectOnCanvas().Width) ? GetRectOnCanvas().Width : EditCur.x;
            _y = (EditCur.y < 0) ? 0 : (EditCur.y > GetRectOnCanvas().Height) ? GetRectOnCanvas().Height : EditCur.y;

            Selection = { _x, _y };
        }

        this_thread::sleep_for(10ms);
        goto timer;
    }
}

void ColColorPicker::PickColor()
{
    // Pick color
    int x = (Selection.x >= ColColorPicker::Background->GetWidth()) ? 
        ColColorPicker::Background->GetWidth()-1 : Selection.x;
    int y = Selection.y;
    Gdiplus::Color newClr;
    ColColorPicker::Background->GetPixel(x, y, &newClr);
    Color.store(newClr);

    volatile const BYTE r = Color.load().GetRed();
    volatile const BYTE g = Color.load().GetGreen();
    volatile const BYTE b = Color.load().GetBlue();

    // Send to containers
    if (RGB)
    {
        std::wstringstream rgb;
        rgb << r;
        rgb << L", " << static_cast<uint8_t>(g);
        rgb << L", " << static_cast<uint8_t>(b);
        // Simplest possible memoisation
        if (RGB->GetText() != rgb.str())
        {
            RGB->SetText(rgb.str().c_str());
        }
    }

    if (HEX)
    {
        std::wstringstream hex;
        hex << L"#" << hexStr(r);
        hex << hexStr(g);
        hex << hexStr(b);
        if (Alpha)
        {
            if (Alpha->GetValue() != 255)
            {
                hex << hexStr((BYTE)Alpha->GetValue());
            }
        }
        // Simplest possible memoisation
        if (HEX->GetText() != hex.str())
        {
            HEX->SetText(hex.str().c_str());
        }
    }

    Rerender();
}

int ColColorPicker::Paint(HDC* hdc, Gdiplus::Graphics* graphics)
{
    delete ColColorPicker::Background;
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

    PickColor();

    Gdiplus::Pen Pen(new Gdiplus::SolidBrush(GetInvertedBW(Color)), 1);
    Gdiplus::Rect selRect = {Selection.x + 11, Selection.y + 10, 8, 8};
    graphics->DrawEllipse(&Pen, selRect);

    delete pGr;
    delete pGr1;
    delete pGr2;
    delete gradient1;
    delete gradient2;

	return 1;
}

Gdiplus::Color GetInvertedBW(Gdiplus::Color Color)
{
    Gdiplus::Color ret;

    ret = Gdiplus::Color(255, 255, 255).GetValue() - Color.GetValue();
    int r = ret.GetRed();
    int g = ret.GetGreen();
    int b = ret.GetBlue();
    int res = (r + g + b) / 3;

    ret = Gdiplus::Color(254, res, res, res);

    return ret;
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
    int width = SrcBitmap1->GetWidth();
    int height = SrcBitmap1->GetHeight();

    Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(width, height);
    byte clr1, clr2;

    try
    {
        Gdiplus::BitmapData Src1Data, Src2Data, DestData;
        SrcBitmap1->LockBits(new Gdiplus::Rect(0, 0, SrcBitmap1->GetWidth(), SrcBitmap1->GetHeight()), Gdiplus::ImageLockMode::ImageLockModeRead, PixelFormat24bppRGB, &Src1Data);

        SrcBitmap2->LockBits(new Gdiplus::Rect(0, 0, SrcBitmap2->GetWidth(), SrcBitmap2->GetHeight()), Gdiplus::ImageLockMode::ImageLockModeRead, PixelFormat24bppRGB, &Src2Data);

        bitmap->LockBits(new Gdiplus::Rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight()), Gdiplus::ImageLockMode::ImageLockModeWrite, PixelFormat24bppRGB, &DestData);

        int xOffset = 3;

        for (int col = 0; col < bitmap->GetHeight() ; col++)
        {
            byte* Src1Ptr = (byte*)Src1Data.Scan0 + col * Src1Data.Stride;
            byte* Src2Ptr = (byte*)Src2Data.Scan0 + col * Src2Data.Stride;
            byte* DestPtr = (byte*)DestData.Scan0 + col * DestData.Stride;

            for (int row = 0; row < bitmap->GetWidth(); row++)
            {
                for (int byt = 0; byt < 3; byt++)
                {
                    clr1 = (Src1Ptr[row * xOffset + byt]);
                    clr2 = (Src2Ptr[row * xOffset + byt]);

                    int res = (int)((float)((float)clr1/255.f)*((float)clr2/255.f)*255.f);

                    DestPtr[row * xOffset + byt] = (byte)(res);
                }
            }
        }

        bitmap->UnlockBits(&DestData);
        SrcBitmap1->UnlockBits(&Src1Data);
        SrcBitmap2->UnlockBits(&Src2Data);

    }
    catch (std::exception ex)
    {
        printf("");
    }

    return bitmap;
}

std::wstring hexStr(BYTE data)
{
    std::wstringstream ss;
    ss << std::hex;
    ss << (int)data;

    if (ss.str().size() != 2)
    {
        std::wstringstream ret;

        for (int i = 0; i < 2 - ss.str().size(); i++)
        {
            ret << 0;
        }
        ret << ss.str();
        return ret.str();
    }

    return ss.str();
}