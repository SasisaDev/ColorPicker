
#if defined(WIN32)
#include "../Element.h"
#include <string>
#define EL_EDITBOX L"colpik_editbox"

class ColEditBox : public ColElement {
protected:
	Gdiplus::Bitmap *Background;
	std::wstring text;

	int selectedRange[2];

	Gdiplus::RectF textBounds;
public:
	WNDPROC OldWndProc;

	virtual int Register(HINSTANCE hInstance, HWND hOwner, int x, int y, int cx, int cy) override;

	void SelectPos1(int x);
	void SelectPos2(int x);

	void SetText(LPCWSTR _Text);
	std::wstring GetText();

	int GetCharacterWidth();
	
	virtual int Paint(HDC* hdc, Gdiplus::Graphics* graphics) override;
};

#endif