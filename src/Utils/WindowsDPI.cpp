//
// Copyright (c) 2025 v0lt
//
// SPDX-License-Identifier: MIT
//

#include "stdafx.h"
#include "WindowsDPI.h"

namespace {
	UINT uSystemDPI = USER_DEFAULT_SCREEN_DPI;

	using tpGetDpiForWindow = UINT(WINAPI*)(HWND hwnd);
	tpGetDpiForWindow fnGetDpiForWindow = nullptr;

	HMODULE hDLLShcore = NULL;
	using tpGetDpiForMonitor = HRESULT(WINAPI*)(HMONITOR hmonitor, /*MONITOR_DPI_TYPE*/int dpiType, UINT* dpiX, UINT* dpiY);
	tpGetDpiForMonitor fnGetDpiForMonitor = nullptr;
}

CWindowsDPI::CWindowsDPI()
{
	HMODULE user32 = ::GetModuleHandleW(L"user32.dll");
	fnGetDpiForWindow = (tpGetDpiForWindow)::GetProcAddress(user32, "GetDpiForWindow");
	hDLLShcore = ::LoadLibraryExW(L"shcore.dll", {}, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hDLLShcore) {
		fnGetDpiForMonitor = (tpGetDpiForMonitor)::GetProcAddress(hDLLShcore, "GetDpiForMonitor");
	}

	using tpGetDpiForSystem = UINT(WINAPI*)(void);
	tpGetDpiForSystem fnGetDpiForSystem = (tpGetDpiForSystem)::GetProcAddress(user32, "GetDpiForSystem");
	if (fnGetDpiForSystem) {
		uSystemDPI = fnGetDpiForSystem();
	} else {
		HDC hdc = ::CreateCompatibleDC(NULL);
		uSystemDPI = ::GetDeviceCaps(hdc, LOGPIXELSY);
		::DeleteDC(hdc);
	}
}

CWindowsDPI::~CWindowsDPI()
{
	if (hDLLShcore) {
		FreeLibrary(hDLLShcore);
		hDLLShcore = NULL;
	}
}

UINT CWindowsDPI::DpiForWindow(HWND hwnd) const
{
	if (fnGetDpiForWindow) {
		return fnGetDpiForWindow(hwnd);
	}
	if (fnGetDpiForMonitor) {
		HMONITOR hMonitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		UINT dpiX = 0;
		UINT dpiY = 0;
		if (fnGetDpiForMonitor(hMonitor, 0 /*MDT_EFFECTIVE_DPI*/, &dpiX, &dpiY) == S_OK) {
			return dpiY;
		}
	}
	return uSystemDPI;
}