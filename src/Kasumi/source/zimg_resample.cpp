// VirtualDub - Video processing and capture application
// Graphics support library
//
// Copyright (C) 2026 v0lt
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#include <stdafx.h>
#include <float.h>
#include <math.h>
#include <vd2/system/vdalloc.h>
#include <vd2/system/vdstl.h>
#include <vd2/system/memory.h>
#include <vd2/system/math.h>
#include <vd2/system/cpuaccel.h>
#include <vd2/Kasumi/pixmap.h>
#include <vd2/Kasumi/pixmaputils.h>
#include <vd2/Kasumi/zimg_resample.h>
#include "uberblit_gen.h"

#include "../../src/zimg/api/zimg++.hpp"
#include <memory>

using namespace nsVDPixmap;

struct FormatZimgDesc_t {
	VDPixmapFormat format;
	unsigned planes;
	zimg_pixel_type_e pixel_type;
	unsigned subsample_w;
	unsigned subsample_h;
	zimg_color_family_e color_family;
	zimg_matrix_coefficients_e matrix_coefficients;
	zimg_pixel_range_e pixel_range;
	zimg_field_parity_e field_parity;
	zimg_chroma_location_e chroma_location;
};

static const FormatZimgDesc_t FormatZimgDescNull = { kPixFormat_Null };

