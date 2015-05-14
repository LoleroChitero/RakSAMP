/*
	Updated to 0.3.7 by P3ti
*/

#include "main.h"

// Code taken from http://www.rohitab.com/discuss/topic/37441-how-to-change-color-of-selected-text-in-listbox/page__view__findpost__p__10081946

// Child Window/Control IDs
#define IDC_LBLINFO             100
#define IDC_LSTCUSTOM           101
#define IDC_INPUTBOX			102

// Globals
HWND hwnd, texthwnd, loghwnd, inputhwnd;
HINSTANCE g_hInst;
HFONT g_hfText;
HFONT g_hfInfo;

BOOL bTeleportMenuActive = NULL;
HFONT hTeleportMenuFont = NULL;
HANDLE hTeleportMenuThread = NULL;
HWND hwndTeleportMenu = NULL;

// Prototypes
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CalcWindowTextSize(HWND hWnd, LPRECT rcFit);
BOOL CenterWindow(HWND hWnd, HWND hWndCenter);

LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		// Initialize our window and create our child controls.
	case WM_CREATE:
		{
			hwnd = hWnd;

			HWND hWndChild;

			CenterWindow(hWnd, NULL);

			// Create the info text at the the top.
			hWndChild = CreateWindowEx(0, WC_STATIC, TEXT("RakSAMP " RAKSAMP_VERSION "\n" "Authors: " AUTHOR),
				SS_LEFT | WS_CHILD | WS_VISIBLE,
				0, 0, 0, 0, hWnd, (HMENU)(IDC_LBLINFO), g_hInst, NULL);
			if(!hWndChild) return -1;
			SendMessage(hWndChild, WM_SETFONT, (WPARAM)g_hfInfo, TRUE);
			texthwnd = hWndChild;

			// Create the custom/owner drawn listbox
			hWndChild = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTBOX, NULL, 
				LBS_NOINTEGRALHEIGHT | LBS_OWNERDRAWFIXED | LBS_EXTENDEDSEL |
				WS_VSCROLL | WS_CHILD | WS_TABSTOP | WS_VISIBLE,
				0, 0, 0, 0, hWnd, (HMENU)(IDC_LSTCUSTOM), g_hInst, NULL);
			if(!hWndChild) return -1;
			SendMessage(hWndChild, WM_SETFONT, (WPARAM)g_hfText, FALSE);
			loghwnd = hWndChild;

			// Create the input box.
			hWndChild = CreateWindowEx(0, WC_EDIT, NULL,
				WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER,
				0, 0, 0, 0, hWnd, (HMENU)IDC_INPUTBOX, g_hInst, NULL);
			if(!hWndChild) return -1;
			SendMessage(hWndChild, EM_LIMITTEXT, 255, 0);
			SendMessage(hWndChild, WM_SETFONT, (WPARAM)g_hfText, FALSE);
			inputhwnd = hWndChild;

			// Create the send button.
			hWndChild = CreateWindowEx(0, WC_BUTTON, TEXT("&Send"),
				BS_DEFPUSHBUTTON | BS_TEXT | WS_CHILD | WS_TABSTOP | WS_VISIBLE,
				0, 0, 0, 0, hWnd, (HMENU)IDCANCEL, g_hInst, NULL);
			if(!hWndChild) return -1;
			SendMessage(hWndChild, WM_SETFONT, (WPARAM)g_hfText, FALSE);

			HICON hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_MAIN));
			SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

			SetFocus(inputhwnd);
		}
		return 0;

		// This message is sent for each owner drawn child and only once for LBS_OWNERDRAWFIXED.
		// If we had used LBS_OWNERDRAWVARIABLE it is sent for each item.
	case WM_MEASUREITEM:
		{
			LPMEASUREITEMSTRUCT lpMeasureItem = (LPMEASUREITEMSTRUCT)lParam;
			// Is this measure request for our control?
			if(lpMeasureItem->CtlID == IDC_LSTCUSTOM)
			{
				TEXTMETRIC tm;
				HWND hWndItem = GetDlgItem(hWnd, IDC_LSTCUSTOM);
				HDC  hdcItem  = GetDC(hWndItem);

				if(GetTextMetrics(hdcItem, &tm))
					// Set the item height to that of the font + 10px padding
					lpMeasureItem->itemHeight = tm.tmInternalLeading + 
					tm.tmHeight + tm.tmExternalLeading + 8;

				ReleaseDC(hWndItem, hdcItem);
				return TRUE;
			}
		}
		break;

		// This message is sent for each owner drawn child when it needs to be rendered.
	case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;
			// Is this draw request for our control?
			if(lpDrawItem->CtlID == IDC_LSTCUSTOM)
			{
				// Get the text pointer
				LPTSTR lpText = (LPTSTR)lpDrawItem->itemData;
				COLORREF textColor = RGB(0, 0, 0);
				COLORREF bkColor = RGB(255, 255, 255);

				if(!strncmp(lpText, "[CHAT] ", 7))
					textColor = RGB(settings.bChatColorRed, settings.bChatColorGreen, settings.bChatColorBlue);

				if(!strncmp(lpText, "[CMSG] ", 7))
					textColor = RGB(settings.bCMsgRed, settings.bCMsgGreen, settings.bCMsgBlue);

				if(!strncmp(lpText, "[CP] ", 5))
					textColor = RGB(settings.bCPAlertRed, settings.bCPAlertGreen, settings.bCPAlertBlue);

				if(settings.iFind)
				{
					for(int i = 0; i < MAX_FIND_ITEMS; i++)
					{
						if(!settings.findItems[i].iExists)
							continue;

						if(strstr(lpText, settings.findItems[i].szFind))
						{
							textColor = RGB(settings.findItems[i].bTextRed,
								settings.findItems[i].bTextGreen,
								settings.findItems[i].bTextBlue);
							bkColor = RGB(settings.findItems[i].bBkRed,
								settings.findItems[i].bBkGreen,
								settings.findItems[i].bBkBlue);
						}
					}
				}

				// Set colors based on selected state
				if(lpDrawItem->itemState & ODS_SELECTED)
				{
					SetBkColor(lpDrawItem->hDC, RGB(230, 255, 255));
					SetTextColor(lpDrawItem->hDC, textColor);
				}
				else
				{
					SetBkColor(lpDrawItem->hDC, bkColor);
					SetTextColor(lpDrawItem->hDC, textColor);
				}

				// Draw the text and fill in the background at the same time
				//  with 5px offset for padding
				ExtTextOut(lpDrawItem->hDC,
					lpDrawItem->rcItem.left + 5, lpDrawItem->rcItem.top + 5,
					ETO_OPAQUE | ETO_CLIPPED, &lpDrawItem->rcItem,
					lpText, lstrlen(lpText), NULL);

				// If this item has the focus add the focus rect
				if(lpDrawItem->itemState & ODS_FOCUS)
					DrawFocusRect(lpDrawItem->hDC, &lpDrawItem->rcItem);

				return TRUE;
			}
		}
		break;

		// This message is sent for each item deleted from an owner drawn control so that 
		//  you can perform custom clean up.
	case WM_DELETEITEM:
		{
			LPDELETEITEMSTRUCT lpDeleteItem = (LPDELETEITEMSTRUCT)lParam;
			// Is this delete request for our control?
			if(lpDeleteItem->CtlID == IDC_LSTCUSTOM)
			{
				delete (LPTSTR)lpDeleteItem->itemData;
				return TRUE;
			}
		}
		break;

		// We accept this message so we can set a minimum window size. This only sets the users
		// tracking size. The window itself can always be resized smaller programmatically unless
		// you restrict it in WM_WINDOWPOSCHANGING/WM_WINDOWPOSCHANGED. 
	case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO lpInfo = (LPMINMAXINFO)lParam;
			if(lpInfo)
				lpInfo->ptMinTrackSize.x = 250, lpInfo->ptMinTrackSize.y = 300;
		}
		return 0;

		// These next two messages are better to use rather than WM_MOVE/WM_SIZE.
		// Remember WM_MOVE/WM_SIZE are from 16bit windows. In 32bit windows the window
		// manager only sends these two messages and the DefWindowProc() handler actually
		// accepts them and converts them to WM_MOVE/WM_SIZE.
		// 
		// We accept this so we can scale our controls to the client size.
	case WM_WINDOWPOSCHANGING:
	case WM_WINDOWPOSCHANGED:
		{
			HDWP hDWP;
			RECT rc;

			// Create a deferred window handle.
			if(hDWP = BeginDeferWindowPos(4))
			{
				// Deferring 4 child controls
				GetClientRect(hWnd, &rc);

				// Calculate the size needed for the static text at the top
				RECT rcText = { 10, 20, rc.right - 20, 0 } ;
				CalcWindowTextSize(GetDlgItem(hWnd, IDC_LBLINFO), &rcText);

				// Defer each window move/size until end and do them all at once.
				hDWP = DeferWindowPos(hDWP, GetDlgItem(hWnd, IDC_LBLINFO), NULL,
					10, 10, rc.right - 20, rcText.bottom,
					SWP_NOZORDER | SWP_NOREDRAW);

				// Move/Size the listbox
				hDWP = DeferWindowPos(hDWP, GetDlgItem(hWnd, IDC_LSTCUSTOM), NULL,
					10, rcText.bottom + 10, rc.right - 20, rc.bottom - rcText.bottom - 75,
					SWP_NOZORDER | SWP_NOREDRAW);

				// Move/Size the input box
				hDWP = DeferWindowPos(hDWP, GetDlgItem(hWnd, IDC_INPUTBOX), NULL,
					10, rc.bottom - 40, rc.right - 90, 30,
					SWP_NOZORDER | SWP_NOREDRAW);

				// Move/Size the send button
				hDWP = DeferWindowPos(hDWP, GetDlgItem(hWnd, IDCANCEL), NULL,
					(rc.right) - 70, rc.bottom - 40, 60, 30,
					SWP_NOZORDER | SWP_NOREDRAW);

				// Resize all windows under the deferred window handled at the same time.
				EndDeferWindowPos(hDWP);

				// We told DeferWindowPos not to redraw the controls so we can redraw
				// them here all at once.
				RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN | 
					RDW_ERASE | RDW_NOFRAME | RDW_UPDATENOW);
			}
		}
		return 0;

		// Handle the notifications of button presses.
	case WM_COMMAND:
		// If it was a button press and it came from our button.
		if(wParam == MAKELONG(IDCANCEL, BN_CLICKED) ||
			(GetFocus() == inputhwnd) && (0x8000 & GetKeyState(VK_RETURN)))
		{
			// process typed command
			if(GetWindowTextLength(inputhwnd) == 0)
				break;

			char str[512];
			SendMessage(inputhwnd, WM_GETTEXT, (WPARAM)512, (LPARAM)str);
			SendMessage(inputhwnd, WM_SETTEXT, 0, (LPARAM)"");
			RunCommand(str, 0);

			SetFocus(inputhwnd);
		}
		else if(HIWORD(wParam) == LBN_DBLCLK)
		{

		}
		break;

	case WM_DESTROY:
		// We post a WM_QUIT when our window is destroyed so we break the main message loop.
		sampDisconnect(0);
		PostQuitMessage(0);
		break;

	}

	// Not a message we wanted? No problem hand it over to the Default Window Procedure.
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Calculates the size used by text to expand a rect
BOOL CalcWindowTextSize(HWND hWnd, LPRECT rcFit) // by Napalm
{
	BOOL bResult = FALSE;
	LPTSTR lpWindowText;
	HDC hdcWindow;
	INT nLen;

	// If the window has no text return true
	nLen = GetWindowTextLength(hWnd);
	if(nLen == 0)	
		return TRUE;

	// Get the HDC for the window
	if(hdcWindow = GetDC(hWnd)){
		// Allocate room for the text
		lpWindowText = new TCHAR[nLen + 1];
		if(lpWindowText){
			// Get the window text
			if(GetWindowText(hWnd, lpWindowText, nLen + 1)){
				// Use the CALCRECT option to determine the size
				//  of the text in the current font selected into the HDC
				DrawText(hdcWindow, lpWindowText, nLen, rcFit,
					DT_LEFT | DT_WORDBREAK | DT_CALCRECT);
				bResult = TRUE;
			}
			delete lpWindowText;
		}
		ReleaseDC(hWnd, hdcWindow);
	}

	return bResult;
}

