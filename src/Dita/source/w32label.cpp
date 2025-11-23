// VirtualDub - Video processing and capture application
//
// Copyright (C) 1998-2004 Avery Lee
// Copyright (C) 2025 v0lt
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#include <stdafx.h>
#include <vd2/Dita/w32control.h>

class VDUILabelW32 : public VDUIControlW32 {
public:
	bool Create(IVDUIParameters *);
	void PreLayoutBaseW32(const VDUILayoutSpecs&);
};

extern IVDUIWindow *VDCreateUILabel() { return new VDUILabelW32; }

bool VDUILabelW32::Create(IVDUIParameters *pParameters) {
	return CreateW32(pParameters, L"STATIC", pParameters->GetB(nsVDUI::kUIParam_Multiline, false) ? SS_LEFT : SS_CENTERIMAGE|SS_LEFT);
}

void VDUILabelW32::PreLayoutBaseW32(const VDUILayoutSpecs& parentConstraints) {
	SIZE siz = SizeText(parentConstraints.minsize.w, 0, 0);

	mLayoutSpecs.minsize.w	= siz.cx;
	mLayoutSpecs.minsize.h	= siz.cy;
}

///////////////////////////////////////////////////////////////////////////////

class VDUINumericLabelW32 : public VDUILabelW32 {
public:
	VDUINumericLabelW32();

	int GetValue();
	void SetValue(int v);

protected:
	int mValue;
	VDStringW	mFormat;
};

extern IVDUIWindow *VDCreateUINumericLabel() { return new VDUINumericLabelW32; }

VDUINumericLabelW32::VDUINumericLabelW32()
	: mValue(0)
	, mFormat(L"%d")
{
}

int VDUINumericLabelW32::GetValue() {
	return mValue;
}

void VDUINumericLabelW32::SetValue(int v) {
	if (v != mValue) {
		mValue = v;

		SetCaption(VDswprintf(mFormat.c_str(), 1, &v).c_str());
	}
}
