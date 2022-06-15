#pragma once
#ifndef _COLOR_PICKER_H
#define _COLOR_PICKER_H

#if defined(WIN32)
#include "../Element.h"
#include "../EditBox/EditBox.h"
#include "../Slider/Slider.h"
#include <sstream>
#include <iomanip>
#include <codecvt>
#include <string>

#pragma comment (lib, "opengl32.lib")

#define EL_PICKER L"colpik_picker"

Gdiplus::Color HSVtoRGB(float H, float S, float V);
Gdiplus::Bitmap* MultiplyImagePtr(Gdiplus::Bitmap* SrcBitmap1, Gdiplus::Bitmap* SrcBitmap2);
std::wstring hexStr(BYTE data);

class ColColorPicker : public ColElement {
protected:
	static Gdiplus::Bitmap* Background;
	Gdiplus::Color Color;
	int Hue;

	POINT Selection;
	
	static bool ClassRegistered;

	bool tickTimer = false;
	std::thread* TimerThread;
public:
	ColEditBox* RGB;
	ColEditBox* HEX;
	ColEditBox* CMYK;
	ColEditBox* PMS;

	ColSlider* Alpha;

	virtual int Register(HINSTANCE hInstance, HWND hOwner, int x, int y, int cx, int cy) override;

	void SetHue(int hue);
	int GetHue();
	void SetColor(int r, int g, int b);
	Gdiplus::Color GetColor();

	virtual void OnClick(WPARAM e, int x, int y) override;

	void ThreadTimer();

	void PickColor();

	virtual int Paint(HDC* hdc, Gdiplus::Graphics* graphics) override;
};

#endif
#endif