static const FormatZimgDesc_t g_FormatTable[] = {
	{ kPixFormat_Y8,                     1, ZIMG_PIXEL_BYTE,  0, 0, ZIMG_COLOR_GREY, ZIMG_MATRIX_UNSPECIFIED, ZIMG_RANGE_LIMITED, ZIMG_FIELD_PROGRESSIVE, ZIMG_CHROMA_LEFT,  },
	{ kPixFormat_YUV444_Planar,          3, ZIMG_PIXEL_BYTE,  0, 0, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT470_BG,    ZIMG_RANGE_LIMITED, ZIMG_FIELD_PROGRESSIVE, ZIMG_CHROMA_LEFT   },
	{ kPixFormat_YUV422_Planar,          3, ZIMG_PIXEL_BYTE,  1, 0, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT470_BG,    ZIMG_RANGE_LIMITED, ZIMG_FIELD_PROGRESSIVE, ZIMG_CHROMA_LEFT   },
	{ kPixFormat_YUV420_Planar,          3, ZIMG_PIXEL_BYTE,  1, 1, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT470_BG,    ZIMG_RANGE_LIMITED, ZIMG_FIELD_PROGRESSIVE, ZIMG_CHROMA_LEFT   },
	{ kPixFormat_YUV422_Planar_Centered, 3, ZIMG_PIXEL_BYTE,  1, 0, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT470_BG,    ZIMG_RANGE_LIMITED, ZIMG_FIELD_PROGRESSIVE, ZIMG_CHROMA_CENTER },
	{ kPixFormat_YUV420_Planar_Centered, 3, ZIMG_PIXEL_BYTE,  1, 1, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT470_BG,    ZIMG_RANGE_LIMITED, ZIMG_FIELD_PROGRESSIVE, ZIMG_CHROMA_CENTER },

	{ kPixFormat_Y8_FR,                  1, ZIMG_PIXEL_BYTE,  0, 0, ZIMG_COLOR_GREY, ZIMG_MATRIX_UNSPECIFIED, ZIMG_RANGE_FULL,    ZIMG_FIELD_PROGRESSIVE, ZIMG_CHROMA_LEFT,  },
	{ kPixFormat_YUV444_Planar_709,      3, ZIMG_PIXEL_BYTE,  0, 0, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT709,       ZIMG_RANGE_LIMITED, ZIMG_FIELD_PROGRESSIVE, ZIMG_CHROMA_LEFT,  },
	{ kPixFormat_YUV422_Planar_709,      3, ZIMG_PIXEL_BYTE,  1, 0, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT709,       ZIMG_RANGE_LIMITED, ZIMG_FIELD_PROGRESSIVE, ZIMG_CHROMA_LEFT,  },
	{ kPixFormat_YUV420_Planar_709,      3, ZIMG_PIXEL_BYTE,  1, 1, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT709,       ZIMG_RANGE_LIMITED, ZIMG_FIELD_PROGRESSIVE, ZIMG_CHROMA_LEFT,  },
	{ kPixFormat_YUV444_Planar_FR,       3, ZIMG_PIXEL_BYTE,  0, 0, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT470_BG,    ZIMG_RANGE_FULL,    ZIMG_FIELD_PROGRESSIVE, ZIMG_CHROMA_LEFT,  },
	{ kPixFormat_YUV422_Planar_FR,       3, ZIMG_PIXEL_BYTE,  1, 0, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT470_BG,    ZIMG_RANGE_FULL,    ZIMG_FIELD_PROGRESSIVE, ZIMG_CHROMA_LEFT,  },
	{ kPixFormat_YUV420_Planar_FR,       3, ZIMG_PIXEL_BYTE,  1, 1, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT470_BG,    ZIMG_RANGE_FULL,    ZIMG_FIELD_PROGRESSIVE, ZIMG_CHROMA_LEFT,  },
	{ kPixFormat_YUV444_Planar_709_FR,   3, ZIMG_PIXEL_BYTE,  0, 0, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT709,       ZIMG_RANGE_FULL,    ZIMG_FIELD_PROGRESSIVE, ZIMG_CHROMA_LEFT,  },
	{ kPixFormat_YUV422_Planar_709_FR,   3, ZIMG_PIXEL_BYTE,  1, 0, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT709,       ZIMG_RANGE_FULL,    ZIMG_FIELD_PROGRESSIVE, ZIMG_CHROMA_LEFT,  },
	{ kPixFormat_YUV420_Planar_709_FR,   3, ZIMG_PIXEL_BYTE,  1, 1, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT709,       ZIMG_RANGE_FULL,    ZIMG_FIELD_PROGRESSIVE, ZIMG_CHROMA_LEFT,  },

	{ kPixFormat_YUV420i_Planar,         3, ZIMG_PIXEL_BYTE,  1, 1, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT470_BG,    ZIMG_RANGE_LIMITED, ZIMG_FIELD_TOP,         ZIMG_CHROMA_LEFT,  },
	{ kPixFormat_YUV420i_Planar_FR,      3, ZIMG_PIXEL_BYTE,  1, 1, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT470_BG,    ZIMG_RANGE_FULL,    ZIMG_FIELD_TOP,         ZIMG_CHROMA_LEFT,  },
	{ kPixFormat_YUV420i_Planar_709,     3, ZIMG_PIXEL_BYTE,  1, 1, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT709,       ZIMG_RANGE_LIMITED, ZIMG_FIELD_TOP,         ZIMG_CHROMA_LEFT,  },
	{ kPixFormat_YUV420i_Planar_709_FR,  3, ZIMG_PIXEL_BYTE,  1, 1, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT709,       ZIMG_RANGE_FULL,    ZIMG_FIELD_TOP,         ZIMG_CHROMA_LEFT,  },
	{ kPixFormat_YUV420it_Planar,        3, ZIMG_PIXEL_BYTE,  1, 1, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT470_BG,    ZIMG_RANGE_LIMITED, ZIMG_FIELD_TOP,         ZIMG_CHROMA_LEFT,  },
	{ kPixFormat_YUV420it_Planar_FR,     3, ZIMG_PIXEL_BYTE,  1, 1, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT470_BG,    ZIMG_RANGE_FULL,    ZIMG_FIELD_TOP,         ZIMG_CHROMA_LEFT,  },
	{ kPixFormat_YUV420it_Planar_709,    3, ZIMG_PIXEL_BYTE,  1, 1, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT709,       ZIMG_RANGE_LIMITED, ZIMG_FIELD_TOP,         ZIMG_CHROMA_LEFT,  },
	{ kPixFormat_YUV420it_Planar_709_FR, 3, ZIMG_PIXEL_BYTE,  1, 1, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT709,       ZIMG_RANGE_FULL,    ZIMG_FIELD_TOP,         ZIMG_CHROMA_LEFT,  },
	{ kPixFormat_YUV420ib_Planar,        3, ZIMG_PIXEL_BYTE,  1, 1, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT470_BG,    ZIMG_RANGE_LIMITED, ZIMG_FIELD_BOTTOM,      ZIMG_CHROMA_LEFT,  },
	{ kPixFormat_YUV420ib_Planar_FR,     3, ZIMG_PIXEL_BYTE,  1, 1, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT470_BG,    ZIMG_RANGE_FULL,    ZIMG_FIELD_BOTTOM,      ZIMG_CHROMA_LEFT,  },
	{ kPixFormat_YUV420ib_Planar_709,    3, ZIMG_PIXEL_BYTE,  1, 1, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT709,       ZIMG_RANGE_LIMITED, ZIMG_FIELD_BOTTOM,      ZIMG_CHROMA_LEFT,  },
	{ kPixFormat_YUV420ib_Planar_709_FR, 3, ZIMG_PIXEL_BYTE,  1, 1, ZIMG_COLOR_YUV,  ZIMG_MATRIX_BT709,       ZIMG_RANGE_FULL,    ZIMG_FIELD_BOTTOM,      ZIMG_CHROMA_LEFT,  },

	{ kPixFormat_RGB_Planar32F,          3, ZIMG_PIXEL_FLOAT, 0, 0, ZIMG_COLOR_RGB,  ZIMG_MATRIX_RGB,         ZIMG_RANGE_FULL,    ZIMG_FIELD_PROGRESSIVE, ZIMG_CHROMA_LEFT   },
	{ kPixFormat_RGBA_Planar32F,         4, ZIMG_PIXEL_FLOAT, 0, 0, ZIMG_COLOR_RGB,  ZIMG_MATRIX_RGB,         ZIMG_RANGE_FULL,    ZIMG_FIELD_PROGRESSIVE, ZIMG_CHROMA_LEFT   },
};

