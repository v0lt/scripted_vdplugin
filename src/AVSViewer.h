//	VirtualDub - Video processing and capture application
//	Copyright (C) 1998-2001 Avery Lee
//
//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program; if not, write to the Free Software
//	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef f_AVSVIEWER_H
#define f_AVSVIEWER_H

#include <windows.h>

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
