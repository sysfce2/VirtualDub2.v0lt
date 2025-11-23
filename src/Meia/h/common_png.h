// VirtualDub - Video processing and capture application
// Video decoding library
//
// Copyright (C) 1998-2006 Avery Lee
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#ifndef f_VD2_MEIA_COMMON_PNG_H
#define f_VD2_MEIA_COMMON_PNG_H

#include <vd2/system/vdtypes.h>

namespace nsVDPNG {
	extern const uint8 kPNGSignature[8];
};

/// Computes the Adler-32 checksum of a block of memory.
class VDAdler32Checker {
public:
	VDAdler32Checker() : mS1(1), mS2(0) {}

	void Process(const void *src, sint32 len);

	uint32 Adler32() const { return mS1 + (mS2 << 16); }

	static uint32 Adler32(const void *src, sint32 len) {
		VDAdler32Checker checker;
		checker.Process(src, len);
		return checker.Adler32();
	}

protected:
	uint32	mS1;
	uint32	mS2;
};

#endif
