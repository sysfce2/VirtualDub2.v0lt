// VirtualDub - Video processing and capture application
// Video decoding library
//
// Copyright (C) 1998-2006 Avery Lee
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#include "common_png.h"

namespace nsVDPNG {
	extern const uint8 kPNGSignature[8]={137,80,78,71,13,10,26,10};
};

void VDAdler32Checker::Process(const void *src, sint32 len) {
	const uint8 *s = (const uint8 *)src;

	while(len > 0) {
		uint32 tc = len;
		if (tc > 0x1000)
			tc = 0x1000;

		len -= tc;
		do {
			mS1 += *s++;
			mS2 += mS1;
		} while(--tc);

		mS1 %= 65521;
		mS2 %= 65521;
	}
}
