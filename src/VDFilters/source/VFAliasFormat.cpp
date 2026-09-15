// VirtualDub - Video processing and capture application
//
// Copyright (C) 1998-2007 Avery Lee
// Copyright (C) 2017 Anton Shekhovtsov
// Copyright (C) 2026 v0lt
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#include "stdafx.h"
#include <windows.h>
#include "resource.h"
#include <vd2/system/strutil.h>
#include <vd2/VDLib/Dialog.h>
#include <vd2/VDXFrame/VideoFilter.h>
#include <vd2/Kasumi/pixmaputils.h>

using namespace nsVDXPixmap;

struct VDVFilterAliasFormatConfig {
	ColorSpaceMode mColorSpaceMode = kColorSpaceMode_None;
	ColorRangeMode mColorRangeMode = kColorRangeMode_None;
	int alphaMode = -1;
	int scanMode = -1;
};

class VDVFilterAliasFormat;

class VDVFilterAliasFormatConfigDialog : public VDDialogFrameW32
{
public:
	VDVFilterAliasFormatConfigDialog(VDVFilterAliasFormatConfig& config);

	bool OnLoaded();
	void OnDataExchange(bool write);
	bool OnCommand(uint32 id, uint32 extcode);
	void redo();
	void redoFrame();

	IVDXFilterPreview2* fp = nullptr;
	VDVFilterAliasFormat* filter = nullptr;

protected:
	VDVFilterAliasFormatConfig& mConfig;
};

VDVFilterAliasFormatConfigDialog::VDVFilterAliasFormatConfigDialog(VDVFilterAliasFormatConfig& config)
	: VDDialogFrameW32(IDD_FILTER_ALIASFORMAT)
	, mConfig(config)
{
}

bool VDVFilterAliasFormatConfigDialog::OnLoaded()
{
	VDDialogFrameW32::OnLoaded();
	SetFocusToControl(IDC_STATIC_COLORSPACE);
	if (fp) {
		EnableWindow(GetDlgItem(mhdlg, IDC_PREVIEW), TRUE);
		fp->InitButton((VDXHWND)GetDlgItem(mhdlg, IDC_PREVIEW));
	}
	return true;
}

