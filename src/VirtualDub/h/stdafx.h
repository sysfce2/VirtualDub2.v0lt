// VirtualDub - Video processing and capture application
//
// Copyright (C) 1998-2002 Avery Lee
// Copyright (C) 2017 Anton Shekhovtsov
// Copyright (C) 2024-2026 v0lt
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#ifndef f_STDAFX_H
#define f_STDAFX_H

#ifdef _MSC_VER
	#pragma once
#endif

#include <vd2/system/vdtypes.h>
#include <vd2/system/vdstl.h>
#include <vd2/system/math.h>
#include <vd2/system/atomic.h>
#include <vd2/system/refcount.h>
#include <vd2/system/vdalloc.h>
#include <vd2/system/error.h>
#include <vd2/system/text.h>
#include <vd2/system/VDString.h>

#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <list>
#include <algorithm>
#include <memory>

#include <windows.h>

#include "VirtualDub.h"

// Disable some stupid VC++ warnings so we can use warning level 4.  Most of these need
// to be disabled because of the STL.
#ifdef _MSC_VER
#pragma warning(disable: 4018)	// warning C4018: signed/unsigned mismatch
								// This one is just too annoying to leave on, and Intel C/C++'s value range warnings are much more useful.
#endif

#include <vd2/Kasumi/pixmap.h>
#include <vd2/Kasumi/pixmapops.h>
#include <vd2/Kasumi/pixmaputils.h>

#endif
