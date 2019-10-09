#pragma once
#include <Windows.h>
#define _SourceFile L"F:\\UserData.ini"
//UserDataÊý¾Ý

struct UserData{
	int cxClient;
	int cyClient;
	int x;
	int y;
	bool iUnicode;
	wchar_t wSpaceError[0xff];
	wchar_t wUnknowError[0xff];
	UserData()
	{
		cxClient = GetPrivateProfileInt(L"BoxSize", L"Width", 700, _SourceFile);
		cyClient = GetPrivateProfileInt(L"BoxSize", L"Hight", 500, _SourceFile);
		x = GetPrivateProfileInt(L"BoxSize", L"x", 0, _SourceFile);
		y = GetPrivateProfileInt(L"BoxSize", L"y", 0, _SourceFile);
		iUnicode = GetPrivateProfileInt(L"TextFormat", L"IsUnicode", 0, _SourceFile);
		GetPrivateProfileString(L"Strings", L"SpaceError", L" ", wSpaceError, 0xff, _SourceFile);
		GetPrivateProfileString(L"Strings", L"UnknowError", L" ", wUnknowError, 0xff, _SourceFile);
	}
	void Write(int cxClient, int cyClient, int x, int y)
	{
		wchar_t temp[0xff];
		wsprintf(temp, L"%d", cxClient);
		WritePrivateProfileString(L"BoxSize", L"Width", temp, _SourceFile);
		wsprintf(temp, L"%d", cyClient);
		WritePrivateProfileString(L"BoxSize", L"Hignt", temp, _SourceFile);
		wsprintf(temp, L"%d", x);
		WritePrivateProfileString(L"BoxSize", L"x", temp, _SourceFile);
		wsprintf(temp, L"%d", y);
		WritePrivateProfileString(L"BoxSize", L"y", temp, _SourceFile);
	}
};
