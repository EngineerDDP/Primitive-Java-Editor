// Notepad.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Resource.h"
#include "Data.h"

#define MAX_LOADSTRING 100


// ȫ�ֱ���: 

HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

// �˴���ģ���а����ĺ�����ǰ������: 

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

//���ĵ�����

VOID DoCaption(HWND, TCHAR *);
VOID OkMessage(HWND, TCHAR *, TCHAR *);
bool AskAboutSave(HWND, TCHAR *);
wchar_t* AnsiToUnicode(const char* szStr);
char* UnicodeToAnsi(const wchar_t* szStr);
char* GetExeDirectory();
void __cdecl CmdProc(void *);
inline void ResetBuffer(wchar_t *, int);

//�ⲿ����

extern void FileInitialize(HWND);
extern BOOL FileOpen(HWND, PTSTR, PTSTR);
extern BOOL FileSave(HWND, PTSTR, PTSTR);
extern BOOL FileRead(HWND, PTSTR);
extern BOOL FileWrite(HWND, PTSTR, bool Format = UNICODE);
extern BOOL FontChooseFont(HWND);
extern void FontSetFont(HWND, bool e = false);
extern void FontInitialize(HWND);
extern void FontDeinitialize();

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO:  �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_NOTEPAD, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��: 
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NOTEPAD));

	// ����Ϣѭ��: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  ����:  MyRegisterClass()
