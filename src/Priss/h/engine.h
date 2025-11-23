// Priss (NekoAmp 2.0) - MPEG-1/2 audio decoding library
//
// Copyright (C) 2003 Avery Lee
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#ifndef f_VD2_PRISS_ENGINE_H
#define f_VD2_PRISS_ENGINE_H

#include <vd2/system/vdtypes.h>
#include <vd2/Priss/decoder.h>

#include "polyphase.h"

class VDMPEGAudioHuffBitReader;

class VDMPEGAudioDecoder : public IVDMPEGAudioDecoder {
public:
	VDMPEGAudioDecoder();
	~VDMPEGAudioDecoder();

	void	Init();
	void	SetSource(IVDMPEGAudioBitsource *pSource);
	void	SetDestination(sint16 *psDest);
	uint32	GetSampleCount();
	uint32	GetFrameDataSize();
	void	GetStreamInfo(VDMPEGAudioStreamInfo *pasi);
	const char *GetErrorString(int err);
	void	Reset();
	void	ReadHeader();
	void	PrereadFrame();
	bool	DecodeFrame();
	void	ConcealFrame();

protected:
	bool	DecodeLayerI();
	bool	DecodeLayerII();
	void	PrereadLayerIII();
	bool	DecodeLayerIII();

	static void DecodeHuffmanValues(VDMPEGAudioHuffBitReader& bitreader, sint32 *dst, unsigned table, unsigned pairs);

	IVDMPEGAudioBitsource *mpSource;
	VDMPEGAudioStreamInfo mHeader;
	uint32			mFrameDataSize;
	short			*mpSampleDst;
	uint32			mSamplesDecoded;

	uint8			mBitrateIndex;
	uint8			mSamplingRateIndex;
	uint8			mMode;
	uint8			mModeExtension;

	float			mL2Scalefactors[64];
	sint8			mL2Ungroup3[32][3];

	uint8			mFrameBuffer[1728+4];

	enum { kL3BufferSize = 2048 };
	uint8			mL3Buffer[2048];			// Only needs to be 960, but we make it 1024 for address masking
	uint32			mL3BufferPos;				// Tail for layer III circular buffer operation
	uint32			mL3BufferLevel;				// Number of bits in layer III bit reservoir

	float			mL3OverlapBuffer[2][32][18];	// used for overlapping tails of IMDCT output

	float			mL3Windows[4][36];			// IMDCT windows

	union {
		struct {
			float recon[2][576];
		} mL3Data;
	};

	float			mL3Pow43Tab[256];			// [-128,127] ^ (4/3)

	VDMPEGAudioPolyphaseFilter	*mpPolyphaseFilter;

	static const struct L3HuffmanTableDescriptor {
		const uint8 (*table)[2];
		unsigned treelen;
		unsigned bits;
		unsigned linbits;
	} sL3HuffmanTables[];
};

#endif
