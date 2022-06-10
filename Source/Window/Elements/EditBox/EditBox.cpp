#include "EditBox.h"

LRESULT CALLBACK _EditBoxProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
        break;
    }
}

int RegisterEditBox(HINSTANCE hInstance)
{
    WNDCLASS winClass = {};
    winClass.lpfnWndProc = _EditBoxProc;
    winClass.hInstance = hInstance;
    winClass.lpszClassName = EL_EDITBOX;

    RegisterClass(&winClass);

    return true;
}