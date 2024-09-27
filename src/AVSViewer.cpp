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

// *******************************************************************
// *** AVS Modification 										   ***
// *** Tobias Minich, August 2002								   ***
// ***															   ***
// *** Since I'm a c++ newbie, i took the HexEditor as a template. ***
// *** Thanks to Toff for cleaning it up :)						   ***

// Tweakable line modification by christophe.paris@free.fr (Toff)

#pragma warning(disable : 4786)

#include "stdafx.h"

#define f_AVSVIEWER_CPP

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <crtdbg.h>

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include <tchar.h>

#include <vector>
#include <set>
#include <algorithm>
#include <string>

#define INCLUDE_DEPRECATED_FEATURES
#include "Scintilla.h"
#include "SciLexer.h"
//#include "avsconst.h"

#include "resource.h"
//#include "oshelper.h"
//#include "gui.h"
//#include <vd2/system/Error.h>
//#include <vd2/system/list.h>
#include "prefs.h"

#include "AVSViewer.h"
#include "api.h"
//#include "ProgressDialog.h"
//#include "virtualdubmod_messages.h"
#include "accel.h"
#include <vd2/system/vdtypes.h>
#include <vd2/system/text.h>
//#include "avisynth.h"
//#include "avisynth_interface.h"
//#include "Tdll.h"
#include "CAviSynth.h"

//#include "I18N.h"
//#include "modplus.h"

extern HINSTANCE g_hInst;

//extern HMODULE g_hAVSLexer;
//extern Tdll	*g_dllAVSLexer;
extern CAviSynth *g_dllAviSynth;
HWND g_ScriptEditor = (HWND) -1;
std::vector<class AVSEditor*> g_windows;
std::vector<HWND> g_dialogs;
//extern VDubModPreferences		g_VDMPrefs;


void AVSViewerOpen(HWND hwnd);
void AVSViewerChangePrefs(HWND parent);

void init_avs();
void clear_avs();

using namespace std;

//////////////////////////////////////////////////////////////////////////////

extern const char szAVSEditorClassName[]="birdyAVSEditor";
extern const char szAVSViewerClassName[]="birdyAVSViewer";
static const char g_szAVSWarning[]="Script editor warning";

/* struct less_nocase {
	static bool compare_chars(char x, char y) {
		return toupper(x) < toupper(y);
	}
	bool operator()(const string& x, const string& y) const {
		return lexicographical_compare(x.begin(), x.end(),
			y.begin(), y.end(),
			compare_chars);
	}
};
 */
//vector<string> AVSToken;

//char *avsWords;

LPVOID imKeywords, imInternal, imExternal;

//////////////////////////////////////////////////////////////////////////////

struct FindTextOption {
	char szFindString[1024];
	bool bFindReverse;
	bool bWholeWord;
	bool bMatchCase;
	bool bRegexp;
	bool bWrap;
};

int guiMessageBox(HWND hwnd, UINT idText, UINT idCaption, UINT uType) {
	char caption[256];
	char text[4096]; // increased by Fizick (for Avisynth functions list)
	bool error = false;

	// get caption
	if (LoadString(g_hInst, idCaption, (LPTSTR)caption, sizeof caption) == 0)
		error = true;

	// get message body
	if (LoadString(g_hInst, idText, (LPTSTR)text, sizeof text) == 0)
		error = true;

	if (error) {
		return MessageBoxA(hwnd, "Can't retrieve message!", "Internal Error", MB_OK);
	}

	return MessageBoxA(hwnd, text, caption, uType);
}

const int WM_DEFER_ERROR = WM_USER+1;

class AVSEditor {
private:
	const HWND	hwnd;
	HWND	hwndStatus;
	//HWND	hwndRef;
	wchar_t	lpszFileName[MAX_PATH];
	HFONT	hfont;
	void*	coloredit;

	HWND	hwndView;
	WNDPROC OldAVSViewWinProc; // Toff

	bool	bEnableWrite;
//	bool	bTrim;
	bool	bLineNumbers;
	SciFnDirect fnScintilla;
	sptr_t ptrScintilla;
	int		scriptType;

	HWND	hwndFind;
	//ModelessDlgNode mdnFind;
	FindTextOption mFind;

	bool	bModified;

public:
	AVSEditor(HWND);
	~AVSEditor();

	static LRESULT APIENTRY AVSEditorWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) throw();
	static LRESULT APIENTRY SubAVSEditorWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) throw();
	void UpdatePreferences();
	HWND GetHwnd(){ return hwnd; }
	bool CheckFilename(const wchar_t* path) {
		return _wcsicmp(path,lpszFileName)==0;
	}
	void Open(const wchar_t* path);
	void HandleError(const char* s, int line);

private:
	void Init() throw();
	bool Commit() throw();

	void SetStatus(const char *format, ...) throw();
	void UpdateStatus() throw();
	void UpdateLineNumbers();
	void SetAStyle(int style, COLORREF fore, COLORREF back = RGB(0xff,0xff,0xff), int size = 0, const char *face = NULL);

	LRESULT Handle_WM_COMMAND(WPARAM wParam, LPARAM lParam) throw();
	LRESULT Handle_WM_SIZE(WPARAM wParam, LPARAM lParam) throw();
	LRESULT Handle_WM_NOTIFY(HWND hwndFrom, UINT code, NMHDR *phdr) throw();
	LRESULT Handle_WM_DROPFILES(WPARAM wParam, LPARAM lParam);

	sptr_t SendMessageSci(int Message, WPARAM wParam = 0, LPARAM lParam = 0) 
		{return fnScintilla(ptrScintilla, Message, wParam, lParam);}

	void SetScriptType(int type);
	void CheckBracing();
	void DoCalltip();
	
	// Toff ----->
	void RemoveLineCommentInRange(int line);
	bool CommentUncommentLine(int lineNumber, bool comment);
	// <----- Toff

	void Find();
	void FindNext(bool reverse);
	void Jumpto();
	static INT_PTR CALLBACK FindDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK JumpDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

////////////////////////////

AVSEditor::AVSEditor(HWND _hwnd) : hwnd(_hwnd), hfont(0)
{
}

AVSEditor::~AVSEditor() {
	if (hwndFind)
		DestroyWindow(hwndFind);
	if (hfont)
		DeleteObject(hfont);
}

// Toff ----->

bool AVSEditor::CommentUncommentLine(int lineNumber, bool comment)
{
	char buff[2];

	// Get start of line index
	int line_index = SendMessageSci(SCI_POSITIONFROMLINE, lineNumber, 0);
	SendMessageSci(SCI_SETSEL, line_index, line_index+1);
	SendMessageSci(SCI_GETSELTEXT, 0, (LPARAM)buff);	
	
	if(buff[0] == '#')
	{
		if(!comment)
		{
			// This line must be uncommented
			buff[0] = '\0';
			SendMessageSci(SCI_REPLACESEL, 0, (LPARAM)buff);
			return true;
		}
	}
	else
	{
		if(comment)
		{
			// This line must be commented
			SendMessageSci(SCI_SETSEL, line_index, line_index);
			buff[0] = '#';
			buff[1] = '\0';
			SendMessageSci(SCI_REPLACESEL, 0, (LPARAM)buff);
			return true;
		}
	}

	return false; // No change
}

