// VirtualDub - Video processing and capture application
//
// Copyright (C) 2026 v0lt
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#include "stdafx.h"

#include <vd2/system/vdtypes.h>
#include "ScriptInterpreter.h"
#include "ScriptError.h"

#include "misc.h"
#include <vd2/system/cpuaccel.h>
#include <vd2/Kasumi/pixel.h>
#include <vd2/Kasumi/zimg_resample.h>
#include <vd2/Kasumi/resample_kernels.h>
#include <vd2/VDXFrame/VideoFilter.h>
#include <vd2/plugin/vdvideoaccel.h>
#include "resource.h"
#include "filter.h"
#include "vbitmap.h"
#include "f_resize_config.h"

#include "f_resize.inl"

///////////////////////////////////////////////////////////////////////////////

void VDPixmapRectFillRGB32(const VDPixmap& px, const vdrect32f& rDst, uint32 c);


class VDVideoFilterResizeZimg : public VDXVideoFilter {
public:
	VDVideoFilterResizeZimg();
	VDVideoFilterResizeZimg(const VDVideoFilterResizeZimg&);

	virtual bool Init() override;
	virtual void Run()override;
	virtual uint32 GetParams() override;
	virtual bool Configure(VDXHWND hwnd) override;
	virtual void GetSettingString(char *buf, int maxlen) override;
	virtual void GetScriptString(char *buf, int maxlen) override;
	virtual void Start() override;
	virtual void End() override;

	void ScriptConfig(IVDXScriptInterpreter *env, const VDXScriptValue *argv, int argc);
	void ScriptConfig2(IVDXScriptInterpreter *env, const VDXScriptValue *argv, int argc);

	VDXVF_DECLARE_SCRIPT_METHODS();

protected:
	VDResizeFilterData mConfig;

	IVDPixmapResampler *mpResampler;
	IVDPixmapResampler *mpResampler2;

	double	mVDXAImageW;
	double	mVDXAImageH;
	VDXRect	mVDXADestRect;
	int		mVDXADestW;
	int		mVDXADestH;
	int		mVDXADestCroppedW;
	int		mVDXADestCroppedH;
	double	mVDXAHorizFactor;
	double	mVDXAVertFactor;
	int		mVDXAHorizTaps;
	int		mVDXAVertTaps;

	uint32 mVDXAFP_2Tap;
	uint32 mVDXAFP_4Tap;
	uint32 mVDXAFP_6Tap;
	uint32 mVDXAFP_8Tap;
	uint32 mVDXAFP_10Tap;
	uint32 mVDXART_Temp;
	uint32 mVDXATex_HorizFilt;
	uint32 mVDXATex_VertFilt;
};

VDVideoFilterResizeZimg::VDVideoFilterResizeZimg()
	: mpResampler(NULL)
	, mpResampler2(NULL)
	, mVDXAHorizFactor(1)
	, mVDXAVertFactor(1)
	, mVDXAHorizTaps(0)
	, mVDXAVertTaps(0)
	, mVDXAFP_2Tap(0)
	, mVDXAFP_4Tap(0)
	, mVDXAFP_6Tap(0)
	, mVDXAFP_8Tap(0)
	, mVDXART_Temp(0)
	, mVDXATex_HorizFilt(0)
	, mVDXATex_VertFilt(0)
{
}

VDVideoFilterResizeZimg::VDVideoFilterResizeZimg(const VDVideoFilterResizeZimg& src)
	: mConfig(src.mConfig)
	, mpResampler(NULL)
	, mpResampler2(NULL)
	, mVDXAHorizFactor(1)
	, mVDXAVertFactor(1)
	, mVDXAHorizTaps(0)
	, mVDXAVertTaps(0)
	, mVDXAFP_2Tap(0)
	, mVDXAFP_4Tap(0)
	, mVDXAFP_6Tap(0)
	, mVDXAFP_8Tap(0)
	, mVDXAFP_10Tap(0)
	, mVDXART_Temp(0)
	, mVDXATex_HorizFilt(0)
	, mVDXATex_VertFilt(0)
{
}

