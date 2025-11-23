// Asuka - VirtualDub Build/Post-Mortem Utility
//
// Copyright (C) 2005-2007 Avery Lee
// Copyright (C) 2025 v0lt
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#pragma warning(disable: 4786)		// SHUT UP

#include "stdafx.h"
#include <windows.h>
#include <objbase.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include <vd2/system/vdtypes.h>
#include <vd2/system/vdstl.h>
#include <vd2/system/error.h>

#include <vector>
#include <algorithm>

#include "utils.h"

void tool_verinc(bool amd64);
void tool_lookup(const vdfastvector<const wchar_t*>& args, const vdfastvector<const wchar_t*>& switches, bool amd64);
void tool_mapconv(const vdfastvector<const wchar_t*>& args, const vdfastvector<const wchar_t*>& switches, bool amd64);
void tool_fxc(const vdfastvector<const wchar_t*>& args, const vdfastvector<const wchar_t*>& switches, bool amd64);
void tool_fxc10(const vdfastvector<const wchar_t*>& args, const vdfastvector<const wchar_t*>& switches);
void tool_makearray(const vdfastvector<const wchar_t*>& args, const vdfastvector<const wchar_t*>& switches);
void tool_glc(const vdfastvector<const wchar_t*>& args, const vdfastvector<const wchar_t*>& switches);
void tool_fontextract(const vdfastvector<const wchar_t*>& args, const vdfastvector<const wchar_t*>& switches);
void tool_fontencode(const vdfastvector<const wchar_t*>& args, const vdfastvector<const wchar_t*>& switches);
void tool_snapsetup();
void tool_filecreate(const vdfastvector<const wchar_t*>& args, const vdfastvector<const wchar_t*>& switches);
void tool_maketables(const vdfastvector<const wchar_t*>& args, const vdfastvector<const wchar_t*>& switches);
void tool_psa(const vdfastvector<const wchar_t*>& args, const vdfastvector<const wchar_t*>& switches);
void tool_checkimports(const vdfastvector<const wchar_t*>& args, const vdfastvector<const wchar_t*>& switches);
void tool_hash(const vdfastvector<const wchar_t*>& args, const vdfastvector<const wchar_t*>& switches);

int wmain(int argc, wchar_t** argv)
{
	--argc;
	++argv;

	vdfastvector<const wchar_t*> switches, args;
	bool amd64 = false;

	while(const wchar_t* s = *argv++) {
		if (s[0] == '/') {
			if (!_wcsicmp(s + 1, L"amd64")) {
				amd64 = true;
			} else {
				switches.push_back(s + 1);
			}
		} else {
			args.push_back(s);
		}
	}

	// look for mode
	if (args.empty()) {
		help();
	}

	const wchar_t* s = args[0];

	args.erase(args.begin());

	CoInitialize(NULL);

	try {
		if (!_wcsicmp(s, L"verinc")) {
			read_version();
			tool_verinc(amd64);
		}
		else if (!_wcsicmp(s, L"lookup")) {
			tool_lookup(args, switches, amd64);
		}
		else if (!_wcsicmp(s, L"mapconv")) {
			read_version();
			tool_mapconv(args, switches, amd64);
		}
		else if (!_wcsicmp(s, L"fxc")) {
			tool_fxc(args, switches, amd64);
		}
		else if (!_wcsicmp(s, L"fxc10")) {
			tool_fxc10(args, switches);
		}
		else if (!_wcsicmp(s, L"makearray")) {
			tool_makearray(args, switches);
		}
		else if (!_wcsicmp(s, L"glc")) {
			tool_glc(args, switches);
		}
		else if (!_wcsicmp(s, L"fontextract")) {
			tool_fontextract(args, switches);
		}
		else if (!_wcsicmp(s, L"fontencode")) {
			tool_fontencode(args, switches);
		}
		else if (!_wcsicmp(s, L"snapsetup")) {
			tool_snapsetup();
		}
		else if (!_wcsicmp(s, L"filecreate")) {
			tool_filecreate(args, switches);
		}
		else if (!_wcsicmp(s, L"maketables")) {
			tool_maketables(args, switches);
		}
		else if (!_wcsicmp(s, L"psa")) {
			tool_psa(args, switches);
		}
		else if (!_wcsicmp(s, L"checkimports")) {
			tool_checkimports(args, switches);
		}
		else if (!_wcsicmp(s, L"hash")) {
			tool_hash(args, switches);
		}
		else
			help();
	}
	catch (const char* s) {
		fail("%s", s);
	}
	catch (const MyError& e) {
		fail("%s", e.gets());
	}

	CoUninitialize();
	return 0;
}