void AVSEditor::RemoveLineCommentInRange(int line)
{
//	CHARRANGE chr;
	char *buff = NULL;
	int charIndex = 0;
	int charStart = 0;
	int charEnd = 0;
	int lineNumber = 0;

//	int totalLineNumber = SendMessage(hwndView,EM_GETLINECOUNT ,0,0);
	int totalLineNumber = SendMessageSci(SCI_GETLINECOUNT ,0,0);

	// Get cursor position
//	SendMessage(hwndView, EM_EXGETSEL, 0, (LPARAM)&chr);
//	charIndex = chr.cpMin;
	charStart = SendMessageSci(SCI_GETSELECTIONSTART, 0, 0);
	charEnd = SendMessageSci(SCI_GETSELECTIONEND, 0, 0);
	charIndex = charStart;
//	lineNumber = SendMessage(hwndView, EM_LINEFROMCHAR, charIndex, 0);
	lineNumber = SendMessageSci(SCI_LINEFROMPOSITION, charIndex, 0);

	// Go up and find the "#StartTweaking" line
	const char* startLineText = "#StartTweaking";
	size_t startLineLen = strlen(startLineText);

	for( ;lineNumber>=0;lineNumber--) {
//		if (SendMessage(hwndView, EM_LINELENGTH, lineNumber, 0) != startLineLen) continue;
		buff = new char[SendMessageSci(SCI_LINELENGTH, lineNumber)];
		SendMessageSci(SCI_GETLINE, lineNumber, (LPARAM)buff);
		if (strncmp(startLineText,buff,startLineLen) != 0 ) {
			delete [] buff;
			buff = NULL;
			continue;
		}
		break;
	}
	if (buff) delete [] buff;
	buff = NULL;

	if (lineNumber<0) return;

	lineNumber++;
	int firstLine = lineNumber;

	const char* stopLineText = "#StopTweaking";
	size_t stopLineLen = strlen(stopLineText);
	int lastLine = -1;

	for( ;lineNumber<totalLineNumber;lineNumber++) {
		buff = new char[SendMessageSci(SCI_LINELENGTH, lineNumber)];
		SendMessageSci(SCI_GETLINE, lineNumber, (LPARAM)buff);
		if (strncmp(stopLineText,buff,stopLineLen) == 0) {
			lastLine = lineNumber;
			break;
		}
// 	  	CommentUncommentLine(lineNumber, ((lineNumber - firstLine) != line));		
		delete [] buff;
		buff = NULL;
	}
	if (buff) delete [] buff;
	for (lineNumber = firstLine; lineNumber<lastLine; lineNumber++)
			CommentUncommentLine(lineNumber, ((lineNumber - firstLine) != line));		
//	while( (SendMessage(hwndView, EM_LINELENGTH, charIndex, 0) != startLineLen ||
//		   SendMessage(hwndView, EM_GETLINE, lineNumber, (LPARAM)buff) > 0) &&
//		   strncmp(startLineText,buff,startLineLen) != 0 )
/*	while( (SendMessageSci(SCI_LINELENGTH, lineNumber, 0) != startLineLen ||
			 SendMessageSci(SCI_GETLINE, lineNumber, (LPARAM)buff) > 0) &&
			 strncmp(startLineText,buff,startLineLen) != 0 )
	{
		lineNumber--;
		if(lineNumber < 0)
		{
			return;
		}
//		charIndex = SendMessage(hwndView, EM_LINEINDEX, lineNumber, 0);
		charIndex = SendMessageSci(SCI_POSITIONFROMLINE, lineNumber, 0);
	}

	lineNumber++;
	int firstLine = lineNumber;

	StatusDebug(lineNumber, SendMessageSci(SCI_LINELENGTH, 3, 0), 0);

//	charIndex = SendMessage(hwndView, EM_LINEINDEX, firstLine, 0);
	charIndex = SendMessageSci(SCI_POSITIONFROMLINE, firstLine, 0);

	// Go down and stop on the "#StopTweaking" line
	const char* stopLineText = "#StopTweaking";
	int stopLineLen = strlen(stopLineText)+1;

//	while( (SendMessage(hwndView, EM_LINELENGTH, charIndex, 0) != stopLineLen ||
//		   SendMessage(hwndView, EM_GETLINE, lineNumber, (LPARAM)buff) > 0) &&
//		   strncmp(stopLineText,buff,stopLineLen) != 0 )
	while( (SendMessageSci(SCI_LINELENGTH, lineNumber, 0) != stopLineLen ||
			 SendMessageSci(SCI_GETLINE, lineNumber, (LPARAM)buff) > 0) &&
			 strncmp(stopLineText,buff,stopLineLen) != 0 )
	{
		CommentUncommentLine(lineNumber, ((lineNumber - firstLine) != line));		
		lineNumber++;
		if(lineNumber >= totalLineNumber)
		{
			break;
		}
//		charIndex = SendMessage(hwndView, EM_LINEINDEX, lineNumber, 0);
		charIndex = SendMessageSci(SCI_POSITIONFROMLINE, lineNumber, 0);
	}
*/
	// Restore original position
//	SendMessage(hwndView, EM_EXSETSEL, 0, (LPARAM)&chr);
	SendMessageSci(SCI_SETSEL, charStart, charEnd);
}

LRESULT AVSEditor::SubAVSEditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) throw()
{
	AVSEditor *pcd = (AVSEditor *)GetWindowLongPtr(
		(HWND)GetWindowLongPtr(hwnd, GWLP_HWNDPARENT), 0);

	switch (msg)
	{
		case WM_KEYDOWN:
			if ((GetKeyState(VK_SHIFT)>=0) && (GetKeyState(VK_MENU)>=0) && (GetKeyState(VK_CONTROL) < 0))
			{
				if(wParam >= VK_F1 && wParam <= VK_F12)
				{
					pcd->RemoveLineCommentInRange(wParam - VK_F1);
					if (pcd->Commit())
						VDSendReopen(pcd->lpszFileName, pcd);
				}
			}
			break;
	}

	return CallWindowProc(pcd->OldAVSViewWinProc, hwnd, msg, wParam, lParam); 
}

// <----- Toff

void AVSEditor::Init() {
	hwndStatus = CreateStatusWindow(WS_CHILD|WS_VISIBLE|SBARS_SIZEGRIP, "", hwnd, 501);

	hwndView = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"Scintilla",
		"",
		WS_VISIBLE|WS_CHILD|WS_VSCROLL|WS_HSCROLL|ES_MULTILINE,
		0,0,50,50,
		hwnd,
		NULL,
		g_hInst,
		NULL);

	lpszFileName[0] = 0;

	fnScintilla = (SciFnDirect)SendMessage(hwndView,SCI_GETDIRECTFUNCTION,0,0);
	ptrScintilla = (sptr_t)SendMessage(hwndView,SCI_GETDIRECTPOINTER,0,0);

	bLineNumbers = TRUE;
	UpdateLineNumbers();	

//	SetAStyle(STYLE_DEFAULT, RGB(0,0,0), RGB(0xff,0xff,0xff), 11, "Courier New");
//	SendMessageSci(SCI_STYLECLEARALL);	// Copies global style to all others
	SendMessageSci(SCI_SETCODEPAGE, SC_CP_UTF8);
	SetScriptType(SCRIPTTYPE_NONE);
//	SetScriptType(SCRIPTTYPE_AVS);

	// Toff ----->
	OldAVSViewWinProc = (WNDPROC)SetWindowLongPtr(hwndView, GWLP_WNDPROC,	(LPARAM)SubAVSEditorWndProc);
	// <----- Toff

	//if (lpszFileName[0] != 0) Open();

	hwndFind = NULL;
	mFind.szFindString[0] = 0;
	mFind.bFindReverse = false;
	mFind.bWholeWord = false;
	mFind.bMatchCase = false;
	mFind.bRegexp = false;
	mFind.bWrap = true;

	bModified = false;

	DragAcceptFiles(hwnd, TRUE);

	AVSViewerLoadSettings(hwnd,REG_WINDOW_MAIN);

	SetScriptType(scriptType);
}

void AVSEditor::UpdateLineNumbers() {
	SendMessageSci(SCI_SETMARGINTYPEN, 1, SC_MARGIN_NUMBER);
	char *gw = "9";
	int pixelWidth = 4 + 5 * SendMessageSci(SCI_TEXTWIDTH, STYLE_LINENUMBER, (LPARAM) gw);
	SendMessageSci(SCI_SETMARGINWIDTHN, 1, bLineNumbers?pixelWidth:0);
}

void AVSEditor::Open(const wchar_t* path) {
	if(path) wcscpy(lpszFileName, path);

	unsigned char *lpszBuf;
	FILE *f;
	size_t n, r;

	lpszBuf = NULL;
	f = _wfopen(lpszFileName, L"rb");
	if (f == NULL) {
		return;
	}
	fseek(f, 0, SEEK_END);
	r = ftell(f);
	fseek(f, 0, SEEK_SET);
	lpszBuf = (unsigned char *) malloc(r+1);
	n=fread(lpszBuf, sizeof( char ), r, f);
	*(lpszBuf+n) = (char) 0;
	fclose(f);
//    SendMessage(hwndView, WM_SETTEXT, 0, (LPARAM) lpszBuf);
	SendMessageSci(SCI_CLEARALL, 0, 0);
	SendMessageSci(SCI_SETCODEPAGE, SC_CP_UTF8);
	if (lpszBuf[0]==0xef && lpszBuf[1]==0xbb && lpszBuf[2]==0xbf) {
		SendMessageSci(SCI_SETTEXT, 0, (LPARAM) lpszBuf+3);
	} else {
		SendMessageSci(SCI_SETTEXT, 0, (LPARAM) lpszBuf);
	}
	free(lpszBuf);

	char buf[512];

	wsprintfA(buf, "VirtualDub2 Script Editor - [%ls]", lpszFileName);
	SetWindowText(hwnd, buf);

	SendMessageSci(SCI_SETWRAPMODE, g_VDMPrefs.m_bWrapLines ? SC_WRAP_WORD:SC_WRAP_NONE);
	SetAStyle(STYLE_DEFAULT, RGB(0,0,0), RGB(0xff,0xff,0xff), g_VDMPrefs.mAVSViewerFontSize, g_VDMPrefs.mAVSViewerFontFace.c_str());
	SendMessageSci(SCI_STYLECLEARALL);	// Copies global style to all others

/*	if (!_stricmp(strrchr((lpszFileName), (int) '.'), ".avs")) {
		SetScriptType(SCRIPTTYPE_AVS);
	} else {
		SetScriptType(SCRIPTTYPE_NONE);
	}*/
	SetScriptType(GetScriptType(lpszFileName));
}

void AVSEditor::HandleError(const char* s, int line){
	SendMessageSci(SCI_GOTOLINE, line-1);
	char* s1 = _strdup(s);
	PostMessage(hwnd,WM_DEFER_ERROR,0,(LPARAM)s1);
}