bool VDVideoFilterResizeZimg::Init() {
	mConfig.ExchangeWithRegistry(false);
	return true;
}

void VDVideoFilterResizeZimg::Run() {
	VDPixmap pxdst = VDPixmap::copy(*fa->dst.mpPixmap);
	VDPixmap pxsrc = VDPixmap::copy(*fa->src.mpPixmap);

	if (fa->fma && fa->fma->fmpixmap) {
		pxdst.info = *fa->fma->fmpixmap->GetPixmapInfo(fa->dst.mpPixmap);
		pxsrc.info = *fa->fma->fmpixmap->GetPixmapInfo(fa->src.mpPixmap);
	}

	const float fx1 = 0.0f;
	const float fy1 = 0.0f;
	const float fx2 = mConfig.mDstRect.left;
	const float fy2 = mConfig.mDstRect.top;
	const float fx3 = mConfig.mDstRect.right;
	const float fy3 = mConfig.mDstRect.bottom;
	const float fx4 = (float)pxdst.w;
	const float fy4 = (float)pxdst.h;

	uint32 fill = mConfig.mFillColor;
	VDPixmapRectFillRGB32(pxdst, vdrect32f(fx1, fy1, fx4, fy2), fill);
	VDPixmapRectFillRGB32(pxdst, vdrect32f(fx1, fy2, fx2, fy3), fill);
	VDPixmapRectFillRGB32(pxdst, vdrect32f(fx3, fy2, fx4, fy3), fill);
	VDPixmapRectFillRGB32(pxdst, vdrect32f(fx1, fy3, fx4, fy4), fill);

	if (mConfig.mbInterlaced) {
		VDPixmap pxdst1(pxdst);
		VDPixmap pxsrc1(pxsrc);
		pxdst1.pitch += pxdst1.pitch;
		pxdst1.pitch2 += pxdst1.pitch2;
		pxdst1.pitch3 += pxdst1.pitch3;
		pxdst1.h = (pxdst1.h + 1) >> 1;
		pxsrc1.pitch += pxsrc1.pitch;
		pxsrc1.pitch2 += pxsrc1.pitch2;
		pxsrc1.pitch3 += pxsrc1.pitch3;
		pxsrc1.h = (pxsrc1.h + 1) >> 1;

		VDPixmap pxdst2(pxdst);
		VDPixmap pxsrc2(pxsrc);
		pxdst2.data = (char *)pxdst2.data + pxdst2.pitch;
		pxdst2.data2 = (char *)pxdst2.data2 + pxdst2.pitch2;
		pxdst2.data3 = (char *)pxdst2.data3 + pxdst2.pitch3;
		pxdst2.pitch += pxdst2.pitch;
		pxdst2.pitch2 += pxdst2.pitch2;
		pxdst2.pitch3 += pxdst2.pitch3;
		pxdst2.h = (pxdst2.h + 0) >> 1;
		pxsrc2.data = (char *)pxsrc2.data + pxsrc2.pitch;
		pxsrc2.data2 = (char *)pxsrc2.data2 + pxsrc2.pitch2;
		pxsrc2.data3 = (char *)pxsrc2.data3 + pxsrc2.pitch3;
		pxsrc2.pitch += pxsrc2.pitch;
		pxsrc2.pitch2 += pxsrc2.pitch2;
		pxsrc2.pitch3 += pxsrc2.pitch3;
		pxsrc2.h = (pxsrc2.h + 0) >> 1;

		mpResampler ->Process(pxdst1, pxsrc1);
		mpResampler2->Process(pxdst2, pxsrc2);
	} else {
		mpResampler->Process(pxdst, pxsrc);
	}
}

