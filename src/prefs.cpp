/*
 * Copyright (C) 2016-2019 Anton Shekhovtsov
 * Copyright (C) 2024 v0lt
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "stdafx.h"
#include <vd2/system/registry.h>
#include "prefs.h"
#include "AVSViewer.h"
#include "resource.h"

extern HINSTANCE g_hInst;
VDubModPreferences2 g_VDMPrefs;

void LoadPrefs()
{
	VDRegistryKey key(REG_KEY_APP);

	g_VDMPrefs.m_bScriptEditorSingleInstance = key.getInt("singleInstance", 1);
	g_VDMPrefs.m_bScriptEditorAutoPopup = key.getInt("autoPopup", 0);
	g_VDMPrefs.m_bWrapLines = key.getInt("wrapLines", 0);

	g_VDMPrefs.mAVSViewerFontSize = key.getInt("fontsize", 14);

	if (!key.getString("fontface", g_VDMPrefs.mAVSViewerFontFace)) {
		g_VDMPrefs.mAVSViewerFontFace = L"Consolas";
	}
}

void SavePrefs()
{
	VDRegistryKey key(REG_KEY_APP);

	key.setInt("singleInstance", g_VDMPrefs.m_bScriptEditorSingleInstance);
	key.setInt("autoPopup", g_VDMPrefs.m_bScriptEditorAutoPopup);
	key.setInt("wrapLines", g_VDMPrefs.m_bWrapLines);

	key.setInt("fontsize", g_VDMPrefs.mAVSViewerFontSize);
	key.setString("fontface", g_VDMPrefs.mAVSViewerFontFace.c_str());
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
	VDDialogPrefsScriptEditor *obj = (VDDialogPrefsScriptEditor *)GetWindowLongPtr(hdlg, DWLP_USER);

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
	const wchar_t *s = mPrefs.mAVSViewerFontFace.c_str();
	SetDlgItemTextW(mhwnd, IDC_FONT_TEXT, VDswprintf(L"%s, %d pt", 2, &s, &mPrefs.mAVSViewerFontSize).c_str());
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
