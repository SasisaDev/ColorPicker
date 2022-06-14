#include "Text.h"
#include "..\Slider\Slider.h"

#if defined(WIN32)

HFONT ColText::hFont = 0;

int ColText::Register(HINSTANCE hInstance, HWND hOwner, int x, int y, int cx, int cy)
{
	auto ret = ColElement::Register(hInstance, hOwner, x, y, cx, cy);

    if (ColText::hFont == 0)
    {
        ColText::hFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            VARIABLE_PITCH, L"Segoe UI");
    }

    return ret;
}

void ColText::SetText(LPCWSTR _Text)
{
    text = _Text;
    if (rerenderCallback)
    {
        rerenderCallback();
    }
}

int ColText::Paint(HDC* hdc, Gdiplus::Graphics* graphics)
{
    Gdiplus::Font Font(*hdc, ColText::hFont);

    int result = graphics->DrawString(text.c_str(), text.size(), &Font, Gdiplus::PointF(area.left + 2,
        area.top), Gdiplus::StringFormat::GenericDefault(), new Gdiplus::SolidBrush(Gdiplus::Color(112, 112, 112)));
    if (result != Gdiplus::Status::Ok)
    {
        return 0;
    }

    return 1;
}
#endif