// Asuka - VirtualDub Build/Post-Mortem Utility
//
// Copyright (C) 2005 Avery Lee
// Copyright (C) 2026 v0lt
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#include "stdafx.h"
#include <vd2/system/vdtypes.h>
#include <stdio.h>
#include <windows.h>

namespace {
	BOOL WINAPI CtrlCFunc(DWORD event) {
		return event == CTRL_C_EVENT || event == CTRL_BREAK_EVENT;
	}
}

void tool_snapsetup() {
	SetConsoleCtrlHandler(CtrlCFunc, TRUE);

	BOOL fontSmoothing;

	// disable font smoothing
	SystemParametersInfoW(SPI_GETFONTSMOOTHING, 0, &fontSmoothing, 0);
	SystemParametersInfoW(SPI_SETFONTSMOOTHING, FALSE, NULL, 0);

	// disable title bar gradient
	const INT kColorsToSet[]={ COLOR_GRADIENTACTIVECAPTION, COLOR_GRADIENTINACTIVECAPTION };
	const COLORREF oldColorValues[]={ GetSysColor(COLOR_GRADIENTACTIVECAPTION), GetSysColor(COLOR_GRADIENTINACTIVECAPTION) };
	const COLORREF newColorValues[]={ GetSysColor(COLOR_ACTIVECAPTION), GetSysColor(COLOR_INACTIVECAPTION) };

	SetSysColors(2, kColorsToSet, newColorValues);

	// refresh screen
	InvalidateRect(NULL, NULL, TRUE);

	puts("Display set up for screenshots. Press Return to restore system settings.");
	getchar();

	// restore title bar gradient
	SetSysColors(2, kColorsToSet, oldColorValues);

	// restore font smoothing
	SystemParametersInfoW(SPI_SETFONTSMOOTHING, fontSmoothing, NULL, 0);

	// refresh screen
	InvalidateRect(NULL, NULL, TRUE);
}
