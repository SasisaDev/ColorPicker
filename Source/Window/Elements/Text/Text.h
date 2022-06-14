#ifndef _TEXT_H
#define _TEXT_H

#if defined(WIN32)
#include "../Element.h"
#include <string>
#define EL_TEXT L"colpik_text"

class ColText : public ColElement 
{
protected:
	std::wstring text;

	static HFONT hFont;
public:

	virtual int Register(HINSTANCE hInstance, HWND hOwner, int x, int y, int cx, int cy) override;

	void SetText(LPCWSTR _Text);

	virtual int Paint(HDC* hdc, Gdiplus::Graphics* graphics) override;
};

#endif

#endif