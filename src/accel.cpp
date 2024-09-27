//	VirtualDubMod - Video processing and capture application based on VirtualDub by Avery Lee
//	Copyright (C) 1998-2002 VirtualDubMod Developers
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

//////////////////////////////////////////////////////////////////////////////
// Configurable hotkeys for main window, capture window and script editor	//
// Tobias Minich, November 2002												//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//																			//
// IMPORTANT NOTICE FOR DEVELOPERS!!                                        //
//																			//
// - Accelerator tables for main, capture & script editor are gone now,		//
//	 being replaced by this code. It is no longer necessary to put hotkeys	//
//	 into the menu resource.												//
//																			//
// - Whenever you add a menu item, include it here. Some user might want to	//
//	 define a hotkey for it.												//
//																			//
// - To add a new item:														//
//		* APPEND the menu id TO THE END of the respective				 	//
//		  g_accel____Command array.											//
//		  (NEVER INSERT! This will completely fubar changes made by the		//
//		  user!)															//
//		* append a default keymapping to g_accel____Default. I hope what's	//
//		  already there is enough to figure out how to do it.				//
//		* increase the respective VDM_ACCEL_____COUNT define in accel.h by	//
//		  the number of items you added.									//
//		* if you added a menu item, thats all. If you want to define a hot-	//
//		  key without a menu item do as above but you also have to add a	//
//		  string resource with the same id describing what the key does.	//
//		  (see ID_AVS_STARTCOMPLETE for an example)							//
//																			//
// - To remove an item:														//
//		* NEVER EVER DELETE IT! (Thats for the same reasons mentioned above)//
//		* Replace the command by ID_COMMAND_DEPRECATED (defined as 0)		//
//		* Put zeros as default keybinding.									//
//																			//
// - If you change a menu id, simply do so here								//
//																			//
// I hope this is clear enough. If you have questions, dont hesitate to ask //
//																			//
//////////////////////////////////////////////////////////////////////////////


#pragma warning(disable : 4786)

#define f_ACCEL_CPP

#include "stdafx.h"

#include "resource.h"
#include "accel.h"
//#include "gui.h"
#include <string>

extern HINSTANCE g_hInst;

// Script Editor

WORD g_accelAVSCommand[VDM_ACCEL_AVS_COUNT] = {
	ID_AVS_COMMENT_LINES,
	ID_AVS_INSERT_FRAMESET,
	ID_FILE_NEW,
	ID_FILE_OPEN,
	ID_AVS_INSERT_POS,
	ID_AVS_INSERT_RANGE,
	ID_AVS_FILE_SAVE,
	ID_AVS_INSERT_TRIM,
	ID_AVS_UNCOMMENT_LINES,
	ID_HELP_KEYS,
	ID_REFRESH,
	ID_AVS_SAVE_OPEN,
	ID_AVS_STARTCOMPLETE,
	ID_EDIT_REDO,
	ID_EDIT_UNDO,
	ID_AVS_SCRIPT_NONE,
	ID_AVS_SCRIPT_AVS,
	ID_AVS_SCRIPT_VPS,
	ID_AVS_SCRIPT_VDSCRIPT,
	ID_FILE_REVERT,
	ID_FILE_EXIT,
	ID_AVS_EDIT_LINE,
	ID_AVS_EDIT_OPTIONS,
	ID_HELP_TOFF,
	ID_HELP_AVISYNTH,
	ID_EDIT_FIND,
	ID_EDIT_FINDNEXT,
	ID_AVS_SCRIPT_DECOMB,
	ID_AVS_INSERT_FILENAME,
	//ID_AVS_INSERT_CROP,
	ID_EDIT_FINDPREV,
	ID_EDIT_GOTO
};