uint32 VDVideoFilterResizeZimg::GetParams() {
	using namespace vd2;
	const VDXPixmapLayout& src = *fa->src.mpPixmapLayout;
	VDPixmapFormatEx format = ExtractBaseFormat(src.format);

	switch(format) {
		case kPixFormat_XRGB8888:
		case kPixFormat_XRGB64:
		case kPixFormat_RGB_Planar:
		case kPixFormat_RGBA_Planar:
		case kPixFormat_RGB_Planar16:
		case kPixFormat_RGBA_Planar16:
		case kPixFormat_RGB_Planar32F:
		case kPixFormat_RGBA_Planar32F:
		case kPixFormat_YUV444_Planar:
		case kPixFormat_YUV422_Planar:
		case kPixFormat_YUV411_Planar:
		case kPixFormat_YUV422_Planar16:
		case kPixFormat_YUV444_Planar16:
		case kPixFormat_YUV422_Alpha_Planar16:
		case kPixFormat_YUV444_Alpha_Planar16:
		case kPixFormat_YUV422_Alpha_Planar:
		case kPixFormat_YUV444_Alpha_Planar:
		case kPixFormat_Y8:
		case kPixFormat_Y16:
			break;

		case kPixFormat_VDXA_RGB:
		case kPixFormat_VDXA_YUV:
		case kPixFormat_YUV420_Planar:
		case kPixFormat_YUV410_Planar:
		case kPixFormat_YUV420_Planar16:
		case kPixFormat_YUV420_Alpha_Planar16:
		case kPixFormat_YUV420_Alpha_Planar:
			if (mConfig.mbInterlaced)
				return FILTERPARAM_NOT_SUPPORTED;

			break;

		default:
			return FILTERPARAM_NOT_SUPPORTED;
	}

	if (mConfig.Validate()) {
		// uh oh.
		return 0;
	}

	double imgw, imgh;
	uint32 framew, frameh;
	mConfig.ComputeSizes(fa->src.w, fa->src.h, imgw, imgh, framew, frameh, true, true, fa->src.mpPixmapLayout->format);
	mConfig.ComputeDestRect(framew, frameh, imgw, imgh);

	switch(fa->src.mpPixmapLayout->format) {
		case nsVDXPixmap::kPixFormat_VDXA_RGB:
		case nsVDXPixmap::kPixFormat_VDXA_YUV:
			{
				if (mConfig.mFilterMode == VDResizeFilterData::FILTER_NONE)
					return FILTERPARAM_NOT_SUPPORTED;

				// compute horizontal and vertical taps
				mVDXAHorizFactor = std::max<float>(1.0f, (float)fa->src.w / (float)imgw);
				mVDXAVertFactor = std::max<float>(1.0f, (float)fa->src.h / (float)imgh);

				static const float kBaseSizePerMode[]={
					1,
					2,
					4,
					2,
					4,
					4,
					4,
					6,
				};

				VDASSERTCT(std::size(kBaseSizePerMode) == VDResizeFilterData::FILTER_COUNT);

				if (mConfig.mFilterMode < VDResizeFilterData::FILTER_TABLEBILINEAR) {
					mVDXAHorizFactor = 1.0f;
					mVDXAVertFactor = 1.0f;
				}

				float basetaps = kBaseSizePerMode[mConfig.mFilterMode];
				mVDXAHorizTaps = (VDCeilToInt(basetaps * mVDXAHorizFactor) + 1) & ~1;
				mVDXAVertTaps = (VDCeilToInt(basetaps * mVDXAVertFactor) + 1) & ~1;

				if (mVDXAHorizTaps > 10 || mVDXAVertTaps > 10)
					return FILTERPARAM_NOT_SUPPORTED;

				fa->src.mBorderWidth = mVDXAHorizTaps >> 1;
				fa->src.mBorderHeight = 0;
			}
			break;
	}

	fa->dst.mpPixmapLayout->format = fa->src.mpPixmapLayout->format;
	fa->dst.mpPixmapLayout->w = framew;
	fa->dst.mpPixmapLayout->h = frameh;
	fa->dst.mpPixmapLayout->pitch = 0;
	fa->dst.depth = 0;

	return FILTERPARAM_SWAP_BUFFERS | FILTERPARAM_SUPPORTS_ALTFORMATS | FILTERPARAM_PURE_TRANSFORM | FILTERPARAM_NORMALIZE16;
}

