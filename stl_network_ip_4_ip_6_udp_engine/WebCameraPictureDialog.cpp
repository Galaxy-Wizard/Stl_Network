// WebCameraPicture.cpp: файл реализации
//

#include "stdafx.h"
#include "stl_network_ip_4_ip_6_udp_engine.h"
#include "WebCameraPictureDialog.h"
#include "afxdialogex.h"

#include "stl_network_ip_4_ip_6_udp_engineDialog.h"
#include "Wave.h"

// Release the format block for a media type.

void _FreeMediaType(AM_MEDIA_TYPE& mt)
{
	if (mt.cbFormat != 0)
	{
		CoTaskMemFree((PVOID)mt.pbFormat);
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL)
	{
		// pUnk should not be used.
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}

// диалоговое окно CWebCameraPicture

IMPLEMENT_DYNAMIC(CWebCameraPictureDialog, CDialogEx)

	CWebCameraPictureDialog::CWebCameraPictureDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWebCameraPictureDialog::IDD, pParent)
{
	main_dialog = NULL;
}

CWebCameraPictureDialog::~CWebCameraPictureDialog()
{
}

void CWebCameraPictureDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_WINDOW_DIRECTSHOW_TO_PAINT, window_direct_show_to_paint);
}


BEGIN_MESSAGE_MAP(CWebCameraPictureDialog, CDialogEx)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CWebCameraPictureDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: добавьте дополнительную инициализацию

	CRect rcClient(0,0,ENGINE_WINDOWS_SIZE_CX/2,ENGINE_WINDOWS_SIZE_CY/2);

	if
		(
		AdjustWindowRect(&rcClient, DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION, FALSE)
		==
		TRUE
		)
	{
		if(
			SetWindowPos(NULL, 0, ENGINE_WINDOWS_SIZE_CY+100, rcClient.Width(), rcClient.Height(), SWP_NOZORDER | SWP_NOREDRAW)
			==
			TRUE
			)
		{
			CRect static_client(0,0,ENGINE_WINDOWS_SIZE_CX/2,ENGINE_WINDOWS_SIZE_CY/2);

			if(
				window_direct_show_to_paint.SetWindowPos(NULL, static_client.left, static_client.top, static_client.Width(), static_client.Height(), SWP_NOZORDER | SWP_NOREDRAW)
				==
				TRUE
				)
			{
				window_direct_show_to_paint.SetWindowTextW(CString());
				CString dummy_string("success");
				RedrawWindow();
			}
		}
	}

	HRESULT hr = S_OK;

	//Инициализируем библиотеку COM
	//	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	if(SUCCEEDED(hr))
	{
		init_capture(window_direct_show_to_paint.m_hWnd);
	}

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

HRESULT SaveGraphFile(IGraphBuilder *pGraph, WCHAR *wszPath) 
{
	const WCHAR wszStreamName[] = L"ActiveMovieGraph"; 
	HRESULT hr;

	IStorage *pStorage = NULL;
	hr = StgCreateDocfile(
		wszPath,
		STGM_CREATE | STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
		0, &pStorage);
	if(FAILED(hr)) 
	{
		return hr;
	}

	IStream *pStream;
	hr = pStorage->CreateStream(
		wszStreamName,
		STGM_WRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE,
		0, 0, &pStream);
	if (FAILED(hr)) 
	{
		pStorage->Release();    
		return hr;
	}

	IPersistStream *pPersist = NULL;
	pGraph->QueryInterface(IID_IPersistStream, (void**)&pPersist);
	hr = pPersist->Save(pStream, TRUE);
	pStream->Release();
	pPersist->Release();
	if (SUCCEEDED(hr)) 
	{
		hr = pStorage->Commit(STGC_DEFAULT);
	}
	pStorage->Release();
	return hr;
}

