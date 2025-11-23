// VirtualDub - Video processing and capture application
//
// Copyright (C) 1998-2006 Avery Lee
// Copyright (C) 2025 v0lt
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#include <stdafx.h>
#include <windows.h>
#include <commctrl.h>
#include <vd2/Dita/w32control.h>

///////////////////////////////////////////////////////////////////////////
//
//	VDUIHotkeyW32
//
///////////////////////////////////////////////////////////////////////////

class VDUIHotkeyW32 : public VDUIControlW32 {
public:
	bool Create(IVDUIParameters *);

	void PreLayoutBase(const VDUILayoutSpecs& parentConstraints);

	void OnCommandCallback(UINT code);

	int GetValue();
	void SetValue(int value);

protected:
	int mValue;
};

extern IVDUIWindow *VDCreateUIHotkey() { return new VDUIHotkeyW32; }

bool VDUIHotkeyW32::Create(IVDUIParameters *pParams) {
	if (!CreateW32(pParams, HOTKEY_CLASSW, WS_TABSTOP))
		return false;

	mValue = 0;
	return true;
}

void VDUIHotkeyW32::PreLayoutBase(const VDUILayoutSpecs& parentConstraints) {
	vduisize pad = mpBase->MapUnitsToPixels(vduisize(8,12));

	mLayoutSpecs.minsize.w = GetSystemMetrics(SM_CXVSCROLL);
	mLayoutSpecs.minsize.h = pad.h;
}

void VDUIHotkeyW32::OnCommandCallback(UINT code) {
	if (code == EN_CHANGE) {
		mValue = SendMessageW(mhwnd, HKM_GETHOTKEY, 0, 0);
		mpBase->ProcessActivation(this, mID);
		mpBase->DispatchEvent(this, mID, IVDUICallback::kEventSelect, 0);
	}
}

int VDUIHotkeyW32::GetValue() {
	return mValue;
}

void VDUIHotkeyW32::SetValue(int value) {
	mValue = value;
	SendMessageW(mhwnd, HKM_SETHOTKEY, value, 0);
}
