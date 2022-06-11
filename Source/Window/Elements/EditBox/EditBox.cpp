#include "EditBox.h"
#include "../../../../resource.h"

LRESULT CALLBACK _EditBoxProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
        break;
    }
}

int ColEditBox::Register(HINSTANCE hInstance, HWND hOwner, int x, int y, int cx, int cy)
{
    ColElement::Register(hInstance, hOwner, x, y, cx, cy);

    handle = CreateWindow(L"edit", L"", WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY, 
        x, y, cx, cy, hOwner, NULL, hInstance, NULL);
    
    Background = Gdiplus::Bitmap::FromHBITMAP(reinterpret_cast<HBITMAP>(LoadImage(hInstance, 
        MAKEINTRESOURCE(IDB_BITMAP3), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION)), NULL);
    if (!Background)
    {
        return false;
    }

    return true;
}

void ColEditBox::SelectPos1(int x)
{
    selectedRange[0] = x;
}

void ColEditBox::SelectPos2(int x)
{
    selectedRange[1] = x;
}

void ColEditBox::SetText(LPCWSTR _Text)
{
    text =_Text;
    SetWindowText(handle, _Text);
}

std::wstring ColEditBox::GetText()
{
    return text;
}

int ColEditBox::Paint(HDC* hdc, Gdiplus::Graphics* graphics)
{
    Gdiplus::ImageAttributes imAtt;
    imAtt.SetColorKey(Gdiplus::Color(0, 0, 0), Gdiplus::Color(200, 200, 200), 
        Gdiplus::ColorAdjustTypeBitmap);

    auto res = graphics->DrawImage(Background, GetRectOnCanvas(), 0, 0, Background->GetWidth(), 
        Background->GetHeight(), Gdiplus::Unit::UnitPixel, &imAtt);
    if (res != Gdiplus::Status::Ok)
    {
        return 0;
    }

    Gdiplus::Font Font(*hdc, CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
        VARIABLE_PITCH, L"Segoe UI"));

    int result = graphics->DrawString(text.c_str(), text.size(), &Font, Gdiplus::PointF(area.left+2,
        area.top), Gdiplus::StringFormat::GenericDefault(), new Gdiplus::SolidBrush(Gdiplus::Color::Black));
    if (result != Gdiplus::Status::Ok)
    {
        return 0;
    }

    return 1;
}