void CaptureWndShot(HWND hwnd, CImage *parameter_image, int width, int height)
{
	if(parameter_image==NULL)
	{
		return;
	}

	HWND active_window_handle = ::GetActiveWindow();
	HWND desktop_window_handle = ::GetDesktopWindow();

	CWnd active_window;

	if(active_window_handle!=NULL)
	{
		active_window.Attach(active_window_handle);
	}


	CWnd desktop_window;
	desktop_window.Attach(desktop_window_handle);

	if(active_window_handle!=NULL)
	{
		desktop_window.SetFocus();
	}

	{
		HDC scrdc, memdc;
		HBITMAP membit;
		scrdc = ::GetDC(hwnd);
		RECT local_rect;
		::GetWindowRect(hwnd, &local_rect);
		int Width = local_rect.right-local_rect.left;
		int Height = local_rect.bottom-local_rect.top;

		if(width!=0)
		{
			Width = width;
		}
		if(height!=0)
		{
			Height = height;
		}
		memdc = CreateCompatibleDC(scrdc);
		membit = CreateCompatibleBitmap(scrdc, Width, Height);
		HBITMAP hOldBitmap =(HBITMAP) SelectObject(memdc, membit);
		BitBlt(memdc, 0, 0, Width, Height, scrdc, 0, 0, SRCCOPY);

		//		parameter_image->Attach(membit);
		//		parameter_image->Save(CString("c:\\temp\\CaptureWndShot.png"));

		parameter_image->Attach(membit);

		SelectObject(memdc, hOldBitmap);
		DeleteObject(memdc);
		//		DeleteObject(membit);
		::ReleaseDC(0,scrdc);
	}

	if(active_window_handle!=NULL)
	{
		active_window.SetFocus();

		active_window.Detach();
	}
}

