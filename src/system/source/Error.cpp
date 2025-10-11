//	VirtualDub - Video processing and capture application
//	System library component
//	Copyright (C) 1998-2004 Avery Lee, All Rights Reserved.
//
//	Beginning with 1.6.0, the VirtualDub system library is licensed
//	differently than the remainder of VirtualDub.  This particular file is
//	thus licensed as follows (the "zlib" license):
//
//	This software is provided 'as-is', without any express or implied
//	warranty.  In no event will the authors be held liable for any
//	damages arising from the use of this software.
//
//	Permission is granted to anyone to use this software for any purpose,
//	including commercial applications, and to alter it and redistribute it
//	freely, subject to the following restrictions:
//
//	1.	The origin of this software must not be misrepresented; you must
//		not claim that you wrote the original software. If you use this
//		software in a product, an acknowledgment in the product
//		documentation would be appreciated but is not required.
//	2.	Altered source versions must be plainly marked as such, and must
//		not be misrepresented as being the original software.
//	3.	This notice may not be removed or altered from any source
//		distribution.

#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>

#include <vd2/system/vdtypes.h>
#include <vd2/system/Error.h>
#include <vd2/system/log.h>

MyError::MyError() {
	mbuf = NULL;
}

MyError::MyError(const MyError& err) {
	mbuf = _strdup(err.mbuf);
}

MyError::MyError(const char *f, ...)
	: mbuf(NULL)
{
	va_list val;

	va_start(val, f);
	vsetf(f, val);
	va_end(val);
}

MyError::~MyError() {
	free(mbuf);
}

void MyError::clear() {
	if (mbuf)			// we do this check because debug free() always does a heapchk even if mbuf==NULL
		free(mbuf);
	mbuf = NULL;
}

void MyError::assign(const MyError& e) {
	if (mbuf)
		free(mbuf);
	mbuf = _strdup(e.mbuf);
}

void MyError::assign(const char *s) {
	if (mbuf)
		free(mbuf);
	mbuf = _strdup(s);
}

void MyError::setf(const char *f, ...) {
	va_list val;

	va_start(val, f);
	vsetf(f,val);
	va_end(val);
}

void MyError::vsetf(const char *f, va_list val) {
	free(mbuf);
	mbuf = NULL;

	int len = _vscprintf(f, val);
	if (len >= 0) {
		size_t size = std::min(len + 1, 32768);

		mbuf = (char*)malloc(size);
		if (!mbuf) {
			return;
		}
		mbuf[0] = 0;

		_vsnprintf_s(mbuf, size, _TRUNCATE, f, val);
	}
}

void MyError::post(HWND hWndParent, const char *title) const {
	if (!mbuf || !*mbuf)
		return;

	VDDEBUG("*** %s: %s\n", title, mbuf);

	VDStringW errstr = VDTextLinesU8orAToW(mbuf, -1);
	VDLog(kVDLogError, VDswprintf(L"Error: %s", 1, errstr));

	MessageBoxW(hWndParent, errstr.c_str(), VDTextAToW(title).c_str(), MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
}

void MyError::discard() {
	free(mbuf);
	mbuf = NULL;
}

void MyError::swap(MyError& err) {
	char *s = err.mbuf;
	err.mbuf = mbuf;
	mbuf = s;
}

void MyError::TransferFrom(MyError& err) {
	if (mbuf)
		free(mbuf);

	mbuf = err.mbuf;
	err.mbuf = NULL;
}

MyMemoryError::MyMemoryError() {
	setf("Out of memory");
}

MyMemoryError::MyMemoryError(size_t requestedSize) {
	setf("Out of memory (unable to allocate %llu bytes)", (unsigned long long)requestedSize);
}

MyUserAbortError::MyUserAbortError() {
	mbuf = _strdup("");
}

MyInternalError::MyInternalError(const char *format, ...) {
	char buf[1024];
	buf[0] = 0;

	va_list val;
	va_start(val, format);
	_vsnprintf_s(buf, _TRUNCATE, format, val);
	va_end(val);

	setf("Internal error: %s", buf);
}