bool VDVideoFilterResizeZimg::Configure(VDXHWND hwnd) {
	VDResizeFilterData *mfd = &mConfig;
	VDResizeFilterData mfd2 = *mfd;

	if (!VDFilterResizeActivateConfigDialog(*mfd, fa->ifp2, fa->src.w, fa->src.h, (VDGUIHandle)hwnd)) {
		*mfd = mfd2;
		return false;
	}

	return true;
}

void VDVideoFilterResizeZimg::GetSettingString(char *buf, int maxlen) {
	SafePrintf(buf, maxlen, " (%s)", VDResizeFilterData::kFilterNames[mConfig.mFilterMode]);
}

void VDVideoFilterResizeZimg::Start() {
	const VDXPixmapLayout& pxdst = *fa->dst.mpPixmapLayout;
	const VDXPixmapLayout& pxsrc = *fa->src.mpPixmapLayout;

	if (const char *error = mConfig.Validate())
		ff->Except("%s", error);

	double dstw, dsth;
	uint32 framew, frameh;
	mConfig.ComputeSizes(pxsrc.w, pxsrc.h, dstw, dsth, framew, frameh, true, true, fa->src.mpPixmapLayout->format);

	mpResampler = VDCreatePixmapZimgResampler();
	if (!mpResampler)
		ff->ExceptOutOfMemory();

	IVDPixmapResampler::FilterMode fmode;
	bool bInterpolationOnly = true;
	float splineFactor = -0.60f;

	switch(mConfig.mFilterMode) {
		case VDResizeFilterData::FILTER_NONE:
			fmode = IVDPixmapResampler::kFilterPoint;
			break;

		case VDResizeFilterData::FILTER_TABLEBILINEAR:
			bInterpolationOnly = false;
		case VDResizeFilterData::FILTER_BILINEAR:
			fmode = IVDPixmapResampler::kFilterLinear;
			break;

		case VDResizeFilterData::FILTER_TABLEBICUBIC060:
			splineFactor = -0.60;
			fmode = IVDPixmapResampler::kFilterCubic;
			bInterpolationOnly = false;
			break;

		case VDResizeFilterData::FILTER_TABLEBICUBIC075:
			bInterpolationOnly = false;
		case VDResizeFilterData::FILTER_BICUBIC:
			splineFactor = -0.75;
			fmode = IVDPixmapResampler::kFilterCubic;
			break;

		case VDResizeFilterData::FILTER_TABLEBICUBIC100:
			bInterpolationOnly = false;
			splineFactor = -1.0;
			fmode = IVDPixmapResampler::kFilterCubic;
			break;

		case VDResizeFilterData::FILTER_LANCZOS3:
			bInterpolationOnly = false;
			fmode = IVDPixmapResampler::kFilterLanczos3;
			break;
	}

	mpResampler->SetSplineFactor(splineFactor);
	mpResampler->SetFilters(fmode, fmode, bInterpolationOnly);

	vdrect32f srcrect(0.0f, 0.0f, (float)pxsrc.w, (float)pxsrc.h);

	if (mConfig.mbInterlaced) {
		mpResampler2 = VDCreatePixmapZimgResampler();
		if (!mpResampler2)
			ff->ExceptOutOfMemory();

		mpResampler2->SetSplineFactor(splineFactor);
		mpResampler2->SetFilters(fmode, fmode, bInterpolationOnly);

		vdrect32f srcrect1(srcrect);
		vdrect32f srcrect2(srcrect);
		vdrect32f dstrect1(mConfig.mDstRect);
		vdrect32f dstrect2(mConfig.mDstRect);

		srcrect1.transform(1.0f, 0.5f, 0.0f, 0.25f);
		dstrect1.transform(1.0f, 0.5f, 0.0f, 0.25f);
		srcrect2.transform(1.0f, 0.5f, 0.0f, -0.25f);
		dstrect2.transform(1.0f, 0.5f, 0.0f, -0.25f);

		VDVERIFY(mpResampler ->Init(dstrect1, pxdst.w, (pxdst.h+1) >> 1, pxdst.format, srcrect1, pxsrc.w, (pxsrc.h+1) >> 1, pxsrc.format));
		VDVERIFY(mpResampler2->Init(dstrect2, pxdst.w, (pxdst.h+0) >> 1, pxdst.format, srcrect2, pxsrc.w, (pxsrc.h+0) >> 1, pxsrc.format));
	} else {
		VDVERIFY(mpResampler->Init(mConfig.mDstRect, pxdst.w, pxdst.h, pxdst.format, srcrect, pxsrc.w, pxsrc.h, pxsrc.format));
	}
}

