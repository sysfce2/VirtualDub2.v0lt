// VirtualDub - Video processing and capture application

// A/V interface library
//
// Copyright (C) 2013 Avery Lee
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#ifndef f_VD2_VDDISPLAY_BICUBIC_H
#define f_VD2_VDDISPLAY_BICUBIC_H

#include <vd2/system/vdtypes.h>

void VDDisplayCreateBicubicTexture(uint32 *dst, int w, int srcw, bool swapRB);

#endif	// f_VD2_VDDISPLAY_BICUBIC_H
