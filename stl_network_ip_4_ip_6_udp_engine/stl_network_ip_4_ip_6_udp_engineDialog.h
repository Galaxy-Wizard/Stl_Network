
// stl_network_ip_4_ip_6_udp_engineDlg.h : файл заголовка
//

#pragma once
#include "afxwin.h"

#include "ReceivedVideoDialog.h"
#include "WebCameraPictureDialog.h"

struct THREADS_INFORMATION
{
//	CWinThread *WinThread;
	HANDLE WinThread;
	CString thread_name;
};

struct GUI_LIST_CONTROL
{
	CString label;
	int is_checked;
};

struct FRAME_PART
{
	BYTE *frame_part_data;
	UINT frame_part_data_size;
	UINT frame_part_number;
	DWORD sequence_frame_number;
	CTime arrival_time;
};

struct FRAME
{
	std::list<FRAME_PART> frame_parts;
	UINT frame_parts_number;
};

struct STREAM_FRAME_INFORMATION
{
	std::list<FRAME> frames;
	DWORD sequence_source_number;
	CString source_address_string;	//	заполняется при подготовке кадра (перенести в frames, потому что характеризует frame)
};

struct WEB_CAMERA_FRAME
{
	BYTE *frame_data;
	UINT frame_data_size;
	CTime arrival_time;
};

struct STREAM_WEB_CAMERA_FRAME_INFORMATION
{
	std::list<WEB_CAMERA_FRAME> frames;
	DWORD sequence_source_number;
	CString source_address_string;	//	заполняется при подготовке кадра (перенести в frames, потому что характеризует frame)
};

struct STREAM_INFORMATION
{
	CComPtr<IStream> stream;
	DWORD sequence_number;
};

struct GUI_CONTROLS_STATE
{
	std::list<GUI_LIST_CONTROL> IDC_LIST_CHAT_state;			//DDX_Control(pDX, IDC_LIST_CHAT, list_chat);
	CString IDC_EDIT_TEXT_state;								//DDX_Control(pDX, IDC_EDIT_TEXT, edit_text);
	std::list<GUI_LIST_CONTROL> IDC_LIST_NODES_state;			//DDX_Control(pDX, IDC_LIST_NODES, list_nodes);
	CString IDC_EDIT_XOR_CODE_state;							//DDX_Control(pDX, IDC_EDIT_XOR_CODE, edit_xor_code);
	CString IDC_EDIT_SERVER_state;								//DDX_Control(pDX, IDC_EDIT_SERVER, edit_server);
	std::list<GUI_LIST_CONTROL> IDC_LIST_EXTERNAL_IP_4_state;	//DDX_Control(pDX, IDC_LIST_EXTERNAL_IP_4, list_external_ip_4);
	int IDC_CHECK_ENABLE_CHAT_state;							//DDX_Control(pDX, IDC_CHECK_ENABLE_CHAT, button_enable_chat);
	int IDC_CHECK_ENABLE_VIDEO_state;							//DDX_Control(pDX, IDC_CHECK_ENABLE_VIDEO, button_enable_draw_video);
	int IDC_CHECK_ENABLE_SOUND_state;							//DDX_Control(pDX, IDC_CHECK_ENABLE_SOUND, button_enable_play_audio);
	int IDC_CHECK_SEND_VIDEO_state;								//DDX_Control(pDX, IDC_CHECK_SEND_VIDEO, button_send_video);
	int IDC_CHECK_SEND_WEB_CAMERA_VIDEO_state;					//DDX_Control(pDX, IDC_CHECK_SEND_WEB_CAMERA_VIDEO, button_send_web_camera_video);
	int IDC_CHECK_SEND_MICROPHONE_AUDIO_state;					//DDX_Control(pDX, IDC_CHECK_SEND_MICROPHONE_AUDIO, button_send_audio);
	CString IDC_EDIT_SPEED_state;								//DDX_Control(pDX, IDC_EDIT_SPEED, edit_speed);
	int IDC_CHECK_ENABLE_SHOW_WEB_CAMERA_state;					//DDX_Control(pDX, IDC_CHECK_ENABLE_SHOW_WEB_CAMERA, button_enable_web_camera);
	int IDC_CHECK_SAVE_PICTURES_state;							//DDX_Control(pDX, IDC_CHECK_SAVE_PICTURES, button_enable_save_video);
	int IDC_CHECK_SAVE_WEB_CAMERA_PICTURES_state;				//DDX_Control(pDX, IDC_CHECK_SAVE_WEB_CAMERA_PICTURES, button_enable_save_web_camera_video);
	int IDC_CHECK_SAVE_MICROPHONE_SOUND_state;					//DDX_Control(pDX, IDC_CHECK_SAVE_MICROPHONE_SOUND, button_enable_save_microphone_audio);
	int IDC_CHECK_RETRANSLATE_VIDEO_state;						//DDX_Control(pDX, IDC_CHECK_RETRANSLATE_VIDEO, button_retranslate_video);
	int IDC_CHECK_RETRANSLATE_WEB_CAMERA_state;					//DDX_Control(pDX, IDC_CHECK_RETRANSLATE_WEB_CAMERA, button_retranslate_web_camera);
	int IDC_CHECK_RETRANSLATE_MICROPHONE_state;					//DDX_Control(pDX, IDC_CHECK_RETRANSLATE_MICROPHONE, button_retranslate_microphone);
	int IDC_CHECK_ENABLE_CHAT_SERVICE_INFORMATION_state;		//DDX_Control(pDX, IDC_CHECK_ENABLE_CHAT_SERVICE_INFORMATION, button_enable_showing_service_information_in_chat);
};