// обработчики сообщений CWebCameraPicture
void CWebCameraPictureDialog::init_capture(HWND parameter_hwnd)
{
	HRESULT hr;

	{
		//Создание графа-фильтров - Create the Capture Graph Builder
		hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&pBuild );
		if (SUCCEEDED(hr) && pBuild!=NULL)
		{
			//Создание менеджера графа фильтров - Create the Filter Graph Manager
			hr = CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pGraph);
			if (SUCCEEDED(hr) && pGraph!=NULL)
			{
				//Initialize the Capture Graph Builder
				pBuild->SetFiltergraph(pGraph);

				// Find system device enumerator to find a video capture device.
				hr = CoCreateInstance(CLSID_SystemDeviceEnum, 0, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (LPVOID*)&pDevEnum);

				if(SUCCEEDED(hr) && pDevEnum!=NULL)
				{

					//создает счетчик для определенной категории устройств
					hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);

					if(SUCCEEDED(hr) && pEnum!=NULL)
					{
						if( pEnum->Next(1, &pVideoMoniker,0) == S_OK && pVideoMoniker!=NULL)
						{
							hr = pVideoMoniker->BindToObject(0, 0, IID_IBaseFilter, (void **)&pVideoCapture);

							if(SUCCEEDED(hr) && pVideoCapture!=NULL)
							{
								pGraph->AddFilter(pVideoCapture, L"Video Capture Filter");


								hr = CoCreateInstance(CLSID_VideoRenderer, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pVideoRenderer);
								if (SUCCEEDED(hr) && pVideoRenderer!=NULL)
								{
									pGraph->AddFilter(pVideoRenderer, L"Video Renderer Filter");

									if (SUCCEEDED(hr))
									{
										// Create the Sample Grabber filter.
										hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
											IID_PPV_ARGS(&pVideoGrabberF));
										if (SUCCEEDED(hr) && pVideoGrabberF!=NULL)
										{
											hr = pGraph->AddFilter(pVideoGrabberF, L"Video Sample Grabber");
											if (SUCCEEDED(hr))
											{
												hr = pVideoGrabberF->QueryInterface(IID_ISampleGrabber, (void**)&pVideoGrabber);

												if (SUCCEEDED(hr) && pVideoGrabber!=NULL)
												{
													AM_MEDIA_TYPE mt;
													ZeroMemory(&mt, sizeof(mt));
													mt.majortype = MEDIATYPE_Video;
													mt.subtype = MEDIASUBTYPE_RGB32;

													hr = pVideoGrabber->SetMediaType(&mt);

													if (SUCCEEDED(hr))
													{
														hr = pVideoGrabber->SetOneShot(FALSE);
														if (SUCCEEDED(hr))
														{

															hr = pVideoGrabber->SetBufferSamples(TRUE);
															if (SUCCEEDED(hr))
															{

																hr = pBuild->RenderStream(/*&PIN_CATEGORY_PREVIEW*/NULL, NULL/*&MEDIATYPE_Video*/, pVideoCapture, pVideoGrabberF, pVideoRenderer);  //3,4,5 Параметры - фильтры, которые объединены в граф

																if (SUCCEEDED(hr))
																{
																	// Specify the owner window.
																	CComPtr<IVideoWindow> pVidWin;
																	OAHWND hWnd = (OAHWND)parameter_hwnd;

																	pGraph->QueryInterface(IID_IVideoWindow, (void **)&pVidWin);

																	pVidWin->put_Owner((OAHWND)hWnd);
																	pVidWin->put_WindowStyle( WS_CHILD | WS_CLIPSIBLINGS );

																	pVidWin->put_Left(0);
																	pVidWin->put_Top(0);
																	pVidWin->put_Width(ENGINE_WINDOWS_SIZE_CX/2);
																	pVidWin->put_Height(ENGINE_WINDOWS_SIZE_CY/2);

																	// Set the owner window to receive event notices.
																	pGraph->QueryInterface(IID_IMediaEventEx, (void **)&pEvent);
																	pEvent->SetNotifyWindow((OAHWND)hWnd, 0, 0);
																}
															}
														}
													}
													_FreeMediaType(mt);
												}
											}
										}
									}
								}
							}
						}
					}


					hr = pDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, &pAudioEnum, 0);

					if(SUCCEEDED(hr) && pAudioEnum!=NULL)
					{
						if( pAudioEnum->Next(1, &pAudioMoniker,0) == S_OK && pAudioMoniker!=NULL)
						{
							hr = pAudioMoniker->BindToObject(0, 0, IID_IBaseFilter, (void **)&pAudioCapture);

							if(SUCCEEDED(hr) && pAudioCapture!=NULL)
							{
								pGraph->AddFilter(pAudioCapture, L"Audio Capture Filter");

								hr = CoCreateInstance(CLSID_NullRenderer, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pAudioRenderer);
								//hr = CoCreateInstance(CLSID_AudioRender, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pAudioRenderer);
								if (SUCCEEDED(hr) && pAudioRenderer!=NULL)
								{
									pGraph->AddFilter(pAudioRenderer, L"Audio Renderer Filter");

									// Create the Sample Grabber filter.
									hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
										IID_PPV_ARGS(&pAudioGrabberF));
									if (SUCCEEDED(hr) && pAudioGrabberF!=NULL)
									{
										hr = pGraph->AddFilter(pAudioGrabberF, L"Audio Sample Grabber");
										if (SUCCEEDED(hr))
										{
											hr = pAudioGrabberF->QueryInterface(IID_ISampleGrabber, (void**)&pAudioGrabber);

											if (SUCCEEDED(hr) && pAudioGrabber!=NULL)
											{
												AM_MEDIA_TYPE mt;
												ZeroMemory(&mt, sizeof(mt));
												mt.majortype = MEDIATYPE_Audio;
												mt.subtype = MEDIASUBTYPE_PCM;

												mt.pbFormat = (LPBYTE)CoTaskMemAlloc(sizeof(WAVEFORMATEX));
												if(mt.pbFormat!=NULL)
												{
													mt.formattype = FORMAT_WaveFormatEx;
													mt.cbFormat = sizeof(WAVEFORMATEX);

													WAVEFORMATEX *wave_format_ex = (WAVEFORMATEX *)mt.pbFormat;

													wave_format_ex->nChannels = 1;
													wave_format_ex->wBitsPerSample = 16;
													wave_format_ex->wFormatTag = WAVE_FORMAT_PCM;
													wave_format_ex->nSamplesPerSec = 22050;
													wave_format_ex->nAvgBytesPerSec = 44100;
													wave_format_ex->nBlockAlign = 2;
													wave_format_ex->cbSize = 0;
												}

												hr = pAudioGrabber->SetMediaType(&mt);

												if (SUCCEEDED(hr))
												{
													hr = pAudioGrabber->SetOneShot(FALSE);
													if (SUCCEEDED(hr))
													{

														hr = pAudioGrabber->SetBufferSamples(TRUE);
														if (SUCCEEDED(hr))
														{
															hr = pAudioGrabber->SetCallback(this,1);
															if (SUCCEEDED(hr))
															{
																hr = pBuild->RenderStream(/*&PIN_CATEGORY_PREVIEW*/NULL, NULL/*&MEDIATYPE_Video*/, pAudioCapture, pAudioGrabberF, pAudioRenderer);  //3,4,5 Параметры - фильтры, которые объединены в граф
															}
														}
													}
												}
												_FreeMediaType(mt);
											}
										}
									}
								}
							}
						}
					}

					// Run the graph.
					pGraph->QueryInterface(IID_IMediaControl, (void **)&pMediaControl);
					pMediaControl->Run();

					SaveGraphFile(pGraph, L"c:\\temp\\stl_network_ip_4_ip_6_udp_engine_source.grf");
				}
			}
		}
	}
}

