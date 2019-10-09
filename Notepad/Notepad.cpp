// Notepad.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Notepad.h"
#include "LinkedList.h"

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
	if (!InitInstance (hInstance, nCmdShow))
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

	return (int) msg.wParam;
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

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NOTEPAD));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_NOTEPAD);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
	int wmId, wmEvent;
	//��ͼ
	PAINTSTRUCT ps;
	//�豸�������
	HDC hdc;
	//���ݴ洢
	static int cxChar, cyChar, cxClient, cyClient,
		//������
		iVertPos = 0,
		//�������
		xPos = 0, yPos = 0, nPos = 0,
		//����������ͳ��
		iChars = 0, iLines = 0,
		//������
		iDeltaPerLine = 0, iAccumDelta = 0;

	//״̬�ж� true������ģʽ/false�����ģʽ
	static bool bJustInput = false;

	//�ַ�������
	static LinkedList Temp;

	//��ȡ�ַ�����
	TEXTMETRIC tm;

	//״̬��
	POINT pFinalLine[2];
	pFinalLine[0].x = 0;

	//������
	SCROLLINFO si;

	//������
	ULONG ulScrollLines;

	//��ʱ����
	wchar_t * temp = nullptr;
	int i = 0, j = 0, k = 0;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��: 
		switch (wmId)
		{
		case IDM_NEWFILE:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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

//���ڳ�ʼ��

	case WM_CREATE:
		hdc = GetDC(hWnd);
		SelectObject(hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, FIXED_PITCH, 0));
		GetTextMetrics(hdc, &tm);
		cxChar = tm.tmAveCharWidth;
		cyChar = tm.tmHeight;
		DeleteObject(SelectObject(hdc,GetStockObject(SYSTEM_FONT)));
		ReleaseDC(hWnd,hdc);
		InvalidateRect(hWnd, NULL, FALSE);
		SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &ulScrollLines, 0);
		if (ulScrollLines)
			iDeltaPerLine = WHEEL_DELTA / ulScrollLines;
		else
			iDeltaPerLine = 0;
	case WM_SIZE:
		//��ȡ����
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		pFinalLine[0].y = cyClient - cyChar;
		pFinalLine[1].y = cyClient - cyChar;
		pFinalLine[1].x = cxClient;
		//����������
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_RANGE | SIF_PAGE;
		si.nMax = iLines + 1;
		si.nMin = 0;
		si.nPage = cyClient / cyChar;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
		break;
	case WM_SETTINGCHANGE:
		SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &ulScrollLines, 0);
		if (ulScrollLines)
			iDeltaPerLine = WHEEL_DELTA / ulScrollLines;
		else
			iDeltaPerLine = 0;
		break;

//��������Ϣ

	case WM_VSCROLL:
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		GetScrollInfo(hWnd, SB_VERT, &si);
		iVertPos = si.nPos;

		switch (LOWORD(wParam))
		{
		case SB_TOP:
			si.nPos = si.nMin;
			break;
		case SB_BOTTOM:
			si.nPos = si.nMax - si.nPage;
			break;
		case SB_LINEUP:
			si.nPos--;
			break;
		case SB_LINEDOWN:
			si.nPos++;
			break;
		case SB_PAGEUP:
			si.nPos -= si.nPage;
			break;
		case SB_PAGEDOWN:
			si.nPos += si.nPage;
			break;
		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;
		default:
			break;
		}
		si.fMask = SIF_POS;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
		GetScrollInfo(hWnd, SB_VERT, &si);
		if (iVertPos != si.nPos)
		{
			ScrollWindow(hWnd, 0, cyChar*(si.nPos - iVertPos), NULL, NULL);
			UpdateWindow(hWnd);
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;

//�����Ϣ

	case WM_LBUTTONDOWN:
		i = LOWORD(lParam);  //x��
		j = HIWORD(lParam);  //y��
		k = (j / cyChar + iVertPos - 1)*(cxClient / cxChar + 1) + i / cxChar + 1;
		nPos = k > iChars ? iChars : k;
		InvalidateRect(hWnd,NULL,TRUE);
		break;
	case WM_MOUSEWHEEL:
		if (iDeltaPerLine)
		{
			iAccumDelta += (short)HIWORD(wParam);
			while (iAccumDelta >= iDeltaPerLine)
			{
				SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, 0);
				iAccumDelta -= iDeltaPerLine;
			}
			while (iAccumDelta <= -iDeltaPerLine)
			{
				SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, 0);
				iAccumDelta += iDeltaPerLine;
			}
		}
		break;