void AVSEditor::SetScriptType(int type){
	scriptType = type;
	SendMessageSci(SCI_SETWRAPMODE, g_VDMPrefs.m_bWrapLines ? SC_WRAP_WORD:SC_WRAP_NONE);
	SetAStyle(STYLE_DEFAULT, RGB(0,0,0), RGB(0xff,0xff,0xff), g_VDMPrefs.mAVSViewerFontSize, g_VDMPrefs.mAVSViewerFontFace.c_str());
	SendMessageSci(SCI_STYLECLEARALL);	// Copies global style to all others
	switch(type) {
		case SCRIPTTYPE_AVS: {
			init_avs();
			/*if (!g_dllAVSLexer->ok) {
				guiMessageBox(hwnd, IDS_ERR_NO_AVSLEXER, IDS_ERR_NO_AVSLEXER_CAP, MB_OK|MB_ICONERROR);
			}*/
			//SendMessageSci(SCI_SETLEXERLANGUAGE, 0, (int) "avslex");
			SendMessageSci(SCI_SETLEXER, SCLEX_AVS);
			//SendMessageSci(SCI_SETSTYLEBITS, 7);

			SendMessageSci(SCI_CLEARREGISTEREDIMAGES);
			SendMessageSci(SCI_REGISTERIMAGE, ICO_SCI_AVS_KEYWORDS, (LPARAM)imKeywords);
			SendMessageSci(SCI_REGISTERIMAGE, ICO_SCI_AVS_INTERNAL, (LPARAM)imInternal);
			SendMessageSci(SCI_REGISTERIMAGE, ICO_SCI_AVS_EXTERNAL, (LPARAM)imExternal);

			SendMessageSci(SCI_SETKEYWORDS, 0, (LPARAM) g_dllAviSynth->coKeywords);
			SendMessageSci(SCI_SETKEYWORDS, 1, (LPARAM) g_dllAviSynth->coInternal);
			if (g_dllAviSynth->coExternal) SendMessageSci(SCI_SETKEYWORDS, 2, (LPARAM) g_dllAviSynth->coExternal);

			SendMessageSci(SCI_SETTABWIDTH, 4, 0);

			const COLORREF black = RGB(0, 0, 0);
			const COLORREF white = RGB(0xFF, 0xFF, 0xFF);
			const COLORREF red = RGB(0xFF, 0, 0);
			const COLORREF offWhite = RGB(0xFF, 0xFB, 0xF0);
			const COLORREF darkGreen = RGB(0, 0x80, 0);
			const COLORREF darkBlue = RGB(0, 0, 0x80);
			const COLORREF darkRed = RGB(0x80,0,0);
			const COLORREF darkViolet = RGB(0x80,0,0x80);
	
			SetAStyle(SCE_AVS_DEFAULT, black, white, g_VDMPrefs.mAVSViewerFontSize, g_VDMPrefs.mAVSViewerFontFace.c_str());
		
			SetAStyle(SCE_AVS_NUMBER, darkBlue);

			// Commands
			SetAStyle(SCE_AVS_DEFAULT, black);
			SendMessageSci(SCI_STYLESETBOLD, SCE_AVS_DEFAULT, 1);
			SetAStyle(SCE_AVS_KEYWORD, darkGreen);
			SendMessageSci(SCI_STYLESETBOLD, SCE_AVS_KEYWORD, 1);
			SetAStyle(SCE_AVS_PLUGIN, darkViolet);
			SendMessageSci(SCI_STYLESETBOLD, SCE_AVS_PLUGIN, 1);
			SetAStyle(SCE_AVS_FILTER, darkViolet);
			SendMessageSci(SCI_STYLESETBOLD, SCE_AVS_FILTER, 1);

			// Comment
			SetAStyle(SCE_AVS_COMMENTBLOCK, RGB(0xAA,0xAA,0xAA));
			SetAStyle(SCE_AVS_COMMENTBLOCKN, RGB(0xAA,0xAA,0xAA));
			SetAStyle(SCE_AVS_COMMENTLINE, RGB(0xAA,0xAA,0xAA));
			//SendMessageSci(SCI_STYLESETITALIC, SCE_AVS_COMMENT, 1);

			SetAStyle(SCE_AVS_OPERATOR, darkBlue);
			SetAStyle(SCE_AVS_STRING, darkRed);
			//SetAStyle(SCE_AVS_FUZZY, red);
			//SetAStyle(SCE_AVS_ERROR, white, red);

			SetAStyle(34, white, black);
			SetAStyle(35, white, red);

			SendMessageSci(SCI_AUTOCSETIGNORECASE, TRUE);
			SendMessageSci(SCI_AUTOCSETFILLUPS, 0, (LPARAM) " (,.");
		} 
		break;

		case SCRIPTTYPE_DECOMB: {
			scriptType = SCRIPTTYPE_DECOMB;
			SendMessageSci(SCI_SETLEXER, SCLEX_NULL);
		}
		break;

		case SCRIPTTYPE_VPS: {
			scriptType = SCRIPTTYPE_VPS;
			SendMessageSci(SCI_SETLEXER, SCLEX_PYTHON);
			SendMessageSci(SCI_SETTABWIDTH, 4, 0);
			const COLORREF black = RGB(0, 0, 0);
			const COLORREF white = RGB(0xFF, 0xFF, 0xFF);
			const COLORREF darkGreen = RGB(0, 0x80, 0);
			const COLORREF darkBlue = RGB(0, 0, 0x80);
			const COLORREF darkRed = RGB(0x80,0,0);
			SetAStyle(SCE_P_DEFAULT, black, white, g_VDMPrefs.mAVSViewerFontSize, g_VDMPrefs.mAVSViewerFontFace.c_str());
			SendMessageSci(SCI_STYLESETBOLD, SCE_P_DEFAULT, 1);
			SetAStyle(SCE_P_WORD, darkGreen);
			SendMessageSci(SCI_STYLESETBOLD, SCE_P_WORD, 1);

			// Comment
			SetAStyle(SCE_P_COMMENTBLOCK, RGB(0xAA,0xAA,0xAA));
			SetAStyle(SCE_P_COMMENTLINE, RGB(0xAA,0xAA,0xAA));

			SetAStyle(SCE_P_OPERATOR, darkBlue);
			SetAStyle(SCE_P_STRING, darkRed);
		}
		break;

		case SCRIPTTYPE_VDSCRIPT: {
			scriptType = SCRIPTTYPE_VDSCRIPT;
			SendMessageSci(SCI_SETLEXER, SCLEX_NULL);
		}
		break;

		default: {
			scriptType = SCRIPTTYPE_NONE;
			SendMessageSci(SCI_SETLEXER, SCLEX_NULL);
		}
	}

	SendMessageSci(SCI_COLOURISE, 0, -1);
	UpdateStatus();
}

bool AVSEditor::Commit() {
		int s;
	char *lpszBuf;
	FILE *f;
	//GETTEXTLENGTHEX gte;
//	GETTEXTEX gt;

	if (lpszFileName[0] == 0) {
		wchar_t szName[MAX_PATH];
		szName[0] = 0;
		OPENFILENAMEW ofn = {};

		ofn.lStructSize			= sizeof(OPENFILENAMEW);
		ofn.hwndOwner			= hwnd;
		ofn.lpstrFilter			= L"All files (*.*)\0*.*\0";
		ofn.nFilterIndex		= 1;
		ofn.lpstrFile			= szName;
		ofn.nMaxFile			= std::size(szName);
		ofn.Flags				= OFN_EXPLORER | OFN_ENABLESIZING;

		if (GetSaveFileNameW(&ofn)) {
			wcscpy_s(lpszFileName, szName);
		} else {
			return false;
		}

	}

//	s = SendMessage(hwndView, WM_GETTEXTLENGTH, 0, 0);
	int cp = SendMessageSci(SCI_GETCODEPAGE);
	s = SendMessageSci(SCI_GETTEXTLENGTH, 0, 0);
	lpszBuf = (char *) malloc(s+1);
//	gt.cb = s;
//	gt.flags = GT_USECRLF;
//	gt.codepage = CP_ACP;
//	gt.lpDefaultChar = NULL;
//	gt.lpUsedDefChar = NULL;
//	SendMessage(hwndView, EM_GETTEXTEX, (WPARAM) &gt, (LPARAM) lpszBuf);
	SendMessageSci(SCI_GETTEXT, s+1, (LPARAM) lpszBuf);
	f = _wfopen(lpszFileName, L"wb");
	if (f) {
		fwrite(lpszBuf, sizeof(char), s, f);
		fclose(f);
	}
	else {
		MessageBoxA(hwnd, "The file cannot be opened for writing.", "Error", MB_OK);
	}
	free(lpszBuf);

	bModified = false;

	return true;
}

void AVSEditor::SetStatus(const char *format, ...) throw() {
	char buf[1024];
	va_list val;

	va_start(val,format);
	_vsnprintf(buf, sizeof buf, format, val);
	va_end(val);

	SetWindowText(hwndStatus, buf);
}