// диалоговое окно Cstl_network_ip_4_ip_6_udp_engineDialog
class Cstl_network_ip_4_ip_6_udp_engineDialog : public CDialogEx
{
// Создание
public:
	Cstl_network_ip_4_ip_6_udp_engineDialog(CWnd* pParent = NULL);	// стандартный конструктор

// Данные диалогового окна
	enum { IDD = IDD_STL_NETWORK_IP_4_IP_6_UDP_ENGINE };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	bool command_terminate_application;

	bool command_threads_stop;

	bool command_threads_video_stop;

	bool command_threads_web_camera_video_stop;

	bool command_threads_audio_stop;

	bool command_threads_enable_save_video_stop;


	bool command_threads_retranslate_video_stop;
	bool command_threads_retranslate_audio_stop;
	bool command_threads_retranslate_web_camera_video_stop;

public:
	bool get_command_terminate_application()
	{
		return command_terminate_application;
	}

	bool set_command_terminate_application(bool command_terminate_application_parameter)
	{
		bool former_command_terminate_application = command_terminate_application;

		command_terminate_application = command_terminate_application_parameter;

		return former_command_terminate_application;
	}

	bool get_command_threads_stop()
	{
		return command_threads_stop;
	}

	bool set_command_threads_stop(bool command_threads_stop_parameter)
	{
		bool former_command_threads_stop = command_threads_stop;

		command_threads_stop = command_threads_stop_parameter;

		return former_command_threads_stop;
	}


	bool get_command_threads_video_stop()
	{
		return command_threads_video_stop;
	}

	bool set_command_threads_video_stop(bool command_threads_video_stop_parameter)
	{
		bool former_command_threads_video_stop = command_threads_video_stop;

		command_threads_video_stop = command_threads_video_stop_parameter;

		return former_command_threads_video_stop;
	}


	bool get_command_threads_web_camera_video_stop()
	{
		return command_threads_web_camera_video_stop;
	}

	bool set_command_threads_web_camera_video_stop(bool command_threads_web_camera_video_stop_parameter)
	{
		bool former_command_threads_web_camera_video_stop = command_threads_web_camera_video_stop;

		command_threads_web_camera_video_stop = command_threads_web_camera_video_stop_parameter;

		return former_command_threads_web_camera_video_stop;
	}


