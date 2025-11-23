// VirtualDub - Video processing and capture application
//
// Copyright (C) 1998-2004 Avery Lee
// Copyright (C) 2025 v0lt
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#include <stdafx.h>
#include <vd2/Dita/w32control.h>

class VDUIGroupW32 : public VDUIControlW32 {
public:
	bool Create(IVDUIParameters *);
	void PreLayoutBase(const VDUILayoutSpecs&);
	void PostLayoutBase(const vduirect& target);
};

extern IVDUIWindow *VDCreateUIGroup() { return new VDUIGroupW32; }

bool VDUIGroupW32::Create(IVDUIParameters *pParameters) {
	return CreateW32(pParameters, L"BUTTON", BS_GROUPBOX);
}

void VDUIGroupW32::PreLayoutBase(const VDUILayoutSpecs& parentConstraints) {
   	vduirect r = mpBase->MapUnitsToPixels(vduirect(0,6,6,12));
	SIZE siz = SizeText(0, 0, 0);

	mLayoutSpecs.minsize.w	= siz.cx;
	mLayoutSpecs.minsize.h	= siz.cy;
   
   	r.right += r.right;
   
   	mLayoutSpecs.minsize.w	= r.right;
   	mLayoutSpecs.minsize.h	= r.top + r.bottom;
   
   	VDUILayoutSpecs ls;

   	ls.minsize.w = parentConstraints.minsize.w - mLayoutSpecs.minsize.w;
   	ls.minsize.h = parentConstraints.minsize.h - mLayoutSpecs.minsize.h;

	vduisize childsize(0, 0);

	tChildren::iterator it(mChildren.begin()), itEnd(mChildren.end());
	for(; it!=itEnd; ++it) {
		IVDUIWindow *pWin = *it;

		pWin->PreLayout(ls);

		childsize.include(pWin->GetLayoutSpecs().minsize);
	}

	childsize.include(vduisize(siz.cx, siz.cy));

   	mLayoutSpecs.minsize += childsize;
}

void VDUIGroupW32::PostLayoutBase(const vduirect& target) {
	SetArea(target);

	vduirect r = mpBase->MapUnitsToPixels(vduirect(0,6,6,12));

	vduirect rDest(target.left + r.right, target.top + r.bottom, target.right - r.right, target.bottom - r.top);

	tChildren::iterator it(mChildren.begin()), itEnd(mChildren.end());
	for(; it!=itEnd; ++it) {
		IVDUIWindow *pWin = *it;

		pWin->PostLayout(rDest);
	}
}
