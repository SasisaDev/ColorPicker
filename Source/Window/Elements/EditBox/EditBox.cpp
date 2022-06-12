#include "EditBox.h"
#include "../../../../resource.h"

#if defined(WIN32)

LRESULT CALLBACK ColEditBoxProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ColEditBox* pOwner = (ColEditBox*)(GetProp(hwnd, TEXT("ColOwner")));

    POINT cur;
    RECT win;
    POINT WinCur;
    POINT EditCur;

    //TO-DO: These procedures are working only if click is INSIDE this element.
    //       Setup global hook, to deselect elements from editbox.

    switch (uMsg)
    {
    case WM_LBUTTONDOWN:
        GetCursorPos(&cur);
        
        GetWindowRect(pOwner->GetOwner(), &win);

        WinCur.x = cur.x - win.left;
        WinCur.y = cur.y - win.top;

        EditCur.x = WinCur.x - pOwner->GetRectOnCanvas().X;
        EditCur.y = WinCur.y - pOwner->GetRectOnCanvas().Y;

        if (EditCur.x >= 0 && EditCur.x <= pOwner->GetRectOnCanvas().Width
            && EditCur.y >= 0 && EditCur.y <= pOwner->GetRectOnCanvas().Height)
        {
            pOwner->SelectPos1(EditCur.x/pOwner->GetCharacterWidth());
        }
        else
        {
            pOwner->SelectPos1(-1);
        }
        break;
    case WM_LBUTTONUP:
        GetCursorPos(&cur);

        GetWindowRect(pOwner->GetOwner(), &win);

        WinCur.x = cur.x - win.left;
        WinCur.y = cur.y - win.top;

        EditCur.x = WinCur.x - pOwner->GetRectOnCanvas().X;
        EditCur.y = WinCur.y - pOwner->GetRectOnCanvas().Y;

        if (EditCur.x >= 0 && EditCur.x <= pOwner->GetRectOnCanvas().Width
            && EditCur.y >= 0 && EditCur.y <= pOwner->GetRectOnCanvas().Height)
        {
            pOwner->SelectPos2(EditCur.x / pOwner->GetCharacterWidth());
            pOwner->Rerender();
        }
        else
        {
            pOwner->SelectPos2(-1);
            pOwner->Rerender();
        }
        break;
    }

    return CallWindowProc(pOwner->OldWndProc, hwnd, uMsg, wParam, lParam);
}

int ColEditBox::Register(HINSTANCE hInstance, HWND hOwner, int x, int y, int cx, int cy)
{
    ColElement::Register(hInstance, hOwner, x, y, cx, cy);

    selectedRange[0] = -1;
    selectedRange[1] = -1;

    handle = CreateWindow(L"edit", L"", WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY, 
        x, y, cx, cy, hOwner, NULL, hInstance, NULL);

    OldWndProc = (WNDPROC)SetWindowLongPtr(handle,
        GWLP_WNDPROC, (LONG_PTR)ColEditBoxProc);
    SetProp(handle, TEXT("ColOwner"), (HANDLE)this);
    
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
    if (rerenderCallback)
    {
        rerenderCallback();
    }
}

std::wstring ColEditBox::GetText()
{
    return text;
}

int ColEditBox::GetCharacterWidth()
{
    return textBounds.Width / text.size();
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

    HFONT hFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        VARIABLE_PITCH, L"Segoe UI");

    Gdiplus::Font Font(*hdc, hFont);

    SendMessage(handle, WM_SETFONT, (WPARAM)hFont, TRUE);

    Gdiplus::PointF TextLayout(area.left + 2, area.top);

    graphics->MeasureString(text.c_str(), text.size(), &Font, TextLayout, &textBounds);

    int result = graphics->DrawString(text.c_str(), text.size(), &Font, TextLayout, Gdiplus::StringFormat::GenericDefault(), new Gdiplus::SolidBrush(Gdiplus::Color::Black));
    if (result != Gdiplus::Status::Ok)
    {
        return 0;
    }

    return 1;
}
#endif