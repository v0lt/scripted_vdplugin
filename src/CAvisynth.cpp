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

// http://avisynth.nl/index.php/The_full_AviSynth_grammar#Keywords
const char coKeywordsDefault[] =
	"function "
	"global "
	"return "
	"try "
	"catch"
	"last "
	"true "
	"false "
	"yes "
	"no "
	"clip "
	"int "
	"float "
	"string "
	"bool "
	"val";

const char coInternalDefault[] =
	//
	// http://avisynth.nl/index.php/Internal_functions
	//
	// Boolean functions
	"IsBool "
	"IsClip "
	"IsFloat "
	"IsInt "
	"IsString "
	"Exist "
	"Defined "
	"FunctionExists "
	"InternalFunctionExists "
	"VarExist "
	// Control functions
	"Apply "
	"Eval "
	"Import "
	"Select "
	"Default "
	"Assert "
	"NOP "
	"Undefined "
	// Global Options
	"SetMemoryMax "
	"SetCacheMode "
	"SetMaxCPU "
	"SetWorkingDir "
	"SetPlanarLegacyAlignment "
	"OPT_AllowFloatAudio "
	"OPT_UseWaveExtensible "
	"OPT_dwChannelMask "
	"OPT_AVIPadScanlines "
	"OPT_VDubPlanarHack "
	"OPT_Enable_V210 "
	"OPT_Enable_Y3_10_10 "
	"OPT_Enable_Y3_10_16 "
	"OPT_Enable_b64a "
	"OPT_Enable_PlanarToPackedRGB "
	// Conversion functions
	"Value "
	"HexValue "
	"Hex "
	"String "
	// Numeric functions
	"Max "
	"Min "
	"MulDiv "
	"Floor "
	"Ceil "
	"Round "
	"Int "
	"Float "
	"Fmod "
	"Pi "
	"Exp "
	"Log "
	"Log10 "
	"Pow "
	"Sqrt "
	"Abs "
	"Sign "
	"Frac "
	"Rand "
	"Spline "
	"ContinuedNumerator "
	"ContinuedDenominator "
	"Sin "
	"Cos "
	"Tan "
	"Asin "
	"Acos "
	"Atan "
	"Atan2 "
	"Sinh "
	"Cosh "
	"Tanh "
	// Bit functions
	"BitAnd "
	"BitNot "
	"BitOr "
	"BitXor "
	"BitLShift "
	"BitShl "
	"BitSal "
	"BitRShiftA "
	"BitRShiftS "
	"BitSar "
	"BitRShiftL "
	"BitRShiftU "
	"BitShr "
	"BitLRotate "
	"BitRol "
	"BitRRotateL "
	"BitRor "
	"BitTest "
	"BitTst "
	"BitSet "
	"BitSetCount "
	"BitClear "
	"BitClr "
	"BitChange "
	"BitChg "
	// Runtime functions
	"AverageLuma "
	"AverageChromaU "
	"AverageChromaV "
	"AverageB "
	"AverageG "
	"AverageR "
	"LumaDifference "
	"ChromaUDifference "
	"ChromaVDifference "
	"RGBDifference "
	"BDifference "
	"GDifference "
	"RDifference "
	"YDifferenceFromPrevious "
	"UDifferenceFromPrevious "
	"VDifferenceFromPrevious "
	"RGBDifferenceFromPrevious "
	"BDifferenceFromPrevious "
	"GDifferenceFromPrevious "
	"RDifferenceFromPrevious "
	"YDifferenceToNext "
	"UDifferenceToNext "
	"VDifferenceToNext "
	"RGBDifferenceToNext "
	"BDifferenceToNext "
	"GDifferenceToNext "
	"RDifferenceToNext "
	"YPlaneMedian "
	"UPlaneMedian "
	"VPlaneMedian "
	"BPlaneMedian "
	"GPlaneMedian "
	"RPlaneMedian "
	"YPlaneMin "
	"UPlaneMin "
	"VPlaneMin "
	"BPlaneMin "
	"GPlaneMin "
	"RPlaneMin "
	"YPlaneMax "
	"UPlaneMax "
	"VPlaneMax "
	"BPlaneMax "
	"GPlaneMax "
	"RPlaneMax "
	"YPlaneMinMaxDifference "
	"UPlaneMinMaxDifference "
	"VPlaneMinMaxDifference "
	"BPlaneMinMaxDifference "
	"GPlaneMinMaxDifference "
	"RPlaneMinMaxDifference "
	// Functions for frame properties
	// - Reserved frame property names
	"_ChromaLocation "
	"_ColorRange "
	"_Primaries "
	"_Matrix "
	"_Transfer "
	"_FieldBased "
	"_AbsoluteTime "
	"_DurationNum "
	"_DurationDen "
	"_Combed "
	"_Field "
	"_PictType "
	"_SARNum "
	"_SARDen "
	"_SceneChangeNext "
	"_SceneChangePrev "
	// - Property set 
	"propSet "
	"propSetInt "
	"propSetFloat "
	"propSetString "
	"propSetArray "
	"propSetClip "
	// - Property get
	"propGetAny "
	"propGetInt "
	"propGetFloat "
	"propGetString "
	"propGetAsArray "
	"propGetClip "
	"propGetAll "
	// - Deleting properties 
	"propDelete "
	"propClearAll "
	// - Other property functions
	"propShow "
	"propGetDataSize "
	"propNumElements "
	"propNumKeys "
	"propGetType "
	"propCopy "
	// Script functions
	"ScriptName "
	"ScriptNameUtf8 "
	"ScriptFile "
	"ScriptFileUtf8 "
	"ScriptDir "
	"ScriptDirUtf8 "
	"SetLogParams "
	"LogMsg "
	"GetProcessInfo "
	// String functions
	"LCase "
	"UCase "
	"StrToUtf8 "
	"StrFromUtf8 "
	"StrLen "
	"RevStr "
	"LeftStr "
	"RightStr "
	"MidStr "
	"FindStr "
	"ReplaceStr "
	"Format "
	"FillStr "
	"StrCmp "
	"StrCmpi "
	"TrimLeft "
	"TrimRight "
	"TrimAll "
	"Chr "
	"Ord "
	"Time "
	// Version functions
	"VersionNumber "
	"VersionString "
	"IsVersionOrGreater "
	// Array functions
	"ArrayAdd "
	"ArrayDel "
	"ArrayIns "
	"ArraySet "
	// Other helper functions
	"BuildPixelType "
	"ColorSpaceNameToPixelType "
	//
	// http://avisynth.nl/index.php/Internal_filters
	//
	// Source filters
	"AviSource "
	"AviFileSource "
	"OpenDMLSource "
	"DirectShowSource "
	"ImageReader "
	"ImageSource "
	"ImageSourceAnim "
	"Import "
	"SegmentedAviSource "
	"SegmentedDirectShowSource "
	"WavSource "
	// Color conversion and adjustment filters
	"ColorYUV "
	"ConvertBackToYUY2 "
	"ConvertToRGB "
	"ConvertToRGB24 "
	"ConvertToRGB32 "
	"ConvertToRGB48 "
	"ConvertToRGB64 "
	"ConvertToPlanarRGB "
	"ConvertToPlanarRGBA "
	"ConvertToYUY2 "
	"ConvertToYV24 "
	"ConvertToYV16 "
	"ConvertToYV12 "
	"ConvertToY8 "
	"ConvertToYUV444 "
	"ConvertToYUV422 "
	"ConvertToYUV420 "
	"ConvertToYUVA444 "
	"ConvertToYUVA422 "
	"ConvertToYUVA420 "
	"ConvertToYUV411 "
	"FixLuminance "
	"Greyscale "
	"Grayscale "
	"Invert "
	"Levels "
	"Limiter "
	"MergeRGB "
	"MergeARGB "
	"MergeLuma "
	"MergeChroma "
	"RGBAdjust "
	"ShowRed "
	"ShowGreen "
	"ShowBlue "
	"ShowAlpha "
	"SwapUV "
	"Tweak "
	"UToY "
	"UToY8 "
	"VToY "
	"VToY8 "
	"YToUV "
	// Overlay and Mask filters
	"ColorKeyMask "
	"Layer "
	"Mask "
	"MaskHS "
	"Merge "
	"Overlay "
	"ResetMask "
	"Subtract "
	// Geometric deformation filters
	"AddBorders "
	"Crop "
	"CropBottom "
	"FlipHorizontal "
	"FlipVertical "
	"Letterbox "
	"HorizontalReduceBy2 "
	"VerticalReduceBy2 "
	"ReduceBy2 "
	"BicubicResize "
	"BilinearResize "
	"BlackmanResize "
	"GaussResize "
	"LanczosResize "
	"Lanczos4Resize "
	"PointResize "
	"SincResize "
	"Spline16Resize "
	"Spline36Resize "
	"Spline64Resize "
	"SkewRows "
	"TurnLeft "
	"TurnRight "
	"Turn180 "
	// Pixel restoration filters
	"Blur "
	"Sharpen "
	"GeneralConvolution "
	"SpatialSoften "
	"TemporalSoften "
	"FixBrokenChromaUpsampling "
	// Timeline editing filters
	"AlignedSplice "
	"UnalignedSplice "
	"AssumeFPS "
	"AssumeScaledFPS "
	"ChangeFPS "
	"ConvertFPS "
	"DeleteFrame "
	"Dissolve "
	"DuplicateFrame "
	"FadeIn0 "
	"FadeIn "
	"FadeIn2 "
	"FadeOut0 "
	"FadeOut "
	"FadeOut2 "
	"FadeIO0 "
	"FadeIO "
	"FadeIO2 "
	"FreezeFrame "
	"Interleave "
	"Loop "
	"Reverse "
	"SelectEven "
	"SelectOdd "
	"SelectEvery "
	"SelectRangeEvery "
	"Trim "
	// Interlace filters
	"AssumeFrameBased "
	"AssumeFieldBased "
	"AssumeBFF "
	"AssumeTFF "
	"Bob "
	"ComplementParity "
	"DoubleWeave "
	"PeculiarBlend "
	"Pulldown "
	"SeparateColumns "
	"SeparateRows "
	"SeparateFields "
	"SwapFields "
	"Weave "
	"WeaveColumns "
	"WeaveRows "
	// Audio processing filters
	"Amplify "
	"AmplifydB "
	"AssumeSampleRate "
	"AudioDub "
	"AudioDubEx "
	"AudioTrim "
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
	"KillVideo "
	"MergeChannels "
	"MixAudio "
	"MonoToStereo "
	"Normalize "
	"ResampleAudio "
	"SuperEQ "
	"SSRC "
	"TimeStretch "
	// Conditional and other meta filters
	"ConditionalFilter "
	"FrameEvaluate "
	"ScriptClip "
	"ConditionalSelect "
	"ConditionalReader "
	"WriteFile "
	"WriteFileIf "
	"WriteFileStart "
	"WriteFileEnd "
	"Animate "
	"ApplyRange "
	"TCPServer "
	"TCPSource "
	// Export filters
	"ImageWriter "
	// Debug filters
	"BlankClip "
	"Blackness "
	"ColorBars "
	"ColorBarsHD "
	"Compare "
	"DumpFilterGraph "
	"SetGraphAnalysis "
	"Echo "
	"Histogram "
	"Info "
	"MessageClip "
	"Preroll "
	"ShowFiveVersions "
	"ShowFrameNumber "
	"ShowSMPTE "
	"ShowTime "
	"StackHorizontal "
	"StackVertical "
	"Subtitle "
	"Tone "
	"Version "
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
	coKeywords = _strdup(coKeywordsDefault);
	coInternal = _strdup(coInternalDefault);
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
			Version = _strdup(s.c_str());

			if (coExternal) delete coExternal;
			coExternal=NULL;
			try {
				AVSValue p = env->GetVar("$PluginFunctions$");
				coExternal = _strdup(p.AsString());
			} catch (...) {;}

			if (coAll) delete coAll;
			coAll=NULL;
			if (coAllScintilla) delete coAllScintilla;
			coAllScintilla=NULL;

			const size_t tmpsize = max(strlen(coInternal), (coExternal ? strlen(coExternal) : 0)) + 1;
			char* temp = new char[tmpsize];

			const size_t coAll_size = strlen(coKeywords) + strlen(coInternal) + (coExternal ? strlen(coExternal) : 0) + 2;
			coAll = new char[coAll_size];

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
			strcpy_s(temp, tmpsize, coKeywords);
			token = strtok(temp, " ");
			sprintf_s(sci, "?%d", ICO_SCI_AVS_KEYWORDS);
			while (token != NULL) {
				c = token;
				AVSToken.insert(c);
				c += sci;
				AVSTokenSci.insert(c);
				token = strtok(NULL, " ");
			}

			strcpy_s(temp, tmpsize, coInternal);
			token2 = strtok(temp, " ");
			sprintf_s(sci, "?%d", ICO_SCI_AVS_INTERNAL);
			while (token2 != NULL) {
				c = token2;
				AVSToken.insert(c);
				c += sci;
				AVSTokenSci.insert(c);
				token2 = strtok(NULL, " ");
			}

			if (coExternal) {
				strcpy_s(temp, tmpsize, coExternal);
				token = strtok(temp, " ");
				sprintf_s(sci, "?%d", ICO_SCI_AVS_EXTERNAL);
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
			strcpy_s(coAll, coAll_size, walkit->c_str());
			for(walkit++; walkit!=AVSToken.end();walkit++) {
				strcat_s(coAll, coAll_size, " ");
				strcat_s(coAll, coAll_size, walkit->c_str());
			}

			const size_t coAllScintilla_size = strlen(coKeywords) + strlen(coInternal) + (coExternal ? strlen(coExternal) : 0) + 2 + (AVSTokenSci.size() * 2);
			coAllScintilla = new char[coAllScintilla_size];

			walkit = AVSTokenSci.begin();
			strcpy_s(coAllScintilla, coAllScintilla_size, walkit->c_str());
			for(walkit++; walkit!=AVSTokenSci.end();walkit++) {
				strcat_s(coAllScintilla, coAllScintilla_size, " ");
				strcat_s(coAllScintilla, coAllScintilla_size, walkit->c_str());
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

	_strlwr(coInternal);
	if (coExternal) {
		_strlwr(coExternal);
	}
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
