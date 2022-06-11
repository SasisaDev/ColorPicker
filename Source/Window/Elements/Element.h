#if defined(WIN32)
#include <Windows.h>
#include <gdiplus.h>

class ColElement {
protected:
	HWND handle;
	RECT area;
	HWND owner;
public:

	virtual int Register(HINSTANCE hInstance, HWND hOwner, int x, int y, int cx, int cy);

	Gdiplus::Rect GetRectOnCanvas();

	virtual int Paint(HDC* hdc, Gdiplus::Graphics* graphics);
};

#endif