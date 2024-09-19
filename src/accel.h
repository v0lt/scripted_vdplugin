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

//  Customizable Hotkeys for VirtualDubMod

#ifndef f_ACCEL_H
#define f_ACCEL_H

#include <windows.h>

#define VK_0	0x30
#define VK_1	0x31
#define VK_2	0x32
#define VK_3	0x33
#define VK_4	0x34
#define VK_5	0x35
#define VK_6	0x36
#define VK_7	0x37
#define VK_8	0x38
#define VK_9	0x39

#define VK_A	0x41
#define VK_B	0x42
#define VK_C	0x43
#define VK_D	0x44
#define VK_E	0x45
#define VK_F	0x46
#define VK_G	0x47
#define VK_H	0x48
#define VK_I	0x49
#define VK_J	0x4A
#define VK_K	0x4B
#define VK_L	0x4C
#define VK_M	0x4D
#define VK_N	0x4E
#define VK_O	0x4F
#define VK_P	0x50
#define VK_Q	0x51
#define VK_R	0x52
#define VK_S	0x53
#define VK_T	0x54
#define VK_U	0x55
#define VK_V	0x56
#define VK_W	0x57
#define VK_X	0x58
#define VK_Y	0x59
#define VK_Z	0x5A

#define ID_COMMAND_DEPRECATED	0

typedef struct tagACCELKEY { 
    BYTE   fVirt; 
    WORD   key; 
} ACCELKEY, *LPACCELKEY;

typedef struct tagACCELKEYS {
	ACCELKEY altkey[2];
} ACCELKEYS, *LPACCELKEYS;

typedef struct tagACCELKEYSTRANSFER {
	char *description;
	ACCELKEYS command;
} ACCELKEYSTRANSFER, *LPACCELKEYSTRANSFER;

// Script Editor

#define VDM_ACCEL_AVS_COUNT		31

typedef struct tagACCELKEYTABLE_AVS {
	ACCELKEYS command[VDM_ACCEL_AVS_COUNT];
} ACCELKEYTABLE_AVS, *LPACCELKEYTABLE_AVS;

void InitDescriptions();
void GetKeyString(BYTE fVirt, WORD key, char *buffer, int len);
HACCEL CreateAVSAccelerators();
HMENU CreateAVSMenu();

#endif