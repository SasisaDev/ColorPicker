#pragma once
#ifndef _ELEMENT_H
#define _ELEMENT_H

#if defined(WIN32)
#include <Windows.h>
#include <gdiplus.h>
#include <functional>

class ColElement {
protected:
	HWND handle;
	RECT area;
	HWND owner;

	std::function<void()> rerenderCallback;
public:

	virtual int Register(HINSTANCE hInstance, HWND hOwner, int x, int y, int cx, int cy);

	void SetRepaintCallback(std::function<void()> callback);

	Gdiplus::Rect GetRectOnCanvas();
	HWND GetOwner() const;

	virtual void OnClick(WPARAM e, int x, int y);

	void Rerender();

	virtual int Paint(HDC* hdc, Gdiplus::Graphics* graphics);
};

#endif

#endif