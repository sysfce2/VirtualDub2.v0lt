// VirtualDub - Video processing and capture application
// System library component
//
// Copyright (C) 1998-2004 Avery Lee, All Rights Reserved.
// Copyright (C) 2023-2025 v0lt
//
// SPDX-License-Identifier: Zlib
//

#include "stdafx.h"
#include <windows.h>

#include <vd2/system/error.h>
#include <vd2/system/filesys.h>
#include <vd2/system/VDString.h>
#include <vd2/system/file.h>

namespace {
	bool IsHardDrivePath(const wchar_t *path)
	{
		const VDStringW rootPath(VDFileGetRootPath(path));

		UINT type = GetDriveTypeW(rootPath.c_str());

		return type == DRIVE_FIXED || type == DRIVE_UNKNOWN || type == DRIVE_REMOVABLE;
	}
};

///////////////////////////////////////////////////////////////////////////////
//
//	VDFile
//
///////////////////////////////////////////////////////////////////////////////

using namespace nsVDFile;

VDFile::VDFile(const char *pszFileName, uint32 flags)
	: mhFile(NULL)
{
	open_internal(VDTextAToW(pszFileName).c_str(), flags, true);
}

VDFile::VDFile(const wchar_t *pwszFileName, uint32 flags)
	: mhFile(NULL)
{
	open_internal(pwszFileName, flags, true);
}

VDFile::VDFile(HANDLE h)
	: mhFile(h)
{
	LARGE_INTEGER filepos = { 0, 0 };
	BOOL result = SetFilePointerEx(h, filepos, &filepos, FILE_CURRENT);

	mFilePosition = filepos.QuadPart;
}

VDFile::~VDFile()
{
	closeNT();
}

void VDFile::open(const char *pszFileName, uint32 flags)
{
	open_internal(VDTextAToW(pszFileName).c_str(), flags, true);
}

void VDFile::open(const wchar_t *pwszFilename, uint32 flags)
{
	open_internal(pwszFilename, flags, true);
}

bool VDFile::openNT(const wchar_t *pwszFilename, uint32 flags)
{
	return open_internal(pwszFilename, flags, false);
}

bool VDFile::open_internal(const wchar_t *pwszFilename, uint32 flags, bool throwOnError)
{
	close();

	mpFilename = _wcsdup(VDFileSplitPath(pwszFilename));
	if (!mpFilename) {
		if (!throwOnError) {
			return false;
		}
		throw MyMemoryError();
	}

	// At least one of the read/write flags must be set.
	VDASSERT(flags & (kRead | kWrite));

	DWORD dwDesiredAccess = 0;

	if (flags & kRead)  dwDesiredAccess  = GENERIC_READ;
	if (flags & kWrite) dwDesiredAccess |= GENERIC_WRITE;

	// Win32 docs are screwed here -- FILE_SHARE_xxx is the inverse of a deny flag.

	DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	if (flags & kDenyRead) {
		dwShareMode = FILE_SHARE_WRITE;
	}
	if (flags & kDenyWrite) {
		dwShareMode &= ~FILE_SHARE_WRITE;
	}

	// One of the creation flags must be set.
	VDASSERT(flags & kCreationMask);

	DWORD dwCreationDisposition;

	uint32 creationType = flags & kCreationMask;

	switch(creationType) {
	case kOpenExisting:     dwCreationDisposition = OPEN_EXISTING;     break;
	case kOpenAlways:       dwCreationDisposition = OPEN_ALWAYS;       break;
	case kCreateAlways:     dwCreationDisposition = CREATE_ALWAYS;     break;
	case kCreateNew:        dwCreationDisposition = CREATE_NEW;        break;
	case kTruncateExisting: dwCreationDisposition = TRUNCATE_EXISTING; break;
	default:
		VDNEVERHERE;
		return false;
	}

	VDASSERT((flags & (kSequential | kRandomAccess)) != (kSequential | kRandomAccess));

	DWORD dwAttributes = FILE_ATTRIBUTE_NORMAL;

	if (flags & kSequential)    dwAttributes |= FILE_FLAG_SEQUENTIAL_SCAN;
	if (flags & kRandomAccess)  dwAttributes |= FILE_FLAG_RANDOM_ACCESS;
	if (flags & kWriteThrough)  dwAttributes |= FILE_FLAG_WRITE_THROUGH;
	if (flags & kUnbuffered)    dwAttributes |= FILE_FLAG_NO_BUFFERING;

	if (!IsHardDrivePath(pwszFilename)) {
		flags &= ~FILE_FLAG_NO_BUFFERING;
	}

	mhFile = CreateFileW(pwszFilename, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwAttributes, NULL);
	DWORD err = GetLastError();

	// If we failed and FILE_FLAG_NO_BUFFERING was set, strip it and try again.
	// VPC and Novell shares sometimes do this....
	if (mhFile == INVALID_HANDLE_VALUE && err != ERROR_FILE_NOT_FOUND && err != ERROR_PATH_NOT_FOUND) {
		if (dwAttributes & FILE_FLAG_NO_BUFFERING) {
			dwAttributes &= ~FILE_FLAG_NO_BUFFERING;
			dwAttributes |= FILE_FLAG_WRITE_THROUGH;

			mhFile = CreateFileW(pwszFilename, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwAttributes, NULL);
			err = GetLastError();
		}
	}

	// INVALID_HANDLE_VALUE isn't NULL.  *sigh*

	if (mhFile == INVALID_HANDLE_VALUE) {
		mhFile = NULL;

		if (!throwOnError) {
			return false;
		}

		throw MyWin32Error(L"Cannot open file \"%s\":\n%%s", err, mpFilename.get());
	}

	mFilePosition = 0;
	return true;
}