//
//  Ŀ��:  ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_NOTEPAD);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����:  InitInstance(HINSTANCE, int)
//
//   Ŀ��:  ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 400, 300, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  ����:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//���ڲ˵�
	static HMENU	hMenu;
	//�������������
	static TCHAR	*wFileName = nullptr,
					*wTitleName = nullptr;
	static CHAR		*aFileName = nullptr,
					*aTitleName = nullptr;
	//�豸�������
	//HDC			hdc;
	//�༭����
	static HWND		hWndEdit;
	//״̬����
	static bool		bSaved = true,
		bDeleteable = false,
		bIsUnicode = false,
		bOpened = false,
		bAutoTab = true,
		bSelfUpdate = false;
	//�ļ�·��
	static CHAR		*correntdir = nullptr;
	//�û����ݳ�ʼ��
	static UserData udData;
	//wchar_t wTempA[0xff];
	static CHAR		*command = nullptr;
	//�༭���Ʊ���
	static int		iLinesNow, iLinesCount, iTabs;
	static bool		bNewLine;
	static wchar_t  *wPreviousLine = nullptr,
					*wCorrentLine = nullptr;

	//�߳̿���
	static uintptr_t uptCmd = NULL;

	//��ʱ����
	int iXtemp, iYtemp, iSelect;
	int wmId, wmEvent;

	switch (message)
	{
	case WM_CREATE:
		//������ʼ��
		bNewLine = false;
		iLinesNow = 0;
		iLinesCount = 0;
		iTabs = 0;
		wPreviousLine = new wchar_t[0x3f]{0x3f};
		wCorrentLine = new wchar_t[0x3f]{0x3f};
		//�Ӵ��ڳ�ʼ��
		hWndEdit = CreateWindow(L"Edit",
			NULL, WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_NOHIDESEL,
			0, 0, 0, 0, hWnd, (HMENU)ID_EDIT, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDC_NOTEPAD));
		hMenu = GetSubMenu(hMenu, 0);
		//�˵���ʼ��
		FileInitialize(hWnd);
		FontInitialize(hWndEdit);
		//���ݳ�ʼ��
		MoveWindow(hWnd, 0, 0, udData.cxClient, udData.cyClient, TRUE);
		bIsUnicode = udData.iUnicode;
		correntdir = GetExeDirectory();
		command = new char[0x200];
		//�����ʼ��
		FontSetFont(hWndEdit, true);
		//�������ļ�����ʼ��
		wFileName = new TCHAR[MAX_PATH]{0};
		wTitleName = new TCHAR[MAX_PATH]{L'��', L'��', L'��', L'\0'};
		aFileName = UnicodeToAnsi(wFileName);
		aTitleName = UnicodeToAnsi(wTitleName);
		DoCaption(hWnd, L"�ޱ���");
		break;

	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmEvent)
		{
		//�Զ�����
		case EN_UPDATE:
			bSaved = false;
			bOpened = true;
			if (!bAutoTab)
				break;
			if (bSelfUpdate)
				break;
			iXtemp = iLinesCount;
			iLinesCount = SendMessage(hWndEdit, EM_GETLINECOUNT, 0, 0);
			iLinesNow = SendMessage(hWndEdit, EM_LINEFROMCHAR, -1, 0);
			if (iXtemp == iLinesCount - 1)
			{
				ResetBuffer(wCorrentLine, 0x3f);
				SendMessage(hWndEdit, EM_GETLINE, iLinesNow, (LPARAM)wCorrentLine);
				ResetBuffer(wPreviousLine, 0x3f);
				SendMessage(hWndEdit, EM_GETLINE, iLinesNow - 1, (LPARAM)wPreviousLine);
				iTabs = 0;
				for (iYtemp = 0; wPreviousLine[iYtemp] == L'\t'; iYtemp++)
					iTabs++;
				if (wPreviousLine[iYtemp] == L'{')
					iTabs++;
				for (iYtemp = iYtemp + 1; wPreviousLine[iYtemp]; iYtemp++)
				{
					if (wPreviousLine[iYtemp] == L'{')
						iTabs++;
					if (wPreviousLine[iYtemp] == L'}')
						iTabs--;
				}
				for (iYtemp = 0; wCorrentLine[iYtemp]; iYtemp++)
				{
					if (wCorrentLine[iYtemp] == L'}')
						iTabs--;
				}
				if (iTabs > 0)
				{
					bSelfUpdate = true;
					SendMessage(hWndEdit, WM_CHAR, L'\t', iTabs);
					bSelfUpdate = false;
				}
				bNewLine = true;
			}
			else if (iXtemp == iLinesCount && iTabs && bNewLine)
			{
				ResetBuffer(wCorrentLine, 0x3f);
				SendMessage(hWndEdit, EM_GETLINE, iLinesNow, (LPARAM)wCorrentLine);
				iXtemp = SendMessage(hWndEdit, EM_LINEINDEX, -1, 0);
				iYtemp = SendMessage(hWndEdit, EM_GETSEL, NULL, NULL);
				if (wCorrentLine[iTabs] == L'}' && iTabs > 0)
				{
					SendMessage(hWndEdit, EM_SETSEL, iXtemp, iXtemp + 1);
					bSelfUpdate = true;
					SendMessage(hWndEdit, WM_CHAR, L'\b', 1);
					SendMessage(hWndEdit, EM_SETSEL, HIWORD(iYtemp), LOWORD(iYtemp));
					bNewLine = false;
					bSelfUpdate = false;
				}
			}
			break;
		case EN_ERRSPACE:
			OkMessage(hWnd, L"�ڴ治�㣬��ر�����Ӧ�ó����Ի�ȡ��������ڴ�ռ䣡", L"����");
			break;
		default:
			break;
		}
		// �����˵�ѡ��: 
		switch (wmId)
		{
		case IDM_NEWFILE:
			if (bSaved || AskAboutSave(hWnd, wTitleName))
			{
				SendMessage(hWndEdit, WM_CLEAR, 0, 0);
				SetWindowText(hWndEdit, L"\0");
				delete[] wFileName;
				delete[] wTitleName;
				wFileName = new TCHAR[MAX_PATH]{0};
				wTitleName = new TCHAR[MAX_PATH]{L'��', L'��', L'��', L'\0'};
				DoCaption(hWnd, wTitleName);
				bSaved = true;
				bOpened = false;
			}
			break;
		case IDM_OPEN:
			if (bSaved || AskAboutSave(hWnd, wTitleName))
			{
				if (FileOpen(hWnd, wFileName, wTitleName))
				{
					SendMessage(hWndEdit, WM_CLEAR, 0, 0);
					SetWindowText(hWndEdit, L"\0");
					DoCaption(hWnd, wTitleName);
					FileRead(hWndEdit, wFileName);
					delete[] aFileName;
					delete[] aTitleName;
					aFileName = UnicodeToAnsi(wFileName);
					aTitleName = UnicodeToAnsi(wTitleName);
					bOpened = true;
				}
			}
			break;
		case IDM_SAVE:
			if (wFileName[0])
			{
				if (FileWrite(hWndEdit, wFileName, bIsUnicode))
				{
					bSaved = true;
				}
				else
					OkMessage(hWnd, L"δ֪����!", L"����");
			}
			else
				SendMessage(hWnd, WM_COMMAND, IDM_SAVEAS, 0);
			break;
		case IDM_SAVEAS:
			if (FileSave(hWnd, wFileName, wTitleName))
			{
				DoCaption(hWnd, wTitleName);
				if (FileWrite(hWndEdit, wFileName, bIsUnicode))
				{
					delete[] aFileName;
					delete[] aTitleName;
					aFileName = UnicodeToAnsi(wFileName);
					aTitleName = UnicodeToAnsi(wTitleName);
					bSaved = true;
				}
				else
					OkMessage(hWnd, L"δ֪����!", L"���Ϊ");
			}
			break;
		case ID_32803:										//Unicode ��ʽ���� (���Ƽ�)
			if (FileSave(hWnd, wFileName, wTitleName))
			{
				DoCaption(hWnd, wTitleName);
				if (FileWrite(hWndEdit, wFileName, 1))
					bSaved = true;
				else
					OkMessage(hWnd, L"δ֪����!", L"���Ϊ");
			}
			break;
		case IDM_UNDO:
			SendMessage(hWndEdit, WM_UNDO, 0, 0);
			break;
		case IDM_COPY:
			SendMessage(hWndEdit, WM_COPY, 0, 0);
			break;
		case IDM_CUT:
			SendMessage(hWndEdit, WM_CUT, 0, 0);
			break;
		case IDM_PASTE:
			SendMessage(hWndEdit, WM_PASTE, 0, 0);
			break;
		case IDM_FONT:
			if (FontChooseFont(hWnd))
				FontSetFont(hWndEdit);
			break;
		case IDM_AUTOTAB:
			bAutoTab = !bAutoTab;
			break;
		case IDM_TEST:
			if (!bSaved)
				AskAboutSave(hWnd, wTitleName);
			if (bSaved && bOpened)
			{
				sprintf_s(command, 0x200, "\"%sTest.bat\" %s %s", correntdir, aFileName, aTitleName);
				for (iYtemp = strlen(correntdir) + 8; command[iYtemp] != '.'; iYtemp++);
				for (; command[iYtemp] != ' '; iYtemp++)
					command[iYtemp] = ' ';
				for (; command[iYtemp] != '.'; iYtemp++);
				command[iYtemp] = 0;
				uptCmd = _beginthread(CmdProc, 0, command);
			}
			break;
		case IDM_BIND:
			if (bSaved && bOpened)
			{
				sprintf_s(command, 0x200, "\"%sTest.bat\" %s null", correntdir, aFileName);
				for (iYtemp = iYtemp = strlen(correntdir) + 8; command[iYtemp] != '.'; iYtemp++);
				command[iYtemp] = 0;
				uptCmd = _beginthread(CmdProc, 0, command);
			}
			else if (!bSaved)
				AskAboutSave(hWnd, wTitleName);
			break;
		case IDM_RUNLAST:
			if (bSaved && bOpened)
			{
				sprintf_s(command, 0x200, "\"%sTest.bat\" null %s", correntdir, aTitleName);
				for (iYtemp = 0; command[iYtemp] != '.'; iYtemp++);
				for (iYtemp++; command[iYtemp] != '.'; iYtemp++);
				command[iYtemp] = 0;
				uptCmd = _beginthread(CmdProc, 0, command);
			}
			break;
		case IDM_OPENCMD:
			uptCmd = _beginthread(CmdProc, 0, "cmd");
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
		//�˵���ʼ��
	case WM_INITMENUPOPUP:
		if (lParam == 1)
		{
			EnableMenuItem((HMENU)wParam, IDM_UNDO, SendMessage(hWndEdit, EM_CANUNDO, 0, 0) ? MF_ENABLED : MF_GRAYED);
			EnableMenuItem((HMENU)wParam, IDM_PASTE, IsClipboardFormatAvailable(CF_TEXT) ? MF_ENABLED : MF_GRAYED);
			iSelect = SendMessage(hWndEdit, EM_GETSEL, 0, 0);
			if (HIWORD(iSelect) == LOWORD(iSelect))
				iXtemp = MF_GRAYED;
			else
				iXtemp = MF_ENABLED;

			EnableMenuItem((HMENU)wParam, IDM_CUT, iXtemp);
			EnableMenuItem((HMENU)wParam, IDM_COPY, iXtemp);
			EnableMenuItem((HMENU)wParam, IDM_DELETE, iXtemp);
			CheckMenuItem((HMENU)wParam, IDM_AUTOTAB, bAutoTab ? MF_CHECKED : MF_UNCHECKED);
		}
		else if (lParam == 2)
		{
			EnableMenuItem((HMENU)wParam, IDM_TEST, !(bOpened));
			EnableMenuItem((HMENU)wParam, IDM_BIND, !(bOpened));
			EnableMenuItem((HMENU)wParam, IDM_RUNLAST, !(bSaved && bOpened));
		}
		break;
	case WM_SETFOCUS:
		SetFocus(hWndEdit);
		break;
	case WM_SIZE:
		MoveWindow(hWndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		break;
	case WM_CLOSE:
		if (!bSaved)
			if (!AskAboutSave(hWnd, wTitleName))
				return 0;
	case WM_DESTROY:
		//ж����Դ
		delete[] wPreviousLine;
		delete[] wCorrentLine;
		delete[] wFileName;
		delete[] aFileName;
		delete[] wTitleName;
		delete[] aTitleName;
		delete[] correntdir;
		delete[] command;
		//ɾ������
		DestroyMenu(hMenu);
		FontDeinitialize();
		//�˳�
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

VOID DoCaption(HWND hwnd, TCHAR * szTitleName)
{
	TCHAR szCaption[64 + MAX_PATH];
	wsprintf(szCaption, L"%s - %s", szTitleName, szTitle);
	SetWindowText(hwnd, szCaption);
}
VOID OkMessage(HWND hwnd, TCHAR * szMessage, TCHAR * szTitleName)
{
	MessageBox(hwnd, szMessage, szTitleName, MB_OK | MB_ICONINFORMATION);
}
bool AskAboutSave(HWND hwnd, TCHAR * szTitleName)
{
	TCHAR szBuffer[0xff];
	BOOL bSaved;
	wsprintf(szBuffer, L"�Ƿ񱣴�� %s �ĸ���?", szTitleName);
	if ((bSaved = MessageBox(hwnd, szBuffer, szTitle, MB_YESNOCANCEL | MB_ICONQUESTION)) == IDYES)
	{
		SendMessage(hwnd, WM_COMMAND, IDM_SAVE, 0);
		return true;
	}
	else if (bSaved == IDNO)
		return true;
	else
		return false;
}


//�����ֽ�char*ת��Ϊ���ֽ�wchar_t*
wchar_t* AnsiToUnicode(const char* szStr)
{
	int nLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0);
	if (nLen == 0)
	{
		return NULL;
	}
	wchar_t* pResult = new wchar_t[nLen];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen);
	return pResult;
}

//�����ֽ�wchar_t*ת��Ϊ���ֽ�char*
char* UnicodeToAnsi(const wchar_t* szStr)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL);
	if (nLen == 0)
	{
		return NULL;
	}
	char* pResult = new char[nLen];
	WideCharToMultiByte(CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL);
	return pResult;
}

char* GetExeDirectory()
{
	static char * tempA = nullptr;
	wchar_t * tempW = new wchar_t[MAX_PATH]{0};
	int i;
	GetModuleFileName(NULL, tempW, MAX_PATH);
	tempA = UnicodeToAnsi(tempW);
	for (i = 0; tempA[i] != '.'; i++);
	for (; tempA[i] != '\\'; i--)
		tempA[i] = 0;
	delete[]tempW;
	return tempA;
}

void __cdecl CmdProc(void * command)
{
	system((char *)command);
	return;
}

inline void ResetBuffer(wchar_t * Buffer, int size)
{
	int iXtemp;
	Buffer[0] = 0x3f;
	for (iXtemp = 1; iXtemp < size; iXtemp++)
		Buffer[iXtemp] = 0;
}