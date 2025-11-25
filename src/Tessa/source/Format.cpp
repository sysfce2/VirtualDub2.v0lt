// VirtualDub - Video processing and capture application
// 3D acceleration library
//
// Copyright (C) 2013 Avery Lee
//
// SPDX-License-Identifier: GPL-2.0-or-later

//

#include <stdafx.h>
#include <vd2/Tessa/Format.h>

uint32 VDTGetBytesPerBlockRow(VDTFormat format, uint32 w) {
	switch(format) {
		default:
			return 0;

		case kVDTF_R8G8B8A8:
		case kVDTF_B8G8R8A8:
			return w << 2;

		case kVDTF_U8V8:
		case kVDTF_L8A8:
		case kVDTF_R8G8:
		case kVDTF_B5G6R5:
		case kVDTF_B5G5R5A1:
		return w << 1;

		case kVDTF_R8:
			return w;
	}
}

uint32 VDTGetNumBlockRows(VDTFormat format, uint32 h) {
	switch(format) {
		default:
			return 0;

		case kVDTF_R8G8B8A8:
		case kVDTF_B8G8R8A8:
		case kVDTF_U8V8:
		case kVDTF_L8A8:
		case kVDTF_R8G8:
		case kVDTF_B5G6R5:
		case kVDTF_B5G5R5A1:
		case kVDTF_R8:
			return h;
	}
}