bool VDFile::closeNT()
{
	if (mhFile) {
		HANDLE h = mhFile;
		mhFile = NULL;
		if (!CloseHandle(h)) {
			return false;
		}
	}

	return true;
}

void VDFile::close()
{
	if (!closeNT()) {
		throw MyWin32Error(L"Cannot complete file \"%s\": %%s", GetLastError(), mpFilename.get());
	}
}

bool VDFile::truncateNT()
{
	return 0 != SetEndOfFile(mhFile);
}

void VDFile::truncate()
{
	if (!truncateNT()) {
		throw MyWin32Error(L"Cannot truncate file \"%s\": %%s", GetLastError(), mpFilename.get());
	}
}

bool VDFile::extendValidNT(sint64 pos)
{
	return 0 != SetFileValidData(mhFile, pos);
}

void VDFile::extendValid(sint64 pos)
{
	if (!extendValidNT(pos)) {
		throw MyWin32Error(L"Cannot extend file \"%s\": %%s", GetLastError(), mpFilename.get());
	}
}

bool VDFile::enableExtendValid()
{
	// SetFileValidData() requires the SE_MANAGE_VOLUME_NAME privilege, so we must enable it
	// on the process token. We don't attempt to strip the privilege afterward as that would
	// introduce race conditions.
	bool bSuccessful = false;
	DWORD err = 0;

	SetLastError(0);

	HANDLE h;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &h)) {
		LUID luid;

		if (LookupPrivilegeValue(NULL, SE_MANAGE_VOLUME_NAME, &luid)) {
			TOKEN_PRIVILEGES tp;
			tp.PrivilegeCount = 1;
			tp.Privileges[0].Luid = luid;
			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			if (AdjustTokenPrivileges(h, FALSE, &tp, 0, NULL, NULL)) {
				bSuccessful = true;
			} else {
				err = GetLastError();
			}
		}

		CloseHandle(h);
	}

	if (!bSuccessful && err) {
		SetLastError(err);
	}

	return bSuccessful;
}