//������Ϣ

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_F5:
			break;
		case VK_UP:
			if (nPos / (cxClient/cxChar))
				nPos -= cxClient / cxChar + 1;
			HideCaret(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case VK_DOWN:
			if ((iChars - nPos) / (cxClient/cxChar))
				nPos += cxClient / cxChar + 1;
			else
				nPos = iChars;
			HideCaret(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case VK_LEFT:
			if (nPos)
				nPos--;
			HideCaret(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case VK_RIGHT:
			if (nPos < iChars)
				nPos++;
			HideCaret(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
		break;
	case WM_CHAR:
		switch (wParam)
		{
			//�˸�
		case '\b':
			if (nPos > 0)
			{
				Temp.Search(nPos);
				Temp.Delete();
				iChars--;
				nPos--;
			}
			break;
			//�س�
		case '\r':
		case '\n':
			Temp.Search(nPos);
			Temp.Add('\n');
			iChars++;
			nPos++;
			break;
			//Tab��
		case '\t':
			Temp.Search(nPos);
			Temp.Add('\t');
			iChars++;
			nPos++;
			break;
			//Unicode�ַ�
		default:
			Temp.Search(nPos);
			Temp.Add((TCHAR)wParam);
			iChars++;
			nPos++;
			break;
		}
		HideCaret(hWnd);
		InvalidateRect(hWnd, NULL, TRUE);
		//������� ˢ��״̬
		bJustInput = true;
		break;

//���������Ϣ

	case WM_SETFOCUS:
		SetCaretPos(xPos, yPos);
		CreateCaret(hWnd, NULL, 1, cyChar - 1);
		ShowCaret(hWnd);
		break;
	case WM_KILLFOCUS:
		HideCaret(hWnd);
		DestroyCaret();
		break;

	case WM_PAINT:
		//���ƴ���

		hdc = BeginPaint(hWnd, &ps);
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_POS;
		GetScrollInfo(hWnd, SB_VERT, &si);
		SelectObject(hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, FIXED_PITCH, 0));

		Temp.Search(TO_THE_FIRST);
		iLines = 0;
		k = 0;
		xPos = 0;
		yPos = 0;

		for (j = -si.nPos, i = 0; iChars;j++)
		{
			for (i = 0;
				k == nPos ? xPos = i*cxChar, yPos = j*cyChar : 1,
				i < cxClient / cxChar && Temp.Read(&temp);
				k++)
			{
				switch (*temp)
				{
				case '\n':
					i = 0;
					j++;
					iLines++;
					break;
				case '\t':
					i += 4;
					break;
				default:
					if (j < 0)
					{
						i++;
						break;
					}
					if (*temp > 0x4E00 && *temp < 0x9FBF)
					{
						TextOut(hdc, i*cxChar, j*cyChar, temp, 1);
						i += 2;
					}
					else
					{
						TextOut(hdc, i*cxChar, j*cyChar, temp, 1);
						i += 1;
					}
					break;
				}
			}
			k++;
			iLines++;
			if (!Temp.Read(&temp))
				break;
		}

		DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
		EndPaint(hWnd, &ps);

		//�ػ�����
		if (bJustInput && yPos > cyClient - cyChar)
		{
			si.nPos++;
		}
		SetCaretPos(xPos, yPos);
		ShowCaret(hWnd);
		//�ػ������
		iVertPos = si.nPos;
		si.fMask = SIF_RANGE|SIF_POS;
		si.nMin = 0;
		si.nMax = iLines + 1;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
		//������� ˢ��״̬
		bJustInput = false;
		break;
	case WM_DESTROY:
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
