// VirtualDub - Video processing and capture application
//
// Copyright (C) 1998-2004 Avery Lee
// Copyright (C) 2018 Anton Shekhovtsov
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
//	VDUITrackbarW32
//
///////////////////////////////////////////////////////////////////////////

class VDUITrackbarW32 : public VDUIControlW32, public IVDUITrackbar {
public:
	void *AsInterface(uint32 id);

	bool Create(IVDUIParameters *);

	void PreLayoutBase(const VDUILayoutSpecs& parentConstraints);

	void SetRange(sint32 mn, sint32 mx);
	void SetRangeStep(sint32 mn, sint32 mx, sint32 step);

	int GetValue();
	void SetValue(int value);

	void OnScrollCallback(UINT code);

protected:
	sint32	mValue;
	sint32	mStep;
};

extern IVDUIWindow *VDCreateUITrackbar() { return new VDUITrackbarW32; }

void *VDUITrackbarW32::AsInterface(uint32 id) {
	if (id == IVDUITrackbar::kTypeID) return static_cast<IVDUITrackbar *>(this);

	return VDUIControlW32::AsInterface(id);
}

bool VDUITrackbarW32::Create(IVDUIParameters *pParams) {
	mValue = 0;
	mStep = 1;
	return CreateW32(pParams, TRACKBAR_CLASSW, TBS_HORZ|TBS_BOTH|TBS_NOTICKS);
}

void VDUITrackbarW32::PreLayoutBase(const VDUILayoutSpecs& parentConstraints) {
}

void VDUITrackbarW32::SetRange(sint32 mn, sint32 mx) {
	mStep = 1;
	SendMessageW(mhwnd, TBM_SETRANGEMIN, FALSE, mn);
	SendMessageW(mhwnd, TBM_SETRANGEMAX, TRUE, mx);
	OnScrollCallback(0);	// check for clip
}

void VDUITrackbarW32::SetRangeStep(sint32 mn, sint32 mx, sint32 step) {
	mStep = step;
	SendMessageW(mhwnd, TBM_SETRANGEMIN, FALSE, mn/step);
	SendMessageW(mhwnd, TBM_SETRANGEMAX, TRUE, mx/step);
	//OnScrollCallback(0);	// check for clip
}

int VDUITrackbarW32::GetValue() {
	return mValue;
}

void VDUITrackbarW32::SetValue(int value) {
	mValue = value;		// prevents recursion
	SendMessageW(mhwnd, TBM_SETPOS, TRUE, value/mStep);
}

void VDUITrackbarW32::OnScrollCallback(UINT code) {
	sint32 v = SendMessageW(mhwnd, TBM_GETPOS, 0, 0)*mStep;

	if (mValue != v) {
		mValue = v;
		mpBase->ProcessValueChange(this, mID);
		mpBase->DispatchEvent(this, mID, IVDUICallback::kEventSelect, GetValue());
	}
}