void CWebCameraPictureDialog::OnClose()
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного
	{
		Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = main_dialog;

		if(local_main_dialog!=NULL)
		{
			if(local_main_dialog->web_camera_dialog!=NULL)
			{
				CSingleLock lock(&local_main_dialog->delete_web_camera_dialog_critical_section);

				lock.Lock();

				local_main_dialog->web_camera_dialog = NULL;

				lock.Unlock();

				//				audio_captured_fragments_list.clear();
			}
		}
	}

	CDialogEx::OnClose();

	delete this;
}

void CaptureVideoSampleShot(CWebCameraPictureDialog *local_web_camera_dialog, CImage *parameter_image)
{
	if(parameter_image==NULL)
	{
		return;
	}

	if(local_web_camera_dialog==NULL)
	{
		return;
	}

	if(local_web_camera_dialog->pVideoGrabber==NULL)
	{
		return;
	}

	{

		// Find the required buffer size.
		long cbBuffer;
		BYTE *pBuffer = NULL;
		HRESULT hr;

		hr = local_web_camera_dialog->pVideoGrabber->GetCurrentBuffer(&cbBuffer, NULL);
		if (SUCCEEDED(hr))
		{

			pBuffer = (BYTE*)CoTaskMemAlloc(cbBuffer);
			if (pBuffer!=NULL) 
			{
				hr = local_web_camera_dialog->pVideoGrabber->GetCurrentBuffer(&cbBuffer, (long*)pBuffer);

				if (SUCCEEDED(hr))
				{

					AM_MEDIA_TYPE mt;
					ZeroMemory(&mt, sizeof(mt));

					hr = local_web_camera_dialog->pVideoGrabber->GetConnectedMediaType(&mt);

					if (SUCCEEDED(hr))
					{
						if
							(
							mt.formattype==FORMAT_VideoInfo
							&&
							(mt.cbFormat >= sizeof(VIDEOINFOHEADER))
							&&
							(mt.pbFormat != NULL)
							)
						{
							VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER*)mt.pbFormat;

							BITMAPINFOHEADER BIH = pVIH->bmiHeader;

							BITMAPFILEHEADER BFH;

							ZeroMemory(&BFH, sizeof(BITMAPFILEHEADER));

							((BYTE*)(&BFH.bfType))[0] = 'B';
							((BYTE*)(&BFH.bfType))[1] = 'M';
							BFH.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+cbBuffer;
							BFH.bfReserved1;
							BFH.bfReserved2;
							BFH.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);


							IStream *local_stream;

							HRESULT local_create_IStream_result = CreateStreamOnHGlobal ( 0 , TRUE , &local_stream );

							{
								LARGE_INTEGER liBeggining = { 0 };

								//		received_stream->Seek(liBeggining, STREAM_SEEK_END, NULL);
							}

							ULONG local_bytes_written = 0;
							{
								HRESULT local_write_result = local_stream->Write(&BFH,sizeof(BITMAPFILEHEADER),&local_bytes_written);
							}
							{
								HRESULT local_write_result = local_stream->Write(&BIH,sizeof(BITMAPINFOHEADER),&local_bytes_written);
							}
							{
								HRESULT local_write_result = local_stream->Write(pBuffer,cbBuffer,&local_bytes_written);
							}

							parameter_image->Load(local_stream);

							local_stream->Release();
						}				
						_FreeMediaType(mt);	
					}	
				}

				CoTaskMemFree(pBuffer);

				pBuffer = NULL;
			}
		}
	}
}