long VDFile::readData(void *buffer, long length)
{
	DWORD dwActual;

	if (!ReadFile(mhFile, buffer, (DWORD)length, &dwActual, NULL)) {
		throw MyWin32Error(L"Cannot read from file \"%s\": %%s", GetLastError(), mpFilename.get());
	}

	mFilePosition += dwActual;

	return dwActual;
}

void VDFile::read(void *buffer, long length)
{
	if (length != readData(buffer, length)) {
		throw MyWin32Error(L"Cannot read from file \"%s\": Premature end of file.", GetLastError(), mpFilename.get());
	}
}

long VDFile::writeData(const void *buffer, long length)
{
	DWORD dwActual;
	bool success = false;

	if (!WriteFile(mhFile, buffer, (DWORD)length, &dwActual, NULL) || dwActual != (DWORD)length) {
		throw MyWin32Error(L"Cannot write to file \"%s\": %%s", GetLastError(), mpFilename.get());
	}

	mFilePosition += dwActual;

	return dwActual;
}

void VDFile::write(const void *buffer, long length)
{
	if (length != writeData(buffer, length)) {
		throw MyWin32Error(L"Cannot write to file \"%s\": Unable to write all data.", GetLastError(), mpFilename.get());
	}
}

bool VDFile::seekNT(sint64 newPos, eSeekMode mode)
{
	DWORD dwMode;

	switch(mode) {
	case kSeekStart:
		dwMode = FILE_BEGIN;
		break;
	case kSeekCur:
		dwMode = FILE_CURRENT;
		break;
	case kSeekEnd:
		dwMode = FILE_END;
		break;
	default:
		VDNEVERHERE;
		return false;
	}

	LARGE_INTEGER filepos;
	filepos.QuadPart = newPos;
	BOOL result = SetFilePointerEx(mhFile, filepos, &filepos, dwMode);

	if (!result) {
		return false;
	}

	mFilePosition = filepos.QuadPart;
	return true;
}

void VDFile::seek(sint64 newPos, eSeekMode mode)
{
	if (!seekNT(newPos, mode)) {
		throw MyWin32Error(L"Cannot seek within file \"%s\": %%s", GetLastError(), mpFilename.get());
	}
}

bool VDFile::skipNT(sint64 delta)
{
	if (!delta) {
		return true;
	}

	char buf[1024];

	if (delta <= sizeof buf) {
		return (long)delta == readData(buf, (long)delta);
	} else {
		return seekNT(delta, kSeekCur);
	}
}

void VDFile::skip(sint64 delta)
{
	if (!delta) {
		return;
	}

	char buf[1024];

	if (delta > 0 && delta <= sizeof buf) {
		if ((long)delta != readData(buf, (long)delta)) {
			throw MyWin32Error(L"Cannot seek within file \"%s\": %%s", GetLastError(), mpFilename.get());
		}
	} else {
		seek(delta, kSeekCur);
	}
}

sint64 VDFile::size()
{
	LARGE_INTEGER filesize;
	BOOL result = GetFileSizeEx(mhFile, &filesize);

	if (!result) {
		DWORD err = GetLastError();
		throw MyWin32Error(L"Cannot retrieve size of file \"%s\": %%s", err, mpFilename.get());
	}

	return filesize.QuadPart;
}

sint64 VDFile::tell()
{
	return mFilePosition;
}

bool VDFile::flushNT()
{
	return 0 != FlushFileBuffers(mhFile);
}

void VDFile::flush()
{
	if (!flushNT()) {
		throw MyWin32Error(L"Cannot flush file \"%s\": %%s", GetLastError(), mpFilename.get());
	}
}

bool VDFile::isOpen()
{
	return mhFile != 0;
}

VDFileHandle VDFile::getRawHandle()
{
	return mhFile;
}

void *VDFile::AllocUnbuffer(size_t nBytes)
{
	return VirtualAlloc(NULL, nBytes, MEM_COMMIT, PAGE_READWRITE);
}

void VDFile::FreeUnbuffer(void *p)
{
	VirtualFree(p, 0, MEM_RELEASE);
}
