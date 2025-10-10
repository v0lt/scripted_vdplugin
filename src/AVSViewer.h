/*
 * Copyright (C) 1998-2001 Avery Lee
 * Copyright (C) 2016-2019 Anton Shekhovtsov
 * Copyright (C) 2024-2025 v0lt
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef f_AVSVIEWER_H
#define f_AVSVIEWER_H

#define AVSEDITORCLASS (szAVSEditorClassName)
#define DEFAULT_EDITOR_FONT	"Consolas"

#define REG_WINDOW_MAIN "Main"
#define REG_WINDOW_FIND "Find"
#define REG_WINDOW_JUMPTO "Jumpto"

#ifndef f_AVSVIEWER_CPP
extern const wchar_t szAVSEditorClassName[];
#endif

const int SCRIPTTYPE_NONE = 0;
const int SCRIPTTYPE_AVS = 1;
const int SCRIPTTYPE_DECOMB = 2;
const int SCRIPTTYPE_VPS = 3;
const int SCRIPTTYPE_VDSCRIPT = 4;
const int SCRIPTTYPE_MAX = SCRIPTTYPE_VDSCRIPT;

const char *const scripttypeName[SCRIPTTYPE_MAX+1] = {"None", "AviSynth", "Decomb", "VapourSynth", "VirtualDub"};

extern HWND g_ScriptEditor;

bool IsScriptType(const wchar_t *fn, int type);
int GetScriptType(const wchar_t *fn);

void LoadAVSEditorIcons();
ATOM RegisterAVSEditorClass();

class AVSEditor;
AVSEditor* CreateEditor(HWND, HWND, bool);
bool OpenCurrentFile(HWND parent);
bool HandleFilename(HWND hwnd, const wchar_t* path);
bool HandleFileOpenError(HWND hwnd, const wchar_t* path, const char* s, int source);
bool ProcessDialogs(MSG& msg);
bool ProcessHotkeys(MSG& msg);
void HandleError(const char* s, void* userData);
void AVSViewerLoadSettings(HWND hwnd, const char* name);
void AVSViewerSaveSettings(HWND hwnd, const char* name);
void AttachWindows(HWND parent);
void DetachWindows(HWND parent);
void UpdatePreferences();

#endif
