// VirtualDub - Video processing and capture application
//
// Copyright (C) 2013 Avery Lee
// Copyright (C) 2018 Anton Shekhovtsov
// Copyright (C) 2024-2025 v0lt
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#ifndef f_REGISTRY_H
#define f_REGISTRY_H

#include <windows.h>

HKEY OpenRegKey(HKEY hkBase, const char* szKeyName);
HKEY CreateRegKey(HKEY hkBase, const char* szKeyName);
HKEY CreateRegKey64(HKEY hkBase, const char* szKeyName);
BOOL DeleteRegValue(HKEY hkBase, const char* szKeyName, const char* szValueName);
BOOL QueryRegString(HKEY hkBase, const char* szKeyName, const char* szValueName, char* lpBuffer, int cbBuffer);
BOOL SetRegString(HKEY hkBase, const char* szKeyName, const char* szValueName, const char* lpBuffer);
BOOL SetRegString64(HKEY hkBase, const char* szKeyName, const char* szValueName, const char* lpBuffer);

#endif
