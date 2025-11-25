// VirtualDub - Video processing and capture application
// 3D acceleration library
//
// Copyright (C) 2013 Avery Lee
// Copyright (C) 2024-2025 v0lt
//
// SPDX-License-Identifier: GPL-2.0-or-later

//

#include <stdio.h>
#include <stdarg.h>
#include <vd2/Tessa/Context.h>

void VDTBeginScopeF(IVDTProfiler *profiler, uint32 color, const char *format, ...) {
	char buf[256];
	buf[0] = 0;

	va_list val;
	va_start(val, format);
	_vsnprintf_s(buf, _TRUNCATE, format, val);
	va_end(val);

	profiler->BeginScope(color, buf);
}
