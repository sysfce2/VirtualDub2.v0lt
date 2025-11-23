// VirtualDub - Video processing and capture application
//
// Copyright (C) 1998-2003 Avery Lee
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#ifndef f_AF_BASE_H
#define f_AF_BASE_H

#include <vector>
#include <string>

#include <vd2/plugin/vdplugin.h>
#include <vd2/plugin/vdaudiofilt.h>

namespace nsVDAudioFilterBase {
	typedef uint32				Type_U32;
	typedef uint64				Type_U64;
	typedef sint32				Type_S32;
	typedef sint64				Type_S64;
	typedef double				Type_Double;
	typedef std::string			Type_AStr;
	typedef	std::wstring		Type_WStr;
	typedef	std::vector<char>	Type_Block;

	struct ConfigEntryExt {
		VDPluginConfigEntry info;
		ptrdiff_t objoffset;
	};
}

#define VDAFBASE_BEGIN_CONFIG(filtername_)					\
	namespace nsVDAFConfigInfo_##filtername_ {				\
		template<int n> struct ConfigInfo { static const nsVDAudioFilterBase::ConfigEntryExt members; };	\
		template<int n> const nsVDAudioFilterBase::ConfigEntryExt ConfigInfo<n>::members = {0}

#define VDAFBASE_STRUCT_ENTRY(filtername_, idx_, type_, name_, sdesc_, ldesc_)						\

#define VDAFBASE_CONFIG_ENTRY(filtername_, idx_, type_, name_, sdesc_, ldesc_)						\
		template<> struct ConfigInfo<idx_>;										\
		template<> struct ConfigInfo<idx_> : public ConfigInfo<idx_-1> {		\
			nsVDAudioFilterBase::Type_##type_ name_;							\
			static const nsVDAudioFilterBase::ConfigEntryExt members;			\
		};																		\
																				\
		const nsVDAudioFilterBase::ConfigEntryExt ConfigInfo<idx_>::members={ &ConfigInfo<idx_-1>::members.info, idx_, VDPluginConfigEntry::kType##type_, L#name_, sdesc_, ldesc_, offsetof(ConfigInfo<idx_>, name_) }
		
#define VDAFBASE_END_CONFIG(filtername_, idx_)		\
		}											\
		typedef nsVDAFConfigInfo_##filtername_::ConfigInfo<idx_> VDAudioFilterData_##filtername_


class VDAudioFilterBase {
public:
	VDAudioFilterBase() {}
	virtual ~VDAudioFilterBase() {}

	virtual uint32 Run();
	virtual sint64 Seek(sint64 microsecs);
	virtual uint32 Prepare();
	virtual void Start();
	virtual void Stop();
	virtual unsigned Suspend(void *dst, unsigned size);
	virtual void Resume(const void *src, unsigned size);
	virtual unsigned GetParam(unsigned idx, void *dst, unsigned size);
	virtual void SetParam(unsigned idx, const void *src, unsigned size);
	virtual bool Config(struct HWND__ *hwnd);
	virtual uint32 Read(unsigned pin, void *dst, uint32 samples);

	//////////////////////

	virtual void *GetConfigPtr() { return 0; }

	//////////////////////

	static void		VDAPIENTRY DestroyProc				(const VDAudioFilterContext *pContext);
	static uint32	VDAPIENTRY RunProc					(const VDAudioFilterContext *pContext);
	static sint64	VDAPIENTRY SeekProc				(const VDAudioFilterContext *pContext, sint64 microsecs);
	static uint32	VDAPIENTRY PrepareProc				(const VDAudioFilterContext *pContext);
	static void		VDAPIENTRY StartProc				(const VDAudioFilterContext *pContext);
	static void		VDAPIENTRY StopProc				(const VDAudioFilterContext *pContext);
	static unsigned	VDAPIENTRY SuspendProc				(const VDAudioFilterContext *pContext, void *dst, unsigned size);
	static void		VDAPIENTRY ResumeProc				(const VDAudioFilterContext *pContext, const void *src, unsigned size);
	static unsigned	VDAPIENTRY GetParamProc			(const VDAudioFilterContext *pContext, unsigned idx, void *dst, unsigned size);
	static void		VDAPIENTRY SetParamProc			(const VDAudioFilterContext *pContext, unsigned idx, const void *src, unsigned size);
	static bool		VDAPIENTRY ConfigProc				(const VDAudioFilterContext *pContext, struct HWND__ *hwnd);

	static const VDAudioFilterVtbl sVtbl;

protected:
	const nsVDAudioFilterBase::ConfigEntryExt *GetParamEntry(const unsigned idx);

	const VDAudioFilterContext		*mpContext;
};

#endif
