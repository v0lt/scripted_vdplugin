/*
 * Copyright (c) 2002 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

// ************************************************************************
// *** Inspired by ffdshow from http://ffdshow.sf.net					***
// *** TimgFilterAvisynth.cpp											***
// *** Tobias Minich, Feb 2003											***
// ************************************************************************


#include "stdafx.h"
#define _ASSERTE
#include "CAviSynth.h"
#include "Tdll.h"
#include <string>
#include <set>

using namespace std;

struct less_nocase {
	static bool compare_chars(char x, char y) {
		return toupper(x) < toupper(y);
	}
	bool operator()(const string& x, const string& y) const {
		return lexicographical_compare(x.begin(), x.end(),
			y.begin(), y.end(),
			compare_chars);
	}
};

const char coKeywordsDefault[] =	"end return global function clip int bool float default "
								"string last true false try catch";

const char coInternalDefault[] =
	// Numeric Functions
	"Floor "
	"Ceil "
	"Round "
	"Sin "
	"Cos "
	"Pi "
	"Log "
	"Exp "
	"Pow "
	"Sqrt "
	"Abs "
	"Sign "
	"Int "
	"Frac "
	"Float "
	"Value "
	"HexValue "
	"Rand "
	"MulDiv " // added by Fizick
	"Spline " // added by Fizick

	// String Functions
	"LCase "
	"UCase "
	"StrLen "
	"RevStr "
	"LeftStr "
	"RightStr "
	"MidStr "
	"FindStr "
	//String (already internal word)
	"Chr " // added by Fizick
	"Time " // added by Fizick

	// Version Checking Functions
	"VersionNumber "
	"VersionString "

	// Variable Type and File Checking Functions
	"IsBool "
	"IsInt "
	"IsFloat "
	"IsString "
	"IsClip "
	"Exist "
	"Defined "
//	"Default "
	"Eval "
	"Import "
	"Assert "
	"AssertEval "
	"Select " // added by Fizick
	"NOP " // added by Fizick
	"Apply " // added by Fizick
	"SetMemoryMax " // added by Fizick
	"SetPlanarLegacyAlignment " // added by Fizick

	// Plugin Functions
	"LoadPlugin "
	"LoadVirtualDubPlugin "
	"LoadVFAPIPlugin "

	// Source filters for producing a clip
	"AviSource "
	"OpenDMLSource "
	"AviFileSource "
	"WavSource "
	"DirectShowSource "
	"SegmentedAviSource "
	"SegmentedDirectShowSource "
	"BlankClip "
	"Blackness "
	"IPCSource "
	"ImageReader "// added by Fizick
	"ImageSource "// added by Fizick

	// Filters for image processing
	"AddBorders "
	"Blur "
	"Sharpen "
	"ColorYUV "
	"ConvertBackToYUY2 "
	"ConvertToRGB "
	"ConvertToRGB24 "
	"ConvertToRGB32 "
	"ConvertToYUY2 "
	"ConvertToYV12 "
	"Crop "
	"CropBottom "
	"GeneralConvolution "
	"GreyScale "
	"HSIAdjust "
	"Layer "
	"Mask "
	"ResetMask "
	"ColorKeyMask "
	"Letterbox "
	"Levels "
	"Limiter "
	"MergeARGB " // added by Fizick
	"MergeRGB" // added by Fizick
	"Merge " // added by Fizick
	"MergeChroma "
	"MergeLuma "
	"ReduceBy2 "
	"HorizontalReduceBy2 "
	"VerticalReduceBy2 "
	"RGBAdjust "
	"BicubicResize "
	"BilinearResize "
	"LanczosResize "
	"Lanczos4Resize " // added by Fizick
	"PointResize "
	"SpatialSoften "
	"TemporalSoften "
	"SwapUV "
	"UToY "
	"VToY "
	"YToUV "
	"Tweak "
	"ShowAlpha "// added by Fizick
	"ShowRed "// added by Fizick
	"ShowGreen "// added by Fizick
	"ShowBlue "// added by Fizick

	// Filters for Editing
	"AlignedSplice "
	"UnalignedSplice "
	"AssumeFPS "
	"ChangeFPS "
	"ConvertFPS "
	"DeleteFrame "
	"Dissolve "
	"DuplicateFrame "
	"FadeIn "
	"FadeIn2 "
	"FadeOut "
	"FadeOut2 "
	"FadeIn0 "// added by Fizick
	"FadeOut0 "// added by Fizick
	"FadeIO0 "// added by Fizick
	"FadeIO "// added by Fizick
	"FidaIO2 "// added by Fizick
	"FreezeFrame "
	"Loop "
	"SelectEven "
	"SelectOdd "
	"SelectEvery "
	"SelectRangeEvery "
	"Trim "

	// Filters for treating interlaced video
	"AssumeFrameBased "
	"AssumeFieldBased "
	"AssumeBFF "
	"AssumeTFF "
	"Bob "
	"ComplementParity "
	"DoubleWeave "
	"Interleave "
	"Pulldown "
	"SeparateFields "
	"SwapFields "
	"Weave "

	// Audio filters
	"Amplify "
	"AmplifydB "
	"AssumeSampleRate "
	"AudioDub "
	"AudioLength "
	"ConvertAudioTo8bit "
	"ConvertAudioTo16bit "
	"ConvertAudioTo24bit "
	"ConvertAudioTo32bit "
	"ConvertAudioToFloat "
	"ConvertToMono "
	"DelayAudio "
	"EnsureVBRMP3Sync "
	"GetChannel "
	"GetLeftChannel "
	"GetRightChannel "
	"KillAudio "
	"LowPassAudio "
	"HighPassAudio "
	"MergeChannels "
	"MixAudio "
	"MonoToStereo "
	"Normalize "
	"ResampleAudio "
	"SSRC " // added by Fizick
	"SuperEQ "// added by Fizick
	"TimeStretch "// added by Fizick
	"Tone "// added by Fizick

	// Special and exotic filters
	"Animate "
	"ApplyRange "
	"FixBrokenChromaUpsampling "
	"FixLuminance "
	"FlipHorizontal "
	"FlipVertical "
	"ImageWriter "
	"PeculiarBlend "
	"Reverse "
	"Subtitle "
	"TurnLeft "
	"TurnRight "
	"ConditionalFilter " // added by Fizick
	"ConditionalReader " // added by Fizick
	"FrameEvaluate " // added by Fizick
	"ScriptClip "// added by Fizick
	"Turn180 "// added by Fizick
	"WriteFile " // added by Fizick
	"WriteFileIf " // added by Fizick
	"WriteFileStart " // added by Fizick
	"WriteFileEnd " // added by Fizick

	// Debug filters
	"ColorBars "
	"Compare "
	"Histogram "
	"Info "
	"MessageClip "
	"ShowFiveVersions "
	"ShowFrameNumber "
	"ShowSMPTE "
	"StackHorizontal "
	"StackVertical "
	"Subtract "
	"Version "

    "SetWorkingDir "
	"LoadCPlugin "
	"AudioDubEx "
	"ConvertToY8 "
	"ConvertToY411 "
	"ConvertToYV16 "
	"ConvertToYV24 "
	"FadeIO2 "
	"Invert "
	"Merge "
	"UToY8 "
	"VToY8 "
    ;

//====================================== avisynth ======================================
Cvdub_source::Cvdub_source(CAviSynth *Iself, IScriptEnvironment* env):self(Iself)
{
	memset(&vi, 0, sizeof(VideoInfo));
/*	vi.width=self->dxY;
	vi.height=self->dyY;
	vi.fps_numerator=25;
	vi.fps_denominator=1;
	vi.num_frames=1078920;   // 1 hour
	vi.pixel_type=VideoInfo::CS_YV12;
*/}

