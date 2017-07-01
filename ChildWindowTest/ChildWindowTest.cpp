// ChildWindowTest.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ChildWindowTest.h"

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>
#include <string>
#include <windows.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND mainWnd, childA, childB;
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
FILE *g_ic_file_cout_stream; FILE *g_ic_file_cin_stream;
LPCWSTR childClass = L"mychildwin";
int chromeSize = 30;
std::ofstream outfile;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProcChild(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
HWND makeChild(HWND Parent, int x, int y, int color, LPCWSTR name, HWND prevHWND);
void Reparent(HWND parent, HWND child);
void AdjustChildWindow(HWND parent, HWND child);
ATOM RegisterChildClass(LPCWSTR className, int color);
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

//	if (!AllocConsole()) { return 1; }
//	if (freopen_s(&g_ic_file_cout_stream, "CONOUT$", "w", stdout) != 0) { return 1; }

	outfile.open("C:\\temp\\AirNativeExt.log", std::ofstream::out | std::ofstream::app);

	if (outfile.fail()) {
		std::cout << "stream failed" << std::endl;
	}
	std::cout.rdbuf(outfile.rdbuf());

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CHILDWINDOWTEST, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	HWND chromeWidget = (HWND)0x00110572;
	HWND chromeWnd = (HWND)0x00110572;
	HWND firstChild = (HWND) 0x001B05CA;

	LONG style = GetWindowLong(firstChild, GWL_STYLE);
	style = style | WS_CLIPSIBLINGS;
	SetWindowLong(firstChild, GWL_STYLE, style);


	childA = makeChild(chromeWnd, 10, 10, 0x88ff00, L"AAA", firstChild);
	childB = makeChild(mainWnd, 10, 10, 0x8888ff, L"BBB", 0);
	Reparent(chromeWnd, childA);

	EnumChildWindows(chromeWidget, EnumWindowsProc, NULL);

//	EnumChildWindows(mainWnd, EnumWindowsProc, NULL);


	AdjustChildWindow(chromeWnd, childA);
	AdjustChildWindow(mainWnd, childB);


    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHILDWINDOWTEST));

    MSG msg;
    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
//		std::cout << "got message " << std::hex << msg.message << std::endl;
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	TCHAR szWindowText[100];
	int len = ::GetClassName(hwnd, szWindowText, 100);

	std::wstring ws(&szWindowText[0]);
	std::string s(ws.begin(), ws.end());

	std::cout << "Child window " << len << " HWND " << std::hex << hwnd << " class " << s << std::endl;

	return TRUE;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_DBLCLKS;// CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHILDWINDOWTEST));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CHILDWINDOWTEST);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

ATOM RegisterChildClass(LPCWSTR className, int color) {
	MSG msg = { 0 };
	WNDCLASSEXW wc = { 0 };

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0; // CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProcChild;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_CHILDWINDOWTEST));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(color);//(HBRUSH)(COLOR_WINDOWFRAME);
	wc.lpszMenuName = MAKEINTRESOURCEW(IDC_CHILDWINDOWTEST);
	wc.lpszClassName = className;
	wc.hIconSm = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	ATOM atom = RegisterClassExW(&wc);
	if (atom == 0) {
		std::cout << "Error RegisterClassExW " << GetLastError() << std::endl;
	}
	return atom;
}

HWND makeChild(HWND Parent, int x, int y, int color, LPCWSTR name, HWND prevHWND) {
	RegisterChildClass(name, color);
	LONG style = 0;
	if (Parent) {
		style = WS_BORDER | WS_CHILD | WS_VISIBLE;
	}
	else {
		style = WS_BORDER | WS_POPUP | WS_VISIBLE;
	}
	HWND newHWND = CreateWindowW(name, name, style, x, y, 100, 100, Parent, 0, hInst, NULL);
	if (newHWND == NULL) {
		std::cout << "Error CreateWindowW " << GetLastError() << std::endl;
	}
	else if (prevHWND != 0) {
		SetWindowPos(prevHWND, newHWND, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
//		SetWindowPos(newHWND, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
//		SetWindowPos(newHWND, NULL, 300, 300, 0, 0, SWP_NOSIZE);
	}
	return newHWND;
}

void Reparent(HWND parent, HWND child) {
	HWND hwndReturn = ::SetParent(child, parent);
	if (hwndReturn == NULL) {
		std::cout << "Error SetParent GWL_HWNDPARENT " << GetLastError() << std::endl;
	}
	LONG result = SetWindowLong(child, GWL_HWNDPARENT, reinterpret_cast<LONG>(parent));
	if (result == 0) {
		std::cout << "Error SetWindowLong GWL_HWNDPARENT " << GetLastError() << std::endl;
	}
	LONG style = GetWindowLong(child, GWL_STYLE);
	style = style & ~WS_POPUP;
	style = style | WS_CHILD;
	result = SetWindowLong(child, GWL_STYLE, style);
	if (result == 0) {
		std::cout << "Error SetWindowLong GWL_STYLE " << GetLastError() << std::endl;
	}
}

void AdjustChildWindow(HWND parent, HWND child) {
	RECT rect;
	int chromeSize = 30;
	::GetClientRect(parent, &rect);
	int x = chromeSize, y = chromeSize, w = rect.right - rect.left - 2 * chromeSize, h = rect.bottom - rect.top - 2 * chromeSize;
	std::cout << "set embedded window position " << x << ":" << y << ":" << w << ":" << h << std::endl;
	//		SetWindowPos(cWnd, pWnd, 0, 0, w, h,  SWP_FRAMECHANGED);
	MoveWindow(child, x, y, w, h, true);
}


//
//   FUNCTION: InitInstance(HINSTANCE, int)	
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowEx(WS_EX_COMPOSITED, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
      200, 200, 500, 500, nullptr, nullptr, hInstance, nullptr);


   if (!hWnd)
   {
      return FALSE;
   }

   mainWnd = hWnd;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	std::cout << "WndProc " << std::hex << message << std::endl;
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
//            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
//            EndPaint(hWnd, &ps);
        }
		return DefWindowProc(hWnd, message, wParam, lParam);
//		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
//	case WM_WINDOWPOSCHANGING: 
	case WM_SIZE : {
		std::cout << "WM_WINDOWPOSCHANGING " << std::endl;
		WORD nWidth = LOWORD(lParam); // width of client area
		WORD nHeight = HIWORD(lParam); // height of client area
		LRESULT ret = DefWindowProc(hWnd, message, wParam, lParam);
		if (hWnd == mainWnd) {
//			AdjustChildWindow(mainWnd, childB);
			int x = chromeSize, y = chromeSize, w = nWidth - 2 * chromeSize, h = nHeight - 2 * chromeSize;
			std::cout << "set embedded window position " << x << ":" << y << ":" << w << ":" << h << std::endl;
			//		SetWindowPos(cWnd, pWnd, 0, 0, w, h,  SWP_FRAMECHANGED);
			MoveWindow(childB, x, y, w, h, true);

		}
		return ret;
	  }
	default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK WndProcChild(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	std::cout << "WndProcChild " << std::hex << message << std::endl;
	return DefWindowProc(hWnd, message, wParam, lParam);
}

// Message handler for about box.
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
