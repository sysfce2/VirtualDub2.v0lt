// VirtualDub - Video processing and capture application
// System library component
//
// Copyright (C) 1998-2011 Avery Lee, All Rights Reserved.
// Copyright (C) 2023 v0lt
//
// SPDX-License-Identifier: Zlib
//

#include "stdafx.h"
#include <vd2/system/date.h>
#include <vd2/system/w32assist.h>
#include <windows.h>

VDDate VDGetCurrentDate() {
	FILETIME ft;
	::GetSystemTimeAsFileTime(&ft);

	VDDate r;
	r.mTicks = ft.dwLowDateTime + ((uint64)ft.dwHighDateTime << 32);

	return r;
}

VDExpandedDate VDGetLocalDate(const VDDate& date) {
	VDExpandedDate r = {0};

	FILETIME ft;
	ft.dwLowDateTime = (uint32)date.mTicks;
	ft.dwHighDateTime = (uint32)(date.mTicks >> 32);

	SYSTEMTIME st;
	SYSTEMTIME lt;
	if (::FileTimeToSystemTime(&ft, &st) &&
		::SystemTimeToTzSpecificLocalTime(NULL, &st, &lt))
	{
		r.mYear = lt.wYear; 
		r.mMonth = (uint8)lt.wMonth; 
		r.mDayOfWeek = (uint8)lt.wDayOfWeek;
		r.mDay = (uint8)lt.wDay;
		r.mHour = (uint8)lt.wHour;
		r.mMinute = (uint8)lt.wMinute;
		r.mSecond = (uint8)lt.wSecond;
		r.mMilliseconds = (uint16)lt.wMilliseconds;
	}

	return r;
}

void VDConvertExpandedDateToNativeW32(SYSTEMTIME& dst, const VDExpandedDate& src) {
	dst.wYear = src.mYear; 
	dst.wMonth = src.mMonth; 
	dst.wDayOfWeek = src.mDayOfWeek;
	dst.wDay = src.mDay;
	dst.wHour = src.mHour;
	dst.wMinute = src.mMinute;
	dst.wSecond = src.mSecond;
	dst.wMilliseconds = src.mMilliseconds;
}

void VDAppendLocalDateString(VDStringW& dst, const VDExpandedDate& ed) {
	SYSTEMTIME st;

	VDConvertExpandedDateToNativeW32(st, ed);

	{
		int len = ::GetDateFormatW(LOCALE_USER_DEFAULT, LOCALE_USE_CP_ACP, &st, NULL, NULL, 0);

		if (len > 0) {
			vdfastvector<WCHAR> buf;
			buf.resize(len, 0);

			if (::GetDateFormatW(LOCALE_USER_DEFAULT, LOCALE_USE_CP_ACP | DATE_SHORTDATE, &st, NULL, buf.data(), len))
				dst += buf.data();
		}
	}
}

void VDAppendLocalTimeString(VDStringW& dst, const VDExpandedDate& ed) {
	SYSTEMTIME st;

	VDConvertExpandedDateToNativeW32(st, ed);

	{
		int len = ::GetTimeFormatW(LOCALE_USER_DEFAULT, LOCALE_USE_CP_ACP | TIME_NOSECONDS, &st, NULL, NULL, 0);

		if (len > 0) {
			vdfastvector<WCHAR> buf;
			buf.resize(len, 0);

			if (::GetTimeFormatW(LOCALE_USER_DEFAULT, LOCALE_USE_CP_ACP | TIME_NOSECONDS, &st, NULL, buf.data(), len))
				dst += buf.data();
		}
	}
}