PVideoFrame __stdcall Cvdub_source::GetFrame(int n, IScriptEnvironment* env) 
{
	PVideoFrame frame = env->NewVideoFrame(vi);
/*	unsigned char *p = frame->GetWritePtr(PLANAR_Y);
	int stride = frame->GetPitch(PLANAR_Y);
	int y;
	for (y=0; y<self->dyY; y++) 
		memcpy(p+stride*y, self->srcY+self->strideY*y, self->dxY);
	p = frame->GetWritePtr(PLANAR_U);
	stride = frame->GetPitch(PLANAR_U);
	for (y=0; y<self->dyY/2; y++) 
		memcpy(p+stride*y, self->srcU+self->strideUV*y, self->dxUV);
	p = frame->GetWritePtr(PLANAR_V);
	stride = frame->GetPitch(PLANAR_V);
	for (y=0; y<self->dyY/2; y++) 
		memcpy(p+stride*y, self->srcV+self->strideUV*y, self->dxUV);
*/	return frame;
}

//======================================= ffdshow =======================================
CAviSynth::CAviSynth(const char *path)
{
	clip = NULL;
	env = NULL;
	avisynth = NULL;
	Version = NULL;
	coKeywords = strdup(coKeywordsDefault);
	coInternal = strdup(coInternalDefault);
	coExternal = NULL;
	coAll = NULL;
	coAllScintilla = NULL;
	ok = false;
	Version25 = false;
	oldscript[0] = '\0';
	LoadDll(path);
}

