#include "stdafx.h"
#include <Windows.h>
#include <commdlg.h>

#define ANSI 0

static OPENFILENAME ofn;

void FileInitialize(HWND hwnd)
{
	static TCHAR szFilter[] =	L"Java源文件 (*.Java)\0*.java\0" 
								L"文本文档 (*.txt)\0*.txt\0"
								L"All Files(*.*)\0*.*\0\0";

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = NULL;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = NULL;
	ofn.Flags = 0;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = TEXT("java");
	ofn.lCustData = 0L;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;
}

BOOL FileOpen(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFileTitle = pstrTitleName;
	ofn.lpstrTitle = L"打开文件";
	ofn.Flags = OFN_HIDEREADONLY;

	return GetOpenFileName(&ofn);
}

BOOL FileSave(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFileTitle = pstrTitleName;
	ofn.lpstrTitle = L"另存为";
	ofn.Flags = OFN_OVERWRITEPROMPT;

	return GetSaveFileName(&ofn);
}


BOOL FileRead(HWND hwndEdit, PTSTR pstrFileName)
{
	BYTE   bySwap;
	DWORD  dwBytesRead;
	HANDLE hFile;
	int    i, iFileLength, iUniTest;
	PBYTE  pBuffer, pText, pConv;

	// 打开文件

	if (INVALID_HANDLE_VALUE == (hFile = CreateFile(pstrFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)) )
		return FALSE;

	// 获取文件大小
	// 加EOF

	iFileLength = GetFileSize(hFile, NULL);
	pBuffer = new BYTE[iFileLength + 2];

	// 读文件并且添加EOF

	ReadFile(hFile, pBuffer, iFileLength, &dwBytesRead, NULL);
	CloseHandle(hFile);
	pBuffer[iFileLength] = 0;
	pBuffer[iFileLength + 1] = 0;

	// 检查是否为UNICODE

	iUniTest = IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE;

	if (IsTextUnicode(pBuffer, iFileLength, &iUniTest))
	{
		pText = pBuffer + 2;
		iFileLength -= 2;

		if (iUniTest & IS_TEXT_UNICODE_REVERSE_SIGNATURE)
		{
			for (i = 0; i < iFileLength / 2; i++)
			{
				bySwap					   = ((BYTE *)pText)[2 * i];
				((BYTE *)pText)[2 * i]	   = ((BYTE *)pText)[2 * i + 1];
				((BYTE *)pText)[2 * i + 1] = bySwap;
			}
		}

		// 分配内存

		pConv = new BYTE[iFileLength + 2];

		// 转换UNICODE到非UNICODE

#ifndef UNICODE
		WideCharToMultiByte(CP_ACP, 0, (PWSTR)pText, -1, pConv,
			iFileLength + 2, NULL, NULL);

		// If the edit control is Unicode, just copy the string
#else
		lstrcpy((PTSTR)pConv, (PTSTR)pText);
#endif

	}
	else      // 文件非UNICODE
	{
		pText = pBuffer;

		// 为字符分配可用内存空间

		pConv = new BYTE[iFileLength * 2 + 2];

		// 转换UNICODE 到 ANSI

#ifdef UNICODE
		MultiByteToWideChar(CP_ACP, 0, (LPCCH)pText, -1, (PTSTR)pConv, iFileLength + 1);

		// 复制缓冲区
#else
		lstrcpy((PTSTR)pConv, (PTSTR)pText);
#endif
	}

	SetWindowText(hwndEdit, (PTSTR)pConv);
	delete[] pBuffer;
	delete[] pConv;

	return TRUE;
}

BOOL FileWrite(HWND hwndEdit, PTSTR pstrFileName, bool Format = UNICODE)
{
	DWORD  dwBytesWritten;
	HANDLE hFile;
	int    iLength;
	PTSTR  pstrBuffer;
	PCHAR  Temp;
	WORD   wByteOrderMark = 0xFEFF;

	//打开或新建文件

	if (INVALID_HANDLE_VALUE ==
		(hFile = CreateFile(pstrFileName, GENERIC_WRITE, 0,
		NULL, CREATE_ALWAYS, 0, NULL)))
		return FALSE;

	// 分配内存
	// 获取字符数量

	iLength = GetWindowTextLength(hwndEdit);
	pstrBuffer = new TCHAR[iLength + 1];

	if (!pstrBuffer)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	if (Format == UNICODE)
	{
		// 写入UNICODE标识符
		// 写入文件

		WriteFile(hFile, &wByteOrderMark, 2, &dwBytesWritten, NULL);

		// 获取缓冲区数据，写入文件

		GetWindowText(hwndEdit, pstrBuffer, iLength + 1);
		WriteFile(hFile, pstrBuffer, iLength * sizeof(TCHAR),
			&dwBytesWritten, NULL);

		if ((iLength * sizeof(TCHAR)) != (int)dwBytesWritten)
		{
			CloseHandle(hFile);
			delete[] pstrBuffer;
			return FALSE;
		}

		CloseHandle(hFile);
		delete[] pstrBuffer;

		return TRUE;
	}
	else
	{
		//写入ANSI格式

		iLength = GetWindowTextLengthA(hwndEdit);
		Temp = new char[iLength + 1]{'\0'};
		GetWindowTextA(hwndEdit, Temp, iLength + 1);

		WriteFile(hFile, Temp, iLength, &dwBytesWritten, NULL);

		if (iLength != (int)dwBytesWritten)
		{
			CloseHandle(hFile);
			delete[] Temp;
			return FALSE;
		}

		CloseHandle(hFile);
		delete[] Temp;

		return TRUE;
	}
}