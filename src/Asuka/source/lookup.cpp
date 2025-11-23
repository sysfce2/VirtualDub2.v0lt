// Asuka - VirtualDub Build/Post-Mortem Utility
//
// Copyright (C) 2005 Avery Lee
// Copyright (C) 2016 Anton Shekhovtsov
// Copyright (C) 2025 v0lt
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#include "stdafx.h"
#include <vd2/system/vdalloc.h>
#include <vd2/system/vdstl.h>
#include <vd2/system/text.h>
#include <vd2/system/VDString.h>
#include <stdlib.h>
#include <vector>
#include "symbols.h"
#include "utils.h"

void VDNORETURN help_lookup() {
	printf("usage: lookup <map file> <address>\n");
	exit(5);
}

void tool_lookup(const vdfastvector<const wchar_t*>& args, const vdfastvector<const wchar_t*>& switches, bool amd64) {
	if (args.size() < 2)
		help_lookup();

	wchar_t* s;
	sint64 addr = _wcstoi64(args[1], &s, 16);

	if (*s) {
		fail("lookup: invalid address \"%ls\"", args[0]);
	}

	vdautoptr<IVDSymbolSource> pss(VDCreateSymbolSourceLinkMap());

	pss->Init(args[0]);

	const VDSymbol *sym = pss->LookupSymbol(addr);

	if (!sym) {
		fail("symbol not found for address %08x", addr);
	}

	const char *fn;
	int line;

	if (pss->LookupLine(addr, fn, line)) {
		printf("%08I64x   %s + %llx [%s:%d]\n", addr, sym->name, addr - sym->rva, fn, line);
	} else {
		printf("%08I64x   %s + %llx\n", addr, sym->name, addr - sym->rva);
	}
}