void CaptureAudioSampleShot(CWebCameraPictureDialog *local_web_camera_dialog, IStream *parameter_audio_image)
{
	if(parameter_audio_image==NULL)
	{
		return;
	}

	if(local_web_camera_dialog==NULL)
	{
		return;
	}

	if(local_web_camera_dialog->pAudioGrabber==NULL)
	{
		return;
	}

	// Find the required buffer size.
	long cbBuffer;
	BYTE *pBuffer = NULL;
	HRESULT hr;

	hr = local_web_camera_dialog->pAudioGrabber->GetCurrentBuffer(&cbBuffer, NULL);
	if (SUCCEEDED(hr))
	{
		pBuffer = (BYTE*)CoTaskMemAlloc(cbBuffer);
		if (pBuffer!=NULL) 
		{
			hr = local_web_camera_dialog->pAudioGrabber->GetCurrentBuffer(&cbBuffer, (long*)pBuffer);
			if (SUCCEEDED(hr))
			{

				AM_MEDIA_TYPE mt;
				ZeroMemory(&mt, sizeof(mt));

				hr = local_web_camera_dialog->pAudioGrabber->GetConnectedMediaType(&mt);

				if (SUCCEEDED(hr))
				{
					if(mt.formattype==FORMAT_WaveFormatEx)
					{
						WAVEFORMATEX *wave_format_ex = (WAVEFORMATEX *)mt.pbFormat;

						CWave local_wave;

						local_wave.SetData(pBuffer, cbBuffer);

						local_wave.SetSize(cbBuffer);
						local_wave.SetChannels(wave_format_ex->nChannels);
						local_wave.SetBitsPerSample(wave_format_ex->wBitsPerSample);
						local_wave.SetSampleRate(wave_format_ex->nAvgBytesPerSec/(wave_format_ex->nChannels << 1));
						local_wave.SetByteRate(wave_format_ex->nAvgBytesPerSec);

						//local_wave.Play();

						//local_wave.Save(L"c:\\temp\\cwave_source.wav");
						local_wave.Save(parameter_audio_image);
					}				
					_FreeMediaType(mt);	
				}	
			}
			CoTaskMemFree(pBuffer);

			pBuffer = NULL;
		}
	}
}

STDMETHODIMP CWebCameraPictureDialog::SampleCB( double SampleTime, IMediaSample *pSample )
{
	return E_NOTIMPL;
}

