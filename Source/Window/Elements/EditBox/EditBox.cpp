#include "EditBox.h"
#include "../../../../resource.h"

#if defined(WIN32)

LRESULT CALLBACK ColEditBoxProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ColEditBox* pOwner = (ColEditBox*)(GetProp(hwnd, TEXT("ColOwner")));

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
    if(x >= -1 || x < text.size())
    selectedRange[0] = x;
}

void ColEditBox::SelectPos2(int x)
{
    if (x >= -1 || x < text.size())
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

void ColEditBox::OnClick(WPARAM e, int x, int y)
{
    POINT cur;
    RECT win;
    POINT WinCur;
    POINT EditCur;

    switch (e)
    {
    case WM_LBUTTONDOWN:
        GetCursorPos(&cur);

        GetWindowRect(GetOwner(), &win);

        WinCur.x = cur.x - win.left;
        WinCur.y = cur.y - win.top;

        EditCur.x = WinCur.x - GetRectOnCanvas().X;
        EditCur.y = WinCur.y - GetRectOnCanvas().Y;

        if (EditCur.x >= 0 && EditCur.x <= GetRectOnCanvas().Width
            && EditCur.y >= 0 && EditCur.y <= GetRectOnCanvas().Height)
        {
            SelectPos1(EditCur.x / GetCharacterWidth());
            TimerThread = new std::thread(&ColEditBox::ThreadTimer, this);
            tickTimer = true;
        }
        else
        {
            if (selectedRange[0] >= 0 || selectedRange[1] >= 0)
            {
                SelectPos1(-1);
                SelectPos2(-1);
                Rerender();
            }
            tickTimer = false;
        }
        break;
    case WM_LBUTTONUP:
        // TODO: SelectPos to -1 if out-bounds
        tickTimer = false;
        break;
    case WM_LBUTTONDBLCLK:
        SelectPos1(0);
        SelectPos2(text.size()-1);
        Rerender();

        tickTimer = false;
        break;
    }
}

void ColEditBox::ThreadTimer()
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
            SelectPos2(EditCur.x / GetCharacterWidth());
            Rerender();
        }
        else
        {
            if (EditCur.x < 0)
            {
                SelectPos2(0);
            }
            else
            {
                SelectPos2(text.size() - 1);
            }
            Rerender();
        }

        this_thread::sleep_for(33ms);
        goto timer;
    }
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

    if((selectedRange[0] > 0 && selectedRange[0] < text.size())
        && (selectedRange[1] > 0 && selectedRange[1] < text.size()))
    {
        Gdiplus::Rect SelectionRect;
        if (selectedRange[1] >= selectedRange[0])
        {
            SelectionRect = { (GetRectOnCanvas().X + 5) + selectedRange[0] * GetCharacterWidth(), GetRectOnCanvas().Y,
            selectedRange[1] * GetCharacterWidth() - selectedRange[0] * GetCharacterWidth() + 2, GetRectOnCanvas().Height };
        }
        else
        {
            SelectionRect = { (GetRectOnCanvas().X + 5) + selectedRange[1] * GetCharacterWidth(), GetRectOnCanvas().Y,
            selectedRange[0] * GetCharacterWidth() - selectedRange[1] * GetCharacterWidth() + 2, GetRectOnCanvas().Height };
        }
        graphics->FillRectangle(new Gdiplus::SolidBrush(Gdiplus::Color(66, 135, 245)), SelectionRect);
    }

    int result = graphics->DrawString(text.c_str(), text.size(), &Font, TextLayout, Gdiplus::StringFormat::GenericDefault(), new Gdiplus::SolidBrush(Gdiplus::Color::Black));
    if (result != Gdiplus::Status::Ok)
    {
        return 0;
    }

    return 1;
}
#endif