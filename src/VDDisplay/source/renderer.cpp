// VirtualDub - Video processing and capture application

// A/V interface library
//
// Copyright (C) 2013 Avery Lee
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#include <stdafx.h>
#include <vd2/VDDisplay/renderer.h>

VDDisplayImageView::VDDisplayImageView()
	: mbDynamic(false)
	, mUniquenessCounter(0)
{
	memset(&mPixmap, 0, sizeof mPixmap);
}

VDDisplayImageView::~VDDisplayImageView() {
}

void VDDisplayImageView::SetImage(const VDPixmap& px, bool dynamic) {
	SetCachedImage(NULL);

	mPixmap = px;
	mbDynamic = dynamic;
}

void VDDisplayImageView::SetCachedImage(IVDRefUnknown *p) {
	mpCachedImage = p;
}

void VDDisplayImageView::Invalidate() {
	++mUniquenessCounter;
}
