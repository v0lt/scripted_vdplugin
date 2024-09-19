#include "stdafx.h"
#include <vd2/system/VDString.h>
#include <vd2/system/registry.h>
#include <vd2/plugin/vdtool.h>
#include <vd2/VDXFrame/Unknown.h>
#include "CAviSynth.h"
#include "AVSViewer.h"
#include "accel.h"
#include "prefs.h"
#include "api.h"

HINSTANCE g_hInst;
ATOM		g_classAVS;
CAviSynth	*g_dllAviSynth;
HACCEL		g_hAccelAVS;

extern "C" bool Scintilla_RegisterClasses(void *hInstance);
extern "C" bool Scintilla_ReleaseResources();

void VDUISaveWindowPlacementW32(HWND hwnd, const char *name) {
	VDRegistryKey key(REG_KEY_APP"\\Window Placement");

	WINDOWPLACEMENT wp = {sizeof(WINDOWPLACEMENT)};

	if (GetWindowPlacement(hwnd, &wp))
		key.setBinary(name, (const char *)&wp.rcNormalPosition, sizeof(RECT));
}

void VDUIRestoreWindowPlacementW32(HWND hwnd, const char *name) {
	if (!IsZoomed(hwnd) && !IsIconic(hwnd)) {
		VDRegistryKey key(REG_KEY_APP"\\Window Placement");
		RECT r;

		if (key.getBinaryLength(name) == sizeof(r) && key.getBinary(name, (char *)&r, sizeof r)) {
			WINDOWPLACEMENT wp = {sizeof(WINDOWPLACEMENT)};

			if (GetWindowPlacement(hwnd, &wp)) {
				wp.length			= sizeof(WINDOWPLACEMENT);
				wp.flags			= 0;
				wp.showCmd			= SW_SHOWNORMAL;
				wp.rcNormalPosition	= r;

				SetWindowPlacement(hwnd, &wp);
			}
		}
	}
}

void AVSViewerLoadSettings(HWND hwnd, const char* name) {
	if (!hwnd) return;
	VDUIRestoreWindowPlacementW32(hwnd, name);
}

void AVSViewerSaveSettings(HWND hwnd, const char* name) {
	if (!hwnd) return;
	VDUISaveWindowPlacementW32(hwnd, name);
}

void VDRequestFrameSize(vd_framesize& frame)
{
	frame.frametype = 0;
	RECT r = {0,0,100,100};
	frame.frame = r;
}

void init_avs()
{
	if(g_dllAviSynth) return;
	g_dllAviSynth = new CAviSynth("avisynth.dll");
}

void clear_avs()
{
	delete g_dllAviSynth;
	g_dllAviSynth = 0;
}

bool initialize()
{
	LoadPrefs();
	//InitDescriptions();
	g_classAVS = RegisterAVSEditorClass();
	LoadAVSEditorIcons();
	//init_avs();
	Scintilla_RegisterClasses(g_hInst);
	g_hAccelAVS = CreateAVSAccelerators();
	return true;
}

void uninitialize()
{
	Scintilla_ReleaseResources();
	clear_avs();
	DestroyAcceleratorTable(g_hAccelAVS);
}

bool TranslateMessages(MSG& msg)
{
	if (ProcessHotkeys(msg)) return true;
	if (ProcessDialogs(msg)) return true;
	return false;
}

#ifndef PLUGIN

wchar_t g_fileName[MAX_PATH];

void VDGetFilename(wchar_t* buf, size_t n)
{
	wcscpy_s(buf,n,g_fileName);
}

void VDSetFilename(wchar_t* s, void*)
{
	wcscpy_s(g_fileName,MAX_PATH,s);
}

void VDSendReopen(const wchar_t* fileName, void* userData)
{
	::HandleError("Reopen not implemented",userData);
}

int64 VDRequestPos()
{
	return 0;
}

void VDSetPos(int64 pos)
{
}

void VDRequestRange(int64& r0, int64& r1)
{
	r0 = 0;
	r1 = 10;
}

void VDRequestFrameset(vd_frameset& set, int max)
{
	set.count = 0;
	if(max<2) return;
	set.count = 2;
	set.ranges[0].from = 0;
	set.ranges[0].to = 10;
	set.ranges[1].from = 0;
	set.ranges[1].to = 10;
}