void AVSEditor::UpdateStatus() throw() {
	char buf[1024];
	int posx, posy, c;
//	POINT pt;

	//SendMessage(hwndView, EM_GETSEL, (WPARAM) &posx, (LPARAM) &posy);
//	GetCaretPos(&pt);
//	c = SendMessage(hwndView, EM_CHARFROMPOS, 0, (LPARAM) &pt);
//	posy = SendMessage(hwndView, EM_LINEFROMCHAR, c, 0);
//	posx = c - SendMessage(hwndView, EM_LINEINDEX, posy, 0);
	c = SendMessageSci(SCI_GETCURRENTPOS, 0, 0);
	posy = SendMessageSci(SCI_LINEFROMPOSITION, c, 0);
	posx = c - SendMessageSci(SCI_POSITIONFROMLINE, posy, 0);
	wsprintfA(buf, "%d:%d", posy+1, posx+1);
	SendMessageA(hwndStatus, SB_SETTEXT, 2, (LPARAM) &buf);
	SendMessageA(hwndStatus, SB_SETTEXT, 1, (LPARAM) scripttypeName[scriptType]);
}

void AVSEditor::SetAStyle(int style, COLORREF fore, COLORREF back, int size, const char *face) {
	SendMessageSci(SCI_STYLESETFORE, style, fore);
	SendMessageSci(SCI_STYLESETBACK, style, back);
	if (size >= 1)
		SendMessageSci(SCI_STYLESETSIZE, style, size);
	if (face) 
		SendMessageSci(SCI_STYLESETFONT, style, (LPARAM)face);
}


LRESULT AVSEditor::Handle_WM_COMMAND(WPARAM wParam, LPARAM lParam) throw() {
	if ((HWND) lParam == hwndView) {
/*		switch (HIWORD(wParam)) {
		case SCEN_CHANGE:
			CheckBracing();
			break;
		}
*/	} else switch(LOWORD(wParam)) {
	case ID_FILE_EXIT:
		DestroyWindow(hwnd);
		break;
	case ID_FILE_OPEN:
		{
			wchar_t szName[MAX_PATH];
			OPENFILENAMEW ofn;

			szName[0] = 0;

			memset(&ofn, 0, sizeof ofn);

			ofn.lStructSize			= sizeof(OPENFILENAMEW);
			ofn.hwndOwner			= hwnd;
			ofn.lpstrFilter			= L"All files (*.*)\0*.*\0";
			ofn.lpstrCustomFilter	= NULL;
			ofn.nFilterIndex		= 1;
			ofn.lpstrFile			= szName;
			ofn.nMaxFile			= sizeof szName;
			ofn.lpstrFileTitle		= NULL;
			ofn.lpstrInitialDir		= NULL;
			ofn.lpstrTitle			= NULL;
			ofn.Flags				= OFN_EXPLORER | OFN_ENABLESIZING;
			ofn.lpstrDefExt			= NULL;

			if (GetOpenFileNameW(&ofn)) {
				//VDTextAToW(lpszFileName, MAX_PATH, szName, MAX_PATH);
				//wcscpy(lpszFileName, VDTextAToW(szName).c_str());
				Open(szName);
			}
		}
		break;
	case ID_FILE_NEW:
		{
			lpszFileName[0] = 0;
			SendMessageSci(SCI_CLEARALL, 0, 0);
//			SetScriptType(SCRIPTTYPE_NONE);
			SetScriptType(SCRIPTTYPE_AVS);
			SetWindowText(hwnd, "VirtualDub2 Script Editor");
		}
		break;
	case ID_REFRESH:
		{
			if (Commit())
				VDSendReopen(lpszFileName, this);
		}
		break;
	case ID_AVS_SAVE_OPEN:
		{
			if (Commit()) {
				VDSetFilename(lpszFileName,this);
			}
		}
		break;
	case ID_AVS_FILE_SAVE:
		Commit();
		break;
	case ID_FILE_REVERT:
		if (IDOK==guiMessageBox(hwnd, IDS_WARN_AVS_DISCARD_CHANGE, IDS_WARN_AVS_DISCARD_CHANGE_CAP, MB_OKCANCEL)) {
			Open(0);
		}
		break;

	case ID_EDIT_UNDO:
		{
			SendMessageSci(SCI_UNDO, 0, 0);
		}
		break;

	case ID_EDIT_REDO:
		{
			SendMessageSci(SCI_REDO, 0, 0);
		}
		break;

	case ID_EDIT_GOTO:
		Jumpto();
		break;

	case ID_EDIT_FIND:
		Find();
		break;

	case ID_EDIT_FINDNEXT:
		FindNext(false);
		break;

	case ID_EDIT_FINDPREV:
		FindNext(true);
		break;

	case ID_AVS_INSERT_POS:
		{
			int64 p = VDRequestPos();
			char buf[50];
			wsprintfA(buf, "%I64d", p);
			SendMessageA(hwndView, SCI_REPLACESEL, 0, (LPARAM) &buf);
		}
		break;

	case ID_AVS_INSERT_RANGE:
	case ID_AVS_INSERT_TRIM:
		{
			bool trim = false;
			if(LOWORD(wParam)==ID_AVS_INSERT_TRIM) trim = true;
			int64 r0,r1;
			VDRequestRange(r0,r1);
			if (r1-r0<=0) {
				guiMessageBox(hwnd, IDS_ERR_AVS_NORANGE, IDS_ERR_CAPTION, MB_OK|MB_ICONERROR);
				break;
			}

			char buf[50];
			if (scriptType == SCRIPTTYPE_NONE) {
				wsprintfA(buf, "%I64d-%I64d", r0, r1);
			}
			if (scriptType == SCRIPTTYPE_VDSCRIPT) {
				if (trim) 
					wsprintfA(buf, "VirtualDub.subset.AddRange(%I64d,%I64d);", r0, r1);
				else
					wsprintfA(buf, "%I64d,%I64d", r0, r1);
			}
			if (scriptType == SCRIPTTYPE_AVS || scriptType == SCRIPTTYPE_DECOMB) {
				if (r0 == 0 && r1 == 1)
					wsprintfA(buf, (trim)?"Trim(%d,%d)":"%d,%d", 0, -1);// special case of very first frame
				else
					wsprintfA(buf, (trim)?"Trim(%I64d,%I64d)":"%I64d,%I64d", r0, r1 -1);
			}
			if (scriptType == SCRIPTTYPE_VPS) {
				if (r1 == r0+1)
					wsprintfA(buf, (trim)?"clip[%d]":"%d", r0);
				else
					wsprintfA(buf, (trim)?"clip[%I64d:%I64d]":"%I64d:%I64d", r0, r1);
			}
			SendMessageA(hwndView, SCI_REPLACESEL, 0, (LPARAM) &buf);
		}
		break;

	case ID_AVS_INSERT_FRAMESET:
		{
			vd_frameset set;
			vd_basic_range rbuf[1024];
			set.ranges = rbuf;
			VDRequestFrameset(set,1024);
			if (set.count==0) {
				guiMessageBox(hwnd, IDS_ERR_AVS_NOFRAMESET, IDS_ERR_CAPTION, MB_OK|MB_ICONERROR);
				break;
			}

			string buffer;
			char buf[50];
			for(int i = 0; i<set.count; i++) {
				vd_basic_range range = set.ranges[i];

				if (scriptType == SCRIPTTYPE_NONE) {
					wsprintfA(buf, "Trim(%I64d,%I64d)", range.from, range.to);
					if (i>0) buffer += " + ";
				}

				if (scriptType == SCRIPTTYPE_AVS || scriptType == SCRIPTTYPE_DECOMB) {
					if (range.from == 0 && range.to == 1)
						wsprintfA(buf, "Trim(%d,%d)", 0, -1); // special case of one very first frame
					else
						wsprintfA(buf, "Trim(%I64d,%I64d)", range.from, range.to - 1);
					if (i>0) buffer += " ++ ";
				}

				if (scriptType == SCRIPTTYPE_VPS) {
					if (range.to==range.from+1)
						wsprintfA(buf, "clip[%I64d]", range.from);
					else
						wsprintfA(buf, "clip[%I64d:%I64d]", range.from, range.to);
					if (i>0) buffer += " + ";
				}

				if (scriptType == SCRIPTTYPE_VDSCRIPT) {
					wsprintfA(buf, "VirtualDub.subset.AddRange(%I64d,%I64d);", range.from, range.to);
					if (i>0) buffer += "\r\n";
				}

				buffer += buf;
			}
			SendMessageA(hwndView, SCI_REPLACESEL, 0, (LPARAM) buffer.c_str());
		}
		break;

	case ID_AVS_INSERT_FILENAME:
		{
			wchar_t szName[MAX_PATH];
			szName[0] = 0;
			OPENFILENAMEW ofn = {};

			ofn.lStructSize			= sizeof(OPENFILENAMEW);
			ofn.hwndOwner			= hwnd;
			ofn.lpstrFilter			= L"All files (*.*)\0*.*\0";
			ofn.nFilterIndex		= 1;
			ofn.lpstrFile			= szName;
			ofn.nMaxFile			= std::size(szName);
			ofn.Flags				= OFN_EXPLORER | OFN_ENABLESIZING;

			if (GetOpenFileNameW(&ofn)) {
				VDStringA filepath_utf8;
				if (scriptType == SCRIPTTYPE_NONE) {
					filepath_utf8 = VDTextWToU8(szName, -1);
				} else {
					filepath_utf8.sprintf("\"%s\"", VDTextWToU8(szName, -1));
				}
				SendMessageSci(SCI_REPLACESEL, 0, (LPARAM) (char*)(filepath_utf8.c_str()));
			}
		}
		break;

	case ID_AVS_INSERT_CROP:
		{
			vd_framesize frame;
			VDRequestFrameSize(frame);
			char buf[50];
			wsprintfA(buf, "Crop(%d,%d,%d,%d)", frame.frame.left, frame.frame.top, frame.frame.right, frame.frame.bottom);
			SendMessageA(hwndView, SCI_REPLACESEL, 0, (LPARAM) &buf);
		}
		break;

	case ID_AVS_EDIT_LINE:
		{
			bLineNumbers = !bLineNumbers;
			UpdateLineNumbers();
			CheckMenuItem(GetMenu(hwnd), ID_AVS_EDIT_LINE, MF_BYCOMMAND | bLineNumbers?MF_CHECKED:MF_UNCHECKED);
		}
		break;

	case ID_AVS_EDIT_OPTIONS:
		ShowPrefs(hwnd);
		break;

	case ID_AVS_SCRIPT_NONE:
		{
			SetScriptType(SCRIPTTYPE_NONE);
		}
		break;

	case ID_AVS_SCRIPT_AVS:
		{
			SetScriptType(SCRIPTTYPE_AVS);
		}
		break;

	case ID_AVS_SCRIPT_DECOMB:
		{
			SetScriptType(SCRIPTTYPE_DECOMB);
		}
		break;

	case ID_AVS_SCRIPT_VPS:
		{
			SetScriptType(SCRIPTTYPE_VPS);
		}
		break;

	case ID_AVS_SCRIPT_VDSCRIPT:
		{
			SetScriptType(SCRIPTTYPE_VDSCRIPT);
		}
		break;

	case ID_AVS_COMMENT_LINES:
		{
			int first = SendMessageSci(SCI_GETSELECTIONSTART);
			int firstline = SendMessageSci(SCI_LINEFROMPOSITION, first);
			int last = SendMessageSci(SCI_GETSELECTIONEND);
			int lastline = SendMessageSci(SCI_LINEFROMPOSITION, last);
			for(int i = (first>last)?lastline:firstline;i <= ((first>last)?firstline:lastline);i++){
				CommentUncommentLine(i, TRUE);
			}
		}
		break;

	case ID_AVS_UNCOMMENT_LINES:
		{
			int first = SendMessageSci(SCI_GETSELECTIONSTART);
			int firstline = SendMessageSci(SCI_LINEFROMPOSITION, first);
			int last = SendMessageSci(SCI_GETSELECTIONEND);
			int lastline = SendMessageSci(SCI_LINEFROMPOSITION, last);
			for(int i = (first>last)?lastline:firstline;i <= ((first>last)?firstline:lastline);i++){
				CommentUncommentLine(i, FALSE);
			}
		}
		break;

	case ID_AVS_STARTCOMPLETE:
		if (scriptType==SCRIPTTYPE_AVS) {
			if (SendMessageSci(SCI_AUTOCACTIVE)){
				SendMessageSci(SCI_AUTOCCANCEL);
			} else {
				int pos = SendMessageSci(SCI_GETCURRENTPOS);
				int line = SendMessageSci(SCI_LINEFROMPOSITION, pos);
				int posinline = pos - SendMessageSci(SCI_POSITIONFROMLINE, line);
				char *bline = new char[SendMessageSci(SCI_LINELENGTH, line)+1];
				SendMessageSci(SCI_GETLINE, line, (LPARAM) bline);
				*(bline + posinline) = 0;
				_strrev(bline);
				char *found = strpbrk(bline, " .(,");
				ptrdiff_t thelen;
				if (found) {
					thelen = found - bline;
				} else {
					thelen = strlen(bline);
				}
				SendMessageSci(SCI_AUTOCSHOW, thelen, (LPARAM) g_dllAviSynth->coAllScintilla);
				delete [] bline;
			}
		}
		break;

/*	case ID_HELP_WHY:
		guiMessageBox(hwnd, IDS_INFO_WHY_HEXEDITOR, IDS_INFO_WHY_HEXEDITOR_CAP, MB_OK);
		break;*/

	case ID_HELP_KEYS:
		guiMessageBox(hwnd, IDS_INFO_AVSEDITOR_KEY, IDS_INFO_AVSEDITOR_KEY_CAP, MB_OK);
		break;

	case ID_HELP_TOFF:
		guiMessageBox(hwnd, IDS_INFO_AVSEDITOR_TOFF, IDS_INFO_AVSEDITOR_TOFF_CAP, MB_OK);
		break;

	case ID_HELP_AVISYNTH:
		{
			init_avs();
			if (g_dllAviSynth->ok) {
				string v;
				v = g_dllAviSynth->Version;
				if (g_dllAviSynth->coExternal) {
					v += "\n\nExternal Commands: ";
					v += g_dllAviSynth->coExternal;
				}
				char caption[256];
				LoadString(g_hInst, IDS_INFO_AVS_VERSION_CAP, (LPTSTR)caption, sizeof caption);
				MessageBoxA(hwnd, v.c_str(), caption, MB_OK|MB_ICONINFORMATION);
			} else {
				guiMessageBox(hwnd, IDS_ERR_AVS_NOTFOUND, IDS_INFO_AVS_VERSION_CAP, MB_OK|MB_ICONSTOP);
			}
		}
		break;
	}

	return 0;
}

