#ifndef f_VDREMOTE_VDREMOTE_H
#define f_VDREMOTE_VDREMOTE_H

#include <vfw.h>

#include "vdserver.h"

class CAVIFileRemote;

class CAVIFileRemote: public IAVIFile, public IPersistFile, public IClassFactory {
private:
	long m_refs;

	char *szPath;
	IVDubServerLink *ivdsl;
	BOOL fHasAudio;
	BITMAPINFOHEADER *vFormat;
	LONG vFormatLen, vSampleFirst, vSampleLast;
	AVISTREAMINFO vStreamInfo;
	WAVEFORMATEX *aFormat;
	LONG aFormatLen, aSampleFirst, aSampleLast;
	AVISTREAMINFO aStreamInfo;
	PAVIFILE pafTunnel;

	CRITICAL_SECTION csPort;

public:
	IVDubAnimConnection *ivdac;
	static long gRefCnt;

	CAVIFileRemote(const CLSID& rclsid);
	~CAVIFileRemote();

	static HRESULT Create(const CLSID& rclsid, const IID& riid, void **ppv);

	// IUnknown
	STDMETHODIMP QueryInterface(const IID& iid, void **ppv) override;
	STDMETHODIMP_(ULONG) AddRef() override;
	STDMETHODIMP_(ULONG) Release() override;

	// IClassFactory
	STDMETHODIMP CreateInstance (LPUNKNOWN pUnkOuter, REFIID riid,  void * * ppvObj) override;
	STDMETHODIMP LockServer (BOOL fLock) override;

	// IPersist
	STDMETHODIMP GetClassID(LPCLSID lpClassID) override;

	// IPersistFile
	STDMETHODIMP IsDirty();
	STDMETHODIMP Load(LPCOLESTR lpszFileName, DWORD grfMode) override;
	STDMETHODIMP Save(LPCOLESTR lpszFileName, BOOL fRemember) override;
	STDMETHODIMP SaveCompleted(LPCOLESTR lpszFileName) override;
	STDMETHODIMP GetCurFile(LPOLESTR *lplpszFileName) override;

	// IAVIFile
	STDMETHODIMP CreateStream(PAVISTREAM *ppStream, AVISTREAMINFOW *psi) override;
	STDMETHODIMP EndRecord() override;
	STDMETHODIMP GetStream(PAVISTREAM *ppStream, DWORD fccType, LONG lParam) override;
	STDMETHODIMP Info(AVIFILEINFOW *psi, LONG lSize) override;
	STDMETHODIMP ReadData(DWORD fcc, LPVOID lp, LONG *lpcb) override;
	STDMETHODIMP WriteData(DWORD fcc, LPVOID lpBuffer, LONG cbBuffer) override;

	STDMETHODIMP Open(LPCSTR szFile, UINT mode, LPCOLESTR lpszFileName);
	STDMETHODIMP Save(LPCSTR szFile, AVICOMPRESSOPTIONS FAR *lpOptions,
				AVISAVECALLBACK lpfnCallback);
	STDMETHODIMP DeleteStream(DWORD fccType, LONG lParam);

	void LockPort();
	void UnlockPort();

};

///////////////////////////////////

class CAVIStreamRemote;

class CAVIStreamClassFactory: public IClassFactory {
private:
	CAVIStreamRemote *avifile;

public:
	CAVIStreamClassFactory(CAVIStreamRemote *af);

	STDMETHODIMP QueryInterface (REFIID riid, void * * ppvObj) ;
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	STDMETHODIMP CreateInstance (LPUNKNOWN pUnkOuter, REFIID riid,  void * * ppvObj) ;
	STDMETHODIMP LockServer (BOOL fLock) ;
};

class CAVIStreamRemote: public IAVIStream {
private:
	CAVIStreamClassFactory iclassfactory;

	long m_refs;

	CAVIFileRemote *parent;
	AVISTREAMINFO *streamInfo;
	WAVEFORMATEX *wfexFormat;
	BITMAPINFOHEADER *bmihFormat;
	LONG lFormatLen, lSampleFirst, lSampleLast;
	BOOL fAudio;

public:
	static long gRefCnt;

	static HRESULT Create(const CLSID& rclsid, const IID& riid, void **ppv);
	STDMETHODIMP QueryInterface(const IID& iid, void **ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	CAVIStreamRemote(const CLSID& rclsid, void **pUnknown);
	CAVIStreamRemote(CAVIFileRemote *parentPtr, BOOL isAudio, AVISTREAMINFO *asi, void *format, long format_len, long sample_first, long sample_last);
	~CAVIStreamRemote();

	STDMETHODIMP Create(LPARAM lParam1, LPARAM lParam2);
	STDMETHODIMP Delete(LONG lStart, LONG lSamples);
	STDMETHODIMP_(LONG) Info(AVISTREAMINFOW *psi, LONG lSize);
	STDMETHODIMP_(LONG) FindSample(LONG lPos, LONG lFlags);
	STDMETHODIMP Read(LONG lStart, LONG lSamples, LPVOID lpBuffer, LONG cbBuffer, LONG *plBytes, LONG *plSamples);
	STDMETHODIMP ReadData(DWORD fcc, LPVOID lp, LONG *lpcb);
	STDMETHODIMP ReadFormat(LONG lPos, LPVOID lpFormat, LONG *lpcbFormat);
	STDMETHODIMP SetFormat(LONG lPos, LPVOID lpFormat, LONG cbFormat);
	STDMETHODIMP Write(LONG lStart, LONG lSamples, LPVOID lpBuffer,
		LONG cbBuffer, DWORD dwFlags, LONG FAR *plSampWritten, 
		LONG FAR *plBytesWritten);
	STDMETHODIMP WriteData(DWORD fcc, LPVOID lpBuffer, LONG cbBuffer);
	STDMETHODIMP SetInfo(AVISTREAMINFOW *psi, LONG lSize);
};

#endif


