// Asuka - VirtualDub Build/Post-Mortem Utility
//
// Copyright (C) 2005 Avery Lee
// Copyright (C) 2025 v0lt
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#include "stdafx.h"
#include <vd2/system/vdstl.h>
#include <vd2/system/hash.h>
#include <vd2/system/VDString.h>
#include <stdio.h>

void tool_hash(const vdfastvector<const wchar_t*>& args, const vdfastvector<const wchar_t*>& switches) {
	for(vdfastvector<const wchar_t*>::const_iterator it = args.begin(), itEnd = args.end();
		it != itEnd;
		++it)
	{
		VDStringA str = VDTextWToA(*it);

		const char *s = str.c_str();

		printf("%08x \"%s\"\n", VDHashString32I(s), s);
	}
}