STDMETHODIMP CWebCameraPictureDialog::BufferCB( double SampleTime, BYTE *pBuffer, long BufferLen )
{
	Captured_Audio_Data_List_ITEM arrived_audio_data_ip_4;
	ZeroMemory(&arrived_audio_data_ip_4,sizeof(Captured_Audio_Data_List_ITEM));

	arrived_audio_data_ip_4.SampleTime = SampleTime;

	Captured_Audio_Data_List_ITEM arrived_audio_data_ip_6;
	ZeroMemory(&arrived_audio_data_ip_6,sizeof(Captured_Audio_Data_List_ITEM));

	arrived_audio_data_ip_6.SampleTime = SampleTime;

	HRESULT hr = S_OK;

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = main_dialog;

	if(local_main_dialog!=NULL)
	{
		if(local_main_dialog->get_command_terminate_application())
		{
			return S_OK;
		}

		{
			int local_int = 0;
			{
				DWORD dwWaitResult;

				dwWaitResult = WaitForSingleObject( 
					local_main_dialog->do_not_terminate_application_event, // event handle
					0);    // zero wait

				if(dwWaitResult==WAIT_OBJECT_0)
				{
					// Event object was signaled		
					local_int = local_main_dialog->button_send_audio.GetCheck();
				}
				else
				{
					if(local_main_dialog->get_command_terminate_application())
					{
						return S_OK;
					}
				}
			}
			if(local_int!=0)
			{
				AM_MEDIA_TYPE mt;
				ZeroMemory(&mt, sizeof(mt));

				hr = pAudioGrabber->GetConnectedMediaType(&mt);

				if (SUCCEEDED(hr))
				{
					if(mt.formattype==FORMAT_WaveFormatEx)
					{
						WAVEFORMATEX *wave_format_ex = (WAVEFORMATEX *)mt.pbFormat;

						CWave local_wave;

						local_wave.SetData(pBuffer, BufferLen);

						local_wave.SetSize(BufferLen);
						local_wave.SetChannels(wave_format_ex->nChannels);
						local_wave.SetBitsPerSample(wave_format_ex->wBitsPerSample);
						local_wave.SetSampleRate(wave_format_ex->nAvgBytesPerSec/(wave_format_ex->nChannels << 1));
						local_wave.SetByteRate(wave_format_ex->nAvgBytesPerSec);

						//local_wave.Play();

						//local_wave.Save(L"c:\\temp\\cwave_source_BufferCB.wav");

						HRESULT local_create_IStream_result = S_OK;

						{
							local_create_IStream_result = CreateStreamOnHGlobal ( 0 , TRUE , &arrived_audio_data_ip_4.audio_stream );

							if(SUCCEEDED(local_create_IStream_result))
							{
								if(local_wave.Save(arrived_audio_data_ip_4.audio_stream)==TRUE)
								{
									audio_captured_fragments_list_ip_4.push_back(arrived_audio_data_ip_4);
								}
							}
						}

						{
							local_create_IStream_result = CreateStreamOnHGlobal ( 0 , TRUE , &arrived_audio_data_ip_6.audio_stream );

							if(SUCCEEDED(local_create_IStream_result))
							{
								if(local_wave.Save(arrived_audio_data_ip_6.audio_stream)==TRUE)
								{
									audio_captured_fragments_list_ip_6.push_back(arrived_audio_data_ip_6);
								}
							}
						}

						hr = local_create_IStream_result;
					}				
					_FreeMediaType(mt);	
				}
			}
		}
	}

	return hr;
}

void CaptureAudioSampleGetFromTheList_ip_4(CWebCameraPictureDialog *local_web_camera_dialog, IStream **parameter_audio_image)
{
	if(local_web_camera_dialog==NULL)
	{
		return;
	}

	for(;;)
	{
		if(local_web_camera_dialog==NULL)
		{
			return;
		}

		CSingleLock local_lock(&local_web_camera_dialog->audio_captured_fragments_list_critical_section);

		local_lock.Lock();

		if(local_web_camera_dialog->audio_captured_fragments_list_ip_4.size()==0)
		{
			Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (Cstl_network_ip_4_ip_6_udp_engineDialog*)local_web_camera_dialog->main_dialog;

			if(local_main_dialog==NULL)
			{
				return;
			}

			if(local_main_dialog->get_command_threads_audio_stop())
			{
				return;
			}
			else
			{
				Sleep(1);
				continue;
			}
		}
		else
		{
			if(local_web_camera_dialog->audio_captured_fragments_list_ip_4.begin()!=local_web_camera_dialog->audio_captured_fragments_list_ip_4.end())
			{
				CWebCameraPictureDialog::Captured_Audio_Data_List_ITEM local_item = *local_web_camera_dialog->audio_captured_fragments_list_ip_4.begin();

				local_item.audio_stream->Clone(parameter_audio_image);

				local_item.audio_stream->Release();

				local_web_camera_dialog->audio_captured_fragments_list_ip_4.pop_front();
			}

			break;
		}
	}
}

void CaptureAudioSampleGetFromTheList_ip_6(CWebCameraPictureDialog *local_web_camera_dialog, IStream **parameter_audio_image)
{
	if(local_web_camera_dialog==NULL)
	{
		return;
	}

	for(;;)
	{
		if(local_web_camera_dialog==NULL)
		{
			return;
		}

		CSingleLock local_lock(&local_web_camera_dialog->audio_captured_fragments_list_critical_section);

		local_lock.Lock();

		if(local_web_camera_dialog->audio_captured_fragments_list_ip_6.size()==0)
		{
			Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (Cstl_network_ip_4_ip_6_udp_engineDialog*)local_web_camera_dialog->main_dialog;

			if(local_main_dialog==NULL)
			{
				return;
			}

			if(local_main_dialog->get_command_threads_audio_stop())
			{
				return;
			}
			else
			{
				Sleep(1);
				continue;
			}
		}
		else
		{
			if(local_web_camera_dialog->audio_captured_fragments_list_ip_6.begin()!=local_web_camera_dialog->audio_captured_fragments_list_ip_6.end())
			{
				CWebCameraPictureDialog::Captured_Audio_Data_List_ITEM local_item = *local_web_camera_dialog->audio_captured_fragments_list_ip_6.begin();

				local_item.audio_stream->Clone(parameter_audio_image);

				local_item.audio_stream->Release();

				local_web_camera_dialog->audio_captured_fragments_list_ip_6.pop_front();
			}

			break;
		}
	}
}