LRESULT AVSEditor::Handle_WM_SIZE(WPARAM wParam, LPARAM lParam) throw() {
	HDWP hdwp;
	RECT r, rstatus;

	GetClientRect(hwnd, &r);
	GetWindowRect(hwndStatus, &rstatus);
	MapWindowPoints(NULL, hwnd, (LPPOINT)&rstatus, 2);
	int sh = rstatus.bottom-rstatus.top;

	hdwp = BeginDeferWindowPos(2);
	DeferWindowPos(hdwp, hwndStatus, NULL, 0, r.bottom-sh, r.right, sh, SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOCOPYBITS);
	DeferWindowPos(hdwp, hwndView, NULL, 0, 0, r.right, r.bottom-sh, SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOMOVE);
	EndDeferWindowPos(hdwp);

	int st[3];
	st[0] = r.right-150;
	st[1] = r.right-75;
	st[2] = -1;
	SendMessage(hwndStatus, SB_SETPARTS, 3, (LPARAM) &st);
	UpdateStatus();
	
	return 0;
}

LRESULT AVSEditor::Handle_WM_NOTIFY(HWND hwndFrom, UINT code, NMHDR *phdr) throw() {
	if (hwndFrom == hwndView) {
		SCNotification* scn = (SCNotification*)phdr;
		switch(code){
		case SCN_UPDATEUI:
			CheckBracing();
			if(scn->updated & SC_UPDATE_SELECTION) UpdateStatus();
			break;
/*		case SCN_CHARADDED:
			DoCalltip();
			break;*/
/*		case SCN_MODIFIED:
			CheckBracing();
			break;*/
		case SCN_MODIFIED:
			bModified = true;
			break;
		}
	}
	return 0;
}

LRESULT AVSEditor::Handle_WM_DROPFILES(WPARAM wParam, LPARAM lParam) {
	/*
	HDROP hdrop = (HDROP)wParam;
	UINT count = DragQueryFileW(hdrop, 0xFFFFFFFF, NULL, 0);

	vdfastvector<wchar_t> s;
	if (count > 0) {
		UINT chars = DragQueryFileW(hdrop, 0, NULL, 0);
		s.resize(chars+1, 0);

		DragQueryFileW(hdrop, 0, s.data(), chars+1);
	}

	DragFinish(hdrop);

	if (!s.empty() && s[0]) {
		wcscpy(lpszFileName, s.data());
		Open();
	}
	*/
	return 0;
}

void AVSEditor::CheckBracing() {
	int pos = SendMessageSci(SCI_GETCURRENTPOS);
	if(pos>0){
		SendMessageSci(SCI_BRACEHIGHLIGHT, INVALID_POSITION, INVALID_POSITION);
		if ((char) SendMessageSci(SCI_GETCHARAT, pos-1)==')') {
			int pos2 = SendMessageSci(SCI_BRACEMATCH, pos-1);
			if (pos2==INVALID_POSITION){
				SendMessageSci(SCI_BRACEBADLIGHT, pos-1);
			} else {
				SendMessageSci(SCI_BRACEHIGHLIGHT, pos2, pos-1);
			}
		}
	}
}

