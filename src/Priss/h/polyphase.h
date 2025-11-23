// Priss (NekoAmp 2.0) - MPEG-1/2 audio decoding library
//
// Copyright (C) 2003 Avery Lee
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#ifndef f_VD2_PRISS_POLYPHASE_H
#define f_VD2_PRISS_POLYPHASE_H

#include <vd2/system/vdtypes.h>

class VDMPEGAudioPolyphaseFilter {
public:
	void *operator new(size_t);
	void operator delete(void *);

	static VDMPEGAudioPolyphaseFilter *Create();

	VDMPEGAudioPolyphaseFilter();

	virtual bool ShouldRecreate() = 0;
	void Reset();
	void Generate(const float left[32], const float right[32], sint16 *dst);

protected:
	enum OptMode {
		kOptModeFPU,
		kOptModeSSE
	};

	static OptMode GetOptMode();
	virtual void DCTInputButterflies(float x[32], const float in[32]);
	virtual void DCT4x8(float *);
	virtual void Matrix(const float *, bool stereo, int ch);
	virtual void SynthesizeMono(sint16 *dst);
	virtual void SynthesizeStereo(sint16 *dst);

	__declspec(align(16)) union {
		float	mono[32][16];
		float	stereo[32][2][16];
	} mWindow;

	unsigned	mWindowPos;

	float		mFilter[17][32];
};

class VDMPEGAudioPolyphaseFilterFPU : public VDMPEGAudioPolyphaseFilter {
protected:
	bool ShouldRecreate() { return GetOptMode() != kOptModeFPU; }
};

class VDMPEGAudioPolyphaseFilterSSE : public VDMPEGAudioPolyphaseFilter {
protected:
	bool ShouldRecreate() { return GetOptMode() != kOptModeSSE; }

	void DCTInputButterflies(float x[32], const float in[32]);
	void DCT4x8(float *x);
	void SynthesizeStereo(sint16 *dst);
};

#endif