static const FormatZimgDesc_t& GetFormatDesc(const VDPixmapFormat format)
{
	for (const auto& fmtdesc : g_FormatTable) {
		if (fmtdesc.format == format) {
			return fmtdesc;
		}
	}

	return FormatZimgDescNull;
}

///////////////////////////////////////////////////////////////////////////
//
// the zimg resampler
//
///////////////////////////////////////////////////////////////////////////

class VDPixmapZimgResampler : public IVDPixmapResampler
{
public:
	VDPixmapZimgResampler();
	~VDPixmapZimgResampler();

	void SetSplineFactor(double A) override { mSplineFactor = A; }
	void SetFilters(FilterMode h, FilterMode v, bool interpolationOnly) override;
	bool Init(uint32 dw, uint32 dh, int dstformat, uint32 sw, uint32 sh, int srcformat) override;
	bool Init(const vdrect32f& dstrect, uint32 dw, uint32 dh, int dstformat, const vdrect32f& srcrect, uint32 sw, uint32 sh, int srcformat) override;
	void Shutdown() override;

	void Process(const VDPixmap& dst, const VDPixmap& src) override;

protected:
	void ApplyFilters(VDPixmapUberBlitterGenerator& gen, uint32 dw, uint32 dh, float xoffset, float yoffset, float xfactor, float yfactor);

	double				mSplineFactor;
	FilterMode			mFilterH;
	FilterMode			mFilterV;
	bool				mbInterpOnly;

	vdrect32			mDstRectPlane0;

	FormatZimgDesc_t mFmtDesc = FormatZimgDescNull;

	zimgxx::zimage_format mSrcFormat;
	zimgxx::zimage_format mDstFormat;

	zimgxx::zfilter_graph_builder_params mParams;

	zimgxx::FilterGraph mGraph;
	std::unique_ptr<void, std::integral_constant<decltype(&VDAlignedFree), &VDAlignedFree>> mTempBuffer;
};

IVDPixmapResampler *VDCreatePixmapZimgResampler() { return new VDPixmapZimgResampler; }

VDPixmapZimgResampler::VDPixmapZimgResampler()
	: mSplineFactor(-0.6)
	, mFilterH(kFilterCubic)
	, mFilterV(kFilterCubic)
	, mbInterpOnly(false)
{
}

VDPixmapZimgResampler::~VDPixmapZimgResampler()
{
	Shutdown();
}

void VDPixmapZimgResampler::SetFilters(FilterMode h, FilterMode v, bool interpolationOnly)
{
	mFilterH = h;
	mFilterV = v;
	mbInterpOnly = interpolationOnly;
}

bool VDPixmapZimgResampler::Init(uint32 dw, uint32 dh, int dstformat, uint32 sw, uint32 sh, int srcformat)
{
	vdrect32f rSrc(0.0f, 0.0f, (float)sw, (float)sh);
	vdrect32f rDst(0.0f, 0.0f, (float)dw, (float)dh);
	return Init(rDst, dw, dh, dstformat, rSrc, sw, sh, srcformat);
}