void AVSEditor::DoCalltip() {
	int pos = SendMessageSci(SCI_GETCURRENTPOS);
	if(pos>0){
		char ch = (char)SendMessageSci(SCI_GETCHARAT, pos-1);
		if (ch=='(') {
			if (!SendMessageSci(SCI_CALLTIPACTIVE)){
				SendMessageSci(SCI_CALLTIPSHOW, 5, (LPARAM) "Test(a, b)\rTest2");
			}
		}
		if (ch==')') {
			if (SendMessageSci(SCI_CALLTIPACTIVE)){
				SendMessageSci(SCI_CALLTIPCANCEL);
			}
		}
	}
}

////////////////////////////

LRESULT APIENTRY AVSEditor::AVSEditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) throw() {
	AVSEditor *pcd = (AVSEditor *)GetWindowLongPtr(hwnd, 0);

	switch(msg) {

	case WM_NCCREATE:
		if (!(pcd = new AVSEditor(hwnd)))
			return FALSE;

		SetWindowLongPtr(hwnd, 0, (LPARAM)pcd);
		return DefWindowProc(hwnd, msg, wParam, lParam);

	case WM_CREATE:
		CREATESTRUCT *cs;
		cs = (CREATESTRUCT *) lParam;
		//pcd->hwndRef = (HWND) cs->lpCreateParams;
		pcd->Init();
		g_windows.push_back(pcd);
		return 0;

	case WM_SIZE:
		return pcd->Handle_WM_SIZE(wParam, lParam);

	case WM_DESTROY:
		SetWindowLongPtr(pcd->hwndView, GWLP_WNDPROC, (LPARAM)pcd->OldAVSViewWinProc);	

		g_windows.erase(find(g_windows.begin(),g_windows.end(),pcd));
		AVSViewerSaveSettings(hwnd,REG_WINDOW_MAIN);
		delete pcd;
		SetWindowLongPtr(hwnd, 0, 0);
		g_ScriptEditor = (HWND) -1;
		if (g_windows.empty()) clear_avs();
		break;

	case WM_SETFOCUS:
		SetFocus(pcd->hwndView);
		return 0;

	case WM_COMMAND:
		return pcd->Handle_WM_COMMAND(wParam, lParam);

	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;

	case WM_INITMENU:
		{
			HMENU hMenu = (HMENU)wParam;
			DWORD dwEnableFlags;

			dwEnableFlags = (SendMessage(pcd->hwndView, SCI_CANUNDO, 0, 0) ? (MF_BYCOMMAND|MF_ENABLED) : (MF_BYCOMMAND|MF_GRAYED));
			EnableMenuItem(hMenu,ID_EDIT_UNDO, dwEnableFlags);
			
			dwEnableFlags = (SendMessage(pcd->hwndView, SCI_CANREDO, 0, 0) ? (MF_BYCOMMAND|MF_ENABLED) : (MF_BYCOMMAND|MF_GRAYED));
			EnableMenuItem(hMenu,ID_EDIT_REDO, dwEnableFlags);

			dwEnableFlags = (!(pcd->lpszFileName[0] == 0) ? (MF_BYCOMMAND|MF_ENABLED) : (MF_BYCOMMAND|MF_GRAYED));
			EnableMenuItem(hMenu,ID_AVS_INSERT_TRIM, dwEnableFlags);
			EnableMenuItem(hMenu,ID_AVS_INSERT_FRAMESET, dwEnableFlags);
			//EnableMenuItem(hMenu,ID_AVS_INSERT_CROP, dwEnableFlags);

			dwEnableFlags = ((pcd->scriptType != SCRIPTTYPE_DECOMB) ? (MF_BYCOMMAND|MF_ENABLED) : (MF_BYCOMMAND|MF_GRAYED));
			EnableMenuItem(hMenu,ID_AVS_INSERT_FILENAME, dwEnableFlags);

			CheckMenuRadioItem(hMenu, ID_AVS_SCRIPT_NONE, ID_AVS_SCRIPT_VDSCRIPT, ID_AVS_SCRIPT_NONE+pcd->scriptType, MF_BYCOMMAND);

			dwEnableFlags = (((pcd->scriptType == SCRIPTTYPE_AVS) || (pcd->scriptType == SCRIPTTYPE_VPS)) ? (MF_BYCOMMAND|MF_ENABLED) : (MF_BYCOMMAND|MF_GRAYED));
			EnableMenuItem(hMenu, ID_AVS_SAVE_OPEN, dwEnableFlags);

		/*	EnableMenuItem(hMenu,ID_FILE_SAVE, dwEnableFlags);
			EnableMenuItem(hMenu,ID_FILE_REVERT, dwEnableFlags);
			EnableMenuItem(hMenu,ID_EDIT_TRUNCATE, dwEnableFlags);

			dwEnableFlags = (pcd->hFile != INVALID_HANDLE_VALUE ? (MF_BYCOMMAND|MF_ENABLED) : (MF_BYCOMMAND|MF_GRAYED));
			*/
		}
		return 0;

	case WM_NOTIFY:
		{
			NMHDR *phdr = (NMHDR *) lParam;
			return pcd->Handle_WM_NOTIFY(phdr->hwndFrom, phdr->code, phdr);
//			if (phdr->code == WN_KEYDOWN) {
//				MessageBoxA(pcd->hwnd, "Test", "TEST", MB_OK);
//			}
		}
		return 0;

	case WM_ACTIVATE:
		{
			if (LOWORD(wParam)!=WA_INACTIVE) {
				HMENU temp = GetMenu(hwnd);
				SetMenu(hwnd, CreateAVSMenu());
				DestroyMenu(temp);
			}
			if (!g_VDMPrefs.m_bScriptEditorSingleInstance)
				g_ScriptEditor = hwnd;
		}
		return 0;

	/*
	case WM_VDM_SENDFILENAME:
		{
			VDM_FILENAME *fn = (PVDM_FILENAME) lParam;
			if (*fn->name != 0) {
				if (IsScriptType(fn->name,SCRIPTTYPE_AVS)) {
					wcscpy(pcd->lpszFileName, fn->name);
					pcd->Open();
				}
			}
			delete [] fn->name;
			delete [] fn;
		}
		return 0;

	case WM_VDM_SENDPOS:
		{
			char buf[50];
			VDM_POS *pos = (PVDM_POS) lParam;
			//SETTEXTEX st;
			//st.flags = ST_SELECTION;
			//st.codepage = CP_ACP;
			wsprintfA(buf, "%d", pos->pos);
			SendMessageA(pcd->hwndView, SCI_REPLACESEL, 0, (LPARAM) &buf);
			delete [] pos;
		}
		return 0;

	case WM_VDM_SENDRANGE:
		{
			char buf[50];
			VDM_RANGE *range = (PVDM_RANGE) lParam;
			//SETTEXTEX st;
			//st.flags = ST_SELECTION;
			//st.codepage = CP_ACP;
			if (pcd->scriptType == SCRIPTTYPE_NONE)
//				wsprintfA(buf, "%d-%d", range->range.from, range->range.to);
				wsprintfA(buf, "%d-%d", range->range.from, range->range.to - 1); // -1 corrected by Fizick
			else
//				wsprintfA(buf, (range->tag)?"Trim(%d,%d)":"%d,%d", range->range.from, range->range.to);
				// Fixed bug with TRIM position for Avisynth - code changed by Fizick:
				if (range->range.from == 0 && range->range.to == 1)
					wsprintfA(buf, (range->tag)?"Trim(%d,%d)":"%d,%d", 0, -1);// special case of very first frame
				else
					wsprintfA(buf, (range->tag)?"Trim(%d,%d)":"%d,%d", range->range.from, range->range.to -1);// -1 corrected by Fizick
			SendMessageA(pcd->hwndView, SCI_REPLACESEL, 0, (LPARAM) &buf);
			delete [] range;
		}
		return 0;

	case WM_VDM_SENDFRAMESET:
		{
			string buffer;
			char buf[50];
			VDM_FRAMESET *fs = (PVDM_FRAMESET) lParam;
			//SETTEXTEX st;
			//st.flags = ST_SELECTION;
			//st.codepage = CP_ACP;
			VDM_BASIC_RANGE range;
			for(int i = 0; i<fs->count; i++) {
				//range = (fs->ranges + (i*sizeof(VDM_BASIC_RANGE)));
				range = fs->ranges[i];
//				wsprintfA(buf, "Trim(%d,%d)", range.from, range.to);
				// Fixed bug with TRIM position for Avisynth - code changed by Fizick:
				if (range.from == 0 && range.to == 1)
					wsprintfA(buf, "Trim(%d,%d)", 0, -1); // special case of one very first frame
				else
					wsprintfA(buf, "Trim(%d,%d)", range.from, range.to - 1); // -1 corrected by Fizick
				if (i>0) buffer += " ++ ";
				buffer += buf;
			}
			SendMessageA(pcd->hwndView, SCI_REPLACESEL, 0, (LPARAM) buffer.c_str());
			if (fs->count==0) guiMessageBox(hwnd, IDS_ERR_AVS_NOFRAMESET, IDS_ERR_CAPTION, MB_OK|MB_ICONERROR);
			delete [] fs->ranges;
			delete [] fs;
		}
		return 0;

	case WM_VDM_SENDFRAMESIZE:
		{
			char buf[50];
			VDM_FRAMESIZE *s = (PVDM_FRAMESIZE) lParam;
			//SETTEXTEX st;
			//st.flags = ST_SELECTION;
			//st.codepage = CP_ACP;
			wsprintfA(buf, "Crop(%d,%d,%d,%d)", s->frame.left, s->frame.top, s->frame.right, s->frame.bottom);
			SendMessageA(pcd->hwndView, SCI_REPLACESEL, 0, (LPARAM) &buf);
			delete [] s;
		}
		return 0;
		*/

	case WM_DEFER_ERROR:
		{
			char* s = (char*)lParam;
			MessageBoxA(hwnd, s, "File open error", MB_OK);
			free(s);
		}
		return 0;

	case WM_DROPFILES:
		return pcd->Handle_WM_DROPFILES(wParam, lParam);

		/*
	case WM_VDM_SCIEDIT_UPDATE:
		SetMenu(hwnd, CreateAVSMenu());
		pcd->SetScriptType(pcd->scriptType);
		pcd->Handle_WM_SIZE(0, 0);
		return 0;
		*/

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

void AVSEditor::UpdatePreferences() {
	SetMenu(hwnd, CreateAVSMenu());
	SetScriptType(scriptType);
	Handle_WM_SIZE(0, 0);
}

///////////////////////////////////////////////////////////////////////////

INT_PTR CALLBACK AVSEditor::FindDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	AVSEditor* pcd = (AVSEditor*) GetWindowLongPtr(hwnd, DWLP_USER);

	switch (msg) {
	case WM_INITDIALOG:
		{
			SetWindowLongPtr(hwnd, DWLP_USER, lParam);
			pcd = (AVSEditor*) lParam;
			pcd->hwndFind = hwnd;
			g_dialogs.push_back(hwnd);
			//pcd->mdnFind.hdlg = hwnd;
			//guiAddModelessDialog(&pcd->mdnFind);

			FindTextOption* opt = &pcd->mFind;

			SetDlgItemTextA(hwnd, ID_FIND_FINDWHAT, pcd->mFind.szFindString);

			CheckDlgButton(hwnd, ID_FIND_WHOLEWORD, (opt->bWholeWord) ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hwnd, ID_FIND_MATCHCASE, (opt->bMatchCase) ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hwnd, ID_FIND_REGEXP,    (opt->bRegexp   ) ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hwnd, ID_FIND_WRAP,      (opt->bWrap     ) ? BST_CHECKED : BST_UNCHECKED);

			if (opt->bFindReverse)
				CheckDlgButton(hwnd, ID_FIND_DIRECTIONUP, BST_CHECKED);
			else
				CheckDlgButton(hwnd, ID_FIND_DIRECTIONDOWN, BST_CHECKED);

			SetFocus(GetDlgItem(hwnd, ID_FIND_FINDWHAT));
			AVSViewerLoadSettings(hwnd,REG_WINDOW_FIND);
		}
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDCANCEL:
			DestroyWindow(hwnd);
			break;

		case IDOK: // Find Next
			{
				FindTextOption* opt = &pcd->mFind;

				GetDlgItemTextA(hwnd, ID_FIND_FINDWHAT, opt->szFindString, sizeof(opt->szFindString));

				opt->bWholeWord   = (IsDlgButtonChecked(hwnd, ID_FIND_WHOLEWORD  ) == BST_CHECKED);
				opt->bMatchCase   = (IsDlgButtonChecked(hwnd, ID_FIND_MATCHCASE  ) == BST_CHECKED);
				opt->bRegexp      = (IsDlgButtonChecked(hwnd, ID_FIND_REGEXP     ) == BST_CHECKED);
				opt->bWrap        = (IsDlgButtonChecked(hwnd, ID_FIND_WRAP       ) == BST_CHECKED);
				opt->bFindReverse = (IsDlgButtonChecked(hwnd, ID_FIND_DIRECTIONUP) == BST_CHECKED);

				pcd->FindNext(opt->bFindReverse);
			}
			break;
		}
		return TRUE;

	case WM_DESTROY:
		AVSViewerSaveSettings(hwnd,REG_WINDOW_FIND);
		pcd->hwndFind = NULL;
		g_dialogs.erase(find(g_dialogs.begin(),g_dialogs.end(),hwnd));
		//pcd->mdnFind.Remove();
		return TRUE;
	}
	return FALSE;
}