	bool get_command_threads_audio_stop()
	{
		return command_threads_audio_stop;
	}

	bool set_command_threads_audio_stop(bool command_threads_audio_parameter)
	{
		bool former_command_threads_audio_stop = command_threads_audio_stop;

		command_threads_audio_stop = command_threads_audio_parameter;

		return former_command_threads_audio_stop;
	}


	bool get_command_threads_enable_save_video_stop()
	{
		return command_threads_enable_save_video_stop;
	}

	bool set_command_threads_enable_save_video_stop(bool command_threads_enable_save_video_stop_parameter)
	{
		bool former_command_threads_enable_save_video_stop = command_threads_enable_save_video_stop;

		command_threads_enable_save_video_stop = command_threads_enable_save_video_stop_parameter;

		return former_command_threads_enable_save_video_stop;
	}


	bool get_command_threads_retranslate_video_stop()
	{
		return command_threads_retranslate_video_stop;
	}

	bool set_command_threads_retranslate_video_stop(bool command_threads_retranslate_video_stop_parameter)
	{
		bool former_command_threads_retranslate_video_stop = command_threads_retranslate_video_stop;

		command_threads_retranslate_video_stop = command_threads_retranslate_video_stop_parameter;

		return former_command_threads_retranslate_video_stop;
	}

	bool get_command_threads_retranslate_audio_stop()
	{
		return command_threads_retranslate_audio_stop;
	}

	bool set_command_threads_retranslate_audio_stop(bool command_threads_retranslate_audio_stop_parameter)
	{
		bool former_command_threads_retranslate_audio_stop = command_threads_retranslate_audio_stop;

		command_threads_retranslate_audio_stop = command_threads_retranslate_audio_stop_parameter;

		return former_command_threads_retranslate_audio_stop;
	}

	bool get_command_threads_retranslate_web_camera_video_stop()
	{
		return command_threads_retranslate_web_camera_video_stop;
	}

	bool set_command_threads_retranslate_web_camera_video_stop(bool command_threads_retranslate_web_camera_video_stop_parameter)
	{
		bool former_command_threads_retranslate_web_camera_video_stop = command_threads_retranslate_web_camera_video_stop;

		command_threads_retranslate_web_camera_video_stop = command_threads_retranslate_web_camera_video_stop_parameter;

		return former_command_threads_retranslate_web_camera_video_stop;
	}

public:
	afx_msg void OnBnClickedButtonStartListen();
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonRegister();

	
	HANDLE do_not_terminate_application_event;


	CListBox list_chat;
	CEdit edit_text;
	CListBox list_nodes;

	CCriticalSection list_chat_critical_section;

	CEdit edit_xor_code;

	CEdit edit_server;

	CListBox list_external_ip_4;

	UINT redirected_port_number;

	void PrepareVideo(CString parameter_string, BYTE* parameter_data, size_t parameter_data_length);
	void DrawVideo(CString parameter_string, BYTE* parameter_data, size_t parameter_data_length);

	void PrepareWebCameraVideo(CString parameter_string, BYTE* parameter_data, size_t parameter_data_length);
	void DrawWebCameraVideo(CString parameter_string, BYTE* parameter_data, size_t parameter_data_length);

	void PrepareAudio(CString parameter_string, BYTE* parameter_data, size_t parameter_data_length);
	void PlayAudio(CString parameter_string, BYTE* parameter_data, size_t parameter_data_length);

	CCriticalSection draw_video_critical_section;
	CCriticalSection draw_web_camera_video_critical_section;
	CCriticalSection play_audio_critical_section;

	CButton button_enable_chat;
	CButton button_enable_draw_video;
	CButton button_enable_play_audio;


	afx_msg void OnBnClickedCheckSendVideo();
	CButton button_send_video;

	CCriticalSection draw_video_image_critical_section;
	CCriticalSection draw_audio_image_critical_section;
	CCriticalSection draw_web_camera_video_image_critical_section;

