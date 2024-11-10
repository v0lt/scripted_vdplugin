/*
 * Copyright (C) 2016-2019 Anton Shekhovtsov
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef api_header
#define api_header

#include <vd2/system/vdtypes.h>

void VDGetFilename(wchar_t* buf, size_t n);
void VDSetFilename(wchar_t* s, void* userData);
void VDSendReopen(const wchar_t* fileName, void* userData);
int64 VDRequestPos();
void VDRequestRange(int64& r0, int64& r1);
void VDSetPos(int64 pos);

struct vd_basic_range {
	int64 from;
	int64 to;
};

struct vd_frameset {
	int count;
	vd_basic_range* ranges;	// must be assigned by the recipient
};

void VDRequestFrameset(vd_frameset& set, int max);

struct vd_framesize {
	int frametype;				// 0 = input / 1 = output
	RECT frame;					// right and bottom hold width and height respectively
};

void VDRequestFrameSize(vd_framesize&);

#endif