HRESULT STDMETHODCALLTYPE CWebCameraPictureDialog::QueryInterface( 
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)
{
	if(ppvObject==NULL)
	{
		return E_POINTER;
	}

	if(*ppvObject==NULL)
	{
		return E_POINTER;
	}

	if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown )
	{
		*ppvObject = (void *) static_cast<ISampleGrabberCB*> ( this );
		return NOERROR;
	}    

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CWebCameraPictureDialog::AddRef(void)
{
	return 1;
}

ULONG STDMETHODCALLTYPE CWebCameraPictureDialog::Release(void)
{
	return 0;
}

void CaptureVideoSampleShotIntoStream(CWebCameraPictureDialog *local_web_camera_dialog, IStream **parameter_stream)
{
//	if(parameter_stream==NULL)
//	{
//		return;
//	}

	if(local_web_camera_dialog==NULL)
	{
		return;
	}

	if(local_web_camera_dialog->pVideoGrabber==NULL)
	{
		return;
	}

	{

		// Find the required buffer size.
		long cbBuffer;
		BYTE *pBuffer = NULL;
		HRESULT hr;

		hr = local_web_camera_dialog->pVideoGrabber->GetCurrentBuffer(&cbBuffer, NULL);
		if (SUCCEEDED(hr))
		{

			pBuffer = (BYTE*)CoTaskMemAlloc(cbBuffer);
			if (pBuffer!=NULL) 
			{
				hr = local_web_camera_dialog->pVideoGrabber->GetCurrentBuffer(&cbBuffer, (long*)pBuffer);

				if (SUCCEEDED(hr))
				{

					AM_MEDIA_TYPE mt;
					ZeroMemory(&mt, sizeof(mt));

					hr = local_web_camera_dialog->pVideoGrabber->GetConnectedMediaType(&mt);

					if (SUCCEEDED(hr))
					{
						if
							(
							mt.formattype==FORMAT_VideoInfo
							&&
							(mt.cbFormat >= sizeof(VIDEOINFOHEADER))
							&&
							(mt.pbFormat != NULL)
							)
						{
							VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER*)mt.pbFormat;

							BITMAPINFOHEADER BIH = pVIH->bmiHeader;

							BITMAPFILEHEADER BFH;

							ZeroMemory(&BFH, sizeof(BITMAPFILEHEADER));

							((BYTE*)(&BFH.bfType))[0] = 'B';
							((BYTE*)(&BFH.bfType))[1] = 'M';
							BFH.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+cbBuffer;
							BFH.bfReserved1;
							BFH.bfReserved2;
							BFH.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

							
							IStream *local_stream;

							HRESULT local_create_IStream_result = CreateStreamOnHGlobal ( 0 , TRUE , &local_stream );

							if(local_stream!=NULL)
							{
								{
									LARGE_INTEGER liBeggining = { 0 };

									//		received_stream->Seek(liBeggining, STREAM_SEEK_END, NULL);
								}
							
								ULONG local_bytes_written = 0;
								{
									HRESULT local_write_result = local_stream->Write(&BFH,sizeof(BITMAPFILEHEADER),&local_bytes_written);
								}
								{
									HRESULT local_write_result = local_stream->Write(&BIH,sizeof(BITMAPINFOHEADER),&local_bytes_written);
								}
								{
									HRESULT local_write_result = local_stream->Write(pBuffer,cbBuffer,&local_bytes_written);
								}
								local_stream->Clone(parameter_stream);
							}
						}				
						_FreeMediaType(mt);	
					}	
				}

				CoTaskMemFree(pBuffer);

				pBuffer = NULL;
			}
		}
	}
}

