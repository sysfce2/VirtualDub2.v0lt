// VirtualDub - Video processing and capture application
//
// Copyright (C) 2013 Avery Lee
// Copyright (C) 2018 Anton Shekhovtsov
// Copyright (C) 2024-2025 v0lt
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#include <windows.h>
#include "registry.h"

HKEY OpenRegKey(HKEY hkBase, const char* szKeyName)
{
	HKEY hkey;

	return RegOpenKeyExA(hkBase, szKeyName, 0, KEY_ALL_ACCESS, &hkey) == ERROR_SUCCESS
		? hkey
		: NULL;
}

HKEY CreateRegKey(HKEY hkBase, const char* szKeyName)
{
	HKEY hkey;
	DWORD dwDisposition;

	return RegCreateKeyExA(hkBase, szKeyName, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, &dwDisposition) == ERROR_SUCCESS
		? hkey
		: NULL;
}

HKEY CreateRegKey64(HKEY hkBase, const char* szKeyName)
{
	HKEY hkey;
	DWORD dwDisposition;

	return RegCreateKeyExA(hkBase, szKeyName, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | KEY_WOW64_64KEY, NULL, &hkey, &dwDisposition) == ERROR_SUCCESS
		? hkey
		: NULL;
}

BOOL DeleteRegValue(HKEY hkBase, const char* szKeyName, const char* szValueName)
{
	HKEY hkey;
	BOOL success;

	if (!(hkey = OpenRegKey(hkBase, szKeyName))) {
		return FALSE;
	}

	success = (RegDeleteValueA(hkey, szValueName) == ERROR_SUCCESS);

	RegCloseKey(hkey);

	return success;
}

BOOL QueryRegString(HKEY hkBase, const char* szKeyName, const char* szValueName, char* lpBuffer, int cbBuffer)
{
	HKEY hkey;
	BOOL success;
	DWORD type;

	if (!(hkey = OpenRegKey(hkBase, szKeyName))) {
		return FALSE;
	}

	success = (ERROR_SUCCESS == RegQueryValueExA(hkey, szValueName, 0, &type, (LPBYTE)lpBuffer, (LPDWORD)&cbBuffer));

	RegCloseKey(hkey);

	return success;
}

BOOL SetRegString(HKEY hkBase, const char* szKeyName, const char* szValueName, const char* lpBuffer)
{
	HKEY hkey;
	BOOL success;

	if (!(hkey = CreateRegKey(hkBase, szKeyName))) {
		return FALSE;
	}

	success = (ERROR_SUCCESS == RegSetValueExA(hkey, szValueName, 0, REG_SZ, (LPBYTE)lpBuffer, strlen(lpBuffer) + 1));

	RegCloseKey(hkey);

	return success;
}

BOOL SetRegString64(HKEY hkBase, const char* szKeyName, const char* szValueName, const char* lpBuffer)
{
	HKEY hkey;
	BOOL success;

	if (!(hkey = CreateRegKey64(hkBase, szKeyName))) {
		return FALSE;
	}

	success = (ERROR_SUCCESS == RegSetValueExA(hkey, szValueName, 0, REG_SZ, (LPBYTE)lpBuffer, strlen(lpBuffer) + 1));

	RegCloseKey(hkey);

	return success;
}
