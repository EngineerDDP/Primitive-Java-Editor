// Notepad.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Notepad.h"
#include "LinkedList.h"

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明: 
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

 	// TODO:  在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_NOTEPAD, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NOTEPAD));

	// 主消息循环: 
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
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
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
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

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
//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//窗口菜单
	int wmId, wmEvent;
	//绘图
	PAINTSTRUCT ps;
	//设备环境句柄
	HDC hdc;
	//数据存储
	static int cxChar, cyChar, cxClient, cyClient,
		//滚动条
		iVertPos = 0,
		//插入符号
		xPos = 0, yPos = 0, nPos = 0,
		//字数，行数统计
		iChars = 0, iLines = 0,
		//鼠标滚动
		iDeltaPerLine = 0, iAccumDelta = 0;

	//状态判断 true：输入模式/false：浏览模式
	static bool bJustInput = false;

	//字符缓冲区
	static LinkedList Temp;

	//获取字符数据
	TEXTMETRIC tm;

	//状态栏
	POINT pFinalLine[2];
	pFinalLine[0].x = 0;

	//滚动条
	SCROLLINFO si;

	//鼠标滚轮
	ULONG ulScrollLines;

	//临时变量
	wchar_t * temp = nullptr;
	int i = 0, j = 0, k = 0;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择: 
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

//窗口初始化

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
		//获取数据
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		pFinalLine[0].y = cyClient - cyChar;
		pFinalLine[1].y = cyClient - cyChar;
		pFinalLine[1].x = cxClient;
		//滚动条设置
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

//滚动条消息

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

//鼠标消息

	case WM_LBUTTONDOWN:
		i = LOWORD(lParam);  //x轴
		j = HIWORD(lParam);  //y轴
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

//键盘消息

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
			//退格
		case '\b':
			if (nPos > 0)
			{
				Temp.Search(nPos);
				Temp.Delete();
				iChars--;
				nPos--;
			}
			break;
			//回车
		case '\r':
		case '\n':
			Temp.Search(nPos);
			Temp.Add('\n');
			iChars++;
			nPos++;
			break;
			//Tab键
		case '\t':
			Temp.Search(nPos);
			Temp.Add('\t');
			iChars++;
			nPos++;
			break;
			//Unicode字符
		default:
			Temp.Search(nPos);
			Temp.Add((TCHAR)wParam);
			iChars++;
			nPos++;
			break;
		}
		HideCaret(hWnd);
		InvalidateRect(hWnd, NULL, TRUE);
		//输入完毕 刷新状态
		bJustInput = true;
		break;

//插入符号消息

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
		//绘制窗口

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

		//重绘插入符
		if (bJustInput && yPos > cyClient - cyChar)
		{
			si.nPos++;
		}
		SetCaretPos(xPos, yPos);
		ShowCaret(hWnd);
		//重绘滚动条
		iVertPos = si.nPos;
		si.fMask = SIF_RANGE|SIF_POS;
		si.nMin = 0;
		si.nMax = iLines + 1;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
		//绘制完毕 刷新状态
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

// “关于”框的消息处理程序。
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
