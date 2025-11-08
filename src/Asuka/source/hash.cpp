//	Asuka - VirtualDub Build/Post-Mortem Utility
//	Copyright (C) 2005 Avery Lee
//
//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program; if not, write to the Free Software
//	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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
