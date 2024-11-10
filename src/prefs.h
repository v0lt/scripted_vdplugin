/*
 * Copyright (C) 2016-2019 Anton Shekhovtsov
 * Copyright (C) 2024 v0lt
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef prefs_header
#define prefs_header

#include <vd2/system/VDString.h>

#define REG_KEY_APP "Software\\VirtualDub.org\\Script Editor"

struct VDubModPreferences2 {
	char		m_bScriptEditorSingleInstance;
	char		m_bScriptEditorAutoPopup;
	char		m_bWrapLines;
	uint32		mAVSViewerFontSize;
	VDStringW	mAVSViewerFontFace;
} extern g_VDMPrefs;

class VDDialogPrefsScriptEditor {
public:
	HWND mhwnd = nullptr;

	VDubModPreferences2& mPrefs;
	VDDialogPrefsScriptEditor(VDubModPreferences2& p) : mPrefs(p) {}
	static INT_PTR CALLBACK DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);

	void SetFontLabel();
	void InitFont(HWND hwnd, LPLOGFONTW lplf);
	void AVSViewerChooseFont();
};

void ShowPrefs(HWND parent);
void LoadPrefs();
void SavePrefs();

#endif
