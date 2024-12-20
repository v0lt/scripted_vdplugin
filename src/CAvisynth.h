/*
 * Copyright (C) 2003 Tobias Minich
 * Copyright (C) 2024 v0lt
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _CAVISYNTH_H_
#define _CAVISYNTH_H_

#include <string>
#include "avisynth.h"

#define ICO_SCI_AVS_KEYWORDS 1
#define ICO_SCI_AVS_INTERNAL 2
#define ICO_SCI_AVS_EXTERNAL 3

class Tdll;
class CAviSynth {
	private:
		Tdll *avisynth = nullptr;
		IScriptEnvironment* (__stdcall *CreateScriptEnvironment)(int version/* = AVISYNTH_INTERFACE_VERSION*/); //TODO: uncomment when VC6 support will be dropped
		IScriptEnvironment *env = nullptr;
		PClip *clip = nullptr;
		const unsigned char *srcY,*srcU,*srcV;
		char oldscript[2048];
		unsigned int framenum;

		friend class Cvdub_source;
	protected:
	public:
		std::string Version;
		char *coKeywords;
		char *coInternal;
		char *coExternal;
		char *coAll = nullptr;
		char *coAllScintilla = nullptr;
		bool ok = false;
		int InterfaceVer = 0;

		CAviSynth(const char *path);
		void LoadDll(const char *path);
		virtual ~CAviSynth();
};

class Cvdub_source :public IClip {
	private:
		VideoInfo vi = {};
		CAviSynth *self;
	public:
		Cvdub_source(CAviSynth *Iself, IScriptEnvironment* env);
		virtual PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);
		virtual bool __stdcall GetParity(int n) {return false;}
		virtual const VideoInfo& __stdcall GetVideoInfo() {return vi;}
		virtual void __stdcall SetCacheHints(int cachehints,int frame_range) {}
		virtual void __stdcall GetAudio(void* buf, __int64 start, __int64 count, IScriptEnvironment* env) {}
		static AVSValue __cdecl Create(AVSValue args, void *self, IScriptEnvironment *env) {
			return new Cvdub_source((CAviSynth*)self,env);
		}
}; 
#endif
