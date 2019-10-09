#include "stdafx.h"
#include <Windows.h>
#include <commdlg.h>

#define ANSI 0

static OPENFILENAME ofn;

void FileInitialize(HWND hwnd)
{
	static TCHAR szFilter[] =	L"JavaԴ�ļ� (*.Java)\0*.java\0" 
								L"�ı��ĵ� (*.txt)\0*.txt\0"
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
	ofn.lpstrTitle = L"���ļ�";
	ofn.Flags = OFN_HIDEREADONLY;

	return GetOpenFileName(&ofn);
}

BOOL FileSave(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFileTitle = pstrTitleName;
	ofn.lpstrTitle = L"���Ϊ";
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

	// ���ļ�

	if (INVALID_HANDLE_VALUE == (hFile = CreateFile(pstrFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)) )
		return FALSE;

	// ��ȡ�ļ���С
	// ��EOF

	iFileLength = GetFileSize(hFile, NULL);
	pBuffer = new BYTE[iFileLength + 2];

	// ���ļ��������EOF

	ReadFile(hFile, pBuffer, iFileLength, &dwBytesRead, NULL);
	CloseHandle(hFile);
	pBuffer[iFileLength] = 0;
	pBuffer[iFileLength + 1] = 0;

	// ����Ƿ�ΪUNICODE

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

		// �����ڴ�

		pConv = new BYTE[iFileLength + 2];

		// ת��UNICODE����UNICODE

#ifndef UNICODE
		WideCharToMultiByte(CP_ACP, 0, (PWSTR)pText, -1, pConv,
			iFileLength + 2, NULL, NULL);

		// If the edit control is Unicode, just copy the string
#else
		lstrcpy((PTSTR)pConv, (PTSTR)pText);
#endif

	}
	else      // �ļ���UNICODE
	{
		pText = pBuffer;

		// Ϊ�ַ���������ڴ�ռ�

		pConv = new BYTE[iFileLength * 2 + 2];

		// ת��UNICODE �� ANSI

#ifdef UNICODE
		MultiByteToWideChar(CP_ACP, 0, (LPCCH)pText, -1, (PTSTR)pConv, iFileLength + 1);

		// ���ƻ�����
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

	//�򿪻��½��ļ�

	if (INVALID_HANDLE_VALUE ==
		(hFile = CreateFile(pstrFileName, GENERIC_WRITE, 0,
		NULL, CREATE_ALWAYS, 0, NULL)))
		return FALSE;

	// �����ڴ�
	// ��ȡ�ַ�����

	iLength = GetWindowTextLength(hwndEdit);
	pstrBuffer = new TCHAR[iLength + 1];

	if (!pstrBuffer)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	if (Format == UNICODE)
	{
		// д��UNICODE��ʶ��
		// д���ļ�

		WriteFile(hFile, &wByteOrderMark, 2, &dwBytesWritten, NULL);

		// ��ȡ���������ݣ�д���ļ�

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
		//д��ANSI��ʽ

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