void VDVideoFilterResizeZimg::End() {
	delete mpResampler;		mpResampler = NULL;
	delete mpResampler2;	mpResampler2 = NULL;
}

void VDVideoFilterResizeZimg::ScriptConfig(IVDXScriptInterpreter *isi, const VDXScriptValue *argv, int argc) {
	mConfig.mImageW	= argv[0].asDouble();
	mConfig.mImageH	= argv[1].asDouble();
	mConfig.mbUseRelative = false;
	mConfig.mImageAspectMode = VDResizeFilterData::kImageAspectNone;

	if (argv[2].isInt())
		mConfig.mFilterMode = argv[2].asInt();
	else {
		char *s = *argv[2].asString();

		if (!_stricmp(s, "point") || !_stricmp(s, "nearest"))
			mConfig.mFilterMode = VDResizeFilterData::FILTER_NONE;
		else if (!_stricmp(s, "bilinear"))
			mConfig.mFilterMode = VDResizeFilterData::FILTER_BILINEAR;
		else if (!_stricmp(s, "bicubic"))
			mConfig.mFilterMode = VDResizeFilterData::FILTER_BICUBIC;
		else
			VDSCRIPT_EXT_ERROR(FCALL_UNKNOWN_STR);
	}

	mConfig.mbInterlaced = false;

	if (mConfig.mFilterMode & 128) {
		mConfig.mbInterlaced = true;
		mConfig.mFilterMode &= 127;
	}

	mConfig.mFrameMode = VDResizeFilterData::kFrameModeNone;
	if (argc > 3) {
		mConfig.mFrameMode = VDResizeFilterData::kFrameModeToSize;
		mConfig.mFrameW = argv[3].asInt();
		mConfig.mFrameH = argv[4].asInt();
		mConfig.mFillColor = argv[5].asInt();
	}

	// make the sizes somewhat sane
	if (mConfig.mImageW < 1.0f)
		mConfig.mImageW = 1.0f;
	if (mConfig.mImageH < 1.0f)
		mConfig.mImageH = 1.0f;

	if (mConfig.mFrameMode) {
		if (mConfig.mFrameW < mConfig.mImageW)
			mConfig.mFrameW = (int)ceil(mConfig.mImageW);

		if (mConfig.mFrameH < mConfig.mImageH)
			mConfig.mFrameH = (int)ceil(mConfig.mImageH);
	}
}

