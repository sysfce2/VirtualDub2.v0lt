// Asuka - VirtualDub Build/Post-Mortem Utility
//
// Copyright (C) 2005 Avery Lee
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#include <vd2/system/error.h>
#include <vd2/system/file.h>
#include <vd2/system/vdstl.h>
#include <vector>
#include <windows.h>

void tool_resbind(const std::vector<const char *>& args, const std::vector<const char *>& switches, bool amd64) {
	if (args.size() != 4) {
		printf("usage: resbind <exe-name> <source file> <restype> <resname>\n");
		exit(5);
	}

	const char *exename = args[0];
	const char *srcfile = args[1];
	const char *restype = args[2];
	const char *resname = args[3];

	VDFile file(srcfile);

	vdblock<char> buf((size_t)file.size());

	file.read(buf.data(), buf.size());
	file.close();

	HANDLE hUpdate = BeginUpdateResource(exename, FALSE);
	if (!hUpdate)
		throw MyWin32Error("Cannot open \"%s\" for resource edit: %%s.", GetLastError(), exename);

	BOOL success = UpdateResource(hUpdate, restype, resname, 0x0409, buf.data(), buf.size());
	DWORD err = GetLastError();

	EndUpdateResource(hUpdate, !success);

	if (!success)
		throw MyWin32Error("Cannot update \"%s\": %%s.", err, exename);

	printf("Adding \"%s\" to \"%s\" as %s:%s.\n", srcfile, exename, restype, resname);
}