	CCriticalSection list_nodes_critical_section;

	CReceivedVideoDialog *received_video_dialog;
	afx_msg void OnBnClickedButtonClearChat();

	CWebCameraPictureDialog *web_camera_dialog;
	afx_msg void OnBnClickedCheckSendWebCameraVideo();

	CButton button_send_web_camera_video;
	
	std::list<STREAM_INFORMATION> received_video_stream;

	std::list<STREAM_FRAME_INFORMATION> received_video_frame_stream;	//	Добавить синхронизацию ввода-вывода

	std::list<STREAM_INFORMATION> received_web_camera_video_stream;

	std::list<STREAM_WEB_CAMERA_FRAME_INFORMATION> received_web_camera_video_frame_stream;	//	Добавить синхронизацию ввода-вывода

	std::list<STREAM_INFORMATION> received_microphone_stream;

	CCriticalSection threads_list_critical_section;
	std::list<THREADS_INFORMATION> threads_list;

	std::list<STREAM_INFORMATION> retranslate_video_frames_ip_4;
	std::list<STREAM_INFORMATION> retranslate_web_camera_video_frames_ip_4;
	std::list<STREAM_INFORMATION> retranslate_microphone_frames_ip_4;

	std::list<STREAM_INFORMATION> retranslate_video_frames_ip_6;
	std::list<STREAM_INFORMATION> retranslate_web_camera_video_frames_ip_6;
	std::list<STREAM_INFORMATION> retranslate_microphone_frames_ip_6;

	CCriticalSection delete_web_camera_dialog_critical_section;
	CCriticalSection delete_received_video_dialog_critical_section;

	afx_msg void OnBnClickedCheckEnableVideo();
	afx_msg void OnBnClickedCheckEnableSound();
	
	CButton button_send_audio;
	afx_msg void OnBnClickedCheckSendMicrophoneAudio();

	CEdit edit_speed;

	CButton button_enable_web_camera;
	afx_msg void OnBnClickedCheckEnableShowWebCamera();

	GUI_CONTROLS_STATE GUI_CONTROLS_STATE_data;
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	static const UINT_PTR Cstl_network_ip_4_ip_6_udp_engineDialog_timer_nIDEvent = 10000;
	static const UINT_PTR Cstl_network_ip_4_ip_6_udp_engineDialog_draw_video_timer_nIDEvent = 10001;
	static const UINT_PTR Cstl_network_ip_4_ip_6_udp_engineDialog_draw_web_camera_timer_nIDEvent = 10002;
	static const UINT_PTR Cstl_network_ip_4_ip_6_udp_engineDialog_play_sound_timer_nIDEvent = 10003;

	afx_msg void OnClickedCheckEnableChat();

	afx_msg void OnChangeEditServer();
	afx_msg void OnChangeEditText();
	afx_msg void OnChangeEditXorCode();
	afx_msg void OnChangeEditSpeed();
	afx_msg void OnSelchangeListChat();
	afx_msg void OnSelchangeListExternalIp4();
	afx_msg void OnSelchangeListNodes();

	CCriticalSection GUI_update_critical_section;

	CButton button_enable_save_video;
	afx_msg void OnBnClickedCheckSavePictures();

	CButton button_enable_save_web_camera_video;
	afx_msg void OnBnClickedCheckSaveWebCameraPictures();

	CButton button_enable_save_microphone_audio;
	afx_msg void OnBnClickedCheckSaveMicrophoneSound();

	afx_msg void OnBnClickedCheckRetranslateVideo();
	CButton button_retranslate_video;

	afx_msg void OnBnClickedCheckRetranslateWebCamera();
	CButton button_retranslate_web_camera;

	afx_msg void OnBnClickedCheckRetranslateMicrophone();
	CButton button_retranslate_microphone;

	const HICON GetIcon() const
	{
		return m_hIcon;
	}
	CButton button_enable_showing_service_information_in_chat;
	afx_msg void OnBnClickedCheckEnableChatServiceInformation();
};

