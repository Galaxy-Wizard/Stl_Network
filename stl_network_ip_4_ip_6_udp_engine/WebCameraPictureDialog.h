#pragma once

#include <atlbase.h>

#include <atlimage.h>

#include <Dshow.h>
#pragma comment(lib, "Strmiids.lib")

#include "Qedit.h"
#include "afxwin.h"

class Cstl_network_ip_4_ip_6_udp_engineDialog;

// диалоговое окно CWebCameraPicture

class CWebCameraPictureDialog : public CDialogEx, public ISampleGrabberCB
{
	DECLARE_DYNAMIC(CWebCameraPictureDialog)

public:
	CWebCameraPictureDialog(CWnd* pParent = NULL);   // стандартный конструктор
	virtual ~CWebCameraPictureDialog();

	// Данные диалогового окна
	enum { IDD = IDD_DIALOG_WEB_CAMERA_PICTURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	void init_capture(HWND parameter_hwnd);

	CComPtr<IGraphBuilder> pGraph; //менеджер графа фильтров
	CComPtr<ICaptureGraphBuilder2> pBuild; //граф фильтра аудио\видео захвата

	CComPtr<IBaseFilter> pVideoCapture; //видеофильтр
	CComPtr<IBaseFilter> pVideoRenderer; //видеофильтр

	CComPtr<IBaseFilter> pAudioCapture; //аудиофильтр
	CComPtr<IBaseFilter> pAudioRenderer; //аудиофильтр

	CComPtr<ICreateDevEnum> pDevEnum;
	CComPtr<IEnumMoniker> pEnum;
	CComPtr<IEnumMoniker> pAudioEnum;
	CComPtr<IMoniker> pVideoMoniker;
	CComPtr<IMoniker> pAudioMoniker;

	CComPtr<IMediaControl> pMediaControl;
	CComPtr<IMediaEventEx> pEvent;

	CComPtr<IBaseFilter> pVideoGrabberF;
	CComPtr<ISampleGrabber> pVideoGrabber;

	CComPtr<IBaseFilter> pAudioGrabberF;
	CComPtr<ISampleGrabber> pAudioGrabber;
	CImage web_camera_image;

	//	ISampleGrabberCB methods
	STDMETHODIMP SampleCB( double SampleTime, IMediaSample *pSample );
	STDMETHODIMP BufferCB( double SampleTime, BYTE *pBuffer, long BufferLen );
	//	Captured Audio Data List
	struct Captured_Audio_Data_List_ITEM
	{
		IStream *audio_stream;
		double SampleTime;
	};

	std::list<Captured_Audio_Data_List_ITEM> audio_captured_fragments_list_ip_4;

	std::list<Captured_Audio_Data_List_ITEM> audio_captured_fragments_list_ip_6;

	CCriticalSection audio_captured_fragments_list_critical_section;

	HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject);

	ULONG STDMETHODCALLTYPE AddRef( void);

	ULONG STDMETHODCALLTYPE Release( void);

	template<class Q>
	HRESULT
		STDMETHODCALLTYPE
		QueryInterface(_COM_Outptr_ Q** pp)
	{
		return QueryInterface(__uuidof(Q), (void **)pp);
	}


	afx_msg void OnClose();

	void StopMedia()
	{
		HRESULT hr = S_OK;

		if (SUCCEEDED(hr) && pAudioGrabber!=NULL)
		{
			hr = pAudioGrabber->SetCallback(NULL,1);
		}
																						
		if (SUCCEEDED(hr) && pMediaControl!=NULL)
		{
			hr = pMediaControl->Stop();
		}
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *main_dialog;
	CStatic window_direct_show_to_paint;
};

void CaptureWndShot(HWND hwnd, CImage *parameter_image, int width=0, int height=0);
HRESULT SaveGraphFile(IGraphBuilder *pGraph, WCHAR *wszPath) ;

void CaptureVideoSampleShot(CWebCameraPictureDialog *local_web_camera_dialog, CImage *parameter_image);
void CaptureVideoSampleShotIntoStream(CWebCameraPictureDialog *local_web_camera_dialog, IStream **parameter_stream);

void CaptureAudioSampleShot(CWebCameraPictureDialog *local_web_camera_dialog, IStream *parameter_audio_image);
void CaptureAudioSampleGetFromTheList_ip_4(CWebCameraPictureDialog *local_web_camera_dialog, IStream **parameter_audio_image);
void CaptureAudioSampleGetFromTheList_ip_6(CWebCameraPictureDialog *local_web_camera_dialog, IStream **parameter_audio_image);


void _FreeMediaType(AM_MEDIA_TYPE& mt);