void VDVFilterAliasFormatConfigDialog::OnDataExchange(bool write)
{
	if (write) {
		if (IsButtonChecked(IDC_CS_NONE))
			mConfig.mColorSpaceMode = kColorSpaceMode_None;
		else if (IsButtonChecked(IDC_CS_601))
			mConfig.mColorSpaceMode = kColorSpaceMode_601;
		else if (IsButtonChecked(IDC_CS_709))
			mConfig.mColorSpaceMode = kColorSpaceMode_709;

		if (IsButtonChecked(IDC_CR_NONE))
			mConfig.mColorRangeMode = kColorRangeMode_None;
		else if (IsButtonChecked(IDC_CR_LIMITED))
			mConfig.mColorRangeMode = kColorRangeMode_Limited;
		else if (IsButtonChecked(IDC_CR_FULL))
			mConfig.mColorRangeMode = kColorRangeMode_Full;

		if (IsButtonChecked(IDC_ALPHA_NONE))
			mConfig.alphaMode = -1;
		else if (IsButtonChecked(IDC_ALPHA_DISABLED))
			mConfig.alphaMode = FilterModPixmapInfo::kAlphaInvalid;
		else if (IsButtonChecked(IDC_ALPHA_MASK))
			mConfig.alphaMode = FilterModPixmapInfo::kAlphaMask;
		else if (IsButtonChecked(IDC_ALPHA_OPACITY_PM))
			mConfig.alphaMode = FilterModPixmapInfo::kAlphaOpacity_pm;
		else if (IsButtonChecked(IDC_ALPHA_OPACITY))
			mConfig.alphaMode = FilterModPixmapInfo::kAlphaOpacity;

		if (IsButtonChecked(IDC_SCAN_NONE))
			mConfig.scanMode = -1;
		else if (IsButtonChecked(IDC_SCAN_PROGRESSIVE))
			mConfig.scanMode = 0;
		else if (IsButtonChecked(IDC_SCAN_INTERLACED))
			mConfig.scanMode = 1;
	}
	else {
		CheckButton(IDC_CS_NONE, mConfig.mColorSpaceMode == kColorSpaceMode_None);
		CheckButton(IDC_CS_601, mConfig.mColorSpaceMode == kColorSpaceMode_601);
		CheckButton(IDC_CS_709, mConfig.mColorSpaceMode == kColorSpaceMode_709);

		CheckButton(IDC_CR_NONE, mConfig.mColorRangeMode == kColorRangeMode_None);
		CheckButton(IDC_CR_LIMITED, mConfig.mColorRangeMode == kColorRangeMode_Limited);
		CheckButton(IDC_CR_FULL, mConfig.mColorRangeMode == kColorRangeMode_Full);

		CheckButton(IDC_ALPHA_NONE, mConfig.alphaMode == -1);
		CheckButton(IDC_ALPHA_DISABLED, mConfig.alphaMode == FilterModPixmapInfo::kAlphaInvalid);
		CheckButton(IDC_ALPHA_MASK, mConfig.alphaMode == FilterModPixmapInfo::kAlphaMask);
		CheckButton(IDC_ALPHA_OPACITY_PM, mConfig.alphaMode == FilterModPixmapInfo::kAlphaOpacity_pm);
		CheckButton(IDC_ALPHA_OPACITY, mConfig.alphaMode == FilterModPixmapInfo::kAlphaOpacity);

		CheckButton(IDC_SCAN_NONE, mConfig.scanMode ==-1);
		CheckButton(IDC_SCAN_PROGRESSIVE, mConfig.scanMode == 0);
		CheckButton(IDC_SCAN_INTERLACED, mConfig.scanMode == 1);
	}
}