INT_PTR CALLBACK AVSEditor::JumpDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	AVSEditor* pcd = (AVSEditor*) GetWindowLongPtr(hwnd, DWLP_USER);

	switch (msg) {
	case WM_INITDIALOG:
		{
			SetWindowLongPtr(hwnd, DWLP_USER, lParam);
			pcd = (AVSEditor*) lParam;
			g_dialogs.push_back(hwnd);
			AVSViewerLoadSettings(hwnd,REG_WINDOW_JUMPTO);

			char buf[64];
			int64 frame = VDRequestPos();
			wsprintfA(buf, "%I64d", frame);
			SetDlgItemTextA(hwnd, IDC_FRAMENUMBER, buf);
			SetFocus(GetDlgItem(hwnd, IDC_FRAMENUMBER));
			SendDlgItemMessage(hwnd, IDC_FRAMENUMBER, EM_SETSEL, 0, -1);

			int c = pcd->SendMessageSci(SCI_GETCURRENTPOS, 0, 0);
			int line = pcd->SendMessageSci(SCI_LINEFROMPOSITION, c, 0);
			wsprintfA(buf, "%d", line+1);
			SetDlgItemTextA(hwnd, IDC_LINENUMBER, buf);

			CheckDlgButton(hwnd, IDC_JUMPTOFRAME, BST_CHECKED);
			CheckDlgButton(hwnd, IDC_JUMPTOLINE, BST_UNCHECKED);
			EnableWindow(pcd->GetHwnd(),false);
		}
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDCANCEL:
			EndDialog(hwnd, 1);
			return TRUE;

		case IDOK:
			if (IsDlgButtonChecked(hwnd, IDC_JUMPTOFRAME)) {
				char buf[64];
				GetDlgItemTextA(hwnd,IDC_FRAMENUMBER,buf,64);
				int64 frame;
				if(sscanf(buf,"%I64d",&frame)==1)
					VDSetPos(frame);
			} else {
				char buf[64];
				GetDlgItemTextA(hwnd,IDC_LINENUMBER,buf,64);
				int line;
				if(sscanf(buf,"%d",&line)==1)
					pcd->SendMessageSci(SCI_GOTOLINE, line-1);
			}
			EndDialog(hwnd, 0);
			return TRUE;

		case IDC_FRAMENUMBER:
			if (HIWORD(wParam) == EN_CHANGE) {
				CheckDlgButton(hwnd, IDC_JUMPTOFRAME, BST_CHECKED);
				CheckDlgButton(hwnd, IDC_JUMPTOLINE, BST_UNCHECKED);
			}
			break;
		case IDC_LINENUMBER:
			if (HIWORD(wParam) == EN_CHANGE) {
				CheckDlgButton(hwnd, IDC_JUMPTOFRAME, BST_UNCHECKED);
				CheckDlgButton(hwnd, IDC_JUMPTOLINE, BST_CHECKED);
			}
			break;
		case IDC_JUMPTOFRAME:
			SetFocus(GetDlgItem(hwnd, IDC_FRAMENUMBER));
			SendDlgItemMessage(hwnd, IDC_FRAMENUMBER, EM_SETSEL, 0, -1);
			break;
		case IDC_JUMPTOLINE:
			SetFocus(GetDlgItem(hwnd, IDC_LINENUMBER));
			SendDlgItemMessage(hwnd, IDC_LINENUMBER, EM_SETSEL, 0, -1);
			break;
		}
		break;

	case WM_DESTROY:
		EnableWindow(pcd->GetHwnd(),true);
		AVSViewerSaveSettings(hwnd,REG_WINDOW_JUMPTO);
		g_dialogs.erase(find(g_dialogs.begin(),g_dialogs.end(),hwnd));
		return TRUE;
	}
	return FALSE;
}

void AVSEditor::Find() {
	if (hwndFind)
		SetForegroundWindow(hwndFind);
	else
		CreateDialogParam(g_hInst, MAKEINTRESOURCE(IDD_FIND), hwndView, FindDlgProc, (LPARAM) this);
}

void AVSEditor::Jumpto() {
	CreateDialogParam(g_hInst, MAKEINTRESOURCE(IDD_JUMPTO), hwndView, JumpDlgProc, (LPARAM) this);
}

