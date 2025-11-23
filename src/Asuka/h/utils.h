// Asuka - VirtualDub Build/Post-Mortem Utility
//
// Copyright (C) 2005 Avery Lee
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#ifndef f_ASUKA_UTILS_H
#define f_ASUKA_UTILS_H

#include <vd2/system/vdtypes.h>
#include <vd2/system/VDString.h>
#include <string>

void VDNORETURN help();
void VDNORETURN fail(const char *format, ...);

void canonicalize_name(std::string& name);
void canonicalize_name(VDStringA& name);
std::string get_name();
int get_version();
bool read_version();
void inc_version(const char *tag);
bool write_version(const char *tag);

class ProjectSetup {
public:
	ProjectSetup();
	~ProjectSetup();

	void Query();
	void Read(const wchar_t *filename);
	void Write(const wchar_t *filename);

	VDStringA	mCounterTag;
};


#endif
