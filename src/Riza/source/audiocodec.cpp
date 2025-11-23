// VirtualDub - Video processing and capture application
// A/V interface library
//
// Copyright (C) 1998-2007 Avery Lee
// Copyright (C) 2017-2018 Anton Shekhovtsov
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#include <vd2/Riza/audiocodec.h>
#include <windows.h>
#include <mmreg.h>
#include <Ks.h>
#include <Ksmedia.h>

IVDAudioCodec *VDCreateAudioDecompressor(const VDWaveFormat *srcFormat, const VDWaveFormat *dstFormat) {
	switch(srcFormat->mTag) {
		case VDWaveFormat::kTagCCITTMuLaw:
			return VDCreateAudioDecompressorMuLaw(srcFormat, dstFormat);

		case VDWaveFormat::kTagCCITTALaw:
			return VDCreateAudioDecompressorALaw(srcFormat, dstFormat);

		case VDWaveFormat::kTagMPEG1:
		case VDWaveFormat::kTagMPEGLayer3:
			return VDCreateAudioDecompressorMPEG(srcFormat, dstFormat);
	}

	return NULL;
}

long fill_silence_pcm(void* buffer, int samples, const VDWaveFormat *wfex) {
	const int mBlockSize = wfex->mBlockSize;

	if (is_audio_pcm(wfex)) {
		if (wfex->mSampleBits >= 16)
			memset(buffer, 0, mBlockSize*samples);
		else
			memset(buffer, 0x80, mBlockSize*samples);
		return mBlockSize*samples;
	} else if (is_audio_float(wfex)) {
		memset(buffer, 0, mBlockSize*samples);
		return mBlockSize*samples;
	}

	return 0;
}

bool is_audio_pcm(const VDWaveFormat *wfex) {
	WAVEFORMATEXTENSIBLE *wfext = (WAVEFORMATEXTENSIBLE*)wfex;
	if (wfex->mTag == WAVE_FORMAT_PCM) return true;
	if (wfex->mTag == WAVE_FORMAT_EXTENSIBLE && wfext->SubFormat==KSDATAFORMAT_SUBTYPE_PCM) return true;
	return false;
}

bool is_audio_pcm8(const VDWaveFormat *wfex) {
	if (wfex->mSampleBits != 8) return false;
	return is_audio_pcm(wfex);
}

bool is_audio_pcm16(const VDWaveFormat *wfex) {
	if (wfex->mSampleBits != 16) return false;
	return is_audio_pcm(wfex);
}

bool is_audio_float(const VDWaveFormat *wfex) {
	if (wfex->mSampleBits != 32) return false;
	WAVEFORMATEXTENSIBLE *wfext = (WAVEFORMATEXTENSIBLE*)wfex;
	if (wfex->mTag == WAVE_FORMAT_IEEE_FLOAT) return true;
	if (wfex->mTag == WAVE_FORMAT_EXTENSIBLE && wfext->SubFormat==KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) return true;
	return false;
}

int get_audio_sampleBits(const VDWaveFormat *wfex) {
	WAVEFORMATEXTENSIBLE *wfext = (WAVEFORMATEXTENSIBLE*)wfex;
	if (wfex->mTag == WAVE_FORMAT_PCM) return wfex->mSampleBits;
	if (wfex->mTag == WAVE_FORMAT_EXTENSIBLE) return wfext->Samples.wValidBitsPerSample;
	return wfex->mSampleBits;
}

// copy-paste from FFMPEG, copyright FFMPEG developers

#define AV_CH_FRONT_LEFT             0x00000001
#define AV_CH_FRONT_RIGHT            0x00000002
#define AV_CH_FRONT_CENTER           0x00000004
#define AV_CH_LOW_FREQUENCY          0x00000008
#define AV_CH_BACK_LEFT              0x00000010
#define AV_CH_BACK_RIGHT             0x00000020
#define AV_CH_FRONT_LEFT_OF_CENTER   0x00000040
#define AV_CH_FRONT_RIGHT_OF_CENTER  0x00000080
#define AV_CH_BACK_CENTER            0x00000100
#define AV_CH_SIDE_LEFT              0x00000200
#define AV_CH_SIDE_RIGHT             0x00000400
#define AV_CH_TOP_CENTER             0x00000800
#define AV_CH_TOP_FRONT_LEFT         0x00001000
#define AV_CH_TOP_FRONT_CENTER       0x00002000
#define AV_CH_TOP_FRONT_RIGHT        0x00004000
#define AV_CH_TOP_BACK_LEFT          0x00008000
#define AV_CH_TOP_BACK_CENTER        0x00010000
#define AV_CH_TOP_BACK_RIGHT         0x00020000