int APIENTRY WinMain(HINSTANCE hInstance,
										 HINSTANCE hPrevInstance,
										 LPSTR     lpCmdLine,
										 int       nCmdShow)
{
	::g_hInst = hInstance;

	if(__argc>1){
		char* s = __argv[1];
		VDTextAToW(g_fileName,MAX_PATH,s);
	}

	initialize();
	//AVSEdit(NULL, NULL, true);

	while(true){
		MSG msg;
		while(PeekMessage(&msg,0,0,0,PM_NOREMOVE)){
			if(!GetMessage(&msg,0,0,0)){
				return 0;
			}
		
			if(TranslateMessages(msg)) continue;

			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		if (g_ScriptEditor==(HWND)-1) break;
	}

	uninitialize();

	return 0;
}

#endif

#ifdef PLUGIN

const VDXToolContext* g_context;

void VDGetFilename(wchar_t* buf, size_t n)
{
	g_context->mpCallbacks->GetFileName(buf,n);
}

void VDSetFilename(wchar_t* s, void* userData)
{
	g_context->mpCallbacks->SetFileName(s, L"AVIFile/Avisynth input driver (internal)", userData);
}

void VDSendReopen(const wchar_t* fileName, void* userData)
{
	g_context->mpCallbacks->Reopen(fileName, 0, userData);
}

int64 VDRequestPos()
{
	IVDTimeline* t = g_context->mpCallbacks->GetTimeline();
	return t->GetTimelinePos();
}

void VDSetPos(int64 pos)
{
	IVDTimeline* t = g_context->mpCallbacks->GetTimeline();
	t->SetTimelinePos(pos);
}

void VDRequestRange(int64& r0, int64& r1)
{
	IVDTimeline* t = g_context->mpCallbacks->GetTimeline();
	int64 start;
	int64 end;
	t->GetSelection(start,end);
	r0 = start;
	r1 = end;
}

void VDRequestFrameset(vd_frameset& set, int max)
{
	IVDTimeline* t = g_context->mpCallbacks->GetTimeline();
	int count = t->GetSubsetCount();
	set.count = 0;
	if(max<count) return;
	set.count = count;
	for(int i=0; i<count; i++){
		int64 start;
		int64 end;
		t->GetSubsetRange(i,start,end);
		set.ranges[i].from = start;
		set.ranges[i].to = end;
	}
}

class ToolDriver: public vdxunknown<IVDXTool> {
	virtual ~ToolDriver() {
		uninitialize();
	}

	virtual bool VDXAPIENTRY GetMenuInfo(int id, char* name, int name_size, bool* enabled) {
		if (id==0) {
			strcpy(name,"Script Editor");
			*enabled = true;
			return true;
		}
		return false;
	}
	virtual bool VDXAPIENTRY GetCommandId(int id, char* name, int name_size) {
		if (id==0) {
			strcpy(name,"Tools.ScriptEditor");
			return true;
		}
		return false;
	}
	virtual bool VDXAPIENTRY ExecuteMenu(int id, VDXHWND hwndParent) {
		if (id==0) {
			OpenCurrentFile((HWND)hwndParent);
			return true;
		}
		return false;
	}
	virtual bool VDXAPIENTRY TranslateMessage(MSG& msg) {
		return TranslateMessages(msg);
	}
	virtual bool VDXAPIENTRY HandleError(const char* s, int source, void* userData) {
		::HandleError(s,userData);
		return true;
	}
	virtual bool VDXAPIENTRY HandleFileOpen(const wchar_t* fileName, const wchar_t* driverName, VDXHWND hwndParent) {
		return HandleFilename((HWND)hwndParent,fileName);
	}
	virtual bool VDXAPIENTRY HandleFileOpenError(const wchar_t* fileName, const wchar_t* driverName, VDXHWND hwndParent, const char* s, int source) {
    return ::HandleFileOpenError((HWND)hwndParent,fileName,s,source);
	}
	virtual void VDXAPIENTRY Attach(VDXHWND hwndParent) {
		AttachWindows((HWND)hwndParent);
	}
	virtual void VDXAPIENTRY Detach(VDXHWND hwndParent) {
		DetachWindows((HWND)hwndParent);
	}
};

bool VDXAPIENTRY create(const VDXToolContext *pContext, IVDXTool **pp)
{
	if(!initialize()) return false;

	ToolDriver *p = new ToolDriver();
	if(!p) return false;
	*pp = p;
	p->AddRef();
	g_context = pContext;
	return true;
}

bool VDXAPIENTRY config(VDXHWND parent)
{
	ShowPrefs((HWND)parent);
	return true;
}

VDXToolDefinition tool_def={
	sizeof(VDXToolDefinition),
	create
};

VDXPluginInfo plugin_def={
	sizeof(VDXPluginInfo),
	L"Script Editor",
	L"Anton Shekhovtsov",
	L"Text editor for avs scripts.",
	1,
	kVDXPluginType_Tool,
	0,
	11,
	kVDXPlugin_APIVersion,
	kVDXPlugin_ToolAPIVersion,
	kVDXPlugin_ToolAPIVersion,
	&tool_def,
	0,
	config
};

VDPluginInfo* kPlugins[]={&plugin_def,0};

extern "C" VDPluginInfo** VDXAPIENTRY VDGetPluginInfo()
{
	return kPlugins;
}

BOOLEAN WINAPI DllMain( IN HINSTANCE hDllHandle, IN DWORD nReason, IN LPVOID Reserved )
{
	switch ( nReason ){
	case DLL_PROCESS_ATTACH:
		g_hInst = hDllHandle;
		return true;

	case DLL_PROCESS_DETACH:
		return true;
	}

	return true;
}

#endif