void CAviSynth::LoadDll(const char *path)
{
	string s;

	ok = false;
	Version25 = false;
	if (env) delete env;
	env = NULL;
	if (avisynth) delete avisynth;
	avisynth = NULL;
	if (Version) delete Version;

	avisynth = new Tdll(path,NULL);
	avisynth->loadFunction((void**)&CreateScriptEnvironment,"CreateScriptEnvironment");
	if (avisynth->ok) {
		ok = true;
		env = CreateScriptEnvironment(2);
		if (!env) {
			env = CreateScriptEnvironment(1);
		} else {
			Version25 = true;
		}
		if (!env) {
			ok = false;
		} else {
			AVSValue a;
			try {
				AVSValue r = env->Invoke("VersionString", AVSValue(&a,0));
				s = r.AsString();
			} catch (...) {s = "Version < 2.07; VersionString() not implemented";}
			Version = strdup(s.c_str());

			if (coExternal) delete coExternal;
			coExternal=NULL;
			try {
				AVSValue p = env->GetVar("$PluginFunctions$");
				coExternal = strdup(p.AsString());
			} catch (...) {;}

			if (coAll) delete coAll;
			coAll=NULL;
			if (coAllScintilla) delete coAllScintilla;
			coAllScintilla=NULL;
			
 			int tmpsize = max(strlen(coInternal),(coExternal?strlen(coExternal):0))+1;
			char *temp;
			coAll = new char[strlen(coKeywords)+strlen(coInternal)+(coExternal?strlen(coExternal):0)+2];
			char *token;
			char *token2;
			//string *c;
			string c;
			char sci[3];
			set<string,less_nocase> AVSToken;
			set<string,less_nocase> AVSTokenSci;
		
/*			strcpy(temp, coKeywords);
			token = strtok(temp, " ");
			sprintf(sci, "?%d", ICO_SCI_AVS_KEYWORDS);
			while (token != NULL) {
				c = new string;
				*c = token;
				AVSToken.insert(*c);
				*c += sci;
				AVSTokenSci.insert(*c);
				token = strtok(NULL, " ");
				delete c;
			}
*/
			temp = new char[tmpsize];
			strcpy(temp, coKeywords);
			token = strtok(temp, " ");
			sprintf(sci, "?%d", ICO_SCI_AVS_KEYWORDS);
			while (token != NULL) {
				c = token;
				AVSToken.insert(c);
				c += sci;
				AVSTokenSci.insert(c);
				token = strtok(NULL, " ");
			}
		
			strcpy(temp, coInternal);
			token2 = strtok(temp, " ");
			sprintf(sci, "?%d", ICO_SCI_AVS_INTERNAL);
			while (token2 != NULL) {
				c = token2;
				AVSToken.insert(c);
				c += sci;
				AVSTokenSci.insert(c);
				token2 = strtok(NULL, " ");
			}
		
			if (coExternal) {
				strcpy(temp, coExternal);
				token = strtok(temp, " ");
				sprintf(sci, "?%d", ICO_SCI_AVS_EXTERNAL);
				while (token != NULL) {
					c = token;
					AVSToken.insert(c);
					c += sci;
					AVSTokenSci.insert(c);
					token = strtok(NULL, " ");
				}
			}
		
			set<string,less_nocase>::iterator walkit;
			walkit = AVSToken.begin();
			strcpy(coAll, walkit->c_str());
			for(walkit++; walkit!=AVSToken.end();walkit++) {
				strcat(coAll, " ");
				strcat(coAll, walkit->c_str());
			}
		
			coAllScintilla = new char[strlen(coKeywords)+strlen(coInternal)+(coExternal?strlen(coExternal):0)+2+(AVSTokenSci.size()*2)];
			walkit = AVSTokenSci.begin();
			strcpy(coAllScintilla, walkit->c_str());
			for(walkit++; walkit!=AVSTokenSci.end();walkit++) {
				strcat(coAllScintilla, " ");
				strcat(coAllScintilla, walkit->c_str());
			}
 
			AVSToken.clear();
			AVSTokenSci.clear();
			delete [] temp;
 			
		}
/*		if (env) {
			env->AddFunction("ffdshow_source","",Tffdshow_source::Create,this);
			char script[2048];
			sprintf(script,"ffdshow_source()\n%s",oldscript);
			AVSValue eval_args[]={script,"ffdshow_avisynth_script"};
			try {
				AVSValue val=env->Invoke("Eval",AVSValue(eval_args,2));
				if (val.IsClip()) {
					clip=new PClip;
					*clip=val.AsClip();
					framenum=0;
				} 
				deci->drawOSD(0,50,""); 
			} catch (AvisynthError &err) {
				deci->drawOSD(0,50,err.msg); 
			} 
		} */
	}

	strlwr(coInternal);
	if(coExternal) strlwr(coExternal);
}