extern const ACCELKEYTABLE_AVS g_accelAVSDefault = {{
	{{{FVIRTKEY|FALT|FCONTROL|FSHIFT,	VK_C},{0, 0}}},							//ID_AVS_COMMENT_LINES,
	{{{FVIRTKEY|FCONTROL,				VK_I},{0, 0}}},							//ID_AVS_INSERT_FRAMESET,
	{{{FVIRTKEY|FCONTROL,				VK_N},{0, 0}}},							//ID_FILE_NEW,
	{{{FVIRTKEY|FCONTROL,				VK_O},{0, 0}}},							//ID_FILE_OPEN,
	{{{FVIRTKEY|FCONTROL,				VK_P},{0, 0}}},							//ID_AVS_INSERT_POS,
	{{{FVIRTKEY|FCONTROL,				VK_R},{0, 0}}},							//ID_AVS_INSERT_RANGE,
	{{{FVIRTKEY|FCONTROL,				VK_S},{FVIRTKEY,			VK_F2}}},		//ID_AVS_FILE_SAVE,
	{{{FVIRTKEY|FCONTROL,				VK_T},{0, 0}}},							//ID_AVS_INSERT_TRIM,
	{{{FVIRTKEY|FALT|FCONTROL|FSHIFT,	VK_U},{0, 0}}},							//ID_AVS_UNCOMMENT_LINES,
	{{{FVIRTKEY,						VK_F1},{0, 0}}},						//ID_HELP_KEYS,
	{{{FVIRTKEY,						VK_F5},{0, 0}}},						//ID_REFRESH,
	{{{FVIRTKEY,						VK_F7},{0, 0}}},						//ID_AVS_SAVE_OPEN,
	{{{FVIRTKEY|FCONTROL,				VK_SPACE},{FVIRTKEY|FSHIFT,	VK_SPACE}}},	//ID_AVS_STARTCOMPLETE,
	{{{FVIRTKEY|FCONTROL,				VK_Y},{0, 0}}},							//ID_EDIT_REDO,
	{{{FVIRTKEY|FCONTROL,				VK_Z},{0, 0}}},							//ID_EDIT_UNDO};
	{{{FVIRTKEY|FCONTROL,				VK_0},{0, 0}}},							//ID_AVS_SCRIPT_NONE,
	{{{FVIRTKEY|FCONTROL,				VK_1},{0, 0}}},							//ID_AVS_SCRIPT_AVS
	{{{FVIRTKEY|FCONTROL,				VK_3},{0, 0}}},							//ID_AVS_SCRIPT_VPS
	{{{FVIRTKEY|FCONTROL,				VK_4},{0, 0}}},							//ID_AVS_SCRIPT_VPS
	{{{0,								0},{0, 0}}},							//ID_FILE_REVERT
	{{{FVIRTKEY|FCONTROL,				VK_Q},{0, 0}}},							//ID_FILE_EXIT
	{{{0,								0},{0, 0}}},							//ID_AVS_EDIT_LINE
	{{{0,								0},{0, 0}}},							//ID_AVS_EDIT_OPTIONS
	{{{0,								0},{0, 0}}},							//ID_HELP_TOFF
	{{{0,								0},{0, 0}}},							//ID_HELP_AVISYNTH
	{{{FVIRTKEY|FCONTROL,				VK_F},{0, 0}}},							//ID_EDIT_FIND
	{{{FVIRTKEY,						VK_F3},{0, 0}}},						//ID_EDIT_FINDNEXT
	{{{FVIRTKEY|FCONTROL,				VK_2},{0, 0}}},							//ID_AVS_SCRIPT_DECOMB
	{{{FVIRTKEY|FCONTROL|FSHIFT,		VK_O},{0, 0}}},							//ID_INSERT_FILENAME
	//{{{FVIRTKEY|FCONTROL|FSHIFT,		VK_C},{0, 0}}},							//ID_INSERT_CROP
	{{{FVIRTKEY|FSHIFT,					VK_F3},{0, 0}}},						//ID_EDIT_FINDPREV
	{{{FVIRTKEY|FCONTROL,				VK_G},{0, 0}}},							//ID_EDIT_GOTO
}};

ACCELKEYTABLE_AVS g_accelAVS = g_accelAVSDefault;
ACCEL g_accelAVSAccelerator[VDM_ACCEL_AVS_COUNT*2];
//char g_accelAVSDescription[VDM_ACCEL_AVS_COUNT][255];

// General

char g_accelKEYDescription[256][255];
WORD g_accelKEYKeycode[256];

int GetOffsetFromIDAVS(WORD id) {
	for(int i=0; i<VDM_ACCEL_AVS_COUNT; i++) {
		if (id==g_accelAVSCommand[i]) return i;
	}
	return -1;
}

/*
void HandleMenuAVS(std::string str, HMENU mnu) {
	std::string locstr = str;
	char buf[255];
	int c = GetMenuItemCount(mnu);
	for(int i=0; i<c; i++) {
		WORD id = GetMenuItemID(mnu, i);
		GetMenuString(mnu, i, (char *) &buf, 255, MF_BYPOSITION);
		locstr += "\\";
		locstr += buf;
		int pos = GetOffsetFromIDAVS(id);
		if (pos>=0) {
			strcpy((char *) &g_accelAVSDescription[pos][0], locstr.c_str());
		}
		if (HMENU submnu = GetSubMenu(mnu, i)) HandleMenuAVS(locstr, submnu);
		locstr = str;
	}
}

void InitDescriptions() {
	char buffer[255];
	int i;
	int c = 0;
	FillMemory(&g_accelKEYKeycode, sizeof(g_accelKEYKeycode), 0);
	g_accelKEYKeycode[c] = -1;
	strcpy(g_accelKEYDescription[c], "NONE");
	for(i=0; i<=255; i++) {
		if (LoadString(g_hInst, 5000+i, (char *) &buffer, 255)>0) {
			c++;
			g_accelKEYKeycode[c] = i;
			strcpy(g_accelKEYDescription[c], buffer);
		}
	}

	FillMemory(&g_accelAVSDescription, sizeof(g_accelAVSDescription), 0);
	HMENU menu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_AVSVIEWER_MENU));
	std::string str = "";
	HandleMenuAVS(str, menu);
	DestroyMenu(menu);
	for(i=0; i<VDM_ACCEL_AVS_COUNT; i++) {
		if (g_accelAVSDescription[i][0]==0) {
	 		LoadString(g_hInst, IDS_SHORTCUT_NOMENU, (char *) &buffer, 255);
			strcpy(g_accelAVSDescription[i], "\\");
			strcat(g_accelAVSDescription[i], buffer);
			strcat(g_accelAVSDescription[i], "\\");
	 		LoadString(g_hInst, g_accelAVSCommand[i], (char *) &buffer, 255);
			strcat(g_accelAVSDescription[i], buffer);
		} else {
			str = g_accelAVSDescription[i];
			for(c=0; c<str.length(); c++) {
				if (str[c]=='&') str.erase(c, 1);
			}
			strcpy(g_accelAVSDescription[i], str.c_str());
		}
	}
}
*/

