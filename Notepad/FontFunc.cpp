#include "stdafx.h"
#include <windows.h>
#include <commdlg.h>

static LOGFONT logfont = 
				{ -16, 0, 0, 0, 290, '\0', '\0', '\0', 134, 3, 2, 1, 34, L"Î¢ÈíÑÅºÚ Light" };
static HFONT   hFont;

BOOL FontChooseFont(HWND hwnd)
{
	CHOOSEFONT cf;

	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.hwndOwner = hwnd;
	cf.hDC = NULL;
	cf.lpLogFont = &logfont;
	cf.iPointSize = 0;
	cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS;
	cf.rgbColors = 0;
	cf.lCustData = 0;
	cf.lpfnHook = NULL;
	cf.lpTemplateName = NULL;
	cf.hInstance = NULL;
	cf.lpszStyle = NULL;
	cf.nFontType = 0;               // Returned from ChooseFont
	cf.nSizeMin = 0;
	cf.nSizeMax = 0;

	return ChooseFont(&cf);
}

void FontInitialize(HWND hwndEdit)
{
	GetObject(GetStockObject(SYSTEM_FIXED_FONT), sizeof(LOGFONT),
		(PTSTR)&logfont);

	hFont = CreateFontIndirect(&logfont);
	SendMessage(hwndEdit, WM_SETFONT, (WPARAM)hFont, 0);
}

void FontSetFont(HWND hwndEdit, bool e = false)
{
	HFONT hFontNew;
	RECT  rect;

	if (e)
		logfont =
	{ -16, 0, 0, 0, 290, '\0', '\0', '\0', 134, 3, 2, 1, 34, L"Î¢ÈíÑÅºÚ Light" };
	hFontNew = CreateFontIndirect(&logfont);
	SendMessage(hwndEdit, WM_SETFONT, (WPARAM)hFontNew, 0);
	DeleteObject(hFont);
	hFont = hFontNew;
	GetClientRect(hwndEdit, &rect);
	InvalidateRect(hwndEdit, &rect, TRUE);
}

void FontDeinitialize(void)
{
	DeleteObject(hFont);
}