bool VDPixmapZimgResampler::Init(const vdrect32f& dstrect0, uint32 dw, uint32 dh, int dstformat, const vdrect32f& srcrect0, uint32 sw, uint32 sh, int srcformat)
{
	Shutdown();

	if (dstformat != srcformat) {
		return false;
	}

	// supported formats
	mFmtDesc = GetFormatDesc((VDPixmapFormat)srcformat);
	if (mFmtDesc.format == kPixFormat_Null) {
		return false;
	}

	mSrcFormat.pixel_type          = mFmtDesc.pixel_type;
	mSrcFormat.subsample_w         = mFmtDesc.subsample_w;
	mSrcFormat.subsample_h         = mFmtDesc.subsample_h;
	mSrcFormat.color_family        = mFmtDesc.color_family;
	mSrcFormat.matrix_coefficients = mFmtDesc.matrix_coefficients;
	mSrcFormat.pixel_range         = mFmtDesc.pixel_range;
	mSrcFormat.field_parity        = mFmtDesc.field_parity;
	mSrcFormat.chroma_location     = mFmtDesc.chroma_location;

	mDstFormat = mSrcFormat;

	mSrcFormat.width  = sw;
	mSrcFormat.height = sh;

	mDstFormat.width  = dw;
	mDstFormat.height = dh;

	// convert destination flips to source flips
	vdrect32f dstrect(dstrect0);
	vdrect32f srcrect(srcrect0);

	if (dstrect.left > dstrect.right) {
		std::swap(dstrect.left, dstrect.right);
		std::swap(srcrect.left, srcrect.right);
	}

	if (dstrect.top > dstrect.bottom) {
		std::swap(dstrect.top, dstrect.bottom);
		std::swap(srcrect.top, srcrect.bottom);
	}

	// compute source step factors
	float xfactor = (float)srcrect.width()  / (float)dstrect.width();
	float yfactor = (float)srcrect.height() / (float)dstrect.height();

	// clip destination rect
	if (dstrect.left < 0) {
		float clipx1 = -dstrect.left;
		srcrect.left += xfactor * clipx1;
		dstrect.left = 0.0f;
	}

	if (dstrect.top < 0) {
		float clipy1 = -dstrect.top;
		srcrect.top += yfactor * clipy1;
		dstrect.top = 0.0f;
	}

	float clipx2 = dstrect.right - (float)dw;
	if (clipx2 > 0) {
		srcrect.right -= xfactor * clipx2;
		dstrect.right = (float)dw;
	}

	float clipy2 = dstrect.bottom - (float)dh;
	if (clipy2 > 0) {
		srcrect.bottom -= yfactor * clipy2;
		dstrect.bottom = (float)dh;
	}

	// compute plane 0 dest rect in integral quanta
	const VDPixmapFormatInfo& formatInfo = VDPixmapGetInfo(dstformat);
	mDstRectPlane0.left		= VDCeilToInt(dstrect.left	 - 0.5f);
	mDstRectPlane0.top		= VDCeilToInt(dstrect.top	 - 0.5f);
	mDstRectPlane0.right	= VDCeilToInt(dstrect.right	 - 0.5f);
	mDstRectPlane0.bottom	= VDCeilToInt(dstrect.bottom - 0.5f);

	// compute plane 0 stepping parameters
	float xoffset = (((float)mDstRectPlane0.left + 0.5f) - dstrect.left) * xfactor + srcrect.left;
	float yoffset = (((float)mDstRectPlane0.top  + 0.5f) - dstrect.top ) * yfactor + srcrect.top;

	mSrcFormat.active_region.left   = srcrect.left;
	mSrcFormat.active_region.top    = srcrect.top;
	mSrcFormat.active_region.width  = srcrect.width();
	mSrcFormat.active_region.height = srcrect.height();

	mGraph = { zimgxx::FilterGraph::build(mSrcFormat, mDstFormat, &mParams) };

	//unsigned input_buffering = mGraph.get_input_buffering();
	//unsigned output_buffering = mGraph.get_output_buffering();

	size_t tmp_size = mGraph.get_tmp_size();
	mTempBuffer.reset(VDAlignedMalloc(tmp_size, 64));

	return true;
}

void VDPixmapZimgResampler::Shutdown()
{
}

void VDPixmapZimgResampler::Process(const VDPixmap& dst, const VDPixmap& src)
{
	if (!mGraph) {
		return;
	}

	zimgxx::zimage_buffer srcBuffer;
	zimgxx::zimage_buffer dstBuffer;

	const unsigned mask = -1;

	switch (mFmtDesc.planes) {
	case 4:
		srcBuffer.plane[3] = { src.data4, src.pitch4, mask };
		dstBuffer.plane[3] = { dst.data4, dst.pitch4, mask };
	case 3:
		srcBuffer.plane[2] = { src.data3, src.pitch3, mask };
		dstBuffer.plane[2] = { dst.data3, dst.pitch3, mask };
	case 2:
		srcBuffer.plane[1] = { src.data2, src.pitch2, mask };
		dstBuffer.plane[1] = { dst.data2, dst.pitch2, mask };
	case 1:
		srcBuffer.plane[0] = { src.data,  src.pitch,  mask };
		dstBuffer.plane[0] = { dst.data,  dst.pitch,  mask };
		break;
	default:
		return;
	}

	mGraph.process(srcBuffer.as_const(), dstBuffer, mTempBuffer.get());
}

void VDPixmapZimgResampler::ApplyFilters(VDPixmapUberBlitterGenerator& gen, uint32 dw, uint32 dh, float xoffset, float yoffset, float xfactor, float yfactor)
{
}
