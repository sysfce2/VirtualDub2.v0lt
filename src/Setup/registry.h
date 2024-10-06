#ifndef f_REGISTRY_H
#define f_REGISTRY_H

#include <windows.h>

HKEY OpenRegKey(HKEY hkBase, const char *szKeyName);
HKEY CreateRegKey(HKEY hkBase, const char *szKeyName);
HKEY CreateRegKey64(HKEY hkBase, const char *szKeyName);
BOOL DeleteRegValue(HKEY hkBase, const char *szKeyName, const char *szValueName);
BOOL QueryRegString(HKEY hkBase, const char *szKeyName, const char *szValueName, char *lpBuffer, int cbBuffer);
BOOL SetRegString(HKEY hkBase, const char *szKeyName, const char *szValueName, const char *lpBuffer);
BOOL SetRegString64(HKEY hkBase, const char *szKeyName, const char *szValueName, const char *lpBuffer);

#endif
