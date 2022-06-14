#ifndef _EDIT_BOX_H
#define _EDIT_BOX_H

#if defined(WIN32)
#include "../Element.h"
#include <string>
#include <thread>
#define EL_EDITBOX L"colpik_editbox"

class ColEditBox : public ColElement {
protected:
	Gdiplus::Bitmap *Background;
	std::wstring text;

	int selectedRange[2];

	Gdiplus::RectF textBounds;

	static HFONT hFont;

	bool tickTimer = false;
	std::thread* TimerThread;
public:
	WNDPROC OldWndProc;

	virtual int Register(HINSTANCE hInstance, HWND hOwner, int x, int y, int cx, int cy) override;

	void SelectPos1(int x);
	void SelectPos2(int x);

	void SetText(LPCWSTR _Text);
	std::wstring GetText();

	int GetCharacterWidth();

	virtual void OnClick(WPARAM e, int x, int y) override;

	void ThreadTimer();
	
	virtual int Paint(HDC* hdc, Gdiplus::Graphics* graphics) override;
};

#endif

#endif