#define AV_CH_LAYOUT_MONO              (AV_CH_FRONT_CENTER)
#define AV_CH_LAYOUT_STEREO            (AV_CH_FRONT_LEFT|AV_CH_FRONT_RIGHT)
#define AV_CH_LAYOUT_2POINT1           (AV_CH_LAYOUT_STEREO|AV_CH_LOW_FREQUENCY)
#define AV_CH_LAYOUT_2_1               (AV_CH_LAYOUT_STEREO|AV_CH_BACK_CENTER)
#define AV_CH_LAYOUT_SURROUND          (AV_CH_LAYOUT_STEREO|AV_CH_FRONT_CENTER)
#define AV_CH_LAYOUT_3POINT1           (AV_CH_LAYOUT_SURROUND|AV_CH_LOW_FREQUENCY)
#define AV_CH_LAYOUT_4POINT0           (AV_CH_LAYOUT_SURROUND|AV_CH_BACK_CENTER)
#define AV_CH_LAYOUT_4POINT1           (AV_CH_LAYOUT_4POINT0|AV_CH_LOW_FREQUENCY)
#define AV_CH_LAYOUT_2_2               (AV_CH_LAYOUT_STEREO|AV_CH_SIDE_LEFT|AV_CH_SIDE_RIGHT)
#define AV_CH_LAYOUT_QUAD              (AV_CH_LAYOUT_STEREO|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT)
#define AV_CH_LAYOUT_5POINT0           (AV_CH_LAYOUT_SURROUND|AV_CH_SIDE_LEFT|AV_CH_SIDE_RIGHT)
#define AV_CH_LAYOUT_5POINT1           (AV_CH_LAYOUT_5POINT0|AV_CH_LOW_FREQUENCY)
#define AV_CH_LAYOUT_5POINT0_BACK      (AV_CH_LAYOUT_SURROUND|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT)
#define AV_CH_LAYOUT_5POINT1_BACK      (AV_CH_LAYOUT_5POINT0_BACK|AV_CH_LOW_FREQUENCY)
#define AV_CH_LAYOUT_6POINT0           (AV_CH_LAYOUT_5POINT0|AV_CH_BACK_CENTER)
#define AV_CH_LAYOUT_6POINT0_FRONT     (AV_CH_LAYOUT_2_2|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER)
#define AV_CH_LAYOUT_HEXAGONAL         (AV_CH_LAYOUT_5POINT0_BACK|AV_CH_BACK_CENTER)
#define AV_CH_LAYOUT_6POINT1           (AV_CH_LAYOUT_5POINT1|AV_CH_BACK_CENTER)
#define AV_CH_LAYOUT_6POINT1_BACK      (AV_CH_LAYOUT_5POINT1_BACK|AV_CH_BACK_CENTER)
#define AV_CH_LAYOUT_6POINT1_FRONT     (AV_CH_LAYOUT_6POINT0_FRONT|AV_CH_LOW_FREQUENCY)
#define AV_CH_LAYOUT_7POINT0           (AV_CH_LAYOUT_5POINT0|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT)
#define AV_CH_LAYOUT_7POINT0_FRONT     (AV_CH_LAYOUT_5POINT0|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER)
#define AV_CH_LAYOUT_7POINT1           (AV_CH_LAYOUT_5POINT1|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT)
#define AV_CH_LAYOUT_7POINT1_WIDE      (AV_CH_LAYOUT_5POINT1|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER)
#define AV_CH_LAYOUT_7POINT1_WIDE_BACK (AV_CH_LAYOUT_5POINT1_BACK|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER)
#define AV_CH_LAYOUT_OCTAGONAL         (AV_CH_LAYOUT_5POINT0|AV_CH_BACK_LEFT|AV_CH_BACK_CENTER|AV_CH_BACK_RIGHT)
#define AV_CH_LAYOUT_HEXADECAGONAL     (AV_CH_LAYOUT_OCTAGONAL|AV_CH_WIDE_LEFT|AV_CH_WIDE_RIGHT|AV_CH_TOP_BACK_LEFT|AV_CH_TOP_BACK_RIGHT|AV_CH_TOP_BACK_CENTER|AV_CH_TOP_FRONT_CENTER|AV_CH_TOP_FRONT_LEFT|AV_CH_TOP_FRONT_RIGHT)

int default_channel_mask(int n) {
	if(n>8) return 0;
	int ch_map[9] = {
		0,
		AV_CH_LAYOUT_MONO,
		AV_CH_LAYOUT_STEREO,
		AV_CH_LAYOUT_2POINT1,
		AV_CH_LAYOUT_4POINT0,
		AV_CH_LAYOUT_5POINT0_BACK,
		AV_CH_LAYOUT_5POINT1_BACK,
		AV_CH_LAYOUT_6POINT1,
		AV_CH_LAYOUT_7POINT1,
	};
	return ch_map[n];
}
