// VirtualDub - Video processing and capture application
//
// Copyright (C) 1998-2001 Avery Lee
// Copyright (C) 2025-2026 v0lt
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#include "stdafx.h"

#include <windows.h>
#include <vfw.h>

#include "command.h"

///////////////////////////////////////////////////////////////////////////

const struct CodecEntry {
	FOURCC fcc;
	const char *name;
} codec_entries[]={
	{ FCC('VCR1'), "ATI video 1" },
	{ FCC('VCR2'), "ATI video 2" },
	{ FCC('TR20'), "Duck TrueMotion 2.0" },
	{ FCC('dvsd'), "DV" },
	{ FCC('HFYU'), "Huffyuv" },
	{ FCC('I263'), "Intel H.263" },
	{ FCC('I420'), "LifeView YUV12 codec" },
	{ FCC('IR21'), "Indeo Video 2.1" },
	{ FCC('IV31'), "Indeo Video 3.1" },
	{ FCC('IV32'), "Indeo Video 3.2" },
	{ FCC('IV41'), "Indeo Video 4.1" },
	{ FCC('IV50'), "Indeo Video 5.x" },
	{ FCC('UCOD'), "Iterated Systems' ClearVideo" },
	{ FCC('mjpg'), "Motion JPEG" },
	{ FCC('MJPG'), "Motion JPEG" },
	{ FCC('dmb1'), "Motion JPEG (Matrox)" },
	{ FCC('MPG4'), "Microsoft High-Speed MPEG-4 " },
	{ FCC('MP42'), "Microsoft High-Speed MPEG-4 V2" },
	{ FCC('MP43'), "Microsoft High-Speed MPEG-4 V3" },
	{ FCC('DIV3'), "Microsoft High-Speed MPEG-4 V3 [Hack: DivX Low-Motion]" },
	{ FCC('DIV4'), "Microsoft High-Speed MPEG-4 V3 [Hack: DivX Fast-Motion]" },
	{ FCC('AP41'), "Microsoft High-Speed MPEG-4 V3 [Hack: AngelPotion Definitive]" },
	{ FCC('MRLE'), "Microsoft RLE" },
	{ FCC('MSVC'), "Microsoft Video 1" },
	{ FCC('CRAM'), "Microsoft Video 1" },
	{ FCC('DIVX'), "DivX 4+" },
	{ FCC('CVID'), "Radius Cinepak" },
	{ FCC('VIVO'), "VivoActive" },

	{ FCC('vifp'), "VFAPI reader codec" },
	{ FCC('VDST'), "VirtualDub frameclient driver" },
};

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