// Center window in primary monitor or owner/parent.
BOOL CenterWindow(HWND hWnd, HWND hWndCenter = NULL) // by Napalm
{
	RECT rcDlg, rcArea, rcCenter;
	HWND hWndParent;
	DWORD dwStyle, dwStyleCenter;

	// Determine owner window to center against.
	dwStyle = GetWindowLong(hWnd, GWL_STYLE);
	if(!hWndCenter)
		hWndCenter = (dwStyle & WS_CHILD) ? GetParent(hWnd) : GetWindow(hWnd, GW_OWNER);

	// Get coordinates of the window relative to its parent.
	GetWindowRect(hWnd, &rcDlg);
	if(!(dwStyle & WS_CHILD)){
		// Don't center against invisible or minimized windows.
		if(hWndCenter){
			dwStyleCenter = GetWindowLong(hWndCenter, GWL_STYLE);
			if(!(dwStyleCenter & WS_VISIBLE) || (dwStyleCenter & WS_MINIMIZE))
				hWndCenter = NULL;
		}
		// Center within screen coordinates.
		SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
		if(hWndCenter) GetWindowRect(hWndCenter, &rcCenter);
		else rcCenter = rcArea;
	}else{
		// Center within parent client coordinates.
		hWndParent = GetParent(hWnd);
		GetClientRect(hWndParent, &rcArea);
		GetClientRect(hWndCenter, &rcCenter);
		MapWindowPoints(hWndCenter, hWndParent, (LPPOINT)&rcCenter, 2);
	}

	int DlgWidth  = rcDlg.right  - rcDlg.left;
	int DlgHeight = rcDlg.bottom - rcDlg.top;

	// Find dialog's upper left based on rcCenter.
	int xLeft = (rcCenter.left + rcCenter.right)  / 2 - DlgWidth  / 2;
	int yTop  = (rcCenter.top  + rcCenter.bottom) / 2 - DlgHeight / 2;

	// If the dialog is outside the screen, move it inside.
	if(xLeft < rcArea.left) xLeft = rcArea.left;
	else if(xLeft + DlgWidth > rcArea.right) xLeft = rcArea.right - DlgWidth;
	if(yTop < rcArea.top) yTop = rcArea.top;
	else if(yTop + DlgHeight > rcArea.bottom) yTop = rcArea.bottom - DlgHeight;

	// Map screen coordinates to child coordinates.
	return SetWindowPos(hWnd, NULL, xLeft, yTop, 0, 0,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

DWORD WINAPI windowThread(PVOID)
{
	WNDCLASSEX wcex;
	HDC hdcScreen;
	MSG msg;

	// Link in comctl32.dll
	InitCommonControls();

	ZeroMemory(&msg,  sizeof(MSG));
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));

	// Register our Main Window class.
	wcex.cbSize        = sizeof(WNDCLASSEX);
	wcex.hInstance     = g_hInst;
	wcex.lpszClassName = TEXT("MainWindow");
	wcex.lpfnWndProc   = MainWindowProc;
	wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hIconSm       = wcex.hIcon;
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	if(!RegisterClassEx(&wcex))
		return 0;

	// Create a font we can later use on our controls. We use MulDiv and GetDeviceCaps to convert
	// our point size to match the users DPI setting.
	hdcScreen = GetDC(HWND_DESKTOP);

	g_hfText = CreateFont(-MulDiv(11, GetDeviceCaps(hdcScreen, LOGPIXELSY), 72), // 11pt
		0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, TEXT("Tahoma"));

	g_hfInfo = CreateFont(-MulDiv(11, GetDeviceCaps(hdcScreen, LOGPIXELSY), 85),
		0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, TEXT("Arial"));

	ReleaseDC(HWND_DESKTOP, hdcScreen);

	// Create an instance of the Main Window.
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, wcex.lpszClassName, TEXT("RakSAMP " RAKSAMP_VERSION),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CW_USEDEFAULT, CW_USEDEFAULT, 760, 520,
		HWND_DESKTOP, NULL, g_hInst, NULL);

	if(hwnd)
	{
		// Show the main window and enter the message loop.
		ShowWindow(hwnd, 1);
		UpdateWindow(hwnd);
		while(GetMessage(&msg, NULL, 0, 0))
		{
			// If the message was not wanted by the Dialog Manager dispatch it like normal.
			if(!IsDialogMessage(hwnd, &msg)){
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	// Free up our resources and return.
	DeleteObject(g_hfText);
	DeleteObject(g_hfInfo);
	ExitProcess(0);

	return 0;
}

void SetUpWindow(HINSTANCE hInstance)
{
	g_hInst = hInstance;
	CreateThread(NULL, 0, windowThread, NULL, 0, NULL);
}

LRESULT CALLBACK TeleportMenuProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND hwndEditBox = GetDlgItem(hwnd, IDE_INPUTEDIT);
	HWND hwndListBox = GetDlgItem(hwnd, IDL_LISTBOX);
	WORD wSelection;
	char szResponse[257];

	switch(msg)
	{
	case WM_CREATE:
		{
			HINSTANCE hInst = GetModuleHandle(NULL);
			
			hwndListBox = CreateWindowEx(NULL, "LISTBOX", "",
				WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL | WS_BORDER | LBS_HASSTRINGS,
				10, 10, 375, 225, hwnd, (HMENU)IDL_LISTBOX, hInst, NULL);

			for(int i = 0; i < MAX_TELEPORT_ITEMS; i++)
			{
				if(settings.TeleportLocations[i].bCreated)
				{
					int id = SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)settings.TeleportLocations[i].szName);
					SendMessage(hwndListBox, LB_SETITEMDATA, id, (LPARAM)id);
				}
			}
						
			CreateWindowEx(NULL, "BUTTON", "Teleport", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				100, 230, 100, 24, hwnd, (HMENU)IDB_BUTTON1, hInst, NULL);

			CreateWindowEx(NULL, "BUTTON", "Cancel", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				210, 230, 100, 24, hwnd, (HMENU)IDB_BUTTON2, hInst, NULL);
		}
		break;

	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDB_BUTTON1:
					wSelection = (WORD)SendMessage(hwndListBox, LB_GETCURSEL, 0, 0);
					if(wSelection != (WORD)-1)
					{
						SendMessage(hwndListBox, LB_GETTEXT, wSelection, (LPARAM)szResponse);
						useTeleport(wSelection);
						PostQuitMessage(0);
					}
					break;

				case IDB_BUTTON2:
					GetWindowText(hwndEditBox, szResponse, 257);
					PostQuitMessage(0);
					break;
			}
		}

		break;

	case WM_PAINT:
		{
			RECT rect;
			GetClientRect(hwnd, &rect);
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			HDC hdcMem = CreateCompatibleDC(hdc);
			DeleteDC(hdcMem);
			EndPaint(hwnd, &ps);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

DWORD WINAPI TeleportMenuThread(PVOID)
{
	WNDCLASSEX wc;
	MSG Msg;
	HINSTANCE hInstance = GetModuleHandle(NULL);
	RECT conRect;
	if(settings.iConsole)
		GetWindowRect(GetConsoleWindow(), &conRect);
	else
		GetWindowRect(hwnd, &conRect);

	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = TeleportMenuProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "teleportMenuWndClass";
	wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
		return 0;

	hTeleportMenuFont = CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Tahoma");

	hwndTeleportMenu = CreateWindowEx(NULL, "teleportMenuWndClass", "Teleports", NULL,
		conRect.right, conRect.top, 400, 300, NULL, NULL, hInstance, NULL);

	if(hwndTeleportMenu == NULL)
		return 0;

	ShowWindow(hwndTeleportMenu, 1);
	UpdateWindow(hwndTeleportMenu);
	SetForegroundWindow(hwndTeleportMenu);

	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	bTeleportMenuActive = 0;
	SendMessage(hwndTeleportMenu, WM_DESTROY, 0, 0);
	DestroyWindow(hwndTeleportMenu);
	UnregisterClass("teleportMenuWndClass", GetModuleHandle(NULL));
	hTeleportMenuFont = NULL;
	TerminateThread(hTeleportMenuThread, 0);

	return 0;
}