void AVSEditor::FindNext(bool reverse) {
	if (mFind.szFindString[0] == 0) return;

	int len = SendMessageSci(SCI_GETLENGTH);
	int pos = SendMessageSci(SCI_GETCURRENTPOS);

	TextToFind ft = {{0, 0}, 0, {0, 0}};
	ft.lpstrText = mFind.szFindString;

	int flags = 0;

	if	(reverse) {
		ft.chrg.cpMin = pos-1-strlen(mFind.szFindString);
		ft.chrg.cpMax = 0;
		flags = 0;
	} else {
		ft.chrg.cpMin = pos+1;
		ft.chrg.cpMax = len;
		flags = 1;
	}

	if (mFind.bWholeWord) flags |= SCFIND_WHOLEWORD;
	if (mFind.bMatchCase) flags |= SCFIND_MATCHCASE;
	if (mFind.bRegexp)    flags |= SCFIND_REGEXP;

	int ret = SendMessageSci(SCI_FINDTEXT, (WPARAM) flags, (LPARAM) &ft);

	if (mFind.bWrap && ret == -1) {
		if	(reverse) {
			ft.chrg.cpMin = len;
			ft.chrg.cpMax = pos;
		} else {
			ft.chrg.cpMin = 0;
			ft.chrg.cpMax = pos-1;
		}
		ret = SendMessageSci(SCI_FINDTEXT, (WPARAM) flags, (LPARAM) &ft);
	}

	if (ret > -1) {
		if	(reverse)
			SendMessageSci(SCI_SETSEL, (WPARAM) ft.chrgText.cpMax, (LPARAM) ft.chrgText.cpMin);
		else
			SendMessageSci(SCI_SETSEL, (WPARAM) ft.chrgText.cpMin, (LPARAM) ft.chrgText.cpMax);
	}
}

int GetScriptType(const wchar_t *fn) {
	const wchar_t* p = wcsrchr(fn, '.');
	if (!p) return SCRIPTTYPE_NONE;
	if (!_wcsicmp(p, L".avs")) return SCRIPTTYPE_AVS;
	if (!_wcsicmp(p, L".tel")) return SCRIPTTYPE_DECOMB;
	if (!_wcsicmp(p, L".fd")) return SCRIPTTYPE_DECOMB;
	if (!_wcsicmp(p, L".dec")) return SCRIPTTYPE_DECOMB;
	if (!_wcsicmp(p, L".vpy")) return SCRIPTTYPE_VPS;
	if (!_wcsicmp(p, L".vdscript")) return SCRIPTTYPE_VDSCRIPT;
	if (!_wcsicmp(p, L".vdproject")) return SCRIPTTYPE_VDSCRIPT;
	if (!_wcsicmp(p, L".jobs")) return SCRIPTTYPE_VDSCRIPT;
	return SCRIPTTYPE_NONE;
}

bool IsScriptType(const wchar_t *fn, int type) {
	return (GetScriptType(fn)==type);
}

void LoadAVSEditorIcons() {
	HRSRC hrsrc;
	HGLOBAL hGlob;

	hrsrc = FindResource(g_hInst,(LPCTSTR)IDR_SCI_KEYWORDS,_T("STUFF"));
	hGlob = LoadResource(g_hInst,hrsrc);
	imKeywords = LockResource(hGlob);
	hrsrc = FindResource(g_hInst,(LPCTSTR)IDR_SCI_INTERNAL,_T("STUFF"));
	hGlob = LoadResource(g_hInst,hrsrc);
	imInternal = LockResource(hGlob);
	hrsrc = FindResource(g_hInst,(LPCTSTR)IDR_SCI_EXTERNAL,_T("STUFF"));
	hGlob = LoadResource(g_hInst,hrsrc);
	imExternal = LockResource(hGlob);
}

ATOM RegisterAVSEditorClass() {
	WNDCLASS wc1;

	wc1.style			= 0;
	wc1.lpfnWndProc		= AVSEditor::AVSEditorWndProc;
	wc1.cbClsExtra		= 0;
	wc1.cbWndExtra		= sizeof(AVSEditor *);
	wc1.hInstance		= g_hInst;
//	wc1.hIcon			= NULL;
	wc1.hIcon			= LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_AVSEDIT));
		wc1.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc1.hbrBackground	= NULL; //(HBRUSH)(COLOR_WINDOW+1);
	wc1.lpszMenuName	= MAKEINTRESOURCE(IDR_AVSVIEWER_MENU);
	wc1.lpszClassName	= AVSEDITORCLASS;	

	return RegisterClass(&wc1);
}

AVSEditor* CreateEditor(HWND hwndParent, HWND refWND, bool bringfront) {
	if (!g_VDMPrefs.m_bScriptEditorSingleInstance || (g_ScriptEditor == (HWND) -1)) {
		HWND wnd = CreateWindow(
			AVSEDITORCLASS,
			"VirtualDub2 Script Editor",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			hwndParent,
			NULL,
			g_hInst,
			refWND);
		HMENU temp = GetMenu(wnd);
		SetMenu(wnd, CreateAVSMenu());
		DestroyMenu(temp);
		g_ScriptEditor = wnd;
	} else {
		ShowWindow(g_ScriptEditor, SW_SHOW);
	}
	(bringfront)?BringWindowToTop(g_ScriptEditor):SetWindowPos(refWND, g_ScriptEditor, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);

	SetFocus(g_ScriptEditor);

	AVSEditor* pcd = (AVSEditor *)GetWindowLongPtr(g_ScriptEditor, 0);

	return pcd;
}

bool OpenCurrentFile(HWND parent) {
	AVSEditor* obj = CreateEditor(NULL, (HWND)parent, true);
	wchar_t buf[MAX_PATH];
	VDGetFilename(buf,MAX_PATH);
	if (IsScriptType(buf,SCRIPTTYPE_AVS) || IsScriptType(buf,SCRIPTTYPE_VPS)) {
		obj->Open(buf);
		return true;
	}
	return false;
}

bool HandleFilename(HWND hwnd, const wchar_t* path) {
	for(int i=0; i<(int)g_windows.size(); i++) {
		AVSEditor* obj = g_windows[i];
		if (obj->CheckFilename(path)) return true;
	}

	bool handle = false;
	if (IsScriptType(path,SCRIPTTYPE_AVS)) handle = true;
	if (IsScriptType(path,SCRIPTTYPE_VPS)) handle = true;
	if (!handle) return false;
	if (!g_VDMPrefs.m_bScriptEditorAutoPopup) return true;

	if (g_VDMPrefs.m_bScriptEditorSingleInstance && g_ScriptEditor!=(HWND)-1)
		SendMessage(g_ScriptEditor,WM_CLOSE,0,0);

	AVSEditor* obj = CreateEditor(NULL, hwnd, false);
	obj->Open(path);
	return true;
}

bool HandleFileOpenError(HWND hwnd, const wchar_t* path, const char* s, int line) {
	for(int i=0; i<(int)g_windows.size(); i++) {
		AVSEditor* obj = g_windows[i];
		if (obj->CheckFilename(path)){
			obj->Open(path);
			obj->HandleError(s,line);
			return true;
		}
	}

	bool handle = false;
	if (IsScriptType(path,SCRIPTTYPE_AVS)) handle = true;
	if (IsScriptType(path,SCRIPTTYPE_VPS)) handle = true;
	if (IsScriptType(path,SCRIPTTYPE_VDSCRIPT)) handle = true;
	if (!handle) return false;

	if (g_VDMPrefs.m_bScriptEditorSingleInstance && g_ScriptEditor!=(HWND)-1)
		SendMessage(g_ScriptEditor,WM_CLOSE,0,0);

	AVSEditor* obj = CreateEditor(NULL, hwnd, false);
	obj->Open(path);
	obj->HandleError(s,line);
	return true;
}

bool ProcessDialogs(MSG& msg) {
	for(int i=0; i<(int)g_dialogs.size(); i++)
		if (IsDialogMessage(g_dialogs[i],&msg)) return true;
	return false;
}

bool ProcessHotkeys(MSG& msg) {
	extern HACCEL g_hAccelAVS;
	HWND hwnd = GetAncestor(msg.hwnd, GA_ROOT);
	for(int i=0; i<(int)g_windows.size(); i++) {
		AVSEditor* obj = g_windows[i];
		if (obj->GetHwnd()==hwnd) {
			if (TranslateAccelerator(hwnd, g_hAccelAVS, &msg)) return true;
		}
	}
	return false;
}

void UpdatePreferences() {
	for(int i=0; i<(int)g_windows.size(); i++)
		g_windows[i]->UpdatePreferences();
}

void HandleError(const char* s, void* userData)
{
	AVSEditor* obj = (AVSEditor*)userData;
	MessageBoxA(obj->GetHwnd(), s, "File open error", MB_OK);
}

void AttachWindows(HWND parent)
{
	for(int i=0; i<(int)g_windows.size(); i++) {
		AVSEditor* obj = g_windows[i];
		HWND hwnd = obj->GetHwnd();
		//obj->hwndRef = parent;
		ShowWindow(hwnd,SW_SHOWNOACTIVATE);
	}
}

void DetachWindows(HWND parent)
{
	for(int i=0; i<(int)g_windows.size(); i++) {
		AVSEditor* obj = g_windows[i];
		HWND hwnd = obj->GetHwnd();
		ShowWindow(hwnd,SW_HIDE);
	}
}