bool VDVFilterAliasFormatConfigDialog::OnCommand(uint32 id, uint32 extcode)
{
	if (extcode == BN_CLICKED) {
		switch(id) {
			case IDC_CS_NONE:
			case IDC_CS_601:
			case IDC_CS_709:
			case IDC_CR_NONE:
			case IDC_CR_LIMITED:
			case IDC_CR_FULL:
			case IDC_SCAN_NONE:
			case IDC_SCAN_PROGRESSIVE:
			case IDC_SCAN_INTERLACED:
				OnDataExchange(true);
				redo();
				return TRUE;

			case IDC_ALPHA_NONE:
			case IDC_ALPHA_DISABLED:
			case IDC_ALPHA_MASK:
			case IDC_ALPHA_OPACITY_PM:
			case IDC_ALPHA_OPACITY:
				OnDataExchange(true);
				redoFrame();
				return TRUE;

			case IDC_PREVIEW:
				if (fp) fp->Toggle((VDXHWND)mhdlg);
				return TRUE;
		}
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////

class VDVFilterAliasFormat : public VDXVideoFilter
{
public:
	VDVFilterAliasFormat() = default;

	uint32 GetParams();
	void Run();

	bool Configure(VDXHWND hwnd);

	void GetSettingString(char *buf, int maxlen);
	void GetScriptString(char *buf, int maxlen);

	void ScriptConfig(IVDXScriptInterpreter *, const VDXScriptValue *argv, int argc);

	VDXVF_DECLARE_SCRIPT_METHODS();

	VDVFilterAliasFormatConfig mConfig;
};

uint32 VDVFilterAliasFormat::GetParams()
{
	using namespace nsVDXPixmap;

	VDXPixmapLayout& dstl = *fa->dst.mpPixmapLayout;
	int matrix_type = VDPixmapFormatMatrixType(dstl.format);

	if(mConfig.mColorRangeMode || mConfig.mColorSpaceMode) {
		if (matrix_type==2) {
			VDPixmapFormatEx format = dstl.format;
			format.colorSpaceMode = mConfig.mColorSpaceMode;
			format.colorRangeMode = mConfig.mColorRangeMode;
			dstl.format = VDPixmapFormatCombine(format);
		}

		if (matrix_type==1) {
			if (fma && fma->fmpixmap) {
				FilterModPixmapInfo* dst_info = fma->fmpixmap->GetPixmapInfo(fa->dst.mpPixmap);
				if (mConfig.mColorSpaceMode) dst_info->colorSpaceMode = mConfig.mColorSpaceMode;
				if (mConfig.mColorRangeMode) dst_info->colorRangeMode = mConfig.mColorRangeMode;
			}
		}
	}

	if (mConfig.scanMode == 0) { // progressive
		switch (dstl.format) {
		case kPixFormat_YUV420i_Planar:
		case kPixFormat_YUV420it_Planar:
		case kPixFormat_YUV420ib_Planar:
			dstl.format = kPixFormat_YUV420_Planar;
			break;
		case kPixFormat_YUV420i_Planar_FR:
		case kPixFormat_YUV420it_Planar_FR:
		case kPixFormat_YUV420ib_Planar_FR:
			dstl.format = kPixFormat_YUV420_Planar_FR;
			break;
		case kPixFormat_YUV420i_Planar_709:
		case kPixFormat_YUV420it_Planar_709:
		case kPixFormat_YUV420ib_Planar_709:
			dstl.format = kPixFormat_YUV420_Planar_709;
			break;
		case kPixFormat_YUV420i_Planar_709_FR:
		case kPixFormat_YUV420it_Planar_709_FR:
		case kPixFormat_YUV420ib_Planar_709_FR:
			dstl.format = kPixFormat_YUV420_Planar_709_FR;
			break;
		}
	}
	else if (mConfig.scanMode == 1) { // interlaced
		switch (dstl.format) {
		case kPixFormat_YUV420_Planar:        dstl.format = kPixFormat_YUV420i_Planar;        break;
		case kPixFormat_YUV420_Planar_FR:     dstl.format = kPixFormat_YUV420i_Planar_FR;     break;
		case kPixFormat_YUV420_Planar_709:    dstl.format = kPixFormat_YUV420i_Planar_709;    break;
		case kPixFormat_YUV420_Planar_709_FR: dstl.format = kPixFormat_YUV420i_Planar_709_FR; break;
		}
	}

	if (fa->src.mpPixmapLayout->format > nsVDPixmap::kPixFormat_Max_Standard) {
		return FILTERPARAM_NOT_SUPPORTED;
	}

	dstl.pitch = fa->src.mpPixmapLayout->pitch;

	return FILTERPARAM_SUPPORTS_ALTFORMATS | FILTERPARAM_PURE_TRANSFORM;
}

void VDVFilterAliasFormat::Run()
{
	if (fma && fma->fmpixmap) {
		FilterModPixmapInfo* dst_info = fma->fmpixmap->GetPixmapInfo(fa->dst.mpPixmap);
		if (mConfig.alphaMode != -1 && VDPixmapFormatHasAlpha(fa->dst.mpPixmap->format)) {
			dst_info->alpha_type = mConfig.alphaMode;
		}
	}
}

void VDVFilterAliasFormatConfigDialog::redo()
{
	filter->mConfig = mConfig;
	if (fp) fp->RedoSystem();
}

void VDVFilterAliasFormatConfigDialog::redoFrame()
{
	filter->mConfig = mConfig;
	if (fp) fp->RedoFrame();
}

bool VDVFilterAliasFormat::Configure(VDXHWND hwnd)
{
	VDVFilterAliasFormatConfigDialog dlg(mConfig);
	dlg.fp = fa->ifp2;
	dlg.filter = this;
	VDVFilterAliasFormatConfig oldConfig = mConfig;

	if (!dlg.ShowDialog((VDGUIHandle)hwnd)) {
		mConfig = oldConfig;
		return false;
	}

	return true;
}

void VDVFilterAliasFormat::GetSettingString(char *buf, int maxlen)
{
	static const char *kColorMode[]={
		"same",
		"601",
		"709",
	};
	static const char *kRangeMode[]={
		"same",
		"limited",
		"full"
	};
	static const char* kAlphaMode[] = {
		"no alpha",
		"alpha mask",
		"premultiplied alpha",
		"straight alpha",
	};
	static const char* kScanMode[] = {
		"progressive",
		"interlaced",
	};

	SafePrintf(buf, maxlen, " (%s, %s, %s, %s)",
		kColorMode[mConfig.mColorSpaceMode],
		kRangeMode[mConfig.mColorRangeMode],
		mConfig.alphaMode == -1 ? "same" : kAlphaMode[mConfig.alphaMode],
		mConfig.scanMode == -1 ? "same" : kScanMode[mConfig.scanMode]
	);
}

void VDVFilterAliasFormat::GetScriptString(char *buf, int maxlen)
{
	if (mConfig.scanMode != -1) {
		SafePrintf(buf, maxlen, "Config(%d, %d, %d, %d)",
			mConfig.mColorSpaceMode, mConfig.mColorRangeMode, mConfig.alphaMode, mConfig.scanMode);
	}
	else if (mConfig.alphaMode != -1) {
		SafePrintf(buf, maxlen, "Config(%d, %d, %d)",
			mConfig.mColorSpaceMode, mConfig.mColorRangeMode, mConfig.alphaMode);
	}
	else {
		SafePrintf(buf, maxlen, "Config(%d, %d)", mConfig.mColorSpaceMode, mConfig.mColorRangeMode);
	}
}

void VDVFilterAliasFormat::ScriptConfig(IVDXScriptInterpreter *isi, const VDXScriptValue *argv, int argc)
{
	int colorMode = argv[0].asInt();
	int levelMode = argv[1].asInt();
	int alphaMode = (argc > 2) ? argv[2].asInt() : -1;
	int scanMode  = (argc > 3) ? argv[3].asInt() : -1;

	if (colorMode < 0 || colorMode >= kColorSpaceModeCount) {
		isi->ScriptError(VDXScriptError::FCALL_OUT_OF_RANGE);
	}
	if (levelMode < 0 || levelMode >= kColorRangeModeCount) {
		isi->ScriptError(VDXScriptError::FCALL_OUT_OF_RANGE);
	}
	if (alphaMode < -1 || alphaMode > FilterModPixmapInfo::kAlphaOpacity) {
		isi->ScriptError(VDXScriptError::FCALL_OUT_OF_RANGE);
	}
	if (scanMode < -1 || scanMode > 1) {
		isi->ScriptError(VDXScriptError::FCALL_OUT_OF_RANGE);
	}

	mConfig.mColorSpaceMode = (ColorSpaceMode)colorMode;
	mConfig.mColorRangeMode = (ColorRangeMode)levelMode;
	mConfig.alphaMode = alphaMode;
	mConfig.scanMode  = scanMode;
}

VDXVF_BEGIN_SCRIPT_METHODS(VDVFilterAliasFormat)
VDXVF_DEFINE_SCRIPT_METHOD(VDVFilterAliasFormat, ScriptConfig, "ii")
VDXVF_DEFINE_SCRIPT_METHOD(VDVFilterAliasFormat, ScriptConfig, "iii")
VDXVF_DEFINE_SCRIPT_METHOD(VDVFilterAliasFormat, ScriptConfig, "iiii")
VDXVF_END_SCRIPT_METHODS()

extern const VDXFilterDefinition2 g_VDVFAliasFormat = VDXVideoFilterDefinition<VDVFilterAliasFormat>(NULL, "alias format", "Relabel video with a different color space or color encoding without changing video data.");

// warning C4505: 'VDXVideoFilter::[thunk]: __thiscall VDXVideoFilter::`vcall'{24,{flat}}' }'' : unreferenced local function has been removed
#pragma warning(disable: 4505)
