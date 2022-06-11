#include "Element.h"
#include "basedef.h"

int ColElement::Register(HINSTANCE hInstance, HWND hOwner, int x, int y, int cx, int cy)
{
	owner = hOwner;

	area = {};
	area.left = x;
	area.right = x + cx;
	area.top = y;
	area.bottom = y + cy;
	return 0;
}

void ColElement::SetRepaintCallback(std::function<void()> callback)
{
	rerenderCallback = callback;
}

Gdiplus::Rect ColElement::GetRectOnCanvas()
{
	Gdiplus::Rect dest = { area.left, area.top, area.right-area.left, area.bottom-area.top };

	return dest;
}

int ColElement::Paint(HDC* hdc, Gdiplus::Graphics* graphics)
{
	return 0;
}
