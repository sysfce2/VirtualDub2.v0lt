// Sylia is scripting language for the scripting/job system in VirtualDub
//
// Copyright (C) 2013 Avery Lee
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#ifndef f_SYLIA_STRINGHEAP_H
#define f_SYLIA_STRINGHEAP_H

#include <list>

class VDScriptStringHeap {
protected:
	typedef std::list<char *> tStrings;
	tStrings mStrings;

public:
	VDScriptStringHeap();
	~VDScriptStringHeap();

	void Clear();
	void BeginGC();
	void Mark(char *s);
	int EndGC();
	char **Allocate(int);
};

#endif
