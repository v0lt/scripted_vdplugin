/*
 * Copyright (C) 2016-2019 Anton Shekhovtsov
 * Copyright (C) 2024-2025 v0lt
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "stdafx.h"
#include "prefs.h"
#include "AVSViewer.h"
#include "resource.h"
#include "Helper.h"

extern HINSTANCE g_hInst;
VDubModPreferences2 g_VDMPrefs;

void LoadPrefs()
{
	g_VDMPrefs.m_bScriptEditorSingleInstance = 1;
	g_VDMPrefs.m_bScriptEditorAutoPopup = 0;
	g_VDMPrefs.m_bWrapLines = 0;
	g_VDMPrefs.mAVSViewerFontSize = 14;
	g_VDMPrefs.mAVSViewerFontFace = L"Consolas";

	HKEY key;
	LSTATUS lRes = RegOpenKeyA(HKEY_CURRENT_USER, REG_KEY_APP, &key);
	if (lRes == ERROR_SUCCESS) {
		DWORD dwValue;
		DWORD dwType;
		ULONG nBytes = sizeof(DWORD);

		lRes = ::RegQueryValueExA(key, "singleInstance", nullptr, &dwType, reinterpret_cast<LPBYTE>(&dwValue), &nBytes);
		if (lRes == ERROR_SUCCESS && dwType == REG_DWORD) {
			g_VDMPrefs.m_bScriptEditorSingleInstance = !!dwValue;
		}

		lRes = ::RegQueryValueExA(key, "autoPopup", nullptr, &dwType, reinterpret_cast<LPBYTE>(&dwValue), &nBytes);
		if (lRes == ERROR_SUCCESS && dwType == REG_DWORD) {
			g_VDMPrefs.m_bScriptEditorAutoPopup = !!dwValue;
		}

		lRes = ::RegQueryValueExA(key, "wrapLines", nullptr, &dwType, reinterpret_cast<LPBYTE>(&dwValue), &nBytes);
		if (lRes == ERROR_SUCCESS && dwType == REG_DWORD) {
			g_VDMPrefs.m_bWrapLines = !!dwValue;
		}

		lRes = ::RegQueryValueExA(key, "fontsize", nullptr, &dwType, reinterpret_cast<LPBYTE>(&dwValue), &nBytes);
		if (lRes == ERROR_SUCCESS && dwType == REG_DWORD) {
			g_VDMPrefs.mAVSViewerFontSize = dwValue;
		}

		lRes = ::RegQueryValueExW(key, L"fontface", nullptr, &dwType, nullptr, &nBytes);
		if (lRes == ERROR_SUCCESS && dwType == REG_SZ) {
			std::wstring str(nBytes, 0);
			lRes = ::RegQueryValueExW(key, L"fontface", nullptr, &dwType, reinterpret_cast<LPBYTE>(str.data()), &nBytes);
			if (lRes == ERROR_SUCCESS && dwType == REG_SZ) {
				g_VDMPrefs.mAVSViewerFontFace = str;
			}
		}

		RegCloseKey(key);
	}
}

void SavePrefs()
{
	HKEY key;
	LSTATUS lRes = RegCreateKeyA(HKEY_CURRENT_USER, REG_KEY_APP, &key);
	if (lRes == ERROR_SUCCESS) {
		DWORD dwValue;

		dwValue = g_VDMPrefs.m_bScriptEditorSingleInstance;
		lRes = ::RegSetValueExA(key, "singleInstance", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&dwValue), sizeof(DWORD));

		dwValue = g_VDMPrefs.m_bScriptEditorAutoPopup;
		lRes = ::RegSetValueExA(key, "autoPopup", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&dwValue), sizeof(DWORD));

		dwValue = g_VDMPrefs.m_bWrapLines;
		lRes = ::RegSetValueExA(key, "wrapLines", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&dwValue), sizeof(DWORD));

		dwValue = g_VDMPrefs.mAVSViewerFontSize;
		lRes = ::RegSetValueExA(key, "fontsize", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&dwValue), sizeof(DWORD));

		std::wstring str(g_VDMPrefs.mAVSViewerFontFace);
		lRes = ::RegSetValueExW(key, L"fontface", 0, REG_SZ, reinterpret_cast<const BYTE*>(str.c_str()), (DWORD)((str.size() + 1) * sizeof(wchar_t)));

		RegCloseKey(key);
	}
}

void ShowPrefs(HWND parent)
{
	VDDialogPrefsScriptEditor editor(g_VDMPrefs);
	LRESULT r = DialogBoxParamW(g_hInst, MAKEINTRESOURCEW(IDD_OPTIONS), parent, editor.DlgProc, (LPARAM)&editor);
	if (r==0) {
		g_VDMPrefs = editor.mPrefs;
		SavePrefs();
		UpdatePreferences();
	}
}

INT_PTR CALLBACK VDDialogPrefsScriptEditor::DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	VDDialogPrefsScriptEditor *obj = (VDDialogPrefsScriptEditor *)GetWindowLongPtrW(hdlg, DWLP_USER);

	switch(msg) {
	case WM_INITDIALOG:
		SetWindowLongPtrW(hdlg, DWLP_USER, lParam);
		obj = (VDDialogPrefsScriptEditor *)lParam;
		obj->mhwnd = hdlg;
		SendDlgItemMessageW(hdlg, IDC_SINGLEINSTANCE, BM_SETCHECK, obj->mPrefs.m_bScriptEditorSingleInstance ? BST_CHECKED:BST_UNCHECKED, 0);
		SendDlgItemMessageW(hdlg, IDC_AUTOPOPUP, BM_SETCHECK, obj->mPrefs.m_bScriptEditorAutoPopup ? BST_CHECKED:BST_UNCHECKED, 0);
		SendDlgItemMessageW(hdlg, IDC_WRAPLINES, BM_SETCHECK, obj->mPrefs.m_bWrapLines ? BST_CHECKED:BST_UNCHECKED, 0);
		obj->SetFontLabel();
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam)) {
			case IDOK:
				obj->mPrefs.m_bScriptEditorSingleInstance = SendDlgItemMessageW(hdlg, IDC_SINGLEINSTANCE, BM_GETCHECK, 0, 0)==BST_CHECKED;
				obj->mPrefs.m_bScriptEditorAutoPopup = SendDlgItemMessageW(hdlg, IDC_AUTOPOPUP, BM_GETCHECK, 0, 0)==BST_CHECKED;
				obj->mPrefs.m_bWrapLines = SendDlgItemMessageW(hdlg, IDC_WRAPLINES, BM_GETCHECK, 0, 0)==BST_CHECKED;
				EndDialog(hdlg, 0);
				return TRUE;
			case IDCANCEL:
				EndDialog(hdlg, 1);
				return TRUE;
			case IDC_FONT:
				obj->AVSViewerChooseFont();
				return TRUE;
		}
		break;
	}

	return FALSE;
}

void VDDialogPrefsScriptEditor::SetFontLabel()
{
	std::wstring font_label = std::format(L"{}, {} pt", mPrefs.mAVSViewerFontFace.c_str(), mPrefs.mAVSViewerFontSize);

	SetDlgItemTextW(mhwnd, IDC_FONT_TEXT, font_label.c_str());
}

void VDDialogPrefsScriptEditor::InitFont(HWND hwnd, LPLOGFONTW lplf)
{
	HDC hdc = GetDC(hwnd);
	ZeroMemory(lplf, sizeof(*lplf));
	lplf->lfCharSet = (BYTE)GetTextCharset(hdc);
	lplf->lfOutPrecision = OUT_DEFAULT_PRECIS;
	lplf->lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lplf->lfQuality = DEFAULT_QUALITY;
	lplf->lfPitchAndFamily = DEFAULT_PITCH;
	lplf->lfWeight = FW_NORMAL;
	lplf->lfHeight = -MulDiv(mPrefs.mAVSViewerFontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);

	ReleaseDC(hwnd, hdc);

	wcscpy_s(lplf->lfFaceName, mPrefs.mAVSViewerFontFace.c_str());
}

void VDDialogPrefsScriptEditor::AVSViewerChooseFont()
{
	LOGFONTW lf;
	InitFont(mhwnd, &lf);

	CHOOSEFONTW cf = { sizeof(CHOOSEFONTW) };
	cf.hwndOwner = mhwnd;
	cf.lpLogFont = &lf;
	cf.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT | CF_FIXEDPITCHONLY | CF_FORCEFONTEXIST;
	cf.nFontType = SCREEN_FONTTYPE;

	if (!::ChooseFont(&cf)) { // ChooseFontW cannot be used directly (see documentation).
		return;
	}

	mPrefs.mAVSViewerFontSize = cf.iPointSize / 10;
	mPrefs.mAVSViewerFontFace = lf.lfFaceName;

	SetFontLabel();
}
