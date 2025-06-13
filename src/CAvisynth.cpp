/*
 * Copyright (c) 2002 Milan Cutka
 * Copyright (C) 2003 Tobias Minich
 * Copyright (C) 2024-2025 v0lt
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "stdafx.h"
#include "CAviSynth.h"
#include "Tdll.h"
#include "Utils/StringUtil.h"

const AVS_Linkage* AVS_linkage = nullptr;

struct less_nocase {
	static bool compare_chars(char x, char y) {
		return std::toupper(x) < std::toupper(y);
	}
	bool operator()(const std::string& x, const std::string& y) const {
		return std::lexicographical_compare(x.begin(), x.end(),
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

const char coExternalDefault[] =
	"FFIndex "
	"FFAudioSource "
	"FFVideoSource "
	"FFmpegSource2 ";

//====================================== avisynth ======================================
Cvdub_source::Cvdub_source(CAviSynth *Iself, IScriptEnvironment* env)
	: self(Iself)
{
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

CAviSynth::CAviSynth(const wchar_t* path)
{
	coKeywords = _strdup(coKeywordsDefault);
	coInternal = _strdup(coInternalDefault);
	_strlwr(coInternal);
	coExternal = _strdup(coExternalDefault);

	LoadDll(path);
}

void CAviSynth::LoadDll(const wchar_t* path)
{
	std::string s;

	ok = false;
	if (env) {
		delete env;
		env = nullptr;
	}
	if (avisynth) {
		delete avisynth;
		avisynth = nullptr;
	}
	Version.clear();

	avisynth = new Tdll(path, nullptr);
	avisynth->loadFunction((void**)&CreateScriptEnvironment,"CreateScriptEnvironment");
	if (avisynth->ok) {
		ok = true;
		env = CreateScriptEnvironment(6);
		if (!env) {
			ok = false;
		} else {
			AVS_linkage = m_Linkage = env->GetAVSLinkage();
			InterfaceVer = 6;
			AVSValue a;
			try {
				AVSValue r = env->Invoke("VersionString", AVSValue(&a,0));
				s = r.AsString();
			}
			catch (...) {
				s = "VersionString not implemented";
			}
			Version = s;

			const char* avsPluginFunctions = nullptr;
			try {
				AVSValue var = env->GetVar("$PluginFunctions$"); // TODO: This doesn't work! Fix it.
				if (var.IsString()) {
					avsPluginFunctions = var.AsString();
				}
			} catch (...) {;}

			if (avsPluginFunctions && avsPluginFunctions[0]) {
				if (coExternal) {
					free(coExternal);
					coExternal = nullptr;
				}
				coExternal = _strdup(avsPluginFunctions);
			}

			if (coExternal) {
				_strlwr(coExternal);
			}

			std::set<std::string,less_nocase> AVSTokenSci;
			const char* sci = nullptr;
			const char* pos = nullptr;

			sci = "?" _CRT_STRINGIZE(ICO_SCI_AVS_KEYWORDS);
			pos = coKeywords;
			do {
				const char* begin = pos;
				while (*pos != ' ' && *pos) {
					pos++;
				}
				std::string token(begin, pos);
				token.append(sci);
				AVSTokenSci.insert(token);
			} while (0 != *pos++);

			sci = "?" _CRT_STRINGIZE(ICO_SCI_AVS_INTERNAL);
			pos = coInternal;
			do {
				const char* begin = pos;
				while (*pos != ' ' && *pos) {
					pos++;
				}
				std::string token(begin, pos);
				token.append(sci);
				AVSTokenSci.insert(token);
			} while (0 != *pos++);

			if (coExternal) {
				sci = "?" _CRT_STRINGIZE(ICO_SCI_AVS_EXTERNAL);
				pos = coExternal;
				do {
					const char* begin = pos;
					while (*pos != ' ' && *pos) {
						pos++;
					}
					std::string token(begin, pos);
					token.append(sci);
					AVSTokenSci.insert(token);
				} while (0 != *pos++);
			}

			const size_t coAllScintilla_size = strlen(coKeywords) + strlen(coInternal) + (coExternal ? strlen(coExternal) : 0) + 3 + (AVSTokenSci.size() * 2);
			coAllScintilla.clear();
			coAllScintilla.reserve(coAllScintilla_size);
			for (const auto& token : AVSTokenSci) {
				coAllScintilla.append(token);
				coAllScintilla += ' ';
			}
			str_trim_end(coAllScintilla, ' ');
		}
	}
}

CAviSynth::~CAviSynth()
{
	if (coKeywords) {
		free(coKeywords);
		coKeywords = nullptr;
	}
	if (coInternal) {
		free(coInternal);
		coInternal = nullptr;
	}
	if (coExternal) {
		free(coExternal);
		coExternal = nullptr;
	}

	AVS_linkage = m_Linkage;

	if (clip) {
		delete clip;
		clip = nullptr;
	}
	if (env) {
		delete env;
		env = nullptr;
	}

	AVS_linkage = nullptr;
	m_Linkage = nullptr;

	if (avisynth) {
		delete avisynth;
		avisynth = nullptr;
	}
}