CAviSynth::~CAviSynth()
{
	if (Version) delete Version;
	Version=NULL;
	if (coKeywords) delete coKeywords;
	coKeywords=NULL;
	if (coInternal) delete coInternal;
	coInternal=NULL;
	if (coExternal) delete coExternal;
	coExternal=NULL;
	if (coAll) delete coAll;
	coAll=NULL;
	if (coAllScintilla) delete coAllScintilla;
	coAllScintilla=NULL;
	if (clip) delete clip;
	clip=NULL;
	if (env) delete env;
	env=NULL;
	if (avisynth) delete avisynth;
	avisynth=NULL;
}

/*
bool TimgFilterAvisynth::process(TffPict3* &pict,const TpresetSettings *cfg)
{
 if (!cfg->avisynth.script[0]) return false;
 if (strcmp(oldscript,cfg->avisynth.script)!=0)
  {
   strcpy(oldscript,cfg->avisynth.script);
   done();
  }
 Trect3 *r=init(pict,cfg->fullAvisynth);
 if (!clip) return false;
 srcY=getCur(pict,0);srcU=getCur(pict,1);srcV=getCur(pict,2);
 unsigned char *dstY=getNext(pict,0),*dstU=getNext(pict,1),*dstV=getNext(pict,2);

 PVideoFrame frame=(*clip)->GetFrame(framenum++,env); //TODO: replace with actual frame number (when MPEG frame counting will be finished)
 pict->data[0]=(unsigned char*)frame->GetReadPtr(PLANAR_Y);pict->stride[0]=frame->GetPitch(PLANAR_Y);pict->ro[0]=true;
 pict->data[1]=(unsigned char*)frame->GetReadPtr(PLANAR_U);pict->stride[1]=frame->GetPitch(PLANAR_U);pict->ro[1]=true;
 pict->data[2]=(unsigned char*)frame->GetReadPtr(PLANAR_V);pict->stride[2]=frame->GetPitch(PLANAR_V);pict->ro[2]=true;
 //pict->calcDiff();
 return true;
}
*/
