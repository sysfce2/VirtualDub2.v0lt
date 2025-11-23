// Asuka - VirtualDub Build/Post-Mortem Utility
//
// Copyright (C) 2005 Avery Lee
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#ifndef f_VD2_ASUKA_SYMBOLS_H
#define f_VD2_ASUKA_SYMBOLS_H

#include <vd2/system/vdtypes.h>
#include <vd2/system/vdstl.h>
#include <vector>

struct VDSymbol {
	sint64 rva;
	int group;
	long start;
	char *name;
};

struct VDSection {
	sint64	mAbsStart;
	long	mStart;
	long	mLength;
	int		mGroup;

	VDSection(long s=0, long l=0, int g=0) : mStart(s), mLength(l), mGroup(g) {}
};

class VDINTERFACE IVDSymbolSource {
public:
	virtual ~IVDSymbolSource() {}

	virtual void Init(const wchar_t *filename) = 0;
	virtual const VDSymbol *LookupSymbol(sint64 addr) = 0;
	virtual const VDSection *LookupSection(sint64 addr) = 0;
	virtual void GetAllSymbols(vdfastvector<VDSymbol>&) = 0;

	virtual uint32 GetCodeGroupMask() = 0;

	virtual int GetSectionCount() = 0;
	virtual const VDSection *GetSection(int sec) = 0;

	virtual bool LookupLine(sint64 addr, const char *& filename, int& lineno) = 0;
};

IVDSymbolSource *VDCreateSymbolSourceLinkMap();

#endif