void VDVideoFilterResizeZimg::ScriptConfig2(IVDXScriptInterpreter *isi, const VDXScriptValue *argv, int argc) {
	mConfig.mbUseRelative	= (argv[2].asInt() & 1) != 0;

	if (mConfig.mbUseRelative) {
		mConfig.mImageRelW	= argv[0].asDouble();
		mConfig.mImageRelH	= argv[1].asDouble();
	} else {
		mConfig.mImageW	= argv[0].asDouble();
		mConfig.mImageH	= argv[1].asDouble();
	}

	mConfig.mImageAspectNumerator		= argv[3].asDouble();
	mConfig.mImageAspectDenominator	= argv[4].asDouble();
	mConfig.mImageAspectMode			= argv[5].asInt();
	if (mConfig.mImageAspectMode >= VDResizeFilterData::kImageAspectModeCount)
		mConfig.mImageAspectMode = 0;

	mConfig.mFrameW = argv[6].asInt();
	mConfig.mFrameH = argv[7].asInt();

	mConfig.mFrameAspectNumerator		= argv[8].asDouble();
	mConfig.mFrameAspectDenominator	= argv[9].asDouble();
	mConfig.mFrameMode			= argv[10].asInt();
	if (mConfig.mFrameMode >= VDResizeFilterData::kFrameModeCount)
		mConfig.mFrameMode = 0;

	mConfig.mFilterMode = argv[11].asInt();
	mConfig.mbInterlaced = false;

	if (mConfig.mFilterMode & 128) {
		mConfig.mbInterlaced = true;
		mConfig.mFilterMode &= 127;
	}

	mConfig.mAlignment = argv[12].asInt();
	switch(mConfig.mAlignment) {
		case 1:
		case 2:
		case 4:
		case 8:
		case 16:
			break;
		default:
			mConfig.mAlignment = 1;
			break;
	}

	mConfig.mFillColor = argv[13].asInt();
}

VDXVF_BEGIN_SCRIPT_METHODS(VDVideoFilterResizeZimg)
	VDXVF_DEFINE_SCRIPT_METHOD(VDVideoFilterResizeZimg, ScriptConfig, "ddi")
	VDXVF_DEFINE_SCRIPT_METHOD2(VDVideoFilterResizeZimg, ScriptConfig, "dds")
	VDXVF_DEFINE_SCRIPT_METHOD2(VDVideoFilterResizeZimg, ScriptConfig, "ddiiii")
	VDXVF_DEFINE_SCRIPT_METHOD2(VDVideoFilterResizeZimg, ScriptConfig, "ddsiii")
	VDXVF_DEFINE_SCRIPT_METHOD2(VDVideoFilterResizeZimg, ScriptConfig2, "ddiddiiiddiiii")
VDXVF_END_SCRIPT_METHODS()

void VDVideoFilterResizeZimg::GetScriptString(char *buf, int maxlen) {
	int filtmode = mConfig.mFilterMode + (mConfig.mbInterlaced ? 128 : 0);

	SafePrintf(buf, maxlen, "Config(%g,%g,%d,%g,%g,%d,%d,%d,%g,%g,%d,%d,%d,0x%06x)"
		, mConfig.mbUseRelative ? mConfig.mImageRelW : mConfig.mImageW
		, mConfig.mbUseRelative ? mConfig.mImageRelH : mConfig.mImageH
		, mConfig.mbUseRelative
		, mConfig.mImageAspectNumerator
		, mConfig.mImageAspectDenominator
		, mConfig.mImageAspectMode
		, mConfig.mFrameW
		, mConfig.mFrameH
		, mConfig.mFrameAspectNumerator
		, mConfig.mFrameAspectDenominator
		, mConfig.mFrameMode
		, filtmode
		, mConfig.mAlignment
		, mConfig.mFillColor);
}

extern const VDXFilterDefinition filterDef_resize_zimg = VDXVideoFilterDefinition<VDVideoFilterResizeZimg>(
	NULL,
	"resize zimg",
	"Resizes the image to a new size."
	"\n\n[SIMD optimized] [YCbCr processing]"
);

// warning C4505: 'VDXVideoFilter::[thunk]: __thiscall VDXVideoFilter::`vcall'{48,{flat}}' }'' : unreferenced local function has been removed
#pragma warning(disable: 4505)
