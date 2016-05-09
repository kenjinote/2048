#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include <atlbase.h>
#include <atlhost.h>
#include "resource.h"

TCHAR szClassName[] = TEXT("2048");
CComModule g_Module;
IOleInPlaceActiveObject* pOleInPlaceActiveObject;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static CComQIPtr<IWebBrowser2>pWebBrowser2;
	static HWND	hBrowser;
	switch (msg)
	{
	case WM_CREATE:
		::CoInitialize(0);
		g_Module.Init(0, ((LPCREATESTRUCT)lParam)->hInstance);
		AtlAxWinInit();
		hBrowser = CreateWindow(TEXT("AtlAxWin140"), TEXT("Shell.Explorer.2"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		if (hBrowser)
		{
			CComPtr<IUnknown>punkIE;
			if (AtlAxGetControl(hBrowser, &punkIE) == S_OK)
			{
				pWebBrowser2 = punkIE;
				if (pWebBrowser2)
				{
					if (SUCCEEDED(pWebBrowser2->QueryInterface(IID_IOleInPlaceActiveObject, (void**)&pOleInPlaceActiveObject)))
					{
						TCHAR szModulePath[MAX_PATH];
						GetModuleFileName(0, szModulePath, MAX_PATH);
						TCHAR szNavigateToResource[1024];
						wsprintf(szNavigateToResource, L"res://%s/%d", szModulePath, IDR_HTML1);
						pWebBrowser2->Navigate(szNavigateToResource, 0, 0, 0, 0);
						return 0;
					}
				}
			}
		}
		return -1;
	case WM_SETFOCUS:
		SetFocus(hBrowser);
		break;
	case WM_SIZE:
		MoveWindow(hBrowser, 0, 0, LOWORD(lParam), HIWORD(lParam), 0);
		break;
	case WM_DESTROY:
		pWebBrowser2.Release();
		AtlAxWinTerm();
		g_Module.Term();
		::CoUninitialize();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = { 0, WndProc, 0, 0, hInstance, LoadIcon(hInstance,(LPCTSTR)IDI_ICON1), 0, 0, 0, szClassName };
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(szClassName, szClassName, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, 0, 0, hInstance, 0);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		if (pOleInPlaceActiveObject->TranslateAccelerator(&msg) != S_OK)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}
