// Sylia is scripting language for the scripting/job system in VirtualDub
//
// Copyright (C) 2013 Avery Lee
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#ifndef f_SYLIA_VARIABLETABLE_H
#define f_SYLIA_VARIABLETABLE_H

#include "ScriptValue.h"
#include "VectorHeap.h"

class VDScriptStringHeap;

class VariableTableEntry {
public:
	VariableTableEntry *next;
	VDScriptValue v;
	char szName[1];
};

class VariableTable {
private:
	long				lHashTableSize;
	VariableTableEntry	**lpHashTable;
	VectorHeap			varheap;

	long Hash(const char *szName);

public:
	VariableTable(int);
	~VariableTable();

	void MarkStrings(VDScriptStringHeap& heap);

	VariableTableEntry *Lookup(const char *szName);
	VariableTableEntry *Declare(const char *szName);
};

#endif
