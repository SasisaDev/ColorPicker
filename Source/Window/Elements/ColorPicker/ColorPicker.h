
#if defined(WIN32)
#include "../Element.h"
#include <string>
#define EL_PICKER L"colpik_picker"

Gdiplus::Color HSVtoRGB(float H, float S, float V);

class ColColorPicker : public ColElement {
protected:
	Gdiplus::Bitmap* Background;
	Gdiplus::Color Color;
	int Hue;
public:

	virtual int Register(HINSTANCE hInstance, HWND hOwner, int x, int y, int cx, int cy) override;

	void SetHue(int hue);
	int GetHue();
	void SetColor(int r, int g, int b);
	Gdiplus::Color GetColor();

	virtual int Paint(HDC* hdc, Gdiplus::Graphics* graphics) override;
};

#endif