// Asuka - VirtualDub Build/Post-Mortem Utility
//
// Copyright (C) 2005 Avery Lee
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#include <vd2/system/error.h>
#include <vd2/system/file.h>
#include <vd2/system/vdstl.h>
#include <vd2/system/text.h>
#include <vd2/system/VDString.h>
#include <vector>
#include <windows.h>

BOOL CALLBACK EnumResLangsCB(HMODULE hmod, LPCTSTR type, LPCTSTR name, WORD wIDLanguage, LONG_PTR lParam) {
	VDTextOutputFile& out = *(VDTextOutputFile *)lParam;
	char typebuf[32];
	char namebuf[32];

	if (IS_INTRESOURCE(type)) {
		sprintf(typebuf, "%u", (UINT)type);
		type = typebuf;
	}

	if (IS_INTRESOURCE(name)) {
		sprintf(namebuf, "%u", (UINT)name);
		name = namebuf;
	}

	out.FormatLine("static const char g_%s_%s_%04x[]={", type, name, wIDLanguage);
	out.PutLine("};");

	return TRUE;
}

BOOL CALLBACK EnumResNamesCB(HMODULE hmod, LPCTSTR type, LPTSTR name, LONG_PTR lParam) {
	EnumResourceLanguages(hmod, type, name, EnumResLangsCB, lParam);
	return TRUE;
}

BOOL CALLBACK EnumResTypesCB(HMODULE hmod, LPTSTR type, LONG_PTR lParam) {
	EnumResourceNames(hmod, type, EnumResNamesCB, lParam);
	return TRUE;
}

void tool_resextract(const std::vector<const char *>& args, const std::vector<const char *>& switches, bool amd64) {
	if (args.size() != 2) {
		printf("usage: resextract <exe-name> <output file>\n");
		exit(5);
	}

	const char *exename = args[0];
	const char *outfile = args[1];

	VDTextOutputFile out(VDTextAToW(outfile).c_str());

	HMODULE hmod = LoadLibraryEx(exename, NULL, LOAD_LIBRARY_AS_DATAFILE);

	if (!hmod)
		throw MyWin32Error("Cannot load executable module \"%s\": %%s", GetLastError(), exename);

	EnumResourceTypes(hmod, EnumResTypesCB, (LONG_PTR)&out);

	FreeLibrary(hmod);
}
