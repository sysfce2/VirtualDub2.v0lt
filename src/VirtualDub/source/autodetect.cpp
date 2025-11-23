// VirtualDub - Video processing and capture application
//
// Copyright (C) 1998-2001 Avery Lee
// Copyright (C) 2025 v0lt
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#include "stdafx.h"

#include <windows.h>
#include <vfw.h>

#include "command.h"

///////////////////////////////////////////////////////////////////////////

#define R(x) ((((x)&0xFF000000)>>24) | (((x)&0x00FF0000)>>8) | (((x)&0x0000FF00)<<8) | (((x)&0x000000FF)<<24))

const struct CodecEntry {
	FOURCC fcc;
	const char *name;
} codec_entries[]={
	{ R('VCR1'), "ATI video 1" },
	{ R('VCR2'), "ATI video 2" },
	{ R('TR20'), "Duck TrueMotion 2.0" },
	{ R('dvsd'), "DV" },
	{ R('HFYU'), "Huffyuv" },
	{ R('I263'), "Intel H.263" },
	{ R('I420'), "LifeView YUV12 codec" },
	{ R('IR21'), "Indeo Video 2.1" },
	{ R('IV31'), "Indeo Video 3.1" },
	{ R('IV32'), "Indeo Video 3.2" },
	{ R('IV41'), "Indeo Video 4.1" },
	{ R('IV50'), "Indeo Video 5.x" },
	{ R('UCOD'), "Iterated Systems' ClearVideo" },
	{ R('mjpg'), "Motion JPEG" },
	{ R('MJPG'), "Motion JPEG" },
	{ R('dmb1'), "Motion JPEG (Matrox)" },
	{ R('MPG4'), "Microsoft High-Speed MPEG-4 " },
	{ R('MP42'), "Microsoft High-Speed MPEG-4 V2" },
	{ R('MP43'), "Microsoft High-Speed MPEG-4 V3" },
	{ R('DIV3'), "Microsoft High-Speed MPEG-4 V3 [Hack: DivX Low-Motion]" },
	{ R('DIV4'), "Microsoft High-Speed MPEG-4 V3 [Hack: DivX Fast-Motion]" },
	{ R('AP41'), "Microsoft High-Speed MPEG-4 V3 [Hack: AngelPotion Definitive]" },
	{ R('MRLE'), "Microsoft RLE" },
	{ R('MSVC'), "Microsoft Video 1" },
	{ R('CRAM'), "Microsoft Video 1" },
	{ R('DIVX'), "DivX 4+" },
	{ R('CVID'), "Radius Cinepak" },
	{ R('VIVO'), "VivoActive" },

	{ R('vifp'), "VFAPI reader codec" },
	{ R('VDST'), "VirtualDub frameclient driver" },
};

#undef R

const char *LookupVideoCodec(uint32 fccType)
{
	for (int i = 0; i < 3; i++) {
		int c = (int)((fccType >> (8 * i)) & 255);

		if (isalpha(c)) {
			fccType = (fccType & ~(FOURCC)(0xff << (i * 8))) | (toupper(c) << (i * 8));
		}
	}

	for (const auto& codec_entry : codec_entries) {
		if (codec_entry.fcc == fccType) {
			return codec_entry.name;
		}
	}

	return NULL;
}