HACCEL CreateAVSAccelerators() {
	ACCEL *avsAccel = (LPACCEL) &g_accelAVSAccelerator;
	int c = -1;
	ACCELKEY *avsKey;
	for(int i=0; i<VDM_ACCEL_AVS_COUNT; i++) {
		avsKey = &g_accelAVS.command[i].altkey[0];
		if (avsKey->key>0) {
			c++;
			(avsAccel + c)->fVirt = avsKey->fVirt;
			(avsAccel + c)->key = avsKey->key;
			(avsAccel + c)->cmd = g_accelAVSCommand[i];
		}
		avsKey = &g_accelAVS.command[i].altkey[1];
		if (avsKey->key>0) {
			c++;
			(avsAccel + c)->fVirt = avsKey->fVirt;
			(avsAccel + c)->key = avsKey->key;
			(avsAccel + c)->cmd = g_accelAVSCommand[i];
		}
	}
	return CreateAcceleratorTable(avsAccel, c+1);
}

void GetKeyString(BYTE fVirt, WORD key, wchar_t *buffer, int len) {
	bool bRem = false;
	*buffer = 0;
	wchar_t buf[255];
	wchar_t b[2] = L"\0";
	if (fVirt & FALT) {
		if (LoadStringW(g_hInst, 5018, (wchar_t*) &buf, 255)>0) {
			wcsncat(buffer, buf, len);
			wcsncat(buffer, L"+", len-wcslen(buffer));
			bRem = true;
		}
	}
	if (fVirt & FCONTROL) {
		if (LoadStringW(g_hInst, 5017, (wchar_t*) &buf, 255)>0) {
			wcsncat(buffer, buf, len-wcslen(buffer));
			wcsncat(buffer, L"+", len-wcslen(buffer));
			bRem = true;
		}
	}
	if (fVirt & FSHIFT) {
		if (LoadStringW(g_hInst, 5016, (wchar_t*) &buf, 255)>0) {
			wcsncat(buffer, buf, len-wcslen(buffer));
			wcsncat(buffer, L"+", len-wcslen(buffer));
			bRem = true;
		}
	}
	if (bRem) *(buffer + (wcslen(buffer)-1)) = '-';
	if (fVirt & FVIRTKEY) {
		if (LoadStringW(g_hInst, 5000+key, (wchar_t*) &buf, 255)>0)
			wcsncat(buffer, buf, len-wcslen(buffer));
	} else {
		b[0] = (wchar_t)key;
		if (key!=0) wcscat(buffer, b);
	}
}


#define WIN95_MENUITEMINFO_SIZE (offsetof(MENUITEMINFO, cch) + sizeof(UINT))


HMENU CreateAVSMenu() {
	HMENU menu = LoadMenuW(g_hInst, MAKEINTRESOURCEW(IDR_AVSVIEWER_MENU));
	wchar_t buffer[255];
	wchar_t buf[255];
	MENUITEMINFOW mi = {sizeof(MENUITEMINFOW)};
	mi.fMask = MIIM_TYPE;
	mi.dwTypeData = buffer;

	for(int i=0; i<VDM_ACCEL_AVS_COUNT; i++) {
		if (g_accelAVS.command[i].altkey[0].key>0) {
			mi.cch = 255;
			GetMenuItemInfoW(menu, g_accelAVSCommand[i], MF_BYCOMMAND, &mi);
			wcscat(buffer, L"\t");
			GetKeyString(g_accelAVS.command[i].altkey[0].fVirt, g_accelAVS.command[i].altkey[0].key, (wchar_t*) &buf, 255);
			wcscat(buffer, buf);
			if (g_accelAVS.command[i].altkey[1].key>0) {
				wcscat(buffer, L", ");
				GetKeyString(g_accelAVS.command[i].altkey[1].fVirt, g_accelAVS.command[i].altkey[1].key, (wchar_t*) &buf, 255);
				wcscat(buffer, buf);
			}
			SetMenuItemInfoW(menu, g_accelAVSCommand[i], MF_BYCOMMAND, &mi);
		}
	}
	return menu;
}

