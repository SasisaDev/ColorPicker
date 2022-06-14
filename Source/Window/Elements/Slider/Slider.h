#ifndef _SLIDER_H
#define _SLIDER_H

#if defined(WIN32)
#include "../Element.h"
#include <string>
#define EL_SLIDER L"colpik_slider"

int ColSliderPaintInsidesHue(Gdiplus::Rect insides, Gdiplus::Graphics* graphics);
int ColSliderPaintInsidesAlpha(Gdiplus::Rect insides, Gdiplus::Graphics* graphics);

class ColSlider : public ColElement
{
protected:
	int Value;
	int MaxValue = 100;

	Gdiplus::Bitmap *HandleBitmap;

	static bool ClassRegistered;
public:

	virtual int Register(HINSTANCE hInstance, HWND hOwner, int x, int y, int cx, int cy) override;

	void SetValue(int value);
	int GetValue();

	void SetMaxValue(int value);

	virtual void OnClick(WPARAM e, int x, int y) override;

	void ThreadTimer();

	std::function<int(Gdiplus::Rect, Gdiplus::Graphics*)> PaintInsidesProc;
	std::function<void(int)> OnChange;

	virtual int Paint(HDC* hdc, Gdiplus::Graphics* graphics) override;
};

#endif

#endif