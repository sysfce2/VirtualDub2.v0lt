#ifndef f_VDREMOTE_VDREMOTE_H
#define f_VDREMOTE_VDREMOTE_H

#include <vfw.h>

#include "vdserver.h"

class CAVIFileRemote;

class CAVIFileRemote : public IAVIFile, public IPersistFile, public IClassFactory {
private:
	long m_refs;

	char* szPath = nullptr;
	IVDubServerLink* ivdsl = nullptr;
	BOOL fHasAudio;
	BITMAPINFOHEADER* vFormat = nullptr;
	LONG vFormatLen, vSampleFirst, vSampleLast;
	AVISTREAMINFOW vStreamInfo;
	WAVEFORMATEX* aFormat = nullptr;
	LONG aFormatLen, aSampleFirst, aSampleLast;
	AVISTREAMINFOW aStreamInfo;
	PAVIFILE pafTunnel = nullptr;

	CRITICAL_SECTION csPort;

public:
	IVDubAnimConnection* ivdac = nullptr;
	static long gRefCnt;

	CAVIFileRemote(const CLSID& rclsid);
	~CAVIFileRemote();

	static HRESULT Create(const CLSID& rclsid, const IID& riid, void** ppv);

	// IUnknown
	STDMETHODIMP QueryInterface(const IID& iid, void** ppv) override;
	STDMETHODIMP_(ULONG) AddRef() override;
	STDMETHODIMP_(ULONG) Release() override;

	// IClassFactory
	STDMETHODIMP CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, void** ppvObj) override;
	STDMETHODIMP LockServer(BOOL fLock) override;

	// IPersist
	STDMETHODIMP GetClassID(LPCLSID lpClassID) override;

	// IPersistFile
	STDMETHODIMP IsDirty();
	STDMETHODIMP Load(LPCOLESTR lpszFileName, DWORD grfMode) override;
	STDMETHODIMP Save(LPCOLESTR lpszFileName, BOOL fRemember) override;
	STDMETHODIMP SaveCompleted(LPCOLESTR lpszFileName) override;
	STDMETHODIMP GetCurFile(LPOLESTR* lplpszFileName) override;

	// IAVIFile
	STDMETHODIMP CreateStream(PAVISTREAM* ppStream, AVISTREAMINFOW* psi) override;
	STDMETHODIMP EndRecord() override;
	STDMETHODIMP GetStream(PAVISTREAM* ppStream, DWORD fccType, LONG lParam) override;
	STDMETHODIMP Info(AVIFILEINFOW* psi, LONG lSize) override;
	STDMETHODIMP ReadData(DWORD fcc, LPVOID lp, LONG* lpcb) override;
	STDMETHODIMP WriteData(DWORD fcc, LPVOID lpBuffer, LONG cbBuffer) override;
	STDMETHODIMP DeleteStream(DWORD fccType, LONG lParam) override;

	STDMETHODIMP Open(LPCSTR szFile, UINT mode, LPCOLESTR lpszFileName);
	STDMETHODIMP Save(LPCSTR szFile, AVICOMPRESSOPTIONS FAR* lpOptions, AVISAVECALLBACK lpfnCallback);

	void LockPort();
	void UnlockPort();
};

///////////////////////////////////

class CAVIStreamRemote;

class CAVIStreamClassFactory: public IClassFactory {
private:
	CAVIStreamRemote* avifile;

public:
	CAVIStreamClassFactory(CAVIStreamRemote *af);

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObj) override;
	STDMETHODIMP_(ULONG) AddRef() override;
	STDMETHODIMP_(ULONG) Release() override;

	// IClassFactory
	STDMETHODIMP CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, void** ppvObj) override;
	STDMETHODIMP LockServer(BOOL fLock) override;
};

class CAVIStreamRemote : public IAVIStream {
private:
	CAVIStreamClassFactory iclassfactory;

	long m_refs;

	CAVIFileRemote* parent = nullptr;
	AVISTREAMINFO* streamInfo = nullptr;
	WAVEFORMATEX* wfexFormat = nullptr;
	BITMAPINFOHEADER* bmihFormat = nullptr;
	LONG lFormatLen, lSampleFirst, lSampleLast;
	BOOL fAudio;

public:
	static long gRefCnt;

	static HRESULT Create(const CLSID& rclsid, const IID& riid, void** ppv);

	// IUnknown
	STDMETHODIMP QueryInterface(const IID& iid, void** ppv) override;
	STDMETHODIMP_(ULONG) AddRef() override;
	STDMETHODIMP_(ULONG) Release() override;

	CAVIStreamRemote(const CLSID& rclsid, void** pUnknown);
	CAVIStreamRemote(CAVIFileRemote* parentPtr, BOOL isAudio, AVISTREAMINFO* asi, void* format, long format_len, long sample_first, long sample_last);
	~CAVIStreamRemote();

	// IAVIStream
	STDMETHODIMP Create(LPARAM lParam1, LPARAM lParam2) override;
	STDMETHODIMP Delete(LONG lStart, LONG lSamples) override;
	STDMETHODIMP_(LONG) Info(AVISTREAMINFOW* psi, LONG lSize) override;
	STDMETHODIMP_(LONG) FindSample(LONG lPos, LONG lFlags) override;
	STDMETHODIMP Read(LONG lStart, LONG lSamples, LPVOID lpBuffer,
		LONG cbBuffer, LONG* plBytes, LONG* plSamples) override;
	STDMETHODIMP ReadData(DWORD fcc, LPVOID lp, LONG* lpcb) override;
	STDMETHODIMP ReadFormat(LONG lPos, LPVOID lpFormat, LONG* lpcbFormat) override;
	STDMETHODIMP SetFormat(LONG lPos, LPVOID lpFormat, LONG cbFormat) override;
	STDMETHODIMP Write(LONG lStart, LONG lSamples, LPVOID lpBuffer,
		LONG cbBuffer, DWORD dwFlags, LONG FAR* plSampWritten,
		LONG FAR* plBytesWritten) override;
	STDMETHODIMP WriteData(DWORD fcc, LPVOID lpBuffer, LONG cbBuffer) override;
	STDMETHODIMP SetInfo(AVISTREAMINFOW* psi, LONG lSize) override;
};

#endif
