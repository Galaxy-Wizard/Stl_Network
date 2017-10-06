
// stl_network_ip_4_ip_6_udp_engineDialog.cpp : файл реализации
//

#include "stdafx.h"
#include "stl_network_ip_4_ip_6_udp_engine.h"
#include "stl_network_ip_4_ip_6_udp_engineDialog.h"
#include "afxdialogex.h"

#include "network\ip_4\blocksock_ip_4.h"
#include "network\ip_6\blocksock_ip_6.h"

#include "encrypt\encrypt_xor\encrypt_xor.h"

#include "Manager/ApplicationManager.h"
#include "Manager/COMManager.h"

#include "Network/NetworkManager.h"
#include "Network/NetworkHelper.h"

#include "NATBreak/UPNPNATServiceTypes.h"
#include "NATBreak/IGDDiscoverProcess.h"
#include "NATBreak/UPNPNATHTTPClient.h"

#include "Wave.h"

#include "galaxy_namespace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define DEBUG_DrawVideo
#undef DEBUG_DrawVideo

static ApplicationManager am;

boost::thread_group tg;

boost::thread_group tg_redirect;
boost::thread_group tg_redirect_delete;

const CString command_register(L"/register");
const CString command_execute_application(L"/execute_application");
const CString command_video(L"/video");
const CString command_video_end(L"/video_end");
const CString command_audio(L"/audio");
const CString command_audio_end(L"/audio_end");
const CString command_web_camera_video(L"/web_camera_video");
const CString command_web_camera_video_end(L"/web_camera_video_end");


#define use_istream_DEFINITION

const LONGLONG CONST_TERMINATION_WAIT_SECONDS = 20;

const LONGLONG CONST_EXPIRATION_DATA_LIMIT_SECONDS = CONST_TERMINATION_WAIT_SECONDS / 2;

void CaptureScreenShot(CImage *parameter_image);

const int CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME = 50000;

const double CONST_ADSL_SPEED = 500000.0;
const double CONST_ETHERNET_SPEED = 1000000000.0;
const double CONST_30Mbps_SPEED = 30000000.0;
const double CONST_RECOMMENDED_SPEED = CONST_30Mbps_SPEED;

const DWORD CONST_RECOMMENDED_GUI_UPDATE_TIME = 2000;
const DWORD CONST_ONE_SECOND = 1000;
const DWORD CONST_RECOMMENDED_FPS = 100;
const DWORD CONST_RECOMMENDED_FRAME_TIME = CONST_ONE_SECOND / CONST_RECOMMENDED_FPS;
const DWORD CONST_RECOMMENDED_WEB_CAMERA_FRAME_TIME = 1;
const DWORD CONST_RECOMMENDED_SOUND_FRAME_TIME = 1;


const double CONST_VIDEO_SIZE = ((1920 * 1080)*(50.0 / 100.0));
const double CONST_WEB_CAMERA_SIZE = ((320 * 240)*(40.0 / 100.0));

const double CONST_VIDEO_WEB_CAMERA_SCREENS_PER_SECOND = CONST_ETHERNET_SPEED / 8 / CONST_WEB_CAMERA_SIZE;	//	CONST_ETHERNET_SPEED/8 - скорость передачи по сети CONST_ETHERNET_SPEED в байтах, 40.0/100.0 - аспект сжатия формата jpg

const double CONST_VIDEO_SCREENS_PER_SECOND = CONST_ETHERNET_SPEED / 8 / CONST_VIDEO_SIZE;	//	CONST_ETHERNET_SPEED/8 - скорость передачи по сети CONST_ETHERNET_SPEED в байтах, 50.0/100.0 - аспект сжатия формата png

const double CONST_AUDIO_PACKETS_PER_SECOND = 2.0 * 5;

const int CONST_MESSAGE_LENGTH = 10000;
const int CONST_MESSAGE_LENGTH_IMAGE = 2 * 4000 * 3000 * 50 / 100;	//	2*4000x3000x32 - максимальный размер изображения, 50/100 - предполагаемый аспект сжатия
//	Количество секунд для отправки и получения
const int CONST_WAIT_TIME_LISTEN = 0;
const int CONST_WAIT_TIME_SEND = 0;

const UINT port_number_start_const = 10000;
const UINT port_count_const = 1;
const UINT port_number_end_const = port_number_start_const + port_count_const - 1;


UINT __cdecl upnp_external_ipv4_detection_thread(LPVOID parameter);


UINT __cdecl datagram_listen_connection_thread(LPVOID parameter);

UINT __cdecl datagram_connect_connection_thread(LPVOID parameter);

UINT __cdecl datagram_send_connection_thread(LPVOID parameter);

UINT __cdecl stop_waiting_thread(LPVOID parameter);


UINT __cdecl datagram_listen_connection_thread_ip_4(LPVOID parameter);
UINT __cdecl datagram_listen_connection_thread_ip_6(LPVOID parameter);

UINT __cdecl datagram_act_on_request_connection_thread_ip_4(LPVOID parameter);
UINT __cdecl datagram_act_on_request_connection_thread_ip_6(LPVOID parameter);


UINT __cdecl datagram_connect_connection_thread_ip_4(LPVOID parameter);
UINT __cdecl datagram_connect_connection_thread_ip_6(LPVOID parameter);

UINT __cdecl datagram_connect_connection_thread_loop_ip_4(LPVOID parameter);
UINT __cdecl datagram_connect_connection_thread_loop_ip_6(LPVOID parameter);

UINT __cdecl datagram_send_connection_thread_ip_4(LPVOID parameter);
UINT __cdecl datagram_send_connection_thread_ip_6(LPVOID parameter);

UINT __cdecl datagram_register_thread(LPVOID parameter);

UINT __cdecl datagram_register_thread_ip_4(LPVOID parameter);
UINT __cdecl datagram_register_thread_ip_6(LPVOID parameter);


UINT __cdecl datagram_send_video_connection_thread(LPVOID parameter);

UINT __cdecl datagram_send_video_connection_thread_ip_4(LPVOID parameter);
UINT __cdecl datagram_send_video_connection_thread_ip_6(LPVOID parameter);


UINT __cdecl datagram_send_web_camera_video_connection_thread(LPVOID parameter);

UINT __cdecl datagram_send_web_camera_video_connection_thread_ip_4(LPVOID parameter);
UINT __cdecl datagram_send_web_camera_video_connection_thread_ip_6(LPVOID parameter);


UINT __cdecl datagram_send_audio_connection_thread(LPVOID parameter);

UINT __cdecl datagram_send_audio_connection_thread_ip_4(LPVOID parameter);
UINT __cdecl datagram_send_audio_connection_thread_ip_6(LPVOID parameter);

//UINT __cdecl datagram_play_video_connection_thread(LPVOID parameter);
UINT __cdecl datagram_play_audio_connection_thread(LPVOID parameter);

UINT __cdecl datagram_enable_save_video_connection_thread(LPVOID parameter);


//UINT __cdecl datagram_retranslate_thread(LPVOID parameter);

//UINT __cdecl datagram_retranslate_connection_thread_ip_4(LPVOID parameter);
//UINT __cdecl datagram_retranslate_connection_thread_ip_6(LPVOID parameter);

UINT __cdecl datagram_retranslate_video_connection_thread(LPVOID parameter);

UINT __cdecl datagram_retranslate_video_connection_thread_ip_4(LPVOID parameter);
UINT __cdecl datagram_retranslate_video_connection_thread_ip_6(LPVOID parameter);

UINT __cdecl datagram_retranslate_web_camera_video_connection_thread(LPVOID parameter);

UINT __cdecl datagram_retranslate_web_camera_video_connection_thread_ip_4(LPVOID parameter);
UINT __cdecl datagram_retranslate_web_camera_video_connection_thread_ip_6(LPVOID parameter);

UINT __cdecl datagram_retranslate_audio_connection_thread(LPVOID parameter);

UINT __cdecl datagram_retranslate_audio_connection_thread_ip_4(LPVOID parameter);
UINT __cdecl datagram_retranslate_audio_connection_thread_ip_6(LPVOID parameter);


const BYTE CONST_XOR_KEY = 42;
#define service_signature_definition L"stl_network_ip_4_ip_6_udp_engine"
#define service_signature_definition_length wcslen(service_signature_definition)*sizeof(wchar_t)
char service_signature[CONST_MESSAGE_LENGTH];

#define localhost_definition L"localhost"

const BYTE CONST_SEQUENCE_KEY_LENGTH = sizeof(DWORD);

struct thread_listen_parameters_structure_type
{
	Cstl_network_ip_4_ip_6_udp_engineDialog *parameter_main_dialog;
};

struct thread_upnp_external_ipv4_detection_thread_parameters_structure_type
{
	Cstl_network_ip_4_ip_6_udp_engineDialog *parameter_main_dialog;
};

struct thread_act_on_request_parameters_structure_type
{
	Cstl_network_ip_4_ip_6_udp_engineDialog *parameter_main_dialog;
	char *parameter_buffer;
	int parameter_buffer_size;
	network::ip_4::CSockAddr_ip_4* parameter_socket_address_peer_ip_4;
	network::ip_6::CSockAddr_ip_6* parameter_socket_address_peer_ip_6;
};

struct thread_connect_parameters_structure_type
{
	Cstl_network_ip_4_ip_6_udp_engineDialog *parameter_main_dialog;
};

struct thread_retranslate_parameters_structure_type
{
	Cstl_network_ip_4_ip_6_udp_engineDialog *parameter_main_dialog;
	BYTE *parameter_thread_data_to_retranslate;
	size_t parameter_thread_data_to_retranslate_size;
	CString parameter_data_source;
};

struct thread_connect_parameters_structure_loop_type
{
	thread_connect_parameters_structure_type thread_connect_parameters_structure_parameter;

	CString local_address;
	CStringA local_address_internet_address;

	DWORD local_low_subnet_temp_number;
	UINT local_parameter_port_number;
};

struct thread_register_parameters_structure_type
{
	Cstl_network_ip_4_ip_6_udp_engineDialog *parameter_main_dialog;
};

struct thread_send_parameters_structure_type
{
	Cstl_network_ip_4_ip_6_udp_engineDialog *parameter_main_dialog;
};

struct thread_stop_parameters_structure_type
{
	Cstl_network_ip_4_ip_6_udp_engineDialog *parameter_main_dialog;
};

struct thread_send_video_parameters_structure_type
{
	Cstl_network_ip_4_ip_6_udp_engineDialog *parameter_main_dialog;
};

struct thread_send_web_camera_video_parameters_structure_type
{
	Cstl_network_ip_4_ip_6_udp_engineDialog *parameter_main_dialog;
};

struct thread_send_audio_parameters_structure_type
{
	Cstl_network_ip_4_ip_6_udp_engineDialog *parameter_main_dialog;
};

struct thread_play_audio_parameters_structure_type
{
	Cstl_network_ip_4_ip_6_udp_engineDialog *parameter_main_dialog;
	CWave *parameter_wave;
};

struct thread_play_video_parameters_structure_type
{
	Cstl_network_ip_4_ip_6_udp_engineDialog *parameter_main_dialog;
	CImage *parameter_image;
	CString parameter_string;
};

struct thread_enable_save_video_parameters_structure_type
{
	Cstl_network_ip_4_ip_6_udp_engineDialog *parameter_main_dialog;
};


void list_chat_AddString(Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog, CString local_chat, bool local_service_information = true)
{
	if (local_main_dialog != NULL)
	{
		if (local_main_dialog->GUI_CONTROLS_STATE_data.IDC_CHECK_ENABLE_CHAT_state != 0)
		{
			if (
				(local_service_information && local_main_dialog->GUI_CONTROLS_STATE_data.IDC_CHECK_ENABLE_CHAT_SERVICE_INFORMATION_state != 0)
				||
				(!local_service_information)
				)
			{
				GUI_LIST_CONTROL local_chat_state;
				local_chat_state.label = local_chat;
				local_chat_state.is_checked = FALSE;
				local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_CHAT_state.push_back(local_chat_state);
			}
		}
	}
};

void clear_thread_information(Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog)
{
	if (local_main_dialog != NULL)
	{
		CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);

		local_lock.Lock();

		CWinThread *local_current_thread = AfxGetThread();

		for
			(
				std::list<THREADS_INFORMATION>::iterator local_threads_iterator = local_main_dialog->threads_list.begin();
				local_threads_iterator != local_main_dialog->threads_list.end();
				local_threads_iterator++
				)
		{
			HANDLE local_thread = local_threads_iterator->WinThread;

			if (local_thread == local_current_thread->m_hThread)
			{
				local_threads_iterator = local_threads_iterator = local_main_dialog->threads_list.erase(local_threads_iterator);
				break;
			}
		}
	}
}

// Диалоговое окно CAboutDlg используется для описания сведений о приложении

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Данные диалогового окна
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	// Реализация
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// диалоговое окно Cstl_network_ip_4_ip_6_udp_engineDialog



Cstl_network_ip_4_ip_6_udp_engineDialog::Cstl_network_ip_4_ip_6_udp_engineDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cstl_network_ip_4_ip_6_udp_engineDialog::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	command_threads_stop = false;

	command_terminate_application = false;

	do_not_terminate_application_event = NULL;
}

void Cstl_network_ip_4_ip_6_udp_engineDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CHAT, list_chat);
	DDX_Control(pDX, IDC_EDIT_TEXT, edit_text);
	DDX_Control(pDX, IDC_LIST_NODES, list_nodes);
	DDX_Control(pDX, IDC_EDIT_XOR_CODE, edit_xor_code);
	DDX_Control(pDX, IDC_EDIT_SERVER, edit_server);
	DDX_Control(pDX, IDC_LIST_EXTERNAL_IP_4, list_external_ip_4);
	DDX_Control(pDX, IDC_CHECK_ENABLE_CHAT, button_enable_chat);
	DDX_Control(pDX, IDC_CHECK_ENABLE_VIDEO, button_enable_draw_video);
	DDX_Control(pDX, IDC_CHECK_ENABLE_SOUND, button_enable_play_audio);
	DDX_Control(pDX, IDC_CHECK_SEND_VIDEO, button_send_video);
	DDX_Control(pDX, IDC_CHECK_SEND_WEB_CAMERA_VIDEO, button_send_web_camera_video);
	DDX_Control(pDX, IDC_CHECK_SEND_MICROPHONE_AUDIO, button_send_audio);
	DDX_Control(pDX, IDC_EDIT_SPEED, edit_speed);
	DDX_Control(pDX, IDC_CHECK_ENABLE_SHOW_WEB_CAMERA, button_enable_web_camera);
	DDX_Control(pDX, IDC_CHECK_SAVE_PICTURES, button_enable_save_video);
	DDX_Control(pDX, IDC_CHECK_SAVE_WEB_CAMERA_PICTURES, button_enable_save_web_camera_video);
	DDX_Control(pDX, IDC_CHECK_SAVE_MICROPHONE_SOUND, button_enable_save_microphone_audio);
	DDX_Control(pDX, IDC_CHECK_RETRANSLATE_VIDEO, button_retranslate_video);
	DDX_Control(pDX, IDC_CHECK_RETRANSLATE_WEB_CAMERA, button_retranslate_web_camera);
	DDX_Control(pDX, IDC_CHECK_RETRANSLATE_MICROPHONE, button_retranslate_microphone);
	DDX_Control(pDX, IDC_CHECK_ENABLE_CHAT_SERVICE_INFORMATION, button_enable_showing_service_information_in_chat);
}

BEGIN_MESSAGE_MAP(Cstl_network_ip_4_ip_6_udp_engineDialog, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START_LISTEN, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedButtonStartListen)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedButtonSend)
	ON_BN_CLICKED(ID_BUTTON_CLOSE, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedButtonClose)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_REGISTER, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedButtonRegister)
	ON_BN_CLICKED(IDC_CHECK_SEND_VIDEO, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckSendVideo)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_CHAT, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedButtonClearChat)
	ON_BN_CLICKED(IDC_CHECK_SEND_WEB_CAMERA_VIDEO, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckSendWebCameraVideo)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_VIDEO, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckEnableVideo)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_SOUND, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckEnableSound)
	ON_BN_CLICKED(IDC_CHECK_SEND_MICROPHONE_AUDIO, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckSendMicrophoneAudio)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_SHOW_WEB_CAMERA, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckEnableShowWebCamera)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_ENABLE_CHAT, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnClickedCheckEnableChat)
	ON_EN_CHANGE(IDC_EDIT_SERVER, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnChangeEditServer)
	ON_EN_CHANGE(IDC_EDIT_TEXT, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnChangeEditText)
	ON_EN_CHANGE(IDC_EDIT_XOR_CODE, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnChangeEditXorCode)
	ON_EN_CHANGE(IDC_EDIT_SPEED, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnChangeEditSpeed)
	ON_LBN_SELCHANGE(IDC_LIST_CHAT, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnSelchangeListChat)
	ON_LBN_SELCHANGE(IDC_LIST_EXTERNAL_IP_4, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnSelchangeListExternalIp4)
	ON_LBN_SELCHANGE(IDC_LIST_NODES, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnSelchangeListNodes)
	ON_BN_CLICKED(IDC_CHECK_SAVE_PICTURES, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckSavePictures)
	ON_BN_CLICKED(IDC_CHECK_SAVE_WEB_CAMERA_PICTURES, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckSaveWebCameraPictures)
	ON_BN_CLICKED(IDC_CHECK_SAVE_MICROPHONE_SOUND, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckSaveMicrophoneSound)
	ON_BN_CLICKED(IDC_CHECK_RETRANSLATE_VIDEO, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckRetranslateVideo)
	ON_BN_CLICKED(IDC_CHECK_RETRANSLATE_WEB_CAMERA, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckRetranslateWebCamera)
	ON_BN_CLICKED(IDC_CHECK_RETRANSLATE_MICROPHONE, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckRetranslateMicrophone)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_CHAT_SERVICE_INFORMATION, &Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckEnableChatServiceInformation)
END_MESSAGE_MAP()


// обработчики сообщений Cstl_network_ip_4_ip_6_udp_engineDialog

BOOL Cstl_network_ip_4_ip_6_udp_engineDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Добавление пункта "О программе..." в системное меню.

	// IDM_ABOUTBOX должен быть в пределах системной команды.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	::CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Задает значок для этого диалогового окна. Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию

	ZeroMemory(service_signature, CONST_MESSAGE_LENGTH);

	memcpy(service_signature, service_signature_definition, service_signature_definition_length);

	encrypt::encrypt_xor(service_signature, service_signature_definition_length, CONST_XOR_KEY);



	WSADATA wsd;
	int local_result = WSAStartup(0x0202, &wsd);

	if (local_result != NO_ERROR)
	{
		const int local_error_message_size = 10000;
		wchar_t local_error_message[local_error_message_size];

		const int local_system_error_message_size = local_error_message_size - 1000;
		wchar_t local_system_error_message[local_system_error_message_size];

		CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

		wcscpy_s(local_system_error_message, local_error_message_size, CString(L"Ошибка при запуске подсистемы сети."));

		wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

		galaxy::MessageBox(local_error_message, CString(L"Ошибка"));

		return FALSE;
	}


	button_enable_chat.SetCheck(1);
	button_enable_draw_video.SetCheck(1);
	button_enable_play_audio.SetCheck(1);
	button_enable_web_camera.SetCheck(1);

	button_enable_save_video.SetCheck(0);

	button_enable_showing_service_information_in_chat.SetCheck(0);

	CString local_edit_speed_string;
	local_edit_speed_string.Format(L"%0.4f", CONST_RECOMMENDED_SPEED);
	edit_speed.SetWindowTextW(local_edit_speed_string);


	command_threads_video_stop = true;

	command_threads_web_camera_video_stop = true;

	command_threads_audio_stop = true;

	command_threads_enable_save_video_stop = true;

	command_threads_retranslate_video_stop = true;
	command_threads_retranslate_audio_stop = true;
	command_threads_retranslate_web_camera_video_stop = true;


	CRect rcClient;

	GetWindowRect(&rcClient);

	if (
		SetWindowPos(NULL, 650, 0, rcClient.Width(), rcClient.Height(), SWP_NOZORDER | SWP_NOREDRAW)
		==
		TRUE
		)
	{
		CString dumme_string("success");
		RedrawWindow();
	}

	srand(UINT(GetTickCount()));

	do_not_terminate_application_event = CreateEvent(NULL, TRUE, TRUE, L"stl_network_ip_4_ip_6_udp_engine_do_not_terminate_application_event");

	received_video_dialog = new CReceivedVideoDialog(this);
	received_video_dialog->main_dialog = this;
	received_video_dialog->Create(CReceivedVideoDialog::IDD);
	received_video_dialog->ShowWindow(SW_SHOWNORMAL);

	web_camera_dialog = new CWebCameraPictureDialog(this);
	web_camera_dialog->main_dialog = this;
	web_camera_dialog->Create(CWebCameraPictureDialog::IDD);
	web_camera_dialog->ShowWindow(SW_SHOWNORMAL);


	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();
	{

		{
			int list_chat_count = list_chat.GetCount();
			GUI_CONTROLS_STATE_data.IDC_LIST_CHAT_state.clear();
			for (int list_chat_items_counter = 0; list_chat_items_counter < list_chat_count; list_chat_items_counter++)
			{
				CString list_chat_current_item;
				list_chat.GetText(list_chat_items_counter, list_chat_current_item);
				GUI_LIST_CONTROL list_chat_current_item_state;
				list_chat_current_item_state.label = list_chat_current_item;
				list_chat_current_item_state.is_checked = FALSE;
				GUI_CONTROLS_STATE_data.IDC_LIST_CHAT_state.push_back(list_chat_current_item_state);							//DDX_Control(pDX, IDC_LIST_CHAT, list_chat);
			}
		}
		edit_text.GetWindowTextW(GUI_CONTROLS_STATE_data.IDC_EDIT_TEXT_state);										//DDX_Control(pDX, IDC_EDIT_TEXT, edit_text);
		{
			int list_nodes_count = list_nodes.GetCount();
			GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.clear();
			for (int list_nodes_items_counter = 0; list_nodes_items_counter < list_nodes_count; list_nodes_items_counter++)
			{
				CString list_nodes_current_item;
				list_chat.GetText(list_nodes_items_counter, list_nodes_current_item);
				GUI_LIST_CONTROL list_nodes_current_item_state;
				list_nodes_current_item_state.label = list_nodes_current_item;
				list_nodes_current_item_state.is_checked = FALSE;
				GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.push_back(list_nodes_current_item_state);						//DDX_Control(pDX, IDC_LIST_NODES, list_nodes);
			}
		}
		edit_xor_code.GetWindowTextW(GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state);								//DDX_Control(pDX, IDC_EDIT_XOR_CODE, edit_xor_code);
		edit_server.GetWindowTextW(GUI_CONTROLS_STATE_data.IDC_EDIT_SERVER_state);									//DDX_Control(pDX, IDC_EDIT_SERVER, edit_server);
		{
			int list_external_ip_4_count = list_external_ip_4.GetCount();
			GUI_CONTROLS_STATE_data.IDC_LIST_EXTERNAL_IP_4_state.clear();
			for (int list_external_ip_4_items_counter = 0; list_external_ip_4_items_counter < list_external_ip_4_count; list_external_ip_4_items_counter++)
			{
				CString list_external_ip_4_current_item;
				list_external_ip_4.GetText(list_external_ip_4_items_counter, list_external_ip_4_current_item);
				GUI_LIST_CONTROL list_external_ip_4_current_item_state;
				list_external_ip_4_current_item_state.label = list_external_ip_4_current_item;
				list_external_ip_4_current_item_state.is_checked = FALSE;
				GUI_CONTROLS_STATE_data.IDC_LIST_EXTERNAL_IP_4_state.push_back(list_external_ip_4_current_item_state);		//DDX_Control(pDX, IDC_LIST_EXTERNAL_IP_4, list_external_ip_4);
			}
		}
		GUI_CONTROLS_STATE_data.IDC_CHECK_ENABLE_CHAT_state = button_enable_chat.GetCheck();						//DDX_Control(pDX, IDC_CHECK_ENABLE_CHAT, button_enable_chat);
		GUI_CONTROLS_STATE_data.IDC_CHECK_ENABLE_VIDEO_state = button_enable_draw_video.GetCheck();					//DDX_Control(pDX, IDC_CHECK_ENABLE_VIDEO, button_enable_draw_video);
		GUI_CONTROLS_STATE_data.IDC_CHECK_ENABLE_SOUND_state = button_enable_play_audio.GetCheck();					//DDX_Control(pDX, IDC_CHECK_ENABLE_SOUND, button_enable_play_audio);
		GUI_CONTROLS_STATE_data.IDC_CHECK_SEND_VIDEO_state = button_send_video.GetCheck();							//DDX_Control(pDX, IDC_CHECK_SEND_VIDEO, button_send_video);
		GUI_CONTROLS_STATE_data.IDC_CHECK_SEND_WEB_CAMERA_VIDEO_state = button_send_web_camera_video.GetCheck();	//DDX_Control(pDX, IDC_CHECK_SEND_WEB_CAMERA_VIDEO, button_send_web_camera_video);
		GUI_CONTROLS_STATE_data.IDC_CHECK_SEND_MICROPHONE_AUDIO_state = button_send_audio.GetCheck();				//DDX_Control(pDX, IDC_CHECK_SEND_MICROPHONE_AUDIO, button_send_audio);
		edit_speed.GetWindowTextW(GUI_CONTROLS_STATE_data.IDC_EDIT_SPEED_state);									//DDX_Control(pDX, IDC_EDIT_SPEED, edit_speed);
		GUI_CONTROLS_STATE_data.IDC_CHECK_ENABLE_SHOW_WEB_CAMERA_state = button_enable_web_camera.GetCheck();		//DDX_Control(pDX, IDC_CHECK_ENABLE_SHOW_WEB_CAMERA, button_enable_web_camera);

		GUI_CONTROLS_STATE_data.IDC_CHECK_SAVE_PICTURES_state = button_enable_save_video.GetCheck();				//DDX_Control(pDX, IDC_CHECK_SAVE_PICTURES, button_enable_save_video);
		GUI_CONTROLS_STATE_data.IDC_CHECK_SAVE_WEB_CAMERA_PICTURES_state = button_enable_save_video.GetCheck();		//DDX_Control(pDX, IDC_CHECK_SAVE_WEB_CAMERA_PICTURES, button_enable_save_web_camera_video);
		GUI_CONTROLS_STATE_data.IDC_CHECK_SAVE_MICROPHONE_SOUND_state = button_enable_save_video.GetCheck();		//DDX_Control(pDX, IDC_CHECK_SAVE_MICROPHONE_SOUND, button_enable_save_microphone_audio);

		GUI_CONTROLS_STATE_data.IDC_CHECK_RETRANSLATE_VIDEO_state = button_retranslate_video.GetCheck();			//DDX_Control(pDX, IDC_CHECK_RETRANSLATE_VIDEO, button_retranslate_video);
		GUI_CONTROLS_STATE_data.IDC_CHECK_RETRANSLATE_WEB_CAMERA_state = button_retranslate_web_camera.GetCheck();	//DDX_Control(pDX, IDC_CHECK_RETRANSLATE_WEB_CAMERA, button_retranslate_web_camera);
		GUI_CONTROLS_STATE_data.IDC_CHECK_RETRANSLATE_MICROPHONE_state = button_retranslate_microphone.GetCheck();	//DDX_Control(pDX, IDC_CHECK_RETRANSLATE_MICROPHONE, button_retranslate_microphone);
		GUI_CONTROLS_STATE_data.IDC_CHECK_ENABLE_CHAT_SERVICE_INFORMATION_state = button_enable_showing_service_information_in_chat.GetCheck();		//DDX_Control(pDX, IDC_CHECK_ENABLE_CHAT_SERVICE_INFORMATION, button_enable_showing_service_information_in_chat);

	}



	SetTimer(Cstl_network_ip_4_ip_6_udp_engineDialog_timer_nIDEvent, CONST_RECOMMENDED_GUI_UPDATE_TIME, NULL);

	SetTimer(Cstl_network_ip_4_ip_6_udp_engineDialog_draw_video_timer_nIDEvent, CONST_RECOMMENDED_FRAME_TIME, NULL);

	SetTimer(Cstl_network_ip_4_ip_6_udp_engineDialog_draw_web_camera_timer_nIDEvent, CONST_RECOMMENDED_FRAME_TIME, NULL);

	SetTimer(Cstl_network_ip_4_ip_6_udp_engineDialog_play_sound_timer_nIDEvent, CONST_RECOMMENDED_SOUND_FRAME_TIME, NULL);


	redirected_port_number = 0;

	/*//*/
	{
		void *local_upnp_external_ipv4_detection_thread_parameters_structure = new thread_upnp_external_ipv4_detection_thread_parameters_structure_type;

		((thread_upnp_external_ipv4_detection_thread_parameters_structure_type*)local_upnp_external_ipv4_detection_thread_parameters_structure)->parameter_main_dialog = this;

		CWinThread *local_thread = AfxBeginThread(upnp_external_ipv4_detection_thread, local_upnp_external_ipv4_detection_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"upnp_external_ipv4_detection_thread");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&threads_list_critical_section);
		local_lock.Lock();

		threads_list.push_back(local_thread_information);
	}
	/*//*/


	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void Cstl_network_ip_4_ip_6_udp_engineDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок. Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void Cstl_network_ip_4_ip_6_udp_engineDialog::OnPaint()
{
	if (IsIconic())
	{
		::CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR Cstl_network_ip_4_ip_6_udp_engineDialog::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedButtonStartListen()
{
	// TODO: добавьте свой код обработчика уведомлений

	{
		void *local_listen_thread_parameters_structure = new thread_listen_parameters_structure_type;

		((thread_listen_parameters_structure_type*)local_listen_thread_parameters_structure)->parameter_main_dialog = this;

		CWinThread *local_thread = AfxBeginThread(datagram_listen_connection_thread, local_listen_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_listen_connection_thread");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&threads_list_critical_section);
		local_lock.Lock();

		threads_list.push_back(local_thread_information);
	}
}


void Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedButtonConnect()
{
	// TODO: добавьте свой код обработчика уведомлений

	{
		void *local_connect_thread_parameters_structure = new thread_connect_parameters_structure_type;

		((thread_connect_parameters_structure_type*)local_connect_thread_parameters_structure)->parameter_main_dialog = this;

		CWinThread *local_thread = AfxBeginThread(datagram_connect_connection_thread, local_connect_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_connect_connection_thread");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&threads_list_critical_section);
		local_lock.Lock();

		threads_list.push_back(local_thread_information);
	}
}


void Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedButtonSend()
{
	// TODO: добавьте свой код обработчика уведомлений

	{
		void *local_send_thread_parameters_structure = new thread_send_parameters_structure_type;

		((thread_send_parameters_structure_type*)local_send_thread_parameters_structure)->parameter_main_dialog = this;

		CWinThread *local_thread = AfxBeginThread(datagram_send_connection_thread, local_send_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_send_connection_thread");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&threads_list_critical_section);
		local_lock.Lock();

		threads_list.push_back(local_thread_information);
	}
}


void Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedButtonClose()
{
	// TODO: добавьте свой код обработчика уведомлений

	PostMessage(WM_CLOSE);
}


//	Threads

//	listen

UINT __cdecl datagram_listen_connection_thread_ip_4(LPVOID parameter)
{
	thread_listen_parameters_structure_type *local_listen_thread_parameters_structure = (thread_listen_parameters_structure_type *)parameter;

	if (local_listen_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_listen_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_listen_thread_parameters_structure;
		return 0;
	}

	local_main_dialog->redirected_port_number = 0;

	CString local_parameter_address_local(L"0.0.0.0");

	CStringA local_address_internet_address;
	UINT local_parameter_port_number_local = port_number_start_const;

	if (network::ip_4::domain_name_to_internet_4_name(local_parameter_address_local, local_address_internet_address) == false)
	{
		const int local_error_message_size = 10000;
		wchar_t local_error_message[local_error_message_size];

		const int local_system_error_message_size = local_error_message_size - 1000;
		wchar_t local_system_error_message[local_system_error_message_size];

		wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

		CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

		wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

		delete local_listen_thread_parameters_structure;

		galaxy::MessageBox(local_error_message, CString(L"Ошибка"));

		{
			clear_thread_information(local_main_dialog);
		}

		return 0;
	}



looking_udp_port:

	for (; local_parameter_port_number_local <= port_number_end_const;)
	{

		network::ip_4::CBlockingSocket_ip_4 local_blocking_socket_temp;

		network::ip_4::CSockAddr_ip_4 local_socket_address_temp(local_address_internet_address, local_parameter_port_number_local);

		try
		{
			local_blocking_socket_temp.Create(SOCK_DGRAM);

			local_blocking_socket_temp.Bind(local_socket_address_temp);
		}
		catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
		{
			const int local_error_message_size = 10000;
			wchar_t local_error_message[local_error_message_size];

			const int local_system_error_message_size = local_error_message_size - 1000;
			wchar_t local_system_error_message[local_system_error_message_size];

			CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

			local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

			wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

			local_blocking_socket_exception->Delete();

			local_parameter_port_number_local++;

			continue;

			//galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
		}

		local_blocking_socket_temp.Close();

		break;

	}

	if (local_parameter_port_number_local > port_number_end_const)
	{
		CString local_information;
		local_information.Format(
			L"Диагностическое сообщение:\nИсчерпаны все зарезервированные для использования UDP порты (диапазон от %d до %d) через службу UPnP для интерфейсов UPnP %s и %s.\nРекомендации:\n1. Необходимо включить службу UPnP в настройках маршрутизатора.\n2. Необходимо убедиться, что есть свободный порт UDP из указанного диапазона на компьютере и маршрутизаторе.\n3. Необходимо купить лицензионную Windows и установить все обновления для неё.\n4. Повторно нажать кнопку \"Слушать\" или перезапустить программу для определения внешнего адреса ipv4.",
			port_number_start_const,
			port_number_end_const,
			CString(UPNPSERVICE_WANPPPCONNECTION1.c_str()),
			CString(UPNPSERVICE_WANIPCONNECTION1.c_str())
		);

		int local_answer = galaxy::MessageBox(local_information, CString(L"Диагностическое сообщение"));

		if (local_answer == IDOK)
		{
			{
				clear_thread_information(local_main_dialog);
			}

			delete local_listen_thread_parameters_structure;
			return 0;
		}
		else
		{
			{
				clear_thread_information(local_main_dialog);
			}

			delete local_listen_thread_parameters_structure;
			return 0;
		}
	}

	network::ip_4::CBlockingSocket_ip_4 local_blocking_socket;

	network::ip_4::CSockAddr_ip_4 local_socket_address(local_address_internet_address, local_parameter_port_number_local);

	try
	{
		local_blocking_socket.Create(SOCK_DGRAM);

		local_blocking_socket.Bind(local_socket_address);
	}
	catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
	{
		const int local_error_message_size = 10000;
		wchar_t local_error_message[local_error_message_size];

		const int local_system_error_message_size = local_error_message_size - 1000;
		wchar_t local_system_error_message[local_system_error_message_size];

		CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

		local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

		wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

		local_blocking_socket_exception->Delete();

		delete local_listen_thread_parameters_structure;

		galaxy::MessageBox(local_error_message, CString(L"Ошибка"));

		{
			clear_thread_information(local_main_dialog);
		}

		return 0;
	}


	bool local_wan_ip_AddPortMapping_udp_failed = true;
	bool local_wan_ppp_AddPortMapping_udp_failed = true;

	{
		try
		{

			IGDDiscoverProcess discoverer(500000);

			tg_redirect.create_thread(boost::ref(discoverer));
			tg_redirect.join_all();

			IGDDiscoverProcess::IGDControlInformations cinfos =
				discoverer.GetAllIGDControlInformations();
			boost_foreach(IGDDiscoverProcess::ControlInfo info, cinfos)
			{
				if (info.has_problem)
				{
					CString error(L"info.has_problem");
					galaxy::MessageBox(error, CString(L"Information"));

					continue;
				}

				if (info.service_type == UPNPSERVICE_LAYER3FORWARDING1)
				{
				}
				else if (info.service_type == UPNPSERVICE_WANIPCONNECTION1)
				{
					CStringA local_address;

					CStringA local_address_internet_address;


					char chInfo[64];
					if (!gethostname(chInfo, sizeof(chInfo)))
					{
						hostent *sh;
						sh = gethostbyname((char*)&chInfo);
						if (sh != NULL)
						{
							int nAdapter = 0;
							while (sh->h_addr_list[nAdapter])
							{
								struct sockaddr_in adr;
								memcpy(&adr.sin_addr, sh->h_addr_list[nAdapter], sh->h_length);
								local_address += CStringA(inet_ntoa(adr.sin_addr)) + CStringA("\n");
								nAdapter++;
							}
						}
					}

					if (local_address.GetLength() == 0)
					{
						CString error(L"local_address.GetLength()==0");
						galaxy::MessageBox(error, CString(L"Ошибка"));

						break;
					}

					//galaxy::MessageBox(CString(local_address),CString(L"local_address"));

					int pos_return = 0;


					int old_pos_return = pos_return;

					pos_return = local_address.Find('\n', old_pos_return);

					while (pos_return != -1)
					{
						CStringA local_current_address;
						if (pos_return != -1)
						{
							for (int local_counter = old_pos_return; local_counter < pos_return; local_counter++)
							{
								if (local_address.GetAt(local_counter) != '\n')
								{
									local_current_address += local_address.GetAt(local_counter);
								}
								else
								{
									break;
								}
							}
						}


						old_pos_return = pos_return + 1;

						pos_return = local_address.Find('\n', old_pos_return);

						//galaxy::MessageBox(CString(local_current_address) + CString(L"\n") + CString(info.ip.c_str()),CString(L"Information"));

						if (network::ip_4::are_ipv4_from_one_subnet(std::string(local_current_address), std::string(info.ip)))
						{
							//galaxy::MessageBox(CString(local_current_address) + CString(L"\n") + CString(info.ip.c_str()),CString(L"are_ipv4_from_one_subnet == true"));

							UPNPNATHTTPClient soap(info.ip, info.port);

							UPNPNATHTTPClient::SoapResult soap_res;
							soap_res = soap.AddPortMapping(
								std::string("stl_network_ip_4_ip_6_udp_engine"), std::string(local_current_address.GetBuffer()), local_parameter_port_number_local, local_parameter_port_number_local, std::string("UDP"),
								info.ip, info.control_url, info.service_type
							);
							if (UPNPNATHTTPClient::SoapSucceeded == soap_res)
							{
								local_main_dialog->redirected_port_number = local_parameter_port_number_local;
								CString local_message("portmapping succeeded.");

								CString local_error_message;
								local_error_message.Format(L"%s\nlocal_current_address == %s\nlocal_parameter_port_number_local == %d", local_message, CString(local_current_address).GetBuffer(), local_parameter_port_number_local);
								//galaxy::MessageBox(local_error_message,CString(L"Information"));

								local_wan_ip_AddPortMapping_udp_failed = false;

							}
							else if (local_main_dialog->redirected_port_number == 0)
							{
								CString local_error_message;

								CString local_message("local_wan_ip_AddPortMapping_udp_failed = true;");
								local_error_message.Format(L"%s\nlocal_current_address == %s\nlocal_parameter_port_number_local == %d", local_message, CString(local_current_address).GetBuffer(), local_parameter_port_number_local);
								//galaxy::MessageBox(local_error_message,CString(L"Information"));

								local_wan_ip_AddPortMapping_udp_failed = true;
							}
							else
							{
								CString local_error_message;

								CString local_message("local_wan_ip_AddPortMapping_udp_failed = true;\nlocal_main_dialog->redirected_port_number!=0");
								local_error_message.Format(L"%s\nlocal_current_address == %s\nlocal_parameter_port_number_local == %d", local_message, CString(local_current_address).GetBuffer(), local_parameter_port_number_local);
								//galaxy::MessageBox(local_error_message,CString(L"Information"));
							}

							soap_res = soap.AddPortMapping(
								std::string("stl_network_ip_4_ip_6_udp_engine"), std::string(local_current_address.GetBuffer()), local_parameter_port_number_local, local_parameter_port_number_local, std::string("TCP"),
								info.ip, info.control_url, info.service_type
							);
							if (UPNPNATHTTPClient::SoapSucceeded == soap_res)
							{
								local_main_dialog->redirected_port_number = local_parameter_port_number_local;
								CString local_message("portmapping succeeded.");
							}
						}
						else
						{
							galaxy::MessageBox(CString(local_current_address) + CString(L"\n") + CString(info.ip.c_str()), CString(L"are_ipv4_from_one_subnet != true"));
						}
					}
				}
				else if (info.service_type == UPNPSERVICE_WANCOMMONINTERFACECONFIG1)
				{
				}
				else if (info.service_type == UPNPSERVICE_WANPPPCONNECTION1)
				{
					CStringA local_address;

					CStringA local_address_internet_address;


					char chInfo[64];
					if (!gethostname(chInfo, sizeof(chInfo)))
					{
						hostent *sh;
						sh = gethostbyname((char*)&chInfo);
						if (sh != NULL)
						{
							int nAdapter = 0;
							while (sh->h_addr_list[nAdapter])
							{
								struct sockaddr_in adr;
								memcpy(&adr.sin_addr, sh->h_addr_list[nAdapter], sh->h_length);
								local_address += CStringA(inet_ntoa(adr.sin_addr)) + CStringA("\n");
								nAdapter++;
							}
						}
					}

					if (local_address.GetLength() == 0)
					{
						CString error(L"local_address.GetLength()==0");
						galaxy::MessageBox(error, CString(L"Ошибка"));

						break;
					}

					//galaxy::MessageBox(CString(local_address),CString(L"local_address"));

					int pos_return = 0;


					int old_pos_return = pos_return;

					pos_return = local_address.Find('\n', old_pos_return);

					while (pos_return != -1)
					{
						CStringA local_current_address;
						if (pos_return != -1)
						{
							for (int local_counter = old_pos_return; local_counter < pos_return; local_counter++)
							{
								if (local_address.GetAt(local_counter) != '\n')
								{
									local_current_address += local_address.GetAt(local_counter);
								}
								else
								{
									break;
								}
							}
						}


						old_pos_return = pos_return + 1;

						pos_return = local_address.Find('\n', old_pos_return);

						//galaxy::MessageBox(CString(local_current_address) + CString(L"\n") + CString(info.ip.c_str()),CString(L"Information"));

						if (network::ip_4::are_ipv4_from_one_subnet(std::string(local_current_address), std::string(info.ip)))
						{
							//galaxy::MessageBox(CString(local_current_address) + CString(L"\n") + CString(info.ip.c_str()),CString(L"are_ipv4_from_one_subnet == true"));

							UPNPNATHTTPClient soap(info.ip, info.port);

							UPNPNATHTTPClient::SoapResult soap_res;
							soap_res = soap.AddPortMapping(
								std::string("stl_network_ip_4_ppp_6_udp_engine"), std::string(local_current_address.GetBuffer()), local_parameter_port_number_local, local_parameter_port_number_local, std::string("UDP"),
								info.ip, info.control_url, info.service_type
							);
							if (UPNPNATHTTPClient::SoapSucceeded == soap_res)
							{
								local_main_dialog->redirected_port_number = local_parameter_port_number_local;
								CString local_message("portmapping succeeded.");

								CString local_error_message;
								local_error_message.Format(L"%s\nlocal_current_address == %s\nlocal_parameter_port_number_local == %d", local_message, CString(local_current_address).GetBuffer(), local_parameter_port_number_local);
								//galaxy::MessageBox(local_error_message,CString(L"Information"));

								local_wan_ppp_AddPortMapping_udp_failed = false;

							}
							else if (local_main_dialog->redirected_port_number == 0)
							{
								CString local_error_message;

								CString local_message("local_wan_ppp_AddPortMapping_udp_failed = true;");
								local_error_message.Format(L"%s\nlocal_current_address == %s\nlocal_parameter_port_number_local == %d", local_message, CString(local_current_address).GetBuffer(), local_parameter_port_number_local);
								//galaxy::MessageBox(local_error_message,CString(L"Information"));

								local_wan_ppp_AddPortMapping_udp_failed = true;
							}
							else
							{
								CString local_error_message;

								CString local_message("local_wan_ppp_AddPortMapping_udp_failed = true;\nlocal_main_dialog->redirected_port_number!=0");
								local_error_message.Format(L"%s\nlocal_current_address == %s\nlocal_parameter_port_number_local == %d", local_message, CString(local_current_address).GetBuffer(), local_parameter_port_number_local);
								//galaxy::MessageBox(local_error_message,CString(L"Information"));
							}

							soap_res = soap.AddPortMapping(
								std::string("stl_network_ip_4_ip_6_udp_engine"), std::string(local_current_address.GetBuffer()), local_parameter_port_number_local, local_parameter_port_number_local, std::string("TCP"),
								info.ip, info.control_url, info.service_type
							);
							if (UPNPNATHTTPClient::SoapSucceeded == soap_res)
							{
								local_main_dialog->redirected_port_number = local_parameter_port_number_local;
								CString local_message("portmapping succeeded.");
							}
						}
						else
						{
							//galaxy::MessageBox(CString(local_current_address) + CString(L"\n") + CString(info.ip.c_str()),CString(L"are_ipv4_from_one_subnet != true"));
						}
					}
				}
			}

		}
		catch (IGDDiscoverProcess::exception &e)
		{
			CString error(e.what());
			galaxy::MessageBox(error, CString(L"Information"));
		}


		CString local_error_message;
		local_error_message.Format(
			L"local_parameter_port_number_local == %d\nlocal_wan_ip_AddPortMapping_udp_failed = %d\nlocal_wan_ppp_AddPortMapping_udp_failed = %d",
			local_parameter_port_number_local, WORD(local_wan_ip_AddPortMapping_udp_failed), WORD(local_wan_ppp_AddPortMapping_udp_failed));
		//galaxy::MessageBox(local_error_message,CString(L"Information"));
	}

	{
		if (local_wan_ip_AddPortMapping_udp_failed && local_wan_ppp_AddPortMapping_udp_failed)
		{
			CString local_question;
			local_question.Format(
				L"Диагностическое сообщение:\nОткрытие UDP порта %d через службу UPnP для интерфейсов UPnP %s и %s завершилось неудачей.\nХотите попробовать для UDP порта %d?\nПринять - перейти к следующему UDP порту.\nОтказать - работать локально на текущем UDP порту.",
				local_parameter_port_number_local,
				CString(UPNPSERVICE_WANPPPCONNECTION1.c_str()),
				CString(UPNPSERVICE_WANIPCONNECTION1.c_str()),
				local_parameter_port_number_local + 1
			);

			int local_answer = galaxy::MessageBox(local_question, CString(L""));

			if (local_answer == IDOK)
			{
				local_blocking_socket.Close();
				local_parameter_port_number_local++;

				goto looking_udp_port;
			}
			else if (local_answer == IDCANCEL)
			{
			}
		}
	}


	char local_message_to_receive[CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME];

	LONG64 local_receive_counter = 0;

	for (;;)
	{
		ZeroMemory(local_message_to_receive, CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME);

		int local_bytes_received = 0;

		network::ip_4::CSockAddr_ip_4 local_socket_address_peer;

		try
		{
			local_bytes_received = local_blocking_socket.ReceiveDatagram(local_message_to_receive, CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME, (LPSOCKADDR)&local_socket_address_peer, CONST_WAIT_TIME_LISTEN);

			if (local_bytes_received > 0)
			{
				{
					local_receive_counter++;

					void *local_act_on_request_thread_parameters_structure = new thread_act_on_request_parameters_structure_type;

					((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_main_dialog = ((thread_listen_parameters_structure_type*)local_listen_thread_parameters_structure)->parameter_main_dialog;
					((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_buffer = new char[local_bytes_received + sizeof(wchar_t)];
					((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_buffer_size = local_bytes_received;

					memcpy(((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_buffer, local_message_to_receive, local_bytes_received);
					wchar_t zero_word = L'\0';
					memcpy(((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_buffer + local_bytes_received, &zero_word, sizeof(wchar_t));

					((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_socket_address_peer_ip_4 = new network::ip_4::CSockAddr_ip_4(local_socket_address_peer);
					((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_socket_address_peer_ip_6 = NULL;

					CWinThread *local_thread = AfxBeginThread(datagram_act_on_request_connection_thread_ip_4, local_act_on_request_thread_parameters_structure);

					THREADS_INFORMATION local_thread_information;
					local_thread_information.thread_name = CString(L"datagram_act_on_request_connection_thread_ip_4");
					local_thread_information.WinThread = local_thread->m_hThread;

					CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
					local_lock.Lock();

					local_main_dialog->threads_list.push_back(local_thread_information);

					//datagram_act_on_request_connection_thread_ip_4(local_act_on_request_thread_parameters_structure);
				}
			}
		}
		catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
		{
			const int local_error_message_size = 10000;
			wchar_t local_error_message[local_error_message_size];

			const int local_system_error_message_size = local_error_message_size - 1000;
			wchar_t local_system_error_message[local_system_error_message_size];

			CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

			local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

			wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

			int local_error_number = local_blocking_socket_exception->GetErrorNumber();

			local_blocking_socket_exception->Delete();

			if (local_error_number != 0)
			{
				galaxy::MessageBox(local_error_message, CString(L"Ошибка"));
			}
		}

		if (local_main_dialog->get_command_threads_stop())
		{
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_listen_thread_parameters_structure;
		return 1;
	}
}

UINT __cdecl datagram_listen_connection_thread_ip_6(LPVOID parameter)
{
	thread_listen_parameters_structure_type *local_listen_thread_parameters_structure = (thread_listen_parameters_structure_type *)parameter;

	if (local_listen_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_listen_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_listen_thread_parameters_structure;
		return 0;
	}

	CString local_parameter_address_local(L"::0");

	CStringA local_address_internet_address;
	UINT local_parameter_port_number_local = port_number_start_const;

	if (network::ip_6::domain_name_to_internet_6_name(local_parameter_address_local, local_address_internet_address) == false)
	{
		const int local_error_message_size = 10000;
		wchar_t local_error_message[local_error_message_size];

		const int local_system_error_message_size = local_error_message_size - 1000;
		wchar_t local_system_error_message[local_system_error_message_size];

		wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

		CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

		wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

		galaxy::MessageBox(local_error_message, CString(L"Ошибка"));

		{
			clear_thread_information(local_main_dialog);
		}

		return 0;
	}


	for (; local_parameter_port_number_local <= port_number_end_const;)
	{

		network::ip_6::CBlockingSocket_ip_6 local_blocking_socket_temp;

		network::ip_6::CSockAddr_ip_6 local_socket_address_temp(local_address_internet_address, local_parameter_port_number_local);

		try
		{
			local_blocking_socket_temp.Create(SOCK_DGRAM);

			local_blocking_socket_temp.Bind(local_socket_address_temp);
		}
		catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
		{
			const int local_error_message_size = 10000;
			wchar_t local_error_message[local_error_message_size];

			const int local_system_error_message_size = local_error_message_size - 1000;
			wchar_t local_system_error_message[local_system_error_message_size];

			CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

			local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

			wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

			local_blocking_socket_exception->Delete();

			local_parameter_port_number_local++;

			continue;

			//galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
		}

		local_blocking_socket_temp.Close();

		break;

	}

	if (local_parameter_port_number_local > port_number_end_const)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		return 0;
	}

	network::ip_6::CBlockingSocket_ip_6 local_blocking_socket;

	network::ip_6::CSockAddr_ip_6 local_socket_address(local_address_internet_address, local_parameter_port_number_local);

	try
	{
		local_blocking_socket.Create(SOCK_DGRAM);

		local_blocking_socket.Bind(local_socket_address);
	}
	catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
	{
		const int local_error_message_size = 10000;
		wchar_t local_error_message[local_error_message_size];

		const int local_system_error_message_size = local_error_message_size - 1000;
		wchar_t local_system_error_message[local_system_error_message_size];

		CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

		local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

		wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

		local_blocking_socket_exception->Delete();

		delete local_listen_thread_parameters_structure;

		galaxy::MessageBox(local_error_message, CString(L"Ошибка"));

		{
			clear_thread_information(local_main_dialog);
		}

		return 0;
	}


	char local_message_to_receive[CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME];

	LONG64 local_receive_counter = 0;

	for (;;)
	{
		ZeroMemory(local_message_to_receive, CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME);

		int local_bytes_received = 0;

		network::ip_6::CSockAddr_ip_6 local_socket_address_peer;

		try
		{
			local_bytes_received = local_blocking_socket.ReceiveDatagram(local_message_to_receive, CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME, (LPSOCKADDR_IN6)&local_socket_address_peer, CONST_WAIT_TIME_LISTEN);

			if (local_bytes_received > 0)
			{
				{
					local_receive_counter++;

					void *local_act_on_request_thread_parameters_structure = new thread_act_on_request_parameters_structure_type;

					((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_main_dialog = ((thread_listen_parameters_structure_type*)local_listen_thread_parameters_structure)->parameter_main_dialog;
					((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_buffer = new char[local_bytes_received + sizeof(wchar_t)];
					((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_buffer_size = local_bytes_received;

					memcpy(((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_buffer, local_message_to_receive, local_bytes_received);
					wchar_t zero_word = L'\0';
					memcpy(((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_buffer + local_bytes_received, &zero_word, sizeof(wchar_t));

					((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_socket_address_peer_ip_4 = NULL;
					((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_socket_address_peer_ip_6 = new network::ip_6::CSockAddr_ip_6(local_socket_address_peer);;

					CWinThread *local_thread = AfxBeginThread(datagram_act_on_request_connection_thread_ip_6, local_act_on_request_thread_parameters_structure);

					THREADS_INFORMATION local_thread_information;
					local_thread_information.thread_name = CString(L"datagram_act_on_request_connection_thread_ip_6");
					local_thread_information.WinThread = local_thread->m_hThread;

					CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
					local_lock.Lock();

					local_main_dialog->threads_list.push_back(local_thread_information);

					//datagram_act_on_request_connection_thread_ip_6(local_act_on_request_thread_parameters_structure);
				}
			}
		}
		catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
		{
			const int local_error_message_size = 10000;
			wchar_t local_error_message[local_error_message_size];

			const int local_system_error_message_size = local_error_message_size - 1000;
			wchar_t local_system_error_message[local_system_error_message_size];

			CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

			local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

			wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

			int local_error_number = local_blocking_socket_exception->GetErrorNumber();

			local_blocking_socket_exception->Delete();

			if (local_error_number != 0)
			{
				galaxy::MessageBox(local_error_message, CString(L"Ошибка"));
			}
		}

		if (local_main_dialog->get_command_threads_stop())
		{
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_listen_thread_parameters_structure;
		return 1;
	}
}


//	connect

UINT __cdecl datagram_connect_connection_thread_ip_4(LPVOID parameter)
{
	thread_connect_parameters_structure_type *local_connect_thread_parameters_structure = (thread_connect_parameters_structure_type *)parameter;

	if (local_connect_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_connect_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_connect_thread_parameters_structure;
		return 0;
	}

	CString local_address(localhost_definition);

	CStringA local_address_internet_address;

	struct in_addr ia;
	unsigned long addr = 0;
	int ret = 0;
	int idx = 0;

	while (1)
	{
		ret = network::ip_4::lookup_addr_indx_ip_4(idx, &addr);
		if (ret < 0)
		{
			break;
		}
		else if (ret == 0)
		{
			break;
		}
		ia.s_addr = addr;
		//printf("address %d: %s\n", idx, inet_ntoa(ia));
		idx++;

		local_address = CString(inet_ntoa(ia));


		for (DWORD local_low_subnet_temp_number = 0; local_low_subnet_temp_number <= 0xFF; local_low_subnet_temp_number++)
		{

			if (local_address == CString(localhost_definition))
			{
				if (local_low_subnet_temp_number < 1)
				{
					continue;
				}
				else
					if (local_low_subnet_temp_number == 1)
					{
					}
					else
						if (local_low_subnet_temp_number > 1)
						{
							break;
						}
			}

			for (UINT local_parameter_port_number = port_number_start_const; local_parameter_port_number <= port_number_end_const; local_parameter_port_number++)
			{
				{
					void *local_connect_thread_parameters_structure_loop = new thread_connect_parameters_structure_loop_type;

					((thread_connect_parameters_structure_loop_type*)local_connect_thread_parameters_structure_loop)->thread_connect_parameters_structure_parameter = *local_connect_thread_parameters_structure;

					((thread_connect_parameters_structure_loop_type*)local_connect_thread_parameters_structure_loop)->local_address = local_address;
					((thread_connect_parameters_structure_loop_type*)local_connect_thread_parameters_structure_loop)->local_address_internet_address = local_address_internet_address;

					((thread_connect_parameters_structure_loop_type*)local_connect_thread_parameters_structure_loop)->local_low_subnet_temp_number = local_low_subnet_temp_number;
					((thread_connect_parameters_structure_loop_type*)local_connect_thread_parameters_structure_loop)->local_parameter_port_number = local_parameter_port_number;

					CWinThread *local_thread = AfxBeginThread(datagram_connect_connection_thread_loop_ip_4, local_connect_thread_parameters_structure_loop);

					THREADS_INFORMATION local_thread_information;
					local_thread_information.thread_name = CString(L"datagram_connect_connection_thread_loop_ip_4");
					local_thread_information.WinThread = local_thread->m_hThread;

					CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
					local_lock.Lock();

					local_main_dialog->threads_list.push_back(local_thread_information);
				}
			}

			if (local_main_dialog->get_command_threads_stop())
			{
				break;
			}
			else
			{
				Sleep(1);
			}
		}
		if (local_main_dialog->get_command_threads_stop())
		{
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_connect_thread_parameters_structure;
		return 1;
	}
}

UINT __cdecl datagram_connect_connection_thread_ip_6(LPVOID parameter)
{
	thread_connect_parameters_structure_type *local_connect_thread_parameters_structure = (thread_connect_parameters_structure_type *)parameter;

	if (local_connect_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_connect_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_connect_thread_parameters_structure;
		return 0;
	}

	CString local_address(localhost_definition);

	CStringA local_address_internet_address;

	in6_addr ia6;
	sockaddr_in6 addr6;
	int ret = 0;
	int idx = 0;

	while (1)
	{
		ret = network::ip_6::lookup_addr_indx_ip_6(idx, &addr6);
		if (ret < 0)
		{
			break;
		}
		else if (ret == 0)
		{
			break;
		}

		ia6 = addr6.sin6_addr;

		char local_address_buffer[100];
		inet_ntop(AF_INET6, &ia6, local_address_buffer, 100);
		//		printf("address %d: %s\n", idx, local_address_buffer);
		idx++;

		local_address = CString(local_address_buffer);

		for (DWORD local_low_subnet_temp_number = 0; local_low_subnet_temp_number <= 0xFFFF; local_low_subnet_temp_number++)
		{

			for (UINT local_parameter_port_number = port_number_start_const; local_parameter_port_number <= port_number_end_const; local_parameter_port_number++)
			{
				{
					void *local_connect_thread_parameters_structure_loop = new thread_connect_parameters_structure_loop_type;

					((thread_connect_parameters_structure_loop_type*)local_connect_thread_parameters_structure_loop)->thread_connect_parameters_structure_parameter = *local_connect_thread_parameters_structure;

					((thread_connect_parameters_structure_loop_type*)local_connect_thread_parameters_structure_loop)->local_address = local_address;
					((thread_connect_parameters_structure_loop_type*)local_connect_thread_parameters_structure_loop)->local_address_internet_address = local_address_internet_address;

					((thread_connect_parameters_structure_loop_type*)local_connect_thread_parameters_structure_loop)->local_low_subnet_temp_number = local_low_subnet_temp_number;
					((thread_connect_parameters_structure_loop_type*)local_connect_thread_parameters_structure_loop)->local_parameter_port_number = local_parameter_port_number;

					CWinThread *local_thread = AfxBeginThread(datagram_connect_connection_thread_loop_ip_6, local_connect_thread_parameters_structure_loop);

					THREADS_INFORMATION local_thread_information;
					local_thread_information.thread_name = CString(L"datagram_connect_connection_thread_loop_ip_6");
					local_thread_information.WinThread = local_thread->m_hThread;

					CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
					local_lock.Lock();

					local_main_dialog->threads_list.push_back(local_thread_information);
				}
			}

			if (local_main_dialog->get_command_threads_stop())
			{
				break;
			}
			else
			{
				Sleep(1);
			}
		}
		if (local_main_dialog->get_command_threads_stop())
		{
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_connect_thread_parameters_structure;
		return 1;
	}
}


//	send

UINT __cdecl datagram_send_connection_thread_ip_4(LPVOID parameter)
{
	thread_send_parameters_structure_type *local_send_thread_parameters_structure = (thread_send_parameters_structure_type *)parameter;

	if (local_send_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_send_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_send_thread_parameters_structure;
		return 0;
	}

	CString local_address(localhost_definition);

	CStringA local_address_internet_address;

	if (local_main_dialog->get_command_terminate_application())
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_send_thread_parameters_structure;
		return 0;
	}
	int peers_to_send_count = 0;

	{
		DWORD dwWaitResult;

		dwWaitResult = WaitForSingleObject(
			local_main_dialog->do_not_terminate_application_event, // event handle
			0);    // zero wait

		if (dwWaitResult == WAIT_OBJECT_0)
		{
			// Event object was signaled		

			if (local_main_dialog->get_command_terminate_application())
			{
				{
					clear_thread_information(local_main_dialog);
				}

				delete local_send_thread_parameters_structure;
				return 0;
			}

			for (
				std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
				current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
				current_item_iterator++
				)
			{
				if (current_item_iterator->is_checked > 0)
				{
					peers_to_send_count++;
				}
			}
		}
		else
		{
			{
				clear_thread_information(local_main_dialog);
			}

			if (local_main_dialog->get_command_terminate_application())
			{
				delete local_send_thread_parameters_structure;
				return 0;
			}
		}

	}

	for (int peers_to_send_count_counter = 0; peers_to_send_count_counter < peers_to_send_count; peers_to_send_count_counter++)
	{
		double local_data_size_send = 0.0;

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}

		CString peer_to_send;

		int loop_counter = 0;
		for (
			std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
			current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
			current_item_iterator++
			)
		{
			Sleep(1);
			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}
			if (current_item_iterator->is_checked > 0)
			{
				if (peers_to_send_count_counter == loop_counter)
				{
					peer_to_send = current_item_iterator->label;
					break;
				}
				loop_counter++;
			}

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}
		}

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}

		local_address = peer_to_send;

		for (UINT local_parameter_port_number = port_number_start_const; local_parameter_port_number <= port_number_end_const; local_parameter_port_number++)
		{
			network::ip_4::CBlockingSocket_ip_4 local_blocking_socket;

			if (network::ip_4::domain_name_to_internet_4_name(local_address, local_address_internet_address) == false)
			{
				const int local_error_message_size = 10000;
				wchar_t local_error_message[local_error_message_size];

				const int local_system_error_message_size = local_error_message_size - 1000;
				wchar_t local_system_error_message[local_system_error_message_size];

				wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

				CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

				wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

				//delete local_send_thread_parameters_structure;

				//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

				continue;
			}

			network::ip_4::CSockAddr_ip_4 local_socket_address(local_address_internet_address, local_parameter_port_number);

			try
			{
				local_blocking_socket.Create(SOCK_DGRAM);
			}
			catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
			{
				const int local_error_message_size = 10000;
				wchar_t local_error_message[local_error_message_size];

				const int local_system_error_message_size = local_error_message_size - 1000;
				wchar_t local_system_error_message[local_system_error_message_size];

				CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

				local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

				wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

				local_blocking_socket_exception->Delete();

				delete local_send_thread_parameters_structure;

				//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

				{
					clear_thread_information(local_main_dialog);
				}

				return 0;
			}

			int local_bytes_sent = 0;
			int local_error_number = 0;

			char send_buffer[CONST_MESSAGE_LENGTH];

			int send_buffer_data_length = service_signature_definition_length;

			ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH);

			memcpy(send_buffer, service_signature, service_signature_definition_length);

			CString send_data_string;

			int send_data_string_length = 0;


			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}
			{
				send_data_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_TEXT_state;
			}


			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}
			{
				send_data_string_length = send_data_string.GetLength();
			}

			memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

			send_buffer_data_length += send_data_string_length * sizeof(wchar_t);

			CString xor_code_string;

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}
			{
				xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
			}

			char xor_code = _wtoi(xor_code_string);

			encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

			try
			{
				local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer, send_buffer_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

				local_data_size_send += local_bytes_sent;
			}
			catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
			{
				const int local_error_message_size = 10000;
				wchar_t local_error_message[local_error_message_size];

				const int local_system_error_message_size = local_error_message_size;
				wchar_t local_system_error_message[local_system_error_message_size];

				CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

				local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

				wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

				local_error_number = local_blocking_socket_exception->GetErrorNumber();

				local_blocking_socket_exception->Delete();

				if (local_error_number != 0)
				{
					//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
				}
			}

			local_blocking_socket.Close();

			if (local_bytes_sent <= 0 && local_error_number == 0)
			{
				//			galaxy::MessageBox(CString(L"local_bytes_sent<=0"),CString(L"Ошибка"));
			}

			if (local_main_dialog->get_command_threads_stop())
			{
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		if (local_main_dialog->get_command_threads_stop())
		{
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_send_thread_parameters_structure;
		return 1;
	}
}

UINT __cdecl datagram_send_connection_thread_ip_6(LPVOID parameter)
{
	thread_send_parameters_structure_type *local_send_thread_parameters_structure = (thread_send_parameters_structure_type *)parameter;

	if (local_send_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_send_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_send_thread_parameters_structure;
		return 0;
	}

	CString local_address(localhost_definition);

	CStringA local_address_internet_address;

	if (local_main_dialog->get_command_terminate_application())
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_send_thread_parameters_structure;
		return 0;
	}
	int peers_to_send_count = 0;

	{
		DWORD dwWaitResult;

		dwWaitResult = WaitForSingleObject(
			local_main_dialog->do_not_terminate_application_event, // event handle
			0);    // zero wait

		if (dwWaitResult == WAIT_OBJECT_0)
		{
			// Event object was signaled		

			if (local_main_dialog->get_command_terminate_application())
			{
				{
					clear_thread_information(local_main_dialog);
				}

				delete local_send_thread_parameters_structure;
				return 0;
			}

			for (
				std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
				current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
				current_item_iterator++
				)
			{
				if (current_item_iterator->is_checked > 0)
				{
					peers_to_send_count++;
				}
			}
		}
		else
		{
			if (local_main_dialog->get_command_terminate_application())
			{
				{
					clear_thread_information(local_main_dialog);
				}

				delete local_send_thread_parameters_structure;
				return 0;
			}
		}

	}

	for (int peers_to_send_count_counter = 0; peers_to_send_count_counter < peers_to_send_count; peers_to_send_count_counter++)
	{
		double local_data_size_send = 0.0;

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}

		CString peer_to_send;

		int loop_counter = 0;
		for (
			std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
			current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
			current_item_iterator++
			)
		{
			Sleep(1);
			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}
			if (current_item_iterator->is_checked > 0)
			{
				if (peers_to_send_count_counter == loop_counter)
				{
					peer_to_send = current_item_iterator->label;
					break;
				}
				loop_counter++;
			}

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}
		}

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}

		local_address = peer_to_send;

		for (UINT local_parameter_port_number = port_number_start_const; local_parameter_port_number <= port_number_end_const; local_parameter_port_number++)
		{
			network::ip_6::CBlockingSocket_ip_6 local_blocking_socket;

			if (network::ip_6::domain_name_to_internet_6_name(local_address, local_address_internet_address) == false)
			{
				const int local_error_message_size = 10000;
				wchar_t local_error_message[local_error_message_size];

				const int local_system_error_message_size = local_error_message_size - 1000;
				wchar_t local_system_error_message[local_system_error_message_size];

				wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

				CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

				wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

				//delete local_send_thread_parameters_structure;

				//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

				continue;
			}

			network::ip_6::CSockAddr_ip_6 local_socket_address(local_address_internet_address, local_parameter_port_number);

			try
			{
				local_blocking_socket.Create(SOCK_DGRAM);
			}
			catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
			{
				const int local_error_message_size = 10000;
				wchar_t local_error_message[local_error_message_size];

				const int local_system_error_message_size = local_error_message_size - 1000;
				wchar_t local_system_error_message[local_system_error_message_size];

				CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

				local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

				wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

				local_blocking_socket_exception->Delete();

				delete local_send_thread_parameters_structure;

				//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

				{
					clear_thread_information(local_main_dialog);
				}

				return 0;
			}

			int local_bytes_sent = 0;
			int local_error_number = 0;

			char send_buffer[CONST_MESSAGE_LENGTH];

			int send_buffer_data_length = service_signature_definition_length;

			ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH);

			memcpy(send_buffer, service_signature, service_signature_definition_length);

			CString send_data_string;

			int send_data_string_length = 0;


			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}
			{
				send_data_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_TEXT_state;
			}


			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}
			{
				send_data_string_length = send_data_string.GetLength();
			}

			memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

			send_buffer_data_length += send_data_string_length * sizeof(wchar_t);

			CString xor_code_string;

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}
			{
				xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
			}

			char xor_code = _wtoi(xor_code_string);

			encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

			try
			{
				local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer, send_buffer_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

				local_data_size_send += local_bytes_sent;
			}
			catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
			{
				const int local_error_message_size = 10000;
				wchar_t local_error_message[local_error_message_size];

				const int local_system_error_message_size = local_error_message_size;
				wchar_t local_system_error_message[local_system_error_message_size];

				CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

				local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

				wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

				local_error_number = local_blocking_socket_exception->GetErrorNumber();

				local_blocking_socket_exception->Delete();

				if (local_error_number != 0)
				{
					//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
				}
			}

			local_blocking_socket.Close();

			if (local_bytes_sent <= 0 && local_error_number == 0)
			{
				//			galaxy::MessageBox(CString(L"local_bytes_sent<=0"),CString(L"Ошибка"));
			}

			if (local_main_dialog->get_command_threads_stop())
			{
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		if (local_main_dialog->get_command_threads_stop())
		{
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_send_thread_parameters_structure;
		return 1;
	}
}


//	stop

UINT __cdecl stop_waiting_thread(LPVOID parameter)
{
	thread_stop_parameters_structure_type *local_stop_thread_parameters_structure = (thread_stop_parameters_structure_type *)parameter;

	if (local_stop_thread_parameters_structure == NULL)
	{
		ExitProcess(0);
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_stop_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_stop_thread_parameters_structure;
		ExitProcess(0);
		return 0;
	}

	if (local_main_dialog->redirected_port_number != 0)
	{
		try
		{

			IGDDiscoverProcess discoverer(500000);

			tg_redirect_delete.create_thread(boost::ref(discoverer));
			tg_redirect_delete.join_all();

			IGDDiscoverProcess::IGDControlInformations cinfos =
				discoverer.GetAllIGDControlInformations();
			boost_foreach(IGDDiscoverProcess::ControlInfo info, cinfos)
			{
				if (info.has_problem)
				{
					continue;
				}

				if (info.service_type == UPNPSERVICE_LAYER3FORWARDING1)
				{
				}
				else if (info.service_type == UPNPSERVICE_WANIPCONNECTION1)
				{
					UPNPNATHTTPClient soap(info.ip, info.port);

					UPNPNATHTTPClient::SoapResult soap_res;
					soap_res = soap.DeletePortMapping(
						local_main_dialog->redirected_port_number, std::string("UDP"),
						info.control_url, info.service_type
					);
					if (UPNPNATHTTPClient::SoapSucceeded == soap_res)
					{
						CString local_message("portmapping succeeded.");
					}

					soap_res = soap.DeletePortMapping(
						local_main_dialog->redirected_port_number, std::string("TCP"),
						info.control_url, info.service_type
					);
					if (UPNPNATHTTPClient::SoapSucceeded == soap_res)
					{
						CString local_message("portmapping succeeded.");
					}
				}
				else if (info.service_type == UPNPSERVICE_WANCOMMONINTERFACECONFIG1)
				{
				}
				else if (info.service_type == UPNPSERVICE_WANPPPCONNECTION1)
				{
					UPNPNATHTTPClient soap(info.ip, info.port);

					UPNPNATHTTPClient::SoapResult soap_res;
					soap_res = soap.DeletePortMapping(
						local_main_dialog->redirected_port_number, std::string("UDP"),
						info.control_url, info.service_type
					);
					if (UPNPNATHTTPClient::SoapSucceeded == soap_res)
					{
						CString local_message("portmapping succeeded.");
					}

					soap_res = soap.DeletePortMapping(
						local_main_dialog->redirected_port_number, std::string("TCP"),
						info.control_url, info.service_type
					);
					if (UPNPNATHTTPClient::SoapSucceeded == soap_res)
					{
						CString local_message("portmapping succeeded.");
					}
				}
			}

		}
		catch (IGDDiscoverProcess::exception &e)
		{
			CString error(e.what());
		}
	}

	{
		Sleep(1000);

		CTime local_start_time = CTime::GetCurrentTime();

		for
			(
				;
				;
				)
		{
			CTime local_current_time = CTime::GetCurrentTime();

			CTimeSpan local_time_span = local_current_time - local_start_time;

			if (local_time_span.GetTotalSeconds() >= CONST_TERMINATION_WAIT_SECONDS)
			{
				break;
			}

			Sleep(100);

			CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);

			local_lock.Lock();

			size_t local_list_size = local_main_dialog->threads_list.size();

			std::list<THREADS_INFORMATION>::iterator local_threads_iterator;

			if (local_main_dialog->threads_list.size() == 0)
			{
				Sleep(10);
				break;
			}

			local_threads_iterator = local_main_dialog->threads_list.begin();

			if (local_threads_iterator != local_main_dialog->threads_list.end())
			{
				HANDLE local_thread = local_threads_iterator->WinThread;
				CString local_thread_name = local_threads_iterator->thread_name;

				local_lock.Unlock();

				DWORD local_result = WaitForSingleObject(local_thread, 10);
			}
			else
			{
				Sleep(10);

				CString local_error_message(L"Список threads_list запорчен (размер не нулевой и начало равно концу).");
				::MessageBoxW(NULL, local_error_message, L"Ошибка", MB_ICONSTOP);

				break;
			}
		}
	}

	CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);

	local_lock.Lock();

	INT local_threads_number_are_running = local_main_dialog->threads_list.size();

	local_lock.Unlock();

	if (local_threads_number_are_running != 0)
	{
		CString local_threads;
		{
			CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);

			local_lock.Lock();

			std::list<THREADS_INFORMATION>::iterator local_threads_iterator = local_main_dialog->threads_list.begin();

			for (; local_threads_iterator != local_main_dialog->threads_list.end(); local_threads_iterator++)
			{
				local_threads += local_threads_iterator->thread_name;
				local_threads += CString(L"\r\n");
			}
		}

		CString local_error_message;
		local_error_message.Format(L"%d рабочих потоков не завершено корректно!\r\nСписок имён незавершившихся вовремя потоков:\r\n%s", local_threads_number_are_running, local_threads.GetBuffer(local_threads.GetLength()));
		::MessageBoxW(NULL, local_error_message, L"Ошибка", MB_ICONSTOP);
	}

	{
		delete local_stop_thread_parameters_structure;

		WSACleanup();
	}

	/*//*/
	{
		//	Очистка памяти: удаление данных за последние CONST_EXPIRATION_DATA_LIMIT_SECONDS секунд при завершении программы
		std::list<STREAM_FRAME_INFORMATION>::iterator current_video_frame_stream = local_main_dialog->received_video_frame_stream.begin();

		for (; current_video_frame_stream != local_main_dialog->received_video_frame_stream.end(); current_video_frame_stream++)
		{
			if (local_main_dialog->received_video_frame_stream.size() != 0)
			{
				if (current_video_frame_stream->frames.size() != 0)
				{
					for
						(
							std::list<FRAME>::iterator current_video_frame = current_video_frame_stream->frames.begin()
							;
							current_video_frame != current_video_frame_stream->frames.end()
							;
							)
					{
						for (
							std::list<FRAME_PART>::iterator local_frame_parts_iterator = current_video_frame->frame_parts.begin()
							;
							local_frame_parts_iterator != current_video_frame->frame_parts.end()
							;
							local_frame_parts_iterator++
							)
						{
							delete[]local_frame_parts_iterator->frame_part_data;

							local_frame_parts_iterator->frame_part_data = NULL;
							local_frame_parts_iterator->frame_part_data_size = 0;

							//local_frame_parts_iterator = current_video_frame->frame_parts.erase(local_frame_parts_iterator);
						}

						current_video_frame = current_video_frame_stream->frames.erase(current_video_frame);
					}
				}
			}
		}
	}
	/*//*/

	if (local_main_dialog != NULL)
	{
		{
			if (local_main_dialog->received_video_dialog != NULL)
			{
				local_main_dialog->received_video_dialog->PostMessage(WM_CLOSE);
			}

			if (local_main_dialog->web_camera_dialog != NULL)
			{
				local_main_dialog->web_camera_dialog->StopMedia();
				local_main_dialog->web_camera_dialog->PostMessage(WM_CLOSE);
			}
		}

		CloseHandle(local_main_dialog->do_not_terminate_application_event);

		local_main_dialog->EndDialog(IDOK);
	}

	return 1;
}

//	Launchers

//	listen

UINT __cdecl datagram_listen_connection_thread(LPVOID parameter)
{
	thread_listen_parameters_structure_type *local_listen_thread_parameters_structure_source = (thread_listen_parameters_structure_type *)parameter;

	if (local_listen_thread_parameters_structure_source == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = local_listen_thread_parameters_structure_source->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete parameter;

		return 0;
	}

	{
		void *local_listen_thread_parameters_structure = new thread_listen_parameters_structure_type;

		((thread_listen_parameters_structure_type*)local_listen_thread_parameters_structure)->parameter_main_dialog = ((thread_listen_parameters_structure_type*)local_listen_thread_parameters_structure_source)->parameter_main_dialog;

		CWinThread *local_thread = AfxBeginThread(datagram_listen_connection_thread_ip_4, local_listen_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_listen_connection_thread_ip_4");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
		local_lock.Lock();

		local_main_dialog->threads_list.push_back(local_thread_information);
	}

	{
		void *local_listen_thread_parameters_structure = new thread_listen_parameters_structure_type;

		((thread_listen_parameters_structure_type*)local_listen_thread_parameters_structure)->parameter_main_dialog = ((thread_listen_parameters_structure_type*)local_listen_thread_parameters_structure_source)->parameter_main_dialog;

		CWinThread *local_thread = AfxBeginThread(datagram_listen_connection_thread_ip_6, local_listen_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_listen_connection_thread_ip_6");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
		local_lock.Lock();

		local_main_dialog->threads_list.push_back(local_thread_information);
	}


	{
		clear_thread_information(local_main_dialog);
	}

	delete parameter;

	return 1;
}

//	connect

UINT __cdecl datagram_connect_connection_thread(LPVOID parameter)
{
	thread_connect_parameters_structure_type *local_connect_thread_parameters_structure_source = (thread_connect_parameters_structure_type *)parameter;

	if (local_connect_thread_parameters_structure_source == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = local_connect_thread_parameters_structure_source->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete parameter;

		return 0;
	}

	{
		void *local_connect_thread_parameters_structure = new thread_connect_parameters_structure_type;

		((thread_connect_parameters_structure_type*)local_connect_thread_parameters_structure)->parameter_main_dialog = ((thread_connect_parameters_structure_type*)local_connect_thread_parameters_structure_source)->parameter_main_dialog;

		CWinThread *local_thread = AfxBeginThread(datagram_connect_connection_thread_ip_4, local_connect_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_connect_connection_thread_ip_4");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
		local_lock.Lock();

		local_main_dialog->threads_list.push_back(local_thread_information);
	}

	{
		void *local_connect_thread_parameters_structure = new thread_connect_parameters_structure_type;

		((thread_connect_parameters_structure_type*)local_connect_thread_parameters_structure)->parameter_main_dialog = ((thread_connect_parameters_structure_type*)local_connect_thread_parameters_structure_source)->parameter_main_dialog;

		CWinThread *local_thread = AfxBeginThread(datagram_connect_connection_thread_ip_6, local_connect_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_connect_connection_thread_ip_6");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
		local_lock.Lock();

		local_main_dialog->threads_list.push_back(local_thread_information);
	}

	{
		clear_thread_information(local_main_dialog);
	}

	delete parameter;

	return 1;
}

//	send

UINT __cdecl datagram_send_connection_thread(LPVOID parameter)
{
	thread_send_parameters_structure_type *local_send_thread_parameters_structure_source = (thread_send_parameters_structure_type *)parameter;

	if (local_send_thread_parameters_structure_source == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = local_send_thread_parameters_structure_source->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete parameter;

		return 0;
	}

	{
		void *local_send_thread_parameters_structure = new thread_send_parameters_structure_type;

		((thread_send_parameters_structure_type*)local_send_thread_parameters_structure)->parameter_main_dialog = ((thread_send_parameters_structure_type*)local_send_thread_parameters_structure_source)->parameter_main_dialog;

		CWinThread *local_thread = AfxBeginThread(datagram_send_connection_thread_ip_4, local_send_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_send_connection_thread_ip_4");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
		local_lock.Lock();

		local_main_dialog->threads_list.push_back(local_thread_information);
	}

	{
		void *local_send_thread_parameters_structure = new thread_send_parameters_structure_type;

		((thread_send_parameters_structure_type*)local_send_thread_parameters_structure)->parameter_main_dialog = ((thread_send_parameters_structure_type*)local_send_thread_parameters_structure_source)->parameter_main_dialog;

		CWinThread *local_thread = AfxBeginThread(datagram_send_connection_thread_ip_6, local_send_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_send_connection_thread_ip_6");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
		local_lock.Lock();

		local_main_dialog->threads_list.push_back(local_thread_information);
	}

	{
		clear_thread_information(local_main_dialog);
	}

	delete parameter;

	return 1;
}

UINT __cdecl datagram_act_on_request_connection_thread_ip_4(LPVOID parameter)
{
	thread_act_on_request_parameters_structure_type *local_act_on_request_thread_parameters_structure = (thread_act_on_request_parameters_structure_type *)parameter;

	if (local_act_on_request_thread_parameters_structure == NULL)
	{
		return 0;
	}

	char *parameter_buffer = ((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_buffer;
	int parameter_buffer_size = ((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_buffer_size;

	if (parameter_buffer == NULL || parameter_buffer_size <= 0)
	{
		delete local_act_on_request_thread_parameters_structure;
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_act_on_request_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete[] parameter_buffer;
		delete local_act_on_request_thread_parameters_structure;
		return 0;
	}

	for (;;)
	{
		//MessageBoxA(NULL,parameter_buffer,"Incomming connection",MB_ICONINFORMATION);

		if (memcmp(service_signature, parameter_buffer, service_signature_definition_length) == 0)
		{
			//galaxy::MessageBox(NULL,L"Data from client received",L"Incomming connection");

			if (((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_socket_address_peer_ip_4 == NULL)
			{
				break;
			}

			network::ip_4::CSockAddr_ip_4 parameter_socket_address_peer_ip_4(*((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_socket_address_peer_ip_4);

			CString local_port_string;

			local_port_string.Format(L"%d", UINT(parameter_socket_address_peer_ip_4.Port()));

			if (parameter_buffer_size == int(service_signature_definition_length))
			{
				CString local_chat;
				local_chat = CString(L"Партнёр подключился ") + CString(L"с адреса ") + parameter_socket_address_peer_ip_4.DottedDecimal() + CString(L" с порта ") + local_port_string + CString(L"\r\n");

				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
				{
					list_chat_AddString(local_main_dialog, local_chat);
				}
			}
			else
			{
				CString xor_code_string;

				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
				{

					xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
				}

				char xor_code = _wtoi(xor_code_string);

				if (parameter_buffer_size > int(service_signature_definition_length))
				{
					//CString local_chat;
					//local_chat = CString(L"Партнёр отправил сообщение ") + CString(L"с адреса ") + parameter_socket_address_peer_ip_4.DottedDecimal() + CString(L" с порта ") + local_port_string + CString(L"\r\n");

					if (local_main_dialog->get_command_terminate_application())
					{
						break;
					}
					{
						//list_chat_AddString(local_main_dialog,local_chat);
					}

					CString local_message((wchar_t*)(encrypt::encrypt_xor(parameter_buffer + service_signature_definition_length, parameter_buffer_size - service_signature_definition_length, xor_code)));

					if (local_message[0] != L'/')
					{
						CString local_chat;
						local_chat = CString(L"Партнёр отправил сообщение ") + CString(L"с адреса ") + parameter_socket_address_peer_ip_4.DottedDecimal() + CString(L" с порта ") + local_port_string + CString(L"\r\n");

						if (local_main_dialog->get_command_terminate_application())
						{
							break;
						}
						{
							list_chat_AddString(local_main_dialog, local_chat, false);
						}

						if (local_main_dialog->get_command_terminate_application())
						{
							break;
						}
						{
							local_chat = CString(L"Сообщение \"") + local_message + CString(L"\"\r\n");

							list_chat_AddString(local_main_dialog, local_chat, false);
						}
					}
					else
					{
						CString local_command_received;
						int local_buffer_parameter_offset = 0;

						for (int local_buffer_counter = 0; local_buffer_counter < local_message.GetLength(); local_buffer_counter++)
						{
							if (local_message[local_buffer_counter] == L' ')
							{
								local_buffer_parameter_offset = local_buffer_counter + 1;
								break;
							}
							else
							{
								local_buffer_parameter_offset++;
								local_command_received += local_message[local_buffer_counter];
							}
						}

						CString local_command_parameter_received;

						for (int local_buffer_counter = local_buffer_parameter_offset; local_buffer_counter < local_message.GetLength(); local_buffer_counter++)
						{
							if (local_message[local_buffer_counter] == L' ')
							{
								local_buffer_parameter_offset++;
							}
							else
							{
								break;
							}
						}

						for (int local_buffer_counter = local_buffer_parameter_offset; local_buffer_counter < local_message.GetLength(); local_buffer_counter++)
						{
							if (local_message[local_buffer_counter] == L' ')
							{
								break;
							}
							else
							{
								local_command_parameter_received += local_message[local_buffer_counter];
							}
						}

						if (local_command_received == command_register)
						{
							CString local_chat;
							local_chat = CString(L"Партнёр отправил сообщение ") + CString(L"с адреса ") + parameter_socket_address_peer_ip_4.DottedDecimal() + CString(L" с порта ") + local_port_string + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							local_chat = CString(L"Команда \"") + local_command_received + CString(L"\"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							if (local_command_parameter_received.GetLength() == 0)
							{
								local_chat = CString(L"Параметр \"") + parameter_socket_address_peer_ip_4.DottedDecimal() + CString(L"\"\r\n");

								if (local_main_dialog->get_command_terminate_application())
								{
									break;
								}
								{
									list_chat_AddString(local_main_dialog, local_chat);
								}

								if (local_main_dialog->get_command_terminate_application())
								{
									break;
								}
								{
									GUI_LIST_CONTROL socket_address_peer_ip_4_state;
									socket_address_peer_ip_4_state.label = parameter_socket_address_peer_ip_4.DottedDecimal();
									socket_address_peer_ip_4_state.is_checked = FALSE;
									local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.push_back(socket_address_peer_ip_4_state);
								}
							}
							else
							{
								local_chat = CString(L"Параметр \"") + local_command_parameter_received + CString(L"\"\r\n");

								if (local_main_dialog->get_command_terminate_application())
								{
									break;
								}
								{
									list_chat_AddString(local_main_dialog, local_chat);
								}

								if (local_main_dialog->get_command_terminate_application())
								{
									break;
								}
								{
									GUI_LIST_CONTROL command_parameter_received_state;
									command_parameter_received_state.label = local_command_parameter_received;
									command_parameter_received_state.is_checked = FALSE;
									local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.push_back(command_parameter_received_state);
								}
							}
						}
						if (local_command_received == command_execute_application)
						{
							CString local_chat;
							local_chat = CString(L"Партнёр отправил сообщение ") + CString(L"с адреса ") + parameter_socket_address_peer_ip_4.DottedDecimal() + CString(L" с порта ") + local_port_string + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							local_chat = CString(L"Команда \"") + local_command_received + CString(L"\"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							if (local_command_parameter_received.GetLength() == 0)
							{
								local_chat = CString(L"Параметр отсутствует (ошибка)") + CString(L"\r\n");

								if (local_main_dialog->get_command_terminate_application())
								{
									break;
								}
								{
									list_chat_AddString(local_main_dialog, local_chat);
								}

								if (local_main_dialog->get_command_terminate_application())
								{
									break;
								}
							}
							else
							{
								local_chat = CString(L"Параметр \"") + local_command_parameter_received + CString(L"\"\r\n");

								if (local_main_dialog->get_command_terminate_application())
								{
									break;
								}
								{
									list_chat_AddString(local_main_dialog, local_chat);
								}

								if (local_main_dialog->get_command_terminate_application())
								{
									break;
								}
								{
									ShellExecute(NULL, CString("open"), local_command_parameter_received, NULL, NULL, SW_SHOWNORMAL);
								}
							}
						}
						if (local_command_received == command_video)
						{
							size_t data_buffer_offset = service_signature_definition_length + local_command_received.GetLength() * sizeof(wchar_t) + sizeof(wchar_t);
							size_t data_buffer_length = parameter_buffer_size - data_buffer_offset;
							BYTE *data_buffer = new BYTE[data_buffer_length];
							memcpy(data_buffer, parameter_buffer + data_buffer_offset, data_buffer_length);

							//							CString local_chat;
							//							local_chat = CString(L"Команда \"") + local_command_received + CString(L"\"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								//list_chat_AddString(local_main_dialog,local_chat);
							}
							//							local_chat = CString(L"От ") + parameter_socket_address_peer_ip_4.DottedDecimal() + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								//list_chat_AddString(local_main_dialog,local_chat);
							}
							//							if(local_command_parameter_received.GetLength()==0)
							{
								local_main_dialog->PrepareVideo(parameter_socket_address_peer_ip_4.DottedDecimal(), data_buffer, data_buffer_length);
							}
						}
						if (local_command_received == command_video_end)
						{
							size_t data_buffer_offset = service_signature_definition_length + local_command_received.GetLength() * sizeof(wchar_t) + sizeof(wchar_t);
							size_t data_buffer_length = parameter_buffer_size - data_buffer_offset;
							BYTE *data_buffer = new BYTE[data_buffer_length];
							memcpy(data_buffer, parameter_buffer + data_buffer_offset, data_buffer_length);

							CString local_chat;
							local_chat = CString(L"Партнёр отправил сообщение ") + CString(L"с адреса ") + parameter_socket_address_peer_ip_4.DottedDecimal() + CString(L" с порта ") + local_port_string + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							local_chat = CString(L"Команда \"") + local_command_received + CString(L"\"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							local_chat = CString(L"От ") + parameter_socket_address_peer_ip_4.DottedDecimal() + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							//							if(local_command_parameter_received.GetLength()==0)
							{
								local_main_dialog->DrawVideo(parameter_socket_address_peer_ip_4.DottedDecimal(), data_buffer, data_buffer_length);
							}
						}
						if (local_command_received == command_web_camera_video)
						{
							size_t data_buffer_offset = service_signature_definition_length + local_command_received.GetLength() * sizeof(wchar_t) + sizeof(wchar_t);
							size_t data_buffer_length = parameter_buffer_size - data_buffer_offset;
							BYTE *data_buffer = new BYTE[data_buffer_length];
							memcpy(data_buffer, parameter_buffer + data_buffer_offset, data_buffer_length);

							//							CString local_chat;
							//							local_chat = CString(L"Команда \"") + local_command_received + CString(L"\"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								//list_chat_AddString(local_main_dialog,local_chat);								
							}
							//							local_chat = CString(L"От ") + parameter_socket_address_peer_ip_4.DottedDecimal() + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								//list_chat_AddString(local_main_dialog,local_chat);
							}
							//							if(local_command_parameter_received.GetLength()==0)
							{
								local_main_dialog->PrepareWebCameraVideo(parameter_socket_address_peer_ip_4.DottedDecimal(), data_buffer, data_buffer_length);
							}
						}
						if (local_command_received == command_web_camera_video_end)
						{
							size_t data_buffer_offset = service_signature_definition_length + local_command_received.GetLength() * sizeof(wchar_t) + sizeof(wchar_t);
							size_t data_buffer_length = parameter_buffer_size - data_buffer_offset;
							BYTE *data_buffer = new BYTE[data_buffer_length];
							memcpy(data_buffer, parameter_buffer + data_buffer_offset, data_buffer_length);

							CString local_chat;
							local_chat = CString(L"Партнёр отправил сообщение ") + CString(L"с адреса ") + parameter_socket_address_peer_ip_4.DottedDecimal() + CString(L" с порта ") + local_port_string + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							local_chat = CString(L"Команда \"") + local_command_received + CString(L"\"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							local_chat = CString(L"От ") + parameter_socket_address_peer_ip_4.DottedDecimal() + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							//							if(local_command_parameter_received.GetLength()==0)
							{
								local_main_dialog->DrawWebCameraVideo(parameter_socket_address_peer_ip_4.DottedDecimal(), data_buffer, data_buffer_length);
							}
						}
						if (local_command_received == command_audio)
						{
							size_t data_buffer_offset = service_signature_definition_length + local_command_received.GetLength() * sizeof(wchar_t) + sizeof(wchar_t);
							size_t data_buffer_length = parameter_buffer_size - data_buffer_offset;
							BYTE *data_buffer = new BYTE[data_buffer_length];
							memcpy(data_buffer, parameter_buffer + data_buffer_offset, data_buffer_length);

							//CString local_chat;
							//local_chat = CString(L"Команда \"") + local_command_received + CString(L"\"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								//list_chat_AddString(local_main_dialog,local_chat);
							}
							//local_chat = CString(L"От ") + parameter_socket_address_peer_ip_4.DottedDecimal() + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								//list_chat_AddString(local_main_dialog,local_chat);
							}
							//if(local_command_parameter_received.GetLength()==0)
							{
								local_main_dialog->PrepareAudio(parameter_socket_address_peer_ip_4.DottedDecimal(), data_buffer, data_buffer_length);
							}
						}
						if (local_command_received == command_audio_end)
						{
							size_t data_buffer_offset = service_signature_definition_length + local_command_received.GetLength() * sizeof(wchar_t) + sizeof(wchar_t);
							size_t data_buffer_length = parameter_buffer_size - data_buffer_offset;
							BYTE *data_buffer = new BYTE[data_buffer_length];
							memcpy(data_buffer, parameter_buffer + data_buffer_offset, data_buffer_length);

							CString local_chat;
							local_chat = CString(L"Партнёр отправил сообщение ") + CString(L"с адреса ") + parameter_socket_address_peer_ip_4.DottedDecimal() + CString(L" с порта ") + local_port_string + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							local_chat = CString(L"Команда \"") + local_command_received + CString(L"\"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							local_chat = CString(L"От ") + parameter_socket_address_peer_ip_4.DottedDecimal() + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							//							if(local_command_parameter_received.GetLength()==0)
							{
								local_main_dialog->PlayAudio(parameter_socket_address_peer_ip_4.DottedDecimal(), data_buffer, data_buffer_length);
							}
						}
					}
				}
			}

			delete ((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_socket_address_peer_ip_4;
			((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_socket_address_peer_ip_4 = NULL;
		}
		break;
	}

	{
		if (((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_socket_address_peer_ip_4 != NULL)
		{
			delete ((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_socket_address_peer_ip_4;
			((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_socket_address_peer_ip_4 = NULL;
		}

		{
			clear_thread_information(local_main_dialog);
		}

		delete[]parameter_buffer;
		delete local_act_on_request_thread_parameters_structure;
		return 1;
	}
}

UINT __cdecl datagram_act_on_request_connection_thread_ip_6(LPVOID parameter)
{
	thread_act_on_request_parameters_structure_type *local_act_on_request_thread_parameters_structure = (thread_act_on_request_parameters_structure_type *)parameter;

	if (local_act_on_request_thread_parameters_structure == NULL)
	{
		return 0;
	}

	char *parameter_buffer = ((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_buffer;
	int parameter_buffer_size = ((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_buffer_size;

	if (parameter_buffer == NULL || parameter_buffer_size <= 0)
	{
		delete local_act_on_request_thread_parameters_structure;
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_act_on_request_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete[] parameter_buffer;
		delete local_act_on_request_thread_parameters_structure;
		return 0;
	}

	for (;;)
	{
		//MessageBoxA(NULL,parameter_buffer,"Incomming connection",MB_ICONINFORMATION);

		if (memcmp(service_signature, parameter_buffer, service_signature_definition_length) == 0)
		{
			//galaxy::MessageBox(NULL,L"Data from client received",L"Incomming connection");

			if (((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_socket_address_peer_ip_6 == NULL)
			{
				break;
			}

			network::ip_6::CSockAddr_ip_6 parameter_socket_address_peer_ip_6(*((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_socket_address_peer_ip_6);

			CString local_port_string;

			local_port_string.Format(L"%d", UINT(parameter_socket_address_peer_ip_6.Port()));

			if (parameter_buffer_size == int(service_signature_definition_length))
			{
				CString local_chat;
				local_chat = CString(L"Партнёр подключился ") + CString(L"с адреса ") + parameter_socket_address_peer_ip_6.DottedDecimal() + CString(L" с порта ") + local_port_string + CString(L"\r\n");

				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
				{
					list_chat_AddString(local_main_dialog, local_chat);
				}
			}
			else
			{
				CString xor_code_string;

				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
				{
					xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
				}

				char xor_code = _wtoi(xor_code_string);

				if (parameter_buffer_size > int(service_signature_definition_length))
				{
					//CString local_chat;
					//local_chat = CString(L"Партнёр отправил сообщение ") + CString(L"с адреса ") + parameter_socket_address_peer_ip_6.DottedDecimal() + CString(L" с порта ") + local_port_string + CString(L"\r\n");

					if (local_main_dialog->get_command_terminate_application())
					{
						break;
					}
					{
						//list_chat_AddString(local_main_dialog,local_chat);
					}

					CString local_message((wchar_t*)(encrypt::encrypt_xor(parameter_buffer + service_signature_definition_length, parameter_buffer_size - service_signature_definition_length, xor_code)));

					if (local_message[0] != L'/')
					{
						CString local_chat;
						local_chat = CString(L"Партнёр отправил сообщение ") + CString(L"с адреса ") + parameter_socket_address_peer_ip_6.DottedDecimal() + CString(L" с порта ") + local_port_string + CString(L"\r\n");

						if (local_main_dialog->get_command_terminate_application())
						{
							break;
						}
						{
							list_chat_AddString(local_main_dialog, local_chat);
						}

						if (local_main_dialog->get_command_terminate_application())
						{
							break;
						}
						{
							local_chat = CString(L"Сообщение \"") + local_message + CString(L"\"\r\n");

							list_chat_AddString(local_main_dialog, local_chat);
						}
					}
					else
					{
						CString local_command_received;
						int local_buffer_parameter_offset = 0;

						for (int local_buffer_counter = 0; local_buffer_counter < local_message.GetLength(); local_buffer_counter++)
						{
							if (local_message[local_buffer_counter] == L' ')
							{
								local_buffer_parameter_offset = local_buffer_counter + 1;
								break;
							}
							else
							{
								local_buffer_parameter_offset++;
								local_command_received += local_message[local_buffer_counter];
							}
						}

						CString local_command_parameter_received;

						for (int local_buffer_counter = local_buffer_parameter_offset; local_buffer_counter < local_message.GetLength(); local_buffer_counter++)
						{
							if (local_message[local_buffer_counter] == L' ')
							{
								local_buffer_parameter_offset++;
							}
							else
							{
								break;
							}
						}

						for (int local_buffer_counter = local_buffer_parameter_offset; local_buffer_counter < local_message.GetLength(); local_buffer_counter++)
						{
							if (local_message[local_buffer_counter] == L' ')
							{
								break;
							}
							else
							{
								local_command_parameter_received += local_message[local_buffer_counter];
							}
						}

						if (local_command_received == command_register)
						{
							CString local_chat;
							local_chat = CString(L"Партнёр отправил сообщение ") + CString(L"с адреса ") + parameter_socket_address_peer_ip_6.DottedDecimal() + CString(L" с порта ") + local_port_string + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							local_chat = CString(L"Команда \"") + local_command_received + CString(L"\"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							if (local_command_parameter_received.GetLength() == 0)
							{
								local_chat = CString(L"Параметр \"") + parameter_socket_address_peer_ip_6.DottedDecimal() + CString(L"\"\r\n");

								if (local_main_dialog->get_command_terminate_application())
								{
									break;
								}
								{
									list_chat_AddString(local_main_dialog, local_chat);
								}

								if (local_main_dialog->get_command_terminate_application())
								{
									break;
								}
								{
									GUI_LIST_CONTROL socket_address_peer_ip_6_state;
									socket_address_peer_ip_6_state.label = parameter_socket_address_peer_ip_6.DottedDecimal();
									socket_address_peer_ip_6_state.is_checked = FALSE;
									local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.push_back(socket_address_peer_ip_6_state);
								}
							}
							else
							{
								local_chat = CString(L"Параметр \"") + local_command_parameter_received + CString(L"\"\r\n");

								if (local_main_dialog->get_command_terminate_application())
								{
									break;
								}
								{
									list_chat_AddString(local_main_dialog, local_chat);
								}

								if (local_main_dialog->get_command_terminate_application())
								{
									break;
								}
								{
									GUI_LIST_CONTROL command_parameter_received_state;
									command_parameter_received_state.label = local_command_parameter_received;
									command_parameter_received_state.is_checked = FALSE;
									local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.push_back(command_parameter_received_state);
								}
							}
						}
						if (local_command_received == command_execute_application)
						{
							CString local_chat;
							local_chat = CString(L"Партнёр отправил сообщение ") + CString(L"с адреса ") + parameter_socket_address_peer_ip_6.DottedDecimal() + CString(L" с порта ") + local_port_string + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							local_chat = CString(L"Команда \"") + local_command_received + CString(L"\"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							if (local_command_parameter_received.GetLength() == 0)
							{
								local_chat = CString(L"Параметр отсутствует (ошибка)") + CString(L"\r\n");

								if (local_main_dialog->get_command_terminate_application())
								{
									break;
								}
								{
									list_chat_AddString(local_main_dialog, local_chat);
								}

								if (local_main_dialog->get_command_terminate_application())
								{
									break;
								}
							}
							else
							{
								local_chat = CString(L"Параметр \"") + local_command_parameter_received + CString(L"\"\r\n");

								if (local_main_dialog->get_command_terminate_application())
								{
									break;
								}
								{
									list_chat_AddString(local_main_dialog, local_chat);
								}

								if (local_main_dialog->get_command_terminate_application())
								{
									break;
								}
								{
									ShellExecute(NULL, CString("open"), local_command_parameter_received, NULL, NULL, SW_SHOWNORMAL);
								}
							}
						}
						if (local_command_received == command_video)
						{
							size_t data_buffer_offset = service_signature_definition_length + local_command_received.GetLength() * sizeof(wchar_t) + sizeof(wchar_t);
							size_t data_buffer_length = parameter_buffer_size - data_buffer_offset;
							BYTE *data_buffer = new BYTE[data_buffer_length];
							memcpy(data_buffer, parameter_buffer + data_buffer_offset, data_buffer_length);

							//							CString local_chat;
							//							local_chat = CString(L"Команда \"") + local_command_received + CString(L"\"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								//list_chat_AddString(local_main_dialog,local_chat);
							}
							//							local_chat = CString(L"От ") + parameter_socket_address_peer_ip_6.DottedDecimal() + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								//list_chat_AddString(local_main_dialog,local_chat);
							}
							//							if(local_command_parameter_received.GetLength()==0)
							{
								local_main_dialog->PrepareVideo(parameter_socket_address_peer_ip_6.DottedDecimal(), data_buffer, data_buffer_length);
							}
						}
						if (local_command_received == command_video_end)
						{
							size_t data_buffer_offset = service_signature_definition_length + local_command_received.GetLength() * sizeof(wchar_t) + sizeof(wchar_t);
							size_t data_buffer_length = parameter_buffer_size - data_buffer_offset;
							BYTE *data_buffer = new BYTE[data_buffer_length];
							memcpy(data_buffer, parameter_buffer + data_buffer_offset, data_buffer_length);

							CString local_chat;
							local_chat = CString(L"Партнёр отправил сообщение ") + CString(L"с адреса ") + parameter_socket_address_peer_ip_6.DottedDecimal() + CString(L" с порта ") + local_port_string + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							local_chat = CString(L"Команда \"") + local_command_received + CString(L"\"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							local_chat = CString(L"От ") + parameter_socket_address_peer_ip_6.DottedDecimal() + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							//							if(local_command_parameter_received.GetLength()==0)
							{
								local_main_dialog->DrawVideo(parameter_socket_address_peer_ip_6.DottedDecimal(), data_buffer, data_buffer_length);
							}
						}
						if (local_command_received == command_web_camera_video)
						{
							size_t data_buffer_offset = service_signature_definition_length + local_command_received.GetLength() * sizeof(wchar_t) + sizeof(wchar_t);
							size_t data_buffer_length = parameter_buffer_size - data_buffer_offset;
							BYTE *data_buffer = new BYTE[data_buffer_length];
							memcpy(data_buffer, parameter_buffer + data_buffer_offset, data_buffer_length);

							//							CString local_chat;
							//							local_chat = CString(L"Команда \"") + local_command_received + CString(L"\"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								//list_chat_AddString(local_main_dialog,local_chat);
							}
							//							local_chat = CString(L"От ") + parameter_socket_address_peer_ip_6.DottedDecimal() + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								//list_chat_AddString(local_main_dialog,local_chat);
							}
							//							if(local_command_parameter_received.GetLength()==0)
							{
								local_main_dialog->PrepareWebCameraVideo(parameter_socket_address_peer_ip_6.DottedDecimal(), data_buffer, data_buffer_length);
							}
						}
						if (local_command_received == command_web_camera_video_end)
						{
							size_t data_buffer_offset = service_signature_definition_length + local_command_received.GetLength() * sizeof(wchar_t) + sizeof(wchar_t);
							size_t data_buffer_length = parameter_buffer_size - data_buffer_offset;
							BYTE *data_buffer = new BYTE[data_buffer_length];
							memcpy(data_buffer, parameter_buffer + data_buffer_offset, data_buffer_length);

							CString local_chat;
							local_chat = CString(L"Партнёр отправил сообщение ") + CString(L"с адреса ") + parameter_socket_address_peer_ip_6.DottedDecimal() + CString(L" с порта ") + local_port_string + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							local_chat = CString(L"Команда \"") + local_command_received + CString(L"\"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							local_chat = CString(L"От ") + parameter_socket_address_peer_ip_6.DottedDecimal() + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							//							if(local_command_parameter_received.GetLength()==0)
							{
								local_main_dialog->DrawWebCameraVideo(parameter_socket_address_peer_ip_6.DottedDecimal(), data_buffer, data_buffer_length);
							}
						}
						if (local_command_received == command_audio)
						{
							size_t data_buffer_offset = service_signature_definition_length + local_command_received.GetLength() * sizeof(wchar_t) + sizeof(wchar_t);
							size_t data_buffer_length = parameter_buffer_size - data_buffer_offset;
							BYTE *data_buffer = new BYTE[data_buffer_length];
							memcpy(data_buffer, parameter_buffer + data_buffer_offset, data_buffer_length);

							//CString local_chat;
							//local_chat = CString(L"Команда \"") + local_command_received + CString(L"\"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								//list_chat_AddString(local_main_dialog,local_chat);
							}
							//local_chat = CString(L"От ") + parameter_socket_address_peer_ip_6.DottedDecimal() + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								//list_chat_AddString(local_main_dialog,local_chat);
							}
							//if(local_command_parameter_received.GetLength()==0)
							{
								local_main_dialog->PrepareAudio(parameter_socket_address_peer_ip_6.DottedDecimal(), data_buffer, data_buffer_length);
							}
						}
						if (local_command_received == command_audio_end)
						{
							size_t data_buffer_offset = service_signature_definition_length + local_command_received.GetLength() * sizeof(wchar_t) + sizeof(wchar_t);
							size_t data_buffer_length = parameter_buffer_size - data_buffer_offset;
							BYTE *data_buffer = new BYTE[data_buffer_length];
							memcpy(data_buffer, parameter_buffer + data_buffer_offset, data_buffer_length);

							CString local_chat;
							local_chat = CString(L"Партнёр отправил сообщение ") + CString(L"с адреса ") + parameter_socket_address_peer_ip_6.DottedDecimal() + CString(L" с порта ") + local_port_string + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							local_chat = CString(L"Команда \"") + local_command_received + CString(L"\"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							local_chat = CString(L"От ") + parameter_socket_address_peer_ip_6.DottedDecimal() + CString(L"\r\n");

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								list_chat_AddString(local_main_dialog, local_chat);
							}
							//							if(local_command_parameter_received.GetLength()==0)
							{
								local_main_dialog->PlayAudio(parameter_socket_address_peer_ip_6.DottedDecimal(), data_buffer, data_buffer_length);
							}
						}
					}
				}
			}

			delete ((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_socket_address_peer_ip_6;
			((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_socket_address_peer_ip_6 = NULL;
		}

		break;
	}

	{
		if (((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_socket_address_peer_ip_6 != NULL)
		{
			delete ((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_socket_address_peer_ip_6;
			((thread_act_on_request_parameters_structure_type*)local_act_on_request_thread_parameters_structure)->parameter_socket_address_peer_ip_6 = NULL;
		}

		{
			clear_thread_information(local_main_dialog);
		}

		delete[]parameter_buffer;
		delete local_act_on_request_thread_parameters_structure;
		return 1;
	}
}


UINT __cdecl datagram_connect_connection_thread_loop_ip_4(LPVOID parameter)
{
	thread_connect_parameters_structure_loop_type *local_connect_thread_parameters_structure_loop = (thread_connect_parameters_structure_loop_type *)parameter;

	if (local_connect_thread_parameters_structure_loop == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = local_connect_thread_parameters_structure_loop->thread_connect_parameters_structure_parameter.parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_connect_thread_parameters_structure_loop;
		return 0;
	}
	network::ip_4::CBlockingSocket_ip_4 local_blocking_socket;

	if (network::ip_4::domain_name_to_internet_4_name(local_connect_thread_parameters_structure_loop->local_address, local_connect_thread_parameters_structure_loop->local_address_internet_address) == false)
	{
		const int local_error_message_size = 10000;
		wchar_t local_error_message[local_error_message_size];

		const int local_system_error_message_size = local_error_message_size - 1000;
		wchar_t local_system_error_message[local_system_error_message_size];

		wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

		CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

		wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

		delete local_connect_thread_parameters_structure_loop;

		//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

		{
			clear_thread_information(local_main_dialog);
		}

		return 0;
	}

	network::ip_4::CSockAddr_ip_4 local_socket_address_temp(local_connect_thread_parameters_structure_loop->local_address_internet_address, local_connect_thread_parameters_structure_loop->local_parameter_port_number);

	CStringA local_address_internet_address_temp = CStringA(local_socket_address_temp.DottedDecimal());
	for (int local_char_counter = local_address_internet_address_temp.GetLength(); local_char_counter >= 0; local_char_counter--)
	{
		CStringA local_address_internet_address_temp_1;
		for (int local_char_counter_1 = 0; local_char_counter_1 < local_char_counter - 1; local_char_counter_1++)
		{
			local_address_internet_address_temp_1 += local_address_internet_address_temp.GetAt(local_char_counter_1);
		}
		if (local_address_internet_address_temp_1.GetAt(local_address_internet_address_temp_1.GetLength() - 1) == '.')
		{
			local_address_internet_address_temp = local_address_internet_address_temp_1;
			break;
		}
	}
	local_connect_thread_parameters_structure_loop->local_address_internet_address.Format("%s%d", local_address_internet_address_temp, local_connect_thread_parameters_structure_loop->local_low_subnet_temp_number);

	network::ip_4::CSockAddr_ip_4 local_socket_address(local_connect_thread_parameters_structure_loop->local_address_internet_address, local_connect_thread_parameters_structure_loop->local_parameter_port_number);

	double local_data_size_send = 0.0;

	try
	{
		local_blocking_socket.Create(SOCK_DGRAM);
	}
	catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
	{
		const int local_error_message_size = 10000;
		wchar_t local_error_message[local_error_message_size];

		const int local_system_error_message_size = local_error_message_size - 1000;
		wchar_t local_system_error_message[local_system_error_message_size];

		CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

		local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

		wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

		local_blocking_socket_exception->Delete();

		delete local_connect_thread_parameters_structure_loop;

		//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

		{
			clear_thread_information(local_main_dialog);
		}

		return 0;
	}

	int local_bytes_sent = 0;
	int local_error_number = 0;

	try
	{
		local_bytes_sent = local_blocking_socket.SendDatagram(service_signature, service_signature_definition_length, local_socket_address, CONST_WAIT_TIME_SEND);

		local_data_size_send += local_bytes_sent;
	}
	catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
	{
		const int local_error_message_size = 10000;
		wchar_t local_error_message[local_error_message_size];

		const int local_system_error_message_size = local_error_message_size;
		wchar_t local_system_error_message[local_system_error_message_size];

		CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

		local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

		wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

		local_error_number = local_blocking_socket_exception->GetErrorNumber();

		local_blocking_socket_exception->Delete();

		if (local_error_number != 0)
		{
			//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
		}
	}

	local_blocking_socket.Close();

	if (local_bytes_sent <= 0 && local_error_number == 0)
	{
		//			galaxy::MessageBox(CString(L"local_bytes_sent<=0"),CString(L"Ошибка"));
	}

	{

		{
			clear_thread_information(local_main_dialog);
		}

		delete local_connect_thread_parameters_structure_loop;
		return 1;
	}
}

UINT __cdecl datagram_connect_connection_thread_loop_ip_6(LPVOID parameter)
{
	thread_connect_parameters_structure_loop_type *local_connect_thread_parameters_structure_loop = (thread_connect_parameters_structure_loop_type *)parameter;

	if (local_connect_thread_parameters_structure_loop == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = local_connect_thread_parameters_structure_loop->thread_connect_parameters_structure_parameter.parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_connect_thread_parameters_structure_loop;
		return 0;
	}

	network::ip_6::CBlockingSocket_ip_6 local_blocking_socket;

	if (network::ip_6::domain_name_to_internet_6_name(local_connect_thread_parameters_structure_loop->local_address, local_connect_thread_parameters_structure_loop->local_address_internet_address) == false)
	{
		const int local_error_message_size = 10000;
		wchar_t local_error_message[local_error_message_size];

		const int local_system_error_message_size = local_error_message_size - 1000;
		wchar_t local_system_error_message[local_system_error_message_size];

		wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

		CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

		wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

		delete local_connect_thread_parameters_structure_loop;

		//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

		{
			clear_thread_information(local_main_dialog);
		}

		return 0;
	}

	network::ip_6::CSockAddr_ip_6 local_socket_address_temp(local_connect_thread_parameters_structure_loop->local_address_internet_address, local_connect_thread_parameters_structure_loop->local_parameter_port_number);

	CStringA local_address_internet_address_temp = CStringA(local_socket_address_temp.DottedDecimal());
	for (int local_char_counter = local_address_internet_address_temp.GetLength(); local_char_counter >= 0; local_char_counter--)
	{
		CStringA local_address_internet_address_temp_1;
		for (int local_char_counter_1 = 0; local_char_counter_1 < local_char_counter - 1; local_char_counter_1++)
		{
			local_address_internet_address_temp_1 += local_address_internet_address_temp.GetAt(local_char_counter_1);
		}
		if (local_address_internet_address_temp_1.GetAt(local_address_internet_address_temp_1.GetLength() - 1) == ':')
		{
			local_address_internet_address_temp = local_address_internet_address_temp_1;
			break;
		}
	}
	local_connect_thread_parameters_structure_loop->local_address_internet_address.Format("%s%x", local_address_internet_address_temp, local_connect_thread_parameters_structure_loop->local_low_subnet_temp_number);

	network::ip_6::CSockAddr_ip_6 local_socket_address(local_connect_thread_parameters_structure_loop->local_address_internet_address, local_connect_thread_parameters_structure_loop->local_parameter_port_number);

	double local_data_size_send = 0.0;

	try
	{
		local_blocking_socket.Create(SOCK_DGRAM);
	}
	catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
	{
		const int local_error_message_size = 10000;
		wchar_t local_error_message[local_error_message_size];

		const int local_system_error_message_size = local_error_message_size - 1000;
		wchar_t local_system_error_message[local_system_error_message_size];

		CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

		local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

		wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

		local_blocking_socket_exception->Delete();

		delete local_connect_thread_parameters_structure_loop;

		//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

		{
			clear_thread_information(local_main_dialog);
		}

		return 0;
	}

	int local_bytes_sent = 0;
	int local_error_number = 0;

	try
	{
		local_bytes_sent = local_blocking_socket.SendDatagram(service_signature, service_signature_definition_length, local_socket_address, CONST_WAIT_TIME_SEND);

		local_data_size_send += local_bytes_sent;
	}
	catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
	{
		const int local_error_message_size = 10000;
		wchar_t local_error_message[local_error_message_size];

		const int local_system_error_message_size = local_error_message_size;
		wchar_t local_system_error_message[local_system_error_message_size];

		CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

		local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

		wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

		local_error_number = local_blocking_socket_exception->GetErrorNumber();

		local_blocking_socket_exception->Delete();

		if (local_error_number != 0)
		{
			//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
		}
	}

	local_blocking_socket.Close();

	if (local_bytes_sent <= 0 && local_error_number == 0)
	{
		//			galaxy::MessageBox(CString(L"local_bytes_sent<=0"),CString(L"Ошибка"));
	}

	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_connect_thread_parameters_structure_loop;
		return 1;
	}
}

void Cstl_network_ip_4_ip_6_udp_engineDialog::OnClose()
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного

	set_command_terminate_application(true);

	set_command_threads_retranslate_video_stop(true);
	set_command_threads_retranslate_web_camera_video_stop(true);
	set_command_threads_retranslate_audio_stop(true);

	set_command_threads_video_stop(true);
	set_command_threads_web_camera_video_stop(true);
	set_command_threads_audio_stop(true);
	set_command_threads_stop(true);


	ResetEvent(do_not_terminate_application_event);

	{
		void *local_stop_thread_parameters_structure = new thread_stop_parameters_structure_type;

		((thread_stop_parameters_structure_type*)local_stop_thread_parameters_structure)->parameter_main_dialog = this;

		CWinThread *local_thread = AfxBeginThread(stop_waiting_thread, local_stop_thread_parameters_structure);

		//{
		//	DWORD local_result = WaitForSingleObject(local_thread->m_hThread,INFINITE);
		//}
	}


	//{
	//	if(received_video_dialog!=NULL)
	//	{
	//		received_video_dialog->PostMessage(WM_CLOSE);
	//	}

	//	if(web_camera_dialog!=NULL)
	//	{
	//		web_camera_dialog->StopMedia();
	//		web_camera_dialog->PostMessage(WM_CLOSE);
	//	}
	//}

	//CloseHandle(do_not_terminate_application_event);

	//EndDialog(IDOK);
}

//	register

UINT __cdecl datagram_register_thread(LPVOID parameter)
{
	thread_register_parameters_structure_type *local_register_thread_parameters_structure_source = (thread_register_parameters_structure_type *)parameter;

	if (local_register_thread_parameters_structure_source == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = local_register_thread_parameters_structure_source->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete parameter;

		return 0;
	}

	{
		void *local_register_thread_parameters_structure = new thread_register_parameters_structure_type;

		((thread_register_parameters_structure_type*)local_register_thread_parameters_structure)->parameter_main_dialog = ((thread_register_parameters_structure_type*)local_register_thread_parameters_structure_source)->parameter_main_dialog;

		CWinThread *local_thread = AfxBeginThread(datagram_register_thread_ip_4, local_register_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_register_thread_ip_4");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
		local_lock.Lock();

		local_main_dialog->threads_list.push_back(local_thread_information);
	}

	{
		void *local_register_thread_parameters_structure = new thread_register_parameters_structure_type;

		((thread_register_parameters_structure_type*)local_register_thread_parameters_structure)->parameter_main_dialog = ((thread_register_parameters_structure_type*)local_register_thread_parameters_structure_source)->parameter_main_dialog;

		CWinThread *local_thread = AfxBeginThread(datagram_register_thread_ip_6, local_register_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_register_thread_ip_6");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
		local_lock.Lock();

		local_main_dialog->threads_list.push_back(local_thread_information);
	}

	{
		clear_thread_information(local_main_dialog);
	}

	delete parameter;

	return 1;
}

UINT __cdecl datagram_register_thread_ip_4(LPVOID parameter)
{
	thread_register_parameters_structure_type *local_register_thread_parameters_structure = (thread_register_parameters_structure_type *)parameter;

	if (local_register_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_register_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_register_thread_parameters_structure;
		return 0;
	}

	CString local_address(localhost_definition);

	CStringA local_address_internet_address;


	CString servers_list_string;

	if (local_main_dialog->get_command_terminate_application())
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_register_thread_parameters_structure;
		return 0;
	}
	{
		servers_list_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_SERVER_state;
	}

	local_address = servers_list_string;


	for (UINT local_parameter_port_number = port_number_start_const; local_parameter_port_number <= port_number_end_const; local_parameter_port_number++)
	{
		network::ip_4::CBlockingSocket_ip_4 local_blocking_socket;

		if (network::ip_4::domain_name_to_internet_4_name(local_address, local_address_internet_address) == false)
		{
			const int local_error_message_size = 10000;
			wchar_t local_error_message[local_error_message_size];

			const int local_system_error_message_size = local_error_message_size - 1000;
			wchar_t local_system_error_message[local_system_error_message_size];

			wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

			CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

			wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

			delete local_register_thread_parameters_structure;

			//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

			{
				clear_thread_information(local_main_dialog);
			}

			return 0;
		}

		network::ip_4::CSockAddr_ip_4 local_socket_address(local_address_internet_address, local_parameter_port_number);

		double local_data_size_send = 0.0;

		try
		{
			local_blocking_socket.Create(SOCK_DGRAM);
		}
		catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
		{
			const int local_error_message_size = 10000;
			wchar_t local_error_message[local_error_message_size];

			const int local_system_error_message_size = local_error_message_size - 1000;
			wchar_t local_system_error_message[local_system_error_message_size];

			CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

			local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

			wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

			local_blocking_socket_exception->Delete();

			delete local_register_thread_parameters_structure;

			//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

			{
				clear_thread_information(local_main_dialog);
			}

			return 0;
		}

		int local_bytes_sent = 0;
		int local_error_number = 0;

		char send_buffer[CONST_MESSAGE_LENGTH];

		int send_buffer_data_length = service_signature_definition_length;

		ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH);

		memcpy(send_buffer, service_signature, service_signature_definition_length);

		CString send_data_string;

		int send_data_string_length = 0;

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}
		{
			send_data_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_TEXT_state;
		}

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}
		{
			send_data_string_length = send_data_string.GetLength();
		}

		memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

		send_buffer_data_length += send_data_string_length * sizeof(wchar_t);

		CString xor_code_string;

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}
		{
			xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
		}

		char xor_code = _wtoi(xor_code_string);

		encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

		try
		{
			local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer, send_buffer_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

			local_data_size_send += local_bytes_sent;
		}
		catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
		{
			const int local_error_message_size = 10000;
			wchar_t local_error_message[local_error_message_size];

			const int local_system_error_message_size = local_error_message_size;
			wchar_t local_system_error_message[local_system_error_message_size];

			CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

			local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

			wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

			local_error_number = local_blocking_socket_exception->GetErrorNumber();

			local_blocking_socket_exception->Delete();

			if (local_error_number != 0)
			{
				//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
			}
		}

		local_blocking_socket.Close();

		if (local_bytes_sent <= 0 && local_error_number == 0)
		{
			//			galaxy::MessageBox(CString(L"local_bytes_sent<=0"),CString(L"Ошибка"));
		}

		if (local_main_dialog->get_command_threads_stop())
		{
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_register_thread_parameters_structure;
		return 1;
	}
}

UINT __cdecl datagram_register_thread_ip_6(LPVOID parameter)
{
	thread_register_parameters_structure_type *local_register_thread_parameters_structure = (thread_register_parameters_structure_type *)parameter;

	if (local_register_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_register_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_register_thread_parameters_structure;
		return 0;
	}

	CString local_address(localhost_definition);

	CStringA local_address_internet_address;


	CString servers_list_string;

	if (local_main_dialog->get_command_terminate_application())
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_register_thread_parameters_structure;
		return 0;
	}
	{
		servers_list_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_SERVER_state;
	}

	local_address = servers_list_string;


	for (UINT local_parameter_port_number = port_number_start_const; local_parameter_port_number <= port_number_end_const; local_parameter_port_number++)
	{
		network::ip_6::CBlockingSocket_ip_6 local_blocking_socket;

		if (network::ip_6::domain_name_to_internet_6_name(local_address, local_address_internet_address) == false)
		{
			const int local_error_message_size = 10000;
			wchar_t local_error_message[local_error_message_size];

			const int local_system_error_message_size = local_error_message_size - 1000;
			wchar_t local_system_error_message[local_system_error_message_size];

			wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

			CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

			wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

			delete local_register_thread_parameters_structure;

			//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

			{
				clear_thread_information(local_main_dialog);
			}

			return 0;
		}

		network::ip_6::CSockAddr_ip_6 local_socket_address(local_address_internet_address, local_parameter_port_number);

		double local_data_size_send = 0.0;

		try
		{
			local_blocking_socket.Create(SOCK_DGRAM);
		}
		catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
		{
			const int local_error_message_size = 10000;
			wchar_t local_error_message[local_error_message_size];

			const int local_system_error_message_size = local_error_message_size - 1000;
			wchar_t local_system_error_message[local_system_error_message_size];

			CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

			local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

			wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

			local_blocking_socket_exception->Delete();

			delete local_register_thread_parameters_structure;

			//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

			{
				clear_thread_information(local_main_dialog);
			}

			return 0;
		}

		int local_bytes_sent = 0;
		int local_error_number = 0;

		char send_buffer[CONST_MESSAGE_LENGTH];

		int send_buffer_data_length = service_signature_definition_length;

		ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH);

		memcpy(send_buffer, service_signature, service_signature_definition_length);

		CString send_data_string;

		int send_data_string_length = 0;

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}
		{
			send_data_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_TEXT_state;
		}

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}
		{
			send_data_string_length = send_data_string.GetLength();
		}

		memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

		send_buffer_data_length += send_data_string_length * sizeof(wchar_t);

		CString xor_code_string;

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}
		{
			xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
		}

		char xor_code = _wtoi(xor_code_string);

		encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

		try
		{
			local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer, send_buffer_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

			local_data_size_send += local_bytes_sent;
		}
		catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
		{
			const int local_error_message_size = 10000;
			wchar_t local_error_message[local_error_message_size];

			const int local_system_error_message_size = local_error_message_size;
			wchar_t local_system_error_message[local_system_error_message_size];

			CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

			local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

			wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

			local_error_number = local_blocking_socket_exception->GetErrorNumber();

			local_blocking_socket_exception->Delete();

			if (local_error_number != 0)
			{
				//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
			}
		}

		local_blocking_socket.Close();

		if (local_bytes_sent <= 0 && local_error_number == 0)
		{
			//			galaxy::MessageBox(CString(L"local_bytes_sent<=0"),CString(L"Ошибка"));
		}

		if (local_main_dialog->get_command_threads_stop())
		{
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_register_thread_parameters_structure;
		return 1;
	}
}

void Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedButtonRegister()
{
	// TODO: добавьте свой код обработчика уведомлений

	{
		void *local_register_thread_parameters_structure = new thread_register_parameters_structure_type;

		((thread_register_parameters_structure_type*)local_register_thread_parameters_structure)->parameter_main_dialog = this;

		CWinThread *local_thread = AfxBeginThread(datagram_register_thread, local_register_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_register_thread");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&threads_list_critical_section);
		local_lock.Lock();

		threads_list.push_back(local_thread_information);
	}
}


void Cstl_network_ip_4_ip_6_udp_engineDialog::PrepareVideo(CString parameter_string, BYTE* parameter_data, size_t parameter_data_length)
{
	if (get_command_terminate_application())
	{
		if (parameter_data != NULL)
		{
			delete[]parameter_data;
		}

		return;
	}

	CSingleLock local_lock(&draw_video_critical_section);

	local_lock.Lock();

	DWORD received_sequence = *(DWORD*)(parameter_data);

	DWORD received_part = ((DWORD*)(parameter_data))[1];
	DWORD received_total_parts = ((DWORD*)(parameter_data))[2];

	DWORD received_sequence_frame_number = ((DWORD*)(parameter_data))[3];


	{
		std::list<STREAM_FRAME_INFORMATION>::iterator current_received_video_frame_stream = received_video_frame_stream.begin();

		for (; current_received_video_frame_stream != received_video_frame_stream.end(); current_received_video_frame_stream++)
		{
			if (current_received_video_frame_stream->sequence_source_number == received_sequence)
			{
				break;
			}
		}

		FRAME_PART local_frame_part;

		BYTE *local_data = new BYTE[parameter_data_length - sizeof(DWORD) - 3 * sizeof(DWORD)];

		if (local_data == NULL)
		{
			if (parameter_data != NULL)
			{
				delete[]parameter_data;
			}

			return;
		}

		ZeroMemory(local_data, parameter_data_length - sizeof(DWORD) - 3 * sizeof(DWORD));

		memcpy(local_data, parameter_data + sizeof(DWORD) + 3 * sizeof(DWORD), parameter_data_length - sizeof(DWORD) - 3 * sizeof(DWORD));

		local_frame_part.frame_part_data = local_data;

		local_frame_part.frame_part_data_size = parameter_data_length - sizeof(DWORD) - 3 * sizeof(DWORD);

		local_frame_part.frame_part_number = received_part;

		local_frame_part.sequence_frame_number = received_sequence_frame_number;

		CTime local_current_time = CTime::GetCurrentTime();

		local_frame_part.arrival_time = local_current_time;

		if (current_received_video_frame_stream == received_video_frame_stream.end())
		{
			STREAM_FRAME_INFORMATION local_frame_information;

			FRAME local_frame;

			local_frame.frame_parts.push_back(local_frame_part);

			local_frame.frame_parts_number = received_total_parts;

			local_frame_information.frames.push_back(local_frame);

			local_frame_information.sequence_source_number = received_sequence;

			local_frame_information.source_address_string = parameter_string;

			received_video_frame_stream.push_back(local_frame_information);
		}
		else
		{
			std::list<FRAME>::iterator local_frames_iterator = current_received_video_frame_stream->frames.begin();

			std::list<FRAME_PART>::iterator local_frame_parts_iterator;

			for
				(
					;
					local_frames_iterator != current_received_video_frame_stream->frames.end()
					;
					local_frames_iterator++
					)
			{
				local_frame_parts_iterator = local_frames_iterator->frame_parts.begin();
				for (; local_frame_parts_iterator != local_frames_iterator->frame_parts.end(); local_frame_parts_iterator++)
				{
					if (local_frame_parts_iterator->sequence_frame_number == received_sequence_frame_number)
					{
						local_frames_iterator->frame_parts.push_back(local_frame_part);
						break;
					}
				}

				if (local_frame_parts_iterator != local_frames_iterator->frame_parts.end())
				{
					if (local_frame_parts_iterator->sequence_frame_number == received_sequence_frame_number)
					{
						break;
					}
				}
			}

			if (local_frames_iterator == current_received_video_frame_stream->frames.end())
			{
				FRAME local_frame;

				local_frame.frame_parts.push_back(local_frame_part);

				local_frame.frame_parts_number = received_total_parts;

				current_received_video_frame_stream->source_address_string = parameter_string;

				current_received_video_frame_stream->frames.push_back(local_frame);
			}
		}
		//	Складирование закончено
	}

	delete[] parameter_data;

	local_lock.Unlock();

	return;
}

void Cstl_network_ip_4_ip_6_udp_engineDialog::DrawVideo(CString parameter_string, BYTE* parameter_data, size_t parameter_data_length)
{
	{
		if (parameter_data != NULL)
		{
			delete[]parameter_data;
		}

		return;
	}
}


void Cstl_network_ip_4_ip_6_udp_engineDialog::PrepareAudio(CString parameter_string, BYTE* parameter_data, size_t parameter_data_length)
{
	if (get_command_terminate_application())
	{
		if (parameter_data != NULL)
		{
			delete[]parameter_data;
		}

		return;
	}

	CSingleLock local_lock(&play_audio_critical_section);

	local_lock.Lock();

	DWORD received_sequence = *(DWORD*)(parameter_data);

	DWORD received_part = ((DWORD*)(parameter_data))[1];
	DWORD received_total_parts = ((DWORD*)(parameter_data))[2];

	std::list<STREAM_INFORMATION>::iterator current_received_microphone_stream = received_microphone_stream.begin();

	for (; current_received_microphone_stream != received_microphone_stream.end(); current_received_microphone_stream++)
	{
		if (current_received_microphone_stream->sequence_number == received_sequence)
		{
			break;
		}
	}

	HRESULT local_create_IStream_result = S_OK;

	ULONG local_bytes_written = 0;
	if (current_received_microphone_stream == received_microphone_stream.end())
	{
		STREAM_INFORMATION local_new_stream;

		local_new_stream.sequence_number = received_sequence;

		local_create_IStream_result = CreateStreamOnHGlobal(0, TRUE, &local_new_stream.stream);

		received_microphone_stream.push_front(local_new_stream);

		current_received_microphone_stream = received_microphone_stream.begin();
	}

	if (SUCCEEDED(local_create_IStream_result))
	{
		HRESULT local_write_result = current_received_microphone_stream->stream->Write(parameter_data + sizeof(DWORD) + 2 * sizeof(DWORD), parameter_data_length - sizeof(DWORD) - 2 * sizeof(DWORD), &local_bytes_written);

		if (SUCCEEDED(local_write_result))
		{
			int local_IDC_CHECK_RETRANSLATE_MICROPHONE_state = 0;

			{
				CSingleLock lock(&GUI_update_critical_section);
				lock.Lock();

				local_IDC_CHECK_RETRANSLATE_MICROPHONE_state = GUI_CONTROLS_STATE_data.IDC_CHECK_RETRANSLATE_MICROPHONE_state;
			}

			if (local_IDC_CHECK_RETRANSLATE_MICROPHONE_state != 0)
			{

			}
		}
	}

	delete[] parameter_data;

	local_lock.Unlock();
}

void Cstl_network_ip_4_ip_6_udp_engineDialog::PlayAudio(CString parameter_string, BYTE* parameter_data, size_t parameter_data_length)
{
	if (get_command_terminate_application())
	{
		if (parameter_data != NULL)
		{
			delete[]parameter_data;
		}

		return;
	}

	CSingleLock local_lock(&play_audio_critical_section);

	local_lock.Lock();

	DWORD received_sequence = *(DWORD*)(parameter_data);

	std::list<STREAM_INFORMATION>::iterator current_received_microphone_stream = received_microphone_stream.begin();

	for (; current_received_microphone_stream != received_microphone_stream.end(); current_received_microphone_stream++)
	{
		if (current_received_microphone_stream->sequence_number == received_sequence)
		{
			break;
		}
	}

	if (current_received_microphone_stream != received_microphone_stream.end())
	{
		LARGE_INTEGER liBeggining = { 0 };

		current_received_microphone_stream->stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

		STATSTG local_istream_statstg;
		HRESULT local_stat_result = current_received_microphone_stream->stream->Stat(&local_istream_statstg, STATFLAG::STATFLAG_DEFAULT);

		//	Здесь воспроизводится аудио сампле
		{
			CWave *local_wave = new CWave();

			BOOL local_load_result = FALSE;

			if (local_wave != NULL)
			{
				local_load_result = local_wave->Load(current_received_microphone_stream->stream);
			}


			ULARGE_INTEGER zero_size = { 0,0 };
			current_received_microphone_stream->stream->SetSize(zero_size);

			{
				LARGE_INTEGER liBeggining = { 0 };

				current_received_microphone_stream->stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);
			}

			local_lock.Unlock();

			if (local_load_result)
			{
				int local_IDC_CHECK_SAVE_SOUND_state = 0;

				{
					CSingleLock lock(&GUI_update_critical_section);
					lock.Lock();

					local_IDC_CHECK_SAVE_SOUND_state = GUI_CONTROLS_STATE_data.IDC_CHECK_SAVE_MICROPHONE_SOUND_state;
				}

				if (local_IDC_CHECK_SAVE_SOUND_state != 0)
				{
					CString new_sound_file_name;

					SYSTEMTIME local_system_time;

					GetLocalTime(&local_system_time);

					new_sound_file_name.Format
					(
						L"%04d-%02d-%02d %02d-%02d-%02d.%03d.WAV",
						local_system_time.wYear,
						local_system_time.wMonth,
						local_system_time.wDay,
						local_system_time.wHour,
						local_system_time.wMinute,
						local_system_time.wSecond,
						local_system_time.wMilliseconds
					);

					try
					{
						HRESULT local_save_result = local_wave->Save(new_sound_file_name.GetBuffer(2000));
					}
					catch (...)
					{
						CString local_fail(L"Fail");
					}
				}

				int local_IDC_CHECK_RETRANSLATE_MICROPHONE_state = 0;

				{
					CSingleLock lock(&GUI_update_critical_section);
					lock.Lock();

					local_IDC_CHECK_RETRANSLATE_MICROPHONE_state = GUI_CONTROLS_STATE_data.IDC_CHECK_RETRANSLATE_MICROPHONE_state;
				}

				if (local_IDC_CHECK_RETRANSLATE_MICROPHONE_state != 0)
				{
					{
						STREAM_INFORMATION local_retranslate_stream_information_ip_4;

						LARGE_INTEGER liBeggining = { 0 };

						current_received_microphone_stream->stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

						ULONG local_read = 0;
						HRESULT local_clone_IStream_result = current_received_microphone_stream->stream->Clone(&local_retranslate_stream_information_ip_4.stream);
						local_retranslate_stream_information_ip_4.sequence_number = current_received_microphone_stream->sequence_number;

						retranslate_microphone_frames_ip_4.push_back(local_retranslate_stream_information_ip_4);
					}

					{
						STREAM_INFORMATION local_retranslate_stream_information_ip_6;

						LARGE_INTEGER liBeggining = { 0 };

						current_received_microphone_stream->stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

						ULONG local_read = 0;
						HRESULT local_clone_IStream_result = current_received_microphone_stream->stream->Clone(&local_retranslate_stream_information_ip_6.stream);
						local_retranslate_stream_information_ip_6.sequence_number = current_received_microphone_stream->sequence_number;

						retranslate_microphone_frames_ip_6.push_back(local_retranslate_stream_information_ip_6);
					}
				}
			}

			{
				void *local_play_audio_thread_parameters_structure = new thread_play_audio_parameters_structure_type;

				((thread_play_audio_parameters_structure_type*)local_play_audio_thread_parameters_structure)->parameter_main_dialog = this;
				((thread_play_audio_parameters_structure_type*)local_play_audio_thread_parameters_structure)->parameter_wave = local_wave;

				CWinThread *local_thread = AfxBeginThread(datagram_play_audio_connection_thread, local_play_audio_thread_parameters_structure);

				THREADS_INFORMATION local_thread_information;
				local_thread_information.thread_name = CString(L"datagram_play_audio_connection_thread");
				local_thread_information.WinThread = local_thread->m_hThread;

				CSingleLock local_lock(&threads_list_critical_section);
				local_lock.Lock();

				threads_list.push_back(local_thread_information);
			}

		}
	}

	if (parameter_data != NULL)
	{
		delete[]parameter_data;
	}
}

void CaptureScreenShot(CImage *parameter_image)
{
	if (parameter_image == NULL)
	{
		return;
	}

	HWND active_window_handle = ::GetActiveWindow();
	HWND desktop_window_handle = ::GetDesktopWindow();

	CWnd active_window;

	if (active_window_handle != NULL)
	{
		active_window.Attach(active_window_handle);
	}


	CWnd desktop_window;
	desktop_window.Attach(desktop_window_handle);

	if (active_window_handle != NULL)
	{
		desktop_window.SetFocus();
	}

	desktop_window.Detach();

	{
		HDC scrdc, memdc;
		HBITMAP membit;
		scrdc = ::GetDC(::GetDesktopWindow());
		int Height = GetSystemMetrics(SM_CYSCREEN);
		int Width = GetSystemMetrics(SM_CXSCREEN);
		memdc = CreateCompatibleDC(scrdc);
		membit = CreateCompatibleBitmap(scrdc, Width, Height);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(memdc, membit);
		BitBlt(memdc, 0, 0, Width, Height, scrdc, 0, 0, SRCCOPY);

		parameter_image->Attach(membit);

		SelectObject(memdc, hOldBitmap);
		DeleteObject(memdc);
		//		DeleteObject(membit);		//	CImage удаляет прикреплённый HBITMAP
		::ReleaseDC(0, scrdc);
	}

	if (active_window_handle != NULL)
	{
		active_window.SetFocus();

		active_window.Detach();
	}
}

void Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckSendVideo()
{
	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();

	GUI_CONTROLS_STATE_data.IDC_CHECK_SEND_VIDEO_state = button_send_video.GetCheck();

	if (button_send_video.GetCheck() != 0)
	{
		button_retranslate_video.SetCheck(0);
		GUI_CONTROLS_STATE_data.IDC_CHECK_RETRANSLATE_VIDEO_state = 0;

		set_command_threads_video_stop(false);

		{
			void *local_send_video_thread_parameters_structure = new thread_send_video_parameters_structure_type;

			((thread_send_video_parameters_structure_type*)local_send_video_thread_parameters_structure)->parameter_main_dialog = this;

			CWinThread *local_thread = AfxBeginThread(datagram_send_video_connection_thread, local_send_video_thread_parameters_structure);

			THREADS_INFORMATION local_thread_information;
			local_thread_information.thread_name = CString(L"datagram_send_video_connection_thread");
			local_thread_information.WinThread = local_thread->m_hThread;

			CSingleLock local_lock(&threads_list_critical_section);
			local_lock.Lock();

			threads_list.push_back(local_thread_information);
		}
	}
	else
	{
		set_command_threads_video_stop(true);
	}
}

//	send video

UINT __cdecl datagram_send_video_connection_thread(LPVOID parameter)
{
	thread_send_video_parameters_structure_type *local_send_video_thread_parameters_structure_source = (thread_send_video_parameters_structure_type *)parameter;

	if (local_send_video_thread_parameters_structure_source == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = local_send_video_thread_parameters_structure_source->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete parameter;

		return 0;
	}

	{
		void *local_send_video_thread_parameters_structure = new thread_send_video_parameters_structure_type;

		((thread_send_video_parameters_structure_type*)local_send_video_thread_parameters_structure)->parameter_main_dialog = ((thread_send_video_parameters_structure_type*)local_send_video_thread_parameters_structure_source)->parameter_main_dialog;

		CWinThread *local_thread = AfxBeginThread(datagram_send_video_connection_thread_ip_4, local_send_video_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_send_video_connection_thread_ip_4");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
		local_lock.Lock();

		local_main_dialog->threads_list.push_back(local_thread_information);
	}

	{
		void *local_send_video_thread_parameters_structure = new thread_send_video_parameters_structure_type;

		((thread_send_video_parameters_structure_type*)local_send_video_thread_parameters_structure)->parameter_main_dialog = ((thread_send_video_parameters_structure_type*)local_send_video_thread_parameters_structure_source)->parameter_main_dialog;

		CWinThread *local_thread = AfxBeginThread(datagram_send_video_connection_thread_ip_6, local_send_video_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_send_video_connection_thread_ip_6");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
		local_lock.Lock();

		local_main_dialog->threads_list.push_back(local_thread_information);
	}

	{
		clear_thread_information(local_main_dialog);
	}

	delete parameter;

	return 1;
}



//	send video

UINT __cdecl datagram_send_video_connection_thread_ip_4(LPVOID parameter)
{
	DWORD sequence_number = DWORD(rand());

	thread_send_video_parameters_structure_type *local_send_video_thread_parameters_structure = (thread_send_video_parameters_structure_type *)parameter;

	if (local_send_video_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_send_video_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_send_video_thread_parameters_structure;
		return 0;
	}

	char *send_buffer = new char[CONST_MESSAGE_LENGTH_IMAGE];

	if (send_buffer == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_send_video_thread_parameters_structure;
		return 0;
	}

	char *send_buffer_this_time = new char[CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME];

	if (send_buffer_this_time == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete local_send_video_thread_parameters_structure;
		return 0;
	}

	for (;;)
	{
		ULONGLONG local_start_capture_time = GetTickCount64();

		if (local_main_dialog->get_command_threads_video_stop())
		{
			break;
		}

		CString local_address(localhost_definition);

		CStringA local_address_internet_address;

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}

		int peers_to_send_count = 0;
		{
			DWORD dwWaitResult;

			dwWaitResult = WaitForSingleObject(
				local_main_dialog->do_not_terminate_application_event, // event handle
				0);    // zero wait

			if (dwWaitResult == WAIT_OBJECT_0)
			{
				// Event object was signaled		

				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_video_thread_parameters_structure;
					return 0;
				}

				for (
					std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
					current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
					current_item_iterator++
					)
				{
					if (current_item_iterator->is_checked > 0)
					{
						peers_to_send_count++;
					}
				}
			}
			else
			{
				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_video_thread_parameters_structure;
					return 0;
				}
			}
		}

		CImage local_image;

		CaptureScreenShot(&local_image);

		CComPtr<IStream> local_stream;

		HRESULT local_create_IStream_result = CreateStreamOnHGlobal(0, TRUE, &local_stream);

		try
		{
			HRESULT local_save_result = local_image.Save(local_stream, Gdiplus::ImageFormatPNG);
		}
		catch (...)
		{
			continue;
		}

		STATSTG local_istream_statstg;
		HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg, STATFLAG::STATFLAG_DEFAULT);

		double local_data_size_send = 0.0;

		ULONGLONG local_end_capture_time = GetTickCount64();

		ULONGLONG local_capture_time = local_end_capture_time - local_start_capture_time;

		for (int peers_to_send_count_counter = 0; peers_to_send_count_counter < peers_to_send_count; peers_to_send_count_counter++)
		{
			//double local_data_size_send = 0.0;

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

#ifdef use_istream_DEFINITION
			//CImage local_image;
#endif

			CString peer_to_send;

			int loop_counter = 0;
			for (
				std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
				current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
				current_item_iterator++
				)
			{
				Sleep(1);
				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
				if (current_item_iterator->is_checked > 0)
				{
					if (peers_to_send_count_counter == loop_counter)
					{
						peer_to_send = current_item_iterator->label;
						break;
					}
					loop_counter++;
				}

				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
			}

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

			local_address = peer_to_send;


			//	Здесь делаем снимок экрана

			//CSingleLock local_image_lock(&local_main_dialog->draw_video_image_critical_section);

			//local_image_lock.Lock();
#ifdef use_istream_DEFINITION
			//if(!local_image.IsNull())
			//{
			//	local_image.Destroy();
			//}
#endif

			if (network::ip_4::domain_name_to_internet_4_name(local_address, local_address_internet_address) == false)
			{
				const int local_error_message_size = 10000;
				wchar_t local_error_message[local_error_message_size];

				const int local_system_error_message_size = local_error_message_size - 1000;
				wchar_t local_system_error_message[local_system_error_message_size];

				wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

				CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

				wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

				continue;
			}
#ifdef use_istream_DEFINITION
			//CaptureScreenShot(&local_image);

			//CComPtr<IStream> local_stream;

			//HRESULT local_create_IStream_result = CreateStreamOnHGlobal ( 0 , TRUE , &local_stream );

			//try
			//{
			//	HRESULT local_save_result = local_image.Save(local_stream, Gdiplus::ImageFormatPNG);
			//}
			//catch(...)
			//{
			//	continue;
			//}
#endif
			//local_image_lock.Unlock();

			//ULONGLONG local_start_time = GetTickCount64();

			//STATSTG local_istream_statstg;
			//HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg,STATFLAG::STATFLAG_DEFAULT);

			/*/
			//			Тест отображения из IStream
			{
			std::fstream local_file;
			local_file.open("c:\\temp\\file_source.png",std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

			char *local_file_data = new char[CONST_MESSAGE_LENGTH_IMAGE];

			ULONG local_read_bytes = 0;

			LARGE_INTEGER liBeggining = { 0 };

			local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

			local_stream->Read(local_file_data,CONST_MESSAGE_LENGTH_IMAGE,&local_read_bytes);

			local_file.write(local_file_data, local_read_bytes);

			delete [] local_file_data;

			local_file.flush();

			local_file.close();
			}
			//*/



			for (UINT local_parameter_port_number = port_number_start_const; local_parameter_port_number <= port_number_end_const; local_parameter_port_number++)
			{
				network::ip_4::CBlockingSocket_ip_4 local_blocking_socket;

				if (network::ip_4::domain_name_to_internet_4_name(local_address, local_address_internet_address) == false)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					break;
				}

				network::ip_4::CSockAddr_ip_4 local_socket_address(local_address_internet_address, local_parameter_port_number);

				try
				{
					local_blocking_socket.Create(SOCK_DGRAM);
				}
				catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					local_blocking_socket_exception->Delete();

					delete local_send_video_thread_parameters_structure;

					//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete[] send_buffer_this_time;

					return 0;
				}

				int local_bytes_sent = 0;
				int local_error_number = 0;


				int send_buffer_data_length = service_signature_definition_length;

				ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH_IMAGE);

				memcpy(send_buffer, service_signature, service_signature_definition_length);



				CString send_data_string = command_video;

				int send_data_string_length = send_data_string.GetLength();

				memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

				send_buffer_data_length += send_data_string_length * sizeof(wchar_t);

				wchar_t zero_word = L'\0';
				memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
				send_buffer_data_length += sizeof(wchar_t);
				send_data_string_length++;


				memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
				send_buffer_data_length += sizeof(DWORD);
				send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);


				LARGE_INTEGER liBeggining = { 0 };
#ifdef use_istream_DEFINITION
				local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);
#endif


				ULONG local_read = 0;
#ifdef use_istream_DEFINITION
				local_stream->Read(send_buffer + send_buffer_data_length, CONST_MESSAGE_LENGTH_IMAGE - send_buffer_data_length, &local_read);
#else
				local_read = 100000;
#endif
				send_buffer_data_length += local_read;




				CString xor_code_string;

				//if(local_main_dialog->get_command_terminate_application())
				//{
				//	break;
				//}
				{
					xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
				}

				char xor_code = _wtoi(xor_code_string);

				encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

				int local_header_length = service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 3 * sizeof(DWORD);

				int local_part_counter = 0;
				int local_parts_count = local_read / (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 3 * sizeof(DWORD));
				int local_last_part_size = local_read % (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 3 * sizeof(DWORD));

				if (local_last_part_size != 0)
				{
					local_parts_count++;
				}

				int local_sequence_frame_number = rand();

				for (; local_part_counter < local_parts_count; local_part_counter++)
				{
					ULONGLONG local_start_time = GetTickCount64();

					int send_buffer_this_time_data_length = CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME;

					if (local_part_counter == local_parts_count - 1)
					{
						if (local_last_part_size != 0)
						{
							send_buffer_this_time_data_length = local_last_part_size + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 3 * sizeof(DWORD);
						}
					}

					ZeroMemory(send_buffer_this_time, CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME);

					memcpy(send_buffer_this_time, send_buffer, service_signature_definition_length + send_data_string_length * sizeof(wchar_t));

					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[0] = DWORD(local_part_counter + 1);
					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[1] = DWORD(local_parts_count);

					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[2] = DWORD(local_sequence_frame_number);

					encrypt::encrypt_xor(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t), 3 * sizeof(DWORD), xor_code);

					int local_this_time_data_offset = local_part_counter*(CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 3 * sizeof(DWORD));

					memcpy
					(
						send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 3 * sizeof(DWORD),
						send_buffer + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + local_this_time_data_offset,
						send_buffer_this_time_data_length - (service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 3 * sizeof(DWORD))
					);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer_this_time, send_buffer_this_time_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
						break;
					}

					//if(local_main_dialog->get_command_threads_video_stop())
					//{
					//	break;
					//}
					//else
					{
						ULONGLONG local_end_time = GetTickCount64();

						ULONGLONG local_work_time = local_end_time - local_start_time;

						CString local_string_speed;

						if (local_main_dialog->get_command_terminate_application())
						{
							break;
						}
						{
							local_string_speed = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_SPEED_state;
						}
						double local_double_speed = _wtof(local_string_speed);
						if (local_data_size_send != 0.0)
						{
							double local_current_time_in_seconds = double(local_work_time + 1) / 1000.0;
							double local_current_bits_send = (local_bytes_sent*8.0);
							double local_current_speed = local_current_bits_send / local_current_time_in_seconds;

							double local_test_rate_1 = double((local_double_speed / 8) / (local_parts_count*local_bytes_sent));		//	Требуемая частота кадров
							double local_test_rate_2 = double((local_current_speed / 8) / (local_bytes_sent));						//	Текущая частота отдачи кадров
							double local_test_rate_3 =
								double
								(
									1000.0
									*
									local_parts_count
									*
									port_count_const
									*
									peers_to_send_count
									/
									local_capture_time
									);
							//	Текущая частота съёма кадров на часть на порт на пару

							double local_test_rate_4 = local_test_rate_3;

							DWORD time_to_sleep = 0;

							if (local_test_rate_1 >= local_test_rate_4)
							{
								time_to_sleep = 0;
							}
							else
							{
								time_to_sleep = DWORD((1000.0 / local_test_rate_1 - 1000.0 / local_test_rate_4) /
									(
										local_parts_count
										*
										port_count_const
										*
										peers_to_send_count)
								);
							}

							Sleep(time_to_sleep);
						}
					}

				}

				{
					int local_bytes_sent = 0;
					int local_error_number = 0;

					char send_buffer[CONST_MESSAGE_LENGTH];

					int send_buffer_data_length = service_signature_definition_length;

					ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH);

					memcpy(send_buffer, service_signature, service_signature_definition_length);

					CString send_data_string;

					int send_data_string_length = 0;

					send_data_string = command_video_end;

					send_data_string_length = send_data_string.GetLength();

					memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

					send_buffer_data_length += send_data_string_length * sizeof(wchar_t);


					wchar_t zero_word = L'\0';
					memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
					send_buffer_data_length += sizeof(wchar_t);
					send_data_string_length++;


					memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
					send_buffer_data_length += sizeof(DWORD);
					send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);


					CString xor_code_string;

					//if(local_main_dialog->get_command_terminate_application())
					//{
					//	break;
					//}
					{
						xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
					}

					char xor_code = _wtoi(xor_code_string);

					encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer, send_buffer_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
					}
				}



				local_blocking_socket.Close();

				if (local_bytes_sent <= 0 && local_error_number == 0)
				{
					//			galaxy::MessageBox(CString(L"local_bytes_sent<=0"),CString(L"Ошибка"));
				}

				if (local_main_dialog->get_command_threads_video_stop())
				{
					break;
				}
				else
				{
					Sleep(10);
				}
			}
		}

		if (local_main_dialog->get_command_threads_video_stop())
		{
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete[] send_buffer_this_time;

		delete local_send_video_thread_parameters_structure;
		return 1;
	}
}

UINT __cdecl datagram_send_video_connection_thread_ip_6(LPVOID parameter)
{
	DWORD sequence_number = DWORD(rand());

	thread_send_video_parameters_structure_type *local_send_video_thread_parameters_structure = (thread_send_video_parameters_structure_type *)parameter;

	if (local_send_video_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_send_video_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_send_video_thread_parameters_structure;
		return 0;
	}

	char *send_buffer = new char[CONST_MESSAGE_LENGTH_IMAGE];

	if (send_buffer == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_send_video_thread_parameters_structure;
		return 0;
	}

	char *send_buffer_this_time = new char[CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME];

	if (send_buffer_this_time == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete local_send_video_thread_parameters_structure;
		return 0;
	}

	for (;;)
	{
		ULONGLONG local_start_capture_time = GetTickCount64();

		if (local_main_dialog->get_command_threads_video_stop())
		{
			break;
		}

		CString local_address(localhost_definition);

		CStringA local_address_internet_address;

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}

		int peers_to_send_count = 0;
		{
			DWORD dwWaitResult;

			dwWaitResult = WaitForSingleObject(
				local_main_dialog->do_not_terminate_application_event, // event handle
				0);    // zero wait

			if (dwWaitResult == WAIT_OBJECT_0)
			{
				// Event object was signaled		

				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_video_thread_parameters_structure;
					return 0;
				}

				for (
					std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
					current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
					current_item_iterator++
					)
				{
					if (current_item_iterator->is_checked > 0)
					{
						peers_to_send_count++;
					}
				}
			}
			else
			{
				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_video_thread_parameters_structure;
					return 0;
				}
			}

		}

		CImage local_image;

		CaptureScreenShot(&local_image);

		CComPtr<IStream> local_stream;

		HRESULT local_create_IStream_result = CreateStreamOnHGlobal(0, TRUE, &local_stream);

		try
		{
			HRESULT local_save_result = local_image.Save(local_stream, Gdiplus::ImageFormatPNG);
		}
		catch (...)
		{
			continue;
		}

		STATSTG local_istream_statstg;
		HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg, STATFLAG::STATFLAG_DEFAULT);

		double local_data_size_send = 0.0;

		ULONGLONG local_end_capture_time = GetTickCount64();

		ULONGLONG local_capture_time = local_end_capture_time - local_start_capture_time;

		for (int peers_to_send_count_counter = 0; peers_to_send_count_counter < peers_to_send_count; peers_to_send_count_counter++)
		{
			//double local_data_size_send = 0.0;

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

#ifdef use_istream_DEFINITION
			//CImage local_image;
#endif

			CString peer_to_send;

			int loop_counter = 0;
			for (
				std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
				current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
				current_item_iterator++
				)
			{
				Sleep(1);
				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
				if (current_item_iterator->is_checked > 0)
				{
					if (peers_to_send_count_counter == loop_counter)
					{
						peer_to_send = current_item_iterator->label;
						break;
					}
					loop_counter++;
				}

				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
			}

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

			local_address = peer_to_send;


			//	Здесь делаем снимок экрана

			//CSingleLock local_image_lock(&local_main_dialog->draw_video_image_critical_section);

			//local_image_lock.Lock();
#ifdef use_istream_DEFINITION
			//if(!local_image.IsNull())
			//{
			//	local_image.Destroy();
			//}
#endif

			if (network::ip_6::domain_name_to_internet_6_name(local_address, local_address_internet_address) == false)
			{
				const int local_error_message_size = 10000;
				wchar_t local_error_message[local_error_message_size];

				const int local_system_error_message_size = local_error_message_size - 1000;
				wchar_t local_system_error_message[local_system_error_message_size];

				wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

				CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

				wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

				continue;
			}
#ifdef use_istream_DEFINITION
			//CaptureScreenShot(&local_image);

			//CComPtr<IStream> local_stream;

			//HRESULT local_create_IStream_result = CreateStreamOnHGlobal ( 0 , TRUE , &local_stream );

			//try
			//{
			//	HRESULT local_save_result = local_image.Save(local_stream, Gdiplus::ImageFormatPNG);
			//}
			//catch(...)
			//{
			//	continue;
			//}
#endif
			//local_image_lock.Unlock();

			//ULONGLONG local_start_time = GetTickCount64();

			//STATSTG local_istream_statstg;
			//HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg,STATFLAG::STATFLAG_DEFAULT);

			/*/
			//			Тест отображения из IStream
			{
			std::fstream local_file;
			local_file.open("c:\\temp\\file_source.png",std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

			char *local_file_data = new char[CONST_MESSAGE_LENGTH_IMAGE];

			ULONG local_read_bytes = 0;

			LARGE_INTEGER liBeggining = { 0 };

			local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

			local_stream->Read(local_file_data,CONST_MESSAGE_LENGTH_IMAGE,&local_read_bytes);

			local_file.write(local_file_data, local_read_bytes);

			delete [] local_file_data;

			local_file.flush();

			local_file.close();
			}
			//*/



			for (UINT local_parameter_port_number = port_number_start_const; local_parameter_port_number <= port_number_end_const; local_parameter_port_number++)
			{
				network::ip_6::CBlockingSocket_ip_6 local_blocking_socket;

				if (network::ip_6::domain_name_to_internet_6_name(local_address, local_address_internet_address) == false)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					break;
				}

				network::ip_6::CSockAddr_ip_6 local_socket_address(local_address_internet_address, local_parameter_port_number);

				try
				{
					local_blocking_socket.Create(SOCK_DGRAM);
				}
				catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					local_blocking_socket_exception->Delete();

					delete local_send_video_thread_parameters_structure;

					//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete[] send_buffer_this_time;

					return 0;
				}

				int local_bytes_sent = 0;
				int local_error_number = 0;


				int send_buffer_data_length = service_signature_definition_length;

				ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH_IMAGE);

				memcpy(send_buffer, service_signature, service_signature_definition_length);



				CString send_data_string = command_video;

				int send_data_string_length = send_data_string.GetLength();

				memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

				send_buffer_data_length += send_data_string_length * sizeof(wchar_t);

				wchar_t zero_word = L'\0';
				memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
				send_buffer_data_length += sizeof(wchar_t);
				send_data_string_length++;


				memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
				send_buffer_data_length += sizeof(DWORD);
				send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);



				LARGE_INTEGER liBeggining = { 0 };
#ifdef use_istream_DEFINITION
				local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);
#endif


				ULONG local_read = 0;
#ifdef use_istream_DEFINITION
				local_stream->Read(send_buffer + send_buffer_data_length, CONST_MESSAGE_LENGTH_IMAGE - send_buffer_data_length, &local_read);
#else
				local_read = 100000;
#endif
				send_buffer_data_length += local_read;




				CString xor_code_string;

				//if(local_main_dialog->get_command_terminate_application())
				//{
				//	break;
				//}
				{
					xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
				}

				char xor_code = _wtoi(xor_code_string);

				encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

				int local_header_length = service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 3 * sizeof(DWORD);

				int local_part_counter = 0;
				int local_parts_count = local_read / (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 3 * sizeof(DWORD));
				int local_last_part_size = local_read % (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 3 * sizeof(DWORD));

				if (local_last_part_size != 0)
				{
					local_parts_count++;
				}

				int local_sequence_frame_number = rand();

				for (; local_part_counter < local_parts_count; local_part_counter++)
				{
					ULONGLONG local_start_time = GetTickCount64();

					int send_buffer_this_time_data_length = CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME;

					if (local_part_counter == local_parts_count - 1)
					{
						if (local_last_part_size != 0)
						{
							send_buffer_this_time_data_length = local_last_part_size + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 3 * sizeof(DWORD);
						}
					}

					ZeroMemory(send_buffer_this_time, CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME);

					memcpy(send_buffer_this_time, send_buffer, service_signature_definition_length + send_data_string_length * sizeof(wchar_t));

					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[0] = DWORD(local_part_counter + 1);
					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[1] = DWORD(local_parts_count);

					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[2] = DWORD(local_sequence_frame_number);

					encrypt::encrypt_xor(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t), 3 * sizeof(DWORD), xor_code);

					int local_this_time_data_offset = local_part_counter*(CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 3 * sizeof(DWORD));

					memcpy
					(
						send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 3 * sizeof(DWORD),
						send_buffer + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + local_this_time_data_offset,
						send_buffer_this_time_data_length - (service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 3 * sizeof(DWORD))
					);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer_this_time, send_buffer_this_time_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
						break;
					}

					//if(local_main_dialog->get_command_threads_video_stop())
					//{
					//	break;
					//}
					//else
					{
						ULONGLONG local_end_time = GetTickCount64();

						ULONGLONG local_work_time = local_end_time - local_start_time;

						CString local_string_speed;

						if (local_main_dialog->get_command_terminate_application())
						{
							break;
						}
						{
							local_string_speed = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_SPEED_state;
						}
						double local_double_speed = _wtof(local_string_speed);
						if (local_data_size_send != 0.0)
						{
							double local_current_time_in_seconds = double(local_work_time + 1) / 1000.0;
							double local_current_bits_send = (local_bytes_sent*8.0);
							double local_current_speed = local_current_bits_send / local_current_time_in_seconds;

							double local_test_rate_1 = double((local_double_speed / 8) / (local_parts_count*local_bytes_sent));		//	Требуемая частота кадров
							double local_test_rate_2 = double((local_current_speed / 8) / (local_bytes_sent));						//	Текущая частота отдачи кадров
							double local_test_rate_3 =
								double
								(
									1000.0
									*
									local_parts_count
									*
									port_count_const
									*
									peers_to_send_count
									/
									local_capture_time
									);
							//	Текущая частота съёма кадров на часть на порт на пару

							double local_test_rate_4 = local_test_rate_3;

							DWORD time_to_sleep = 0;

							if (local_test_rate_1 >= local_test_rate_4)
							{
								time_to_sleep = 0;
							}
							else
							{
								time_to_sleep = DWORD((1000.0 / local_test_rate_1 - 1000.0 / local_test_rate_4) /
									(
										local_parts_count
										*
										port_count_const
										*
										peers_to_send_count)
								);
							}

							Sleep(time_to_sleep);
						}
					}

				}

				{
					int local_bytes_sent = 0;
					int local_error_number = 0;

					char send_buffer[CONST_MESSAGE_LENGTH];

					int send_buffer_data_length = service_signature_definition_length;

					ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH);

					memcpy(send_buffer, service_signature, service_signature_definition_length);

					CString send_data_string;

					int send_data_string_length = 0;

					send_data_string = command_video_end;

					send_data_string_length = send_data_string.GetLength();

					memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

					send_buffer_data_length += send_data_string_length * sizeof(wchar_t);


					wchar_t zero_word = L'\0';
					memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
					send_buffer_data_length += sizeof(wchar_t);
					send_data_string_length++;


					memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
					send_buffer_data_length += sizeof(DWORD);
					send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);

					CString xor_code_string;

					//if(local_main_dialog->get_command_terminate_application())
					//{
					//	break;
					//}
					{
						xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
					}

					char xor_code = _wtoi(xor_code_string);

					encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer, send_buffer_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
					}
				}



				local_blocking_socket.Close();

				if (local_bytes_sent <= 0 && local_error_number == 0)
				{
					//			galaxy::MessageBox(CString(L"local_bytes_sent<=0"),CString(L"Ошибка"));
				}

				if (local_main_dialog->get_command_threads_video_stop())
				{
					break;
				}
				else
				{
					Sleep(10);
				}
			}
		}

		if (local_main_dialog->get_command_threads_video_stop())
		{
			break;
		}
		else
		{
			Sleep(10);
		}

		if (local_main_dialog->get_command_threads_video_stop())
		{
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete[] send_buffer_this_time;

		delete local_send_video_thread_parameters_structure;
		return 1;
	}
}

void Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedButtonClearChat()
{
	// TODO: добавьте свой код обработчика уведомлений

	CSingleLock lock(&list_chat_critical_section);

	lock.Lock();

	CSingleLock lock_2(&GUI_update_critical_section);
	lock_2.Lock();

	GUI_CONTROLS_STATE_data.IDC_LIST_CHAT_state.clear();


	list_chat.ResetContent();

	lock.Unlock();
}


void Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckSendWebCameraVideo()
{
	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();

	GUI_CONTROLS_STATE_data.IDC_CHECK_SEND_WEB_CAMERA_VIDEO_state = button_send_web_camera_video.GetCheck();

	if (button_send_web_camera_video.GetCheck() != 0)
	{
		button_retranslate_web_camera.SetCheck(0);
		GUI_CONTROLS_STATE_data.IDC_CHECK_RETRANSLATE_WEB_CAMERA_state = 0;

		set_command_threads_web_camera_video_stop(false);

		if (web_camera_dialog == NULL)
		{
			web_camera_dialog = new CWebCameraPictureDialog(this);
			web_camera_dialog->main_dialog = this;
			web_camera_dialog->Create(CWebCameraPictureDialog::IDD);
			web_camera_dialog->ShowWindow(SW_SHOWNORMAL);
		}

		{
			void *local_send_web_camera_video_thread_parameters_structure = new thread_send_web_camera_video_parameters_structure_type;

			((thread_send_web_camera_video_parameters_structure_type*)local_send_web_camera_video_thread_parameters_structure)->parameter_main_dialog = this;

			CWinThread *local_thread = AfxBeginThread(datagram_send_web_camera_video_connection_thread, local_send_web_camera_video_thread_parameters_structure);

			THREADS_INFORMATION local_thread_information;
			local_thread_information.thread_name = CString(L"datagram_send_web_camera_video_connection_thread");
			local_thread_information.WinThread = local_thread->m_hThread;

			CSingleLock local_lock(&threads_list_critical_section);
			local_lock.Lock();

			threads_list.push_back(local_thread_information);
		}
	}
	else
	{
		set_command_threads_web_camera_video_stop(true);

		{
			if (web_camera_dialog != NULL)
			{
				CSingleLock lock(&delete_web_camera_dialog_critical_section);

				lock.Lock();

				CWebCameraPictureDialog *local_web_camera_dialog = web_camera_dialog;

				lock.Unlock();

				if (button_send_audio.GetCheck() == 0)
				{
					web_camera_dialog = NULL;

					local_web_camera_dialog->StopMedia();

					//local_web_camera_dialog->EndDialog(IDOK);
					local_web_camera_dialog->SendMessage(WM_CLOSE);

					//delete local_web_camera_dialog;
				}
			}
		}
	}
}



//	send web camera video

UINT __cdecl datagram_send_web_camera_video_connection_thread(LPVOID parameter)
{
	thread_send_web_camera_video_parameters_structure_type *local_send_web_camera_video_thread_parameters_structure_source = (thread_send_web_camera_video_parameters_structure_type *)parameter;

	if (local_send_web_camera_video_thread_parameters_structure_source == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = local_send_web_camera_video_thread_parameters_structure_source->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete parameter;

		return 0;
	}

	{
		void *local_send_web_camera_video_thread_parameters_structure = new thread_send_web_camera_video_parameters_structure_type;

		((thread_send_web_camera_video_parameters_structure_type*)local_send_web_camera_video_thread_parameters_structure)->parameter_main_dialog = ((thread_send_web_camera_video_parameters_structure_type*)local_send_web_camera_video_thread_parameters_structure_source)->parameter_main_dialog;

		CWinThread *local_thread = AfxBeginThread(datagram_send_web_camera_video_connection_thread_ip_4, local_send_web_camera_video_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_send_web_camera_video_connection_thread_ip_4");
		local_thread_information.WinThread = local_thread->m_hThread;

		local_main_dialog->threads_list.push_back(local_thread_information);
	}

	{
		void *local_send_web_camera_video_thread_parameters_structure = new thread_send_web_camera_video_parameters_structure_type;

		((thread_send_web_camera_video_parameters_structure_type*)local_send_web_camera_video_thread_parameters_structure)->parameter_main_dialog = ((thread_send_web_camera_video_parameters_structure_type*)local_send_web_camera_video_thread_parameters_structure_source)->parameter_main_dialog;

		CWinThread *local_thread = AfxBeginThread(datagram_send_web_camera_video_connection_thread_ip_6, local_send_web_camera_video_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_send_web_camera_video_connection_thread_ip_6");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
		local_lock.Lock();

		local_main_dialog->threads_list.push_back(local_thread_information);
	}

	{
		clear_thread_information(local_main_dialog);
	}

	delete parameter;

	return 1;
}



//	send web camera video

UINT __cdecl datagram_send_web_camera_video_connection_thread_ip_4(LPVOID parameter)
{
	DWORD sequence_number = DWORD(rand());

	thread_send_web_camera_video_parameters_structure_type *local_send_web_camera_video_thread_parameters_structure = (thread_send_web_camera_video_parameters_structure_type *)parameter;

	if (local_send_web_camera_video_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_send_web_camera_video_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_send_web_camera_video_thread_parameters_structure;
		return 0;
	}

	char *send_buffer = new char[CONST_MESSAGE_LENGTH_IMAGE];

	if (send_buffer == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_send_web_camera_video_thread_parameters_structure;
		return 0;
	}

	char *send_buffer_this_time = new char[CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME];

	if (send_buffer_this_time == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete local_send_web_camera_video_thread_parameters_structure;
		return 0;
	}

	for (;;)
	{
		ULONGLONG local_start_time = GetTickCount64();

		if (local_main_dialog->get_command_threads_web_camera_video_stop())
		{
			break;
		}

		CString local_address(localhost_definition);

		CStringA local_address_internet_address;

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}

		int peers_to_send_count = 0;
		{
			DWORD dwWaitResult;

			dwWaitResult = WaitForSingleObject(
				local_main_dialog->do_not_terminate_application_event, // event handle
				0);    // zero wait

			if (dwWaitResult == WAIT_OBJECT_0)
			{
				// Event object was signaled		

				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_web_camera_video_thread_parameters_structure;
					return 0;
				}

				for (
					std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
					current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
					current_item_iterator++
					)
				{
					if (current_item_iterator->is_checked > 0)
					{
						peers_to_send_count++;
					}
				}
			}
			else
			{
				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_web_camera_video_thread_parameters_structure;
					return 0;
				}
			}

		}

		CImage local_image;

		if (local_main_dialog->web_camera_dialog != NULL)
		{
			CSingleLock lock(&local_main_dialog->delete_web_camera_dialog_critical_section);

			lock.Lock();

			//CaptureWndShot(local_main_dialog->web_camera_dialog->m_hWnd,&local_image,ENGINE_WINDOWS_SIZE_CX/2,ENGINE_WINDOWS_SIZE_CY/2);
			CaptureVideoSampleShot(local_main_dialog->web_camera_dialog, &local_image);
		}

		CComPtr<IStream> local_stream;

		HRESULT local_create_IStream_result = CreateStreamOnHGlobal(0, TRUE, &local_stream);

		try
		{
			HRESULT local_save_result = local_image.Save(local_stream, Gdiplus::ImageFormatJPEG);
		}
		catch (...)
		{
			continue;
		}

		STATSTG local_istream_statstg;
		HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg, STATFLAG::STATFLAG_DEFAULT);

		double local_data_size_send = 0.0;

		for (int peers_to_send_count_counter = 0; peers_to_send_count_counter < peers_to_send_count; peers_to_send_count_counter++)
		{
			//double local_data_size_send = 0.0;

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

#ifdef use_istream_DEFINITION
			//			CImage local_image;
#endif

			CString peer_to_send;

			int loop_counter = 0;
			for (
				std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
				current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
				current_item_iterator++
				)
			{
				Sleep(1);
				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
				if (current_item_iterator->is_checked > 0)
				{
					if (peers_to_send_count_counter == loop_counter)
					{
						peer_to_send = current_item_iterator->label;
						break;
					}
					loop_counter++;
				}

				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
			}

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

			local_address = peer_to_send;


			//	Здесь делаем снимок экрана

			//CSingleLock local_image_lock(&local_main_dialog->draw_video_image_critical_section);

			//local_image_lock.Lock();
#ifdef use_istream_DEFINITION
			//			if(!local_image.IsNull())
			//			{
			//				local_image.Destroy();
			//			}
#endif


			if (network::ip_4::domain_name_to_internet_4_name(local_address, local_address_internet_address) == false)
			{
				const int local_error_message_size = 10000;
				wchar_t local_error_message[local_error_message_size];

				const int local_system_error_message_size = local_error_message_size - 1000;
				wchar_t local_system_error_message[local_system_error_message_size];

				wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

				CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

				wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

				continue;
			}

#ifdef use_istream_DEFINITION
			//if(local_main_dialog->web_camera_dialog!=NULL)
			//{
			//	CSingleLock lock(&local_main_dialog->delete_web_camera_dialog_critical_section);

			//	lock.Lock();

			//	//CaptureWndShot(local_main_dialog->web_camera_dialog->m_hWnd,&local_image,ENGINE_WINDOWS_SIZE_CX/2,ENGINE_WINDOWS_SIZE_CY/2);
			//	CaptureVideoSampleShot(local_main_dialog->web_camera_dialog,&local_image);
			//}

			//CComPtr<IStream> local_stream;

			//HRESULT local_create_IStream_result = CreateStreamOnHGlobal ( 0 , TRUE , &local_stream );

			//try
			//{
			//	HRESULT local_save_result = local_image.Save(local_stream, Gdiplus::ImageFormatJPEG);
			//}
			//catch(...)
			//{
			//	continue;
			//}
#endif
			//local_image_lock.Unlock();


			//ULONGLONG local_start_time = GetTickCount64();

			//STATSTG local_istream_statstg;
			//HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg,STATFLAG::STATFLAG_DEFAULT);

			/*/
			//			Тест отображения из IStream
			{
			std::fstream local_file;
			local_file.open("c:\\temp\\file_source.jpg",std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

			char *local_file_data = new char[CONST_MESSAGE_LENGTH_IMAGE];

			ULONG local_read_bytes = 0;

			LARGE_INTEGER liBeggining = { 0 };

			local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

			local_stream->Read(local_file_data,CONST_MESSAGE_LENGTH_IMAGE,&local_read_bytes);

			local_file.write(local_file_data, local_read_bytes);

			delete [] local_file_data;

			local_file.flush();

			local_file.close();
			}
			//*/



			for (UINT local_parameter_port_number = port_number_start_const; local_parameter_port_number <= port_number_end_const; local_parameter_port_number++)
			{
				network::ip_4::CBlockingSocket_ip_4 local_blocking_socket;

				if (network::ip_4::domain_name_to_internet_4_name(local_address, local_address_internet_address) == false)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					break;
				}

				network::ip_4::CSockAddr_ip_4 local_socket_address(local_address_internet_address, local_parameter_port_number);

				try
				{
					local_blocking_socket.Create(SOCK_DGRAM);
				}
				catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					local_blocking_socket_exception->Delete();

					delete local_send_web_camera_video_thread_parameters_structure;

					//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete[] send_buffer_this_time;

					return 0;
				}

				int local_bytes_sent = 0;
				int local_error_number = 0;


				int send_buffer_data_length = service_signature_definition_length;

				ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH_IMAGE);

				memcpy(send_buffer, service_signature, service_signature_definition_length);



				CString send_data_string = command_web_camera_video;

				int send_data_string_length = send_data_string.GetLength();

				memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

				send_buffer_data_length += send_data_string_length * sizeof(wchar_t);

				wchar_t zero_word = L'\0';
				memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
				send_buffer_data_length += sizeof(wchar_t);
				send_data_string_length++;


				memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
				send_buffer_data_length += sizeof(DWORD);
				send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);



				LARGE_INTEGER liBeggining = { 0 };
#ifdef use_istream_DEFINITION
				local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);
#endif


				ULONG local_read = 0;
#ifdef use_istream_DEFINITION
				local_stream->Read(send_buffer + send_buffer_data_length, CONST_MESSAGE_LENGTH_IMAGE - send_buffer_data_length, &local_read);
#else
				local_read = 100000;
#endif
				send_buffer_data_length += local_read;




				CString xor_code_string;

				//if(local_main_dialog->get_command_terminate_application())
				//{
				//	break;
				//}
				{
					xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
				}

				char xor_code = _wtoi(xor_code_string);

				encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

				int local_header_length = service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD);

				int local_part_counter = 0;
				int local_parts_count = local_read / (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));
				int local_last_part_size = local_read % (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));

				if (local_last_part_size != 0)
				{
					local_parts_count++;
				}

				for (; local_part_counter < local_parts_count; local_part_counter++)
				{
					int send_buffer_this_time_data_length = CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME;

					if (local_part_counter == local_parts_count - 1)
					{
						if (local_last_part_size != 0)
						{
							send_buffer_this_time_data_length = local_last_part_size + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD);
						}
					}

					ZeroMemory(send_buffer_this_time, CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME);

					memcpy(send_buffer_this_time, send_buffer, service_signature_definition_length + send_data_string_length * sizeof(wchar_t));

					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[0] = DWORD(local_part_counter + 1);
					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[1] = DWORD(local_parts_count);

					encrypt::encrypt_xor(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t), 2 * sizeof(DWORD), xor_code);

					int local_this_time_data_offset = local_part_counter*(CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));

					memcpy
					(
						send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD),
						send_buffer + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + local_this_time_data_offset,
						send_buffer_this_time_data_length - (service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD))
					);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer_this_time, send_buffer_this_time_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
						break;
					}

					//if(local_main_dialog->get_command_threads_web_camera_video_stop())
					//{
					//	break;
					//}
					//else
					{
						ULONGLONG local_end_time = GetTickCount64();

						ULONGLONG local_work_time = local_end_time - local_start_time;

						if (local_main_dialog->get_command_threads_web_camera_video_stop())
						{
							break;
						}
						else
						{
							CString local_string_speed;

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								local_string_speed = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_SPEED_state;
							}
							double local_double_speed = _wtof(local_string_speed);
							if (local_data_size_send != 0.0)
							{
								DWORD time_to_work = 1000;
								double local_current_time_in_seconds = double(local_work_time) / 1000.0;
								double local_current_bits_send = (local_data_size_send*8.0);
								double local_current_speed = local_current_bits_send / local_current_time_in_seconds;
								double local_current_frame_rate = 1.0 / local_current_time_in_seconds;
								double local_speed_factor = local_double_speed / local_current_speed;
								double local_common_factor = min(local_speed_factor, local_current_frame_rate);

								time_to_work = DWORD(local_common_factor*local_work_time);

								DWORD time_to_sleep = 1000 - time_to_work;

								//Sleep(time_to_sleep);
								Sleep(10);
							}
						}
					}

				}

				{
					int local_bytes_sent = 0;
					int local_error_number = 0;

					char send_buffer[CONST_MESSAGE_LENGTH];

					int send_buffer_data_length = service_signature_definition_length;

					ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH);

					memcpy(send_buffer, service_signature, service_signature_definition_length);

					CString send_data_string;

					int send_data_string_length = 0;

					send_data_string = command_web_camera_video_end;

					send_data_string_length = send_data_string.GetLength();

					memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

					send_buffer_data_length += send_data_string_length * sizeof(wchar_t);


					wchar_t zero_word = L'\0';
					memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
					send_buffer_data_length += sizeof(wchar_t);
					send_data_string_length++;


					memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
					send_buffer_data_length += sizeof(DWORD);
					send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);


					CString xor_code_string;

					//if(local_main_dialog->get_command_terminate_application())
					//{
					//	break;
					//}
					{
						xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
					}

					char xor_code = _wtoi(xor_code_string);

					encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer, send_buffer_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
					}
				}



				local_blocking_socket.Close();

				if (local_bytes_sent <= 0 && local_error_number == 0)
				{
					//			galaxy::MessageBox(CString(L"local_bytes_sent<=0"),CString(L"Ошибка"));
				}

				if (local_main_dialog->get_command_threads_web_camera_video_stop())
				{
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			//ULONGLONG local_end_time = GetTickCount64();

			//ULONGLONG local_work_time = local_end_time - local_start_time;

			//if(local_main_dialog->get_command_threads_web_camera_video_stop())
			//{
			//	break;
			//}
			//else
			//{
			//	CString local_string_speed;

			//	if(local_main_dialog->get_command_terminate_application())
			//	{
			//		break;
			//	}
			//	{
			//		local_string_speed = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_SPEED_state;
			//	}
			//	double local_double_speed = _wtof(local_string_speed);
			//	if(local_data_size_send!=0.0)
			//	{
			//		DWORD time_to_work = 1000;
			//		double local_current_time_in_seconds = double(local_work_time)/1000.0;
			//		double local_current_bits_send = (local_data_size_send*8.0);
			//		double local_current_speed = local_current_bits_send/local_current_time_in_seconds;
			//		double local_current_frame_rate = 1.0/local_current_time_in_seconds;
			//		double local_speed_factor = local_double_speed/local_current_speed;
			//		double local_common_factor = min(local_speed_factor,local_current_frame_rate);

			//		time_to_work = DWORD(local_common_factor*local_work_time);

			//		DWORD time_to_sleep = 1000 - time_to_work;

			//		Sleep(time_to_sleep);
			//	}
			//}
		}

		//ULONGLONG local_end_time = GetTickCount64();

		//ULONGLONG local_work_time = local_end_time - local_start_time;

		//if(local_main_dialog->get_command_threads_web_camera_video_stop())
		//{
		//	break;
		//}
		//else
		//{
		//	CString local_string_speed;

		//	if(local_main_dialog->get_command_terminate_application())
		//	{
		//		break;
		//	}
		//	{
		//		local_string_speed = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_SPEED_state;
		//	}
		//	double local_double_speed = _wtof(local_string_speed);
		//	if(local_data_size_send!=0.0)
		//	{
		//		DWORD time_to_work = 1000;
		//		double local_current_time_in_seconds = double(local_work_time)/1000.0;
		//		double local_current_bits_send = (local_data_size_send*8.0);
		//		double local_current_speed = local_current_bits_send/local_current_time_in_seconds;
		//		double local_current_frame_rate = 1.0/local_current_time_in_seconds;
		//		double local_speed_factor = local_double_speed/local_current_speed;
		//		double local_common_factor = min(local_speed_factor,local_current_frame_rate);

		//		time_to_work = DWORD(local_common_factor*local_work_time);

		//		DWORD time_to_sleep = 1000 - time_to_work;

		//		Sleep(time_to_sleep);
		//	}
		//}

		if (local_main_dialog->get_command_threads_web_camera_video_stop())
		{
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete[] send_buffer_this_time;

		delete local_send_web_camera_video_thread_parameters_structure;
		return 1;
	}
}

UINT __cdecl datagram_send_web_camera_video_connection_thread_ip_6(LPVOID parameter)
{
	DWORD sequence_number = DWORD(rand());

	thread_send_web_camera_video_parameters_structure_type *local_send_web_camera_video_thread_parameters_structure = (thread_send_web_camera_video_parameters_structure_type *)parameter;

	if (local_send_web_camera_video_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_send_web_camera_video_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_send_web_camera_video_thread_parameters_structure;
		return 0;
	}

	char *send_buffer = new char[CONST_MESSAGE_LENGTH_IMAGE];

	if (send_buffer == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_send_web_camera_video_thread_parameters_structure;
		return 0;
	}

	char *send_buffer_this_time = new char[CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME];

	if (send_buffer_this_time == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete local_send_web_camera_video_thread_parameters_structure;
		return 0;
	}

	for (;;)
	{
		ULONGLONG local_start_time = GetTickCount64();

		if (local_main_dialog->get_command_threads_web_camera_video_stop())
		{
			break;
		}

		CString local_address(localhost_definition);

		CStringA local_address_internet_address;

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}
		int peers_to_send_count = 0;
		{
			DWORD dwWaitResult;

			dwWaitResult = WaitForSingleObject(
				local_main_dialog->do_not_terminate_application_event, // event handle
				0);    // zero wait

			if (dwWaitResult == WAIT_OBJECT_0)
			{
				// Event object was signaled		

				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_web_camera_video_thread_parameters_structure;
					return 0;
				}

				for (
					std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
					current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
					current_item_iterator++
					)
				{
					if (current_item_iterator->is_checked > 0)
					{
						peers_to_send_count++;
					}
				}
			}
			else
			{
				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_web_camera_video_thread_parameters_structure;
					return 0;
				}
			}

		}

		CImage local_image;

		if (local_main_dialog->web_camera_dialog != NULL)
		{
			CSingleLock lock(&local_main_dialog->delete_web_camera_dialog_critical_section);

			lock.Lock();

			//CaptureWndShot(local_main_dialog->web_camera_dialog->m_hWnd,&local_image,ENGINE_WINDOWS_SIZE_CX/2,ENGINE_WINDOWS_SIZE_CY/2);
			CaptureVideoSampleShot(local_main_dialog->web_camera_dialog, &local_image);
		}

		CComPtr<IStream> local_stream;

		HRESULT local_create_IStream_result = CreateStreamOnHGlobal(0, TRUE, &local_stream);

		try
		{
			HRESULT local_save_result = local_image.Save(local_stream, Gdiplus::ImageFormatJPEG);
		}
		catch (...)
		{
			continue;
		}

		STATSTG local_istream_statstg;
		HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg, STATFLAG::STATFLAG_DEFAULT);

		double local_data_size_send = 0.0;

		for (int peers_to_send_count_counter = 0; peers_to_send_count_counter < peers_to_send_count; peers_to_send_count_counter++)
		{
			//double local_data_size_send = 0.0;

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

#ifdef use_istream_DEFINITION
			//			CImage local_image;
#endif

			CString peer_to_send;

			int loop_counter = 0;
			for (
				std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
				current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
				current_item_iterator++
				)
			{
				Sleep(1);
				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
				if (current_item_iterator->is_checked > 0)
				{
					if (peers_to_send_count_counter == loop_counter)
					{
						peer_to_send = current_item_iterator->label;
						break;
					}
					loop_counter++;
				}

				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
			}

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

			local_address = peer_to_send;


			//	Здесь делаем снимок экрана

			//CSingleLock local_image_lock(&local_main_dialog->draw_video_image_critical_section);

			//local_image_lock.Lock();
#ifdef use_istream_DEFINITION
			//if(!local_image.IsNull())
			//{
			//	local_image.Destroy();
			//}
#endif

			if (network::ip_6::domain_name_to_internet_6_name(local_address, local_address_internet_address) == false)
			{
				const int local_error_message_size = 10000;
				wchar_t local_error_message[local_error_message_size];

				const int local_system_error_message_size = local_error_message_size - 1000;
				wchar_t local_system_error_message[local_system_error_message_size];

				wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

				CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

				wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

				continue;
			}

#ifdef use_istream_DEFINITION
			//if(local_main_dialog->web_camera_dialog!=NULL)
			//{
			//	CSingleLock lock(&local_main_dialog->delete_web_camera_dialog_critical_section);

			//	lock.Lock();

			//	//CaptureWndShot(local_main_dialog->web_camera_dialog->m_hWnd,&local_image,ENGINE_WINDOWS_SIZE_CX/2,ENGINE_WINDOWS_SIZE_CY/2);
			//	CaptureVideoSampleShot(local_main_dialog->web_camera_dialog,&local_image);
			//}

			//CComPtr<IStream> local_stream;

			//HRESULT local_create_IStream_result = CreateStreamOnHGlobal ( 0 , TRUE , &local_stream );

			//try
			//{
			//	HRESULT local_save_result = local_image.Save(local_stream, Gdiplus::ImageFormatJPEG);
			//}
			//catch(...)
			//{
			//	continue;
			//}
#endif
			//local_image_lock.Unlock();


			//ULONGLONG local_start_time = GetTickCount64();

			//STATSTG local_istream_statstg;
			//HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg,STATFLAG::STATFLAG_DEFAULT);

			/*/
			//			Тест отображения из IStream
			{
			std::fstream local_file;
			local_file.open("c:\\temp\\file_source.jpg",std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

			char *local_file_data = new char[CONST_MESSAGE_LENGTH_IMAGE];

			ULONG local_read_bytes = 0;

			LARGE_INTEGER liBeggining = { 0 };

			local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

			local_stream->Read(local_file_data,CONST_MESSAGE_LENGTH_IMAGE,&local_read_bytes);

			local_file.write(local_file_data, local_read_bytes);

			delete [] local_file_data;

			local_file.flush();

			local_file.close();
			}
			//*/



			for (UINT local_parameter_port_number = port_number_start_const; local_parameter_port_number <= port_number_end_const; local_parameter_port_number++)
			{
				network::ip_6::CBlockingSocket_ip_6 local_blocking_socket;

				if (network::ip_6::domain_name_to_internet_6_name(local_address, local_address_internet_address) == false)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					break;
				}

				network::ip_6::CSockAddr_ip_6 local_socket_address(local_address_internet_address, local_parameter_port_number);

				try
				{
					local_blocking_socket.Create(SOCK_DGRAM);
				}
				catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					local_blocking_socket_exception->Delete();

					delete local_send_web_camera_video_thread_parameters_structure;

					//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete[] send_buffer_this_time;

					return 0;
				}

				int local_bytes_sent = 0;
				int local_error_number = 0;


				int send_buffer_data_length = service_signature_definition_length;

				ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH_IMAGE);

				memcpy(send_buffer, service_signature, service_signature_definition_length);



				CString send_data_string = command_web_camera_video;

				int send_data_string_length = send_data_string.GetLength();

				memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

				send_buffer_data_length += send_data_string_length * sizeof(wchar_t);

				wchar_t zero_word = L'\0';
				memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
				send_buffer_data_length += sizeof(wchar_t);
				send_data_string_length++;


				memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
				send_buffer_data_length += sizeof(DWORD);
				send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);



				LARGE_INTEGER liBeggining = { 0 };
#ifdef use_istream_DEFINITION
				local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);
#endif


				ULONG local_read = 0;
#ifdef use_istream_DEFINITION
				local_stream->Read(send_buffer + send_buffer_data_length, CONST_MESSAGE_LENGTH_IMAGE - send_buffer_data_length, &local_read);
#else
				local_read = 100000;
#endif
				send_buffer_data_length += local_read;




				CString xor_code_string;

				//if(local_main_dialog->get_command_terminate_application())
				//{
				//	break;
				//}
				{
					xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
				}

				char xor_code = _wtoi(xor_code_string);

				encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

				int local_header_length = service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD);

				int local_part_counter = 0;
				int local_parts_count = local_read / (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));
				int local_last_part_size = local_read % (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));

				if (local_last_part_size != 0)
				{
					local_parts_count++;
				}

				for (; local_part_counter < local_parts_count; local_part_counter++)
				{
					int send_buffer_this_time_data_length = CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME;

					if (local_part_counter == local_parts_count - 1)
					{
						if (local_last_part_size != 0)
						{
							send_buffer_this_time_data_length = local_last_part_size + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD);
						}
					}

					ZeroMemory(send_buffer_this_time, CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME);

					memcpy(send_buffer_this_time, send_buffer, service_signature_definition_length + send_data_string_length * sizeof(wchar_t));

					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[0] = DWORD(local_part_counter + 1);
					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[1] = DWORD(local_parts_count);

					encrypt::encrypt_xor(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t), 2 * sizeof(DWORD), xor_code);

					int local_this_time_data_offset = local_part_counter*(CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));

					memcpy
					(
						send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD),
						send_buffer + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + local_this_time_data_offset,
						send_buffer_this_time_data_length - (service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD))
					);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer_this_time, send_buffer_this_time_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
						break;
					}

					//if(local_main_dialog->get_command_threads_web_camera_video_stop())
					//{
					//	break;
					//}
					//else
					{
						ULONGLONG local_end_time = GetTickCount64();

						ULONGLONG local_work_time = local_end_time - local_start_time;

						if (local_main_dialog->get_command_threads_web_camera_video_stop())
						{
							break;
						}
						else
						{
							CString local_string_speed;

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								local_string_speed = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_SPEED_state;
							}
							double local_double_speed = _wtof(local_string_speed);
							if (local_data_size_send != 0.0)
							{
								DWORD time_to_work = 1000;
								double local_current_time_in_seconds = double(local_work_time) / 1000.0;
								double local_current_bits_send = (local_data_size_send*8.0);
								double local_current_speed = local_current_bits_send / local_current_time_in_seconds;
								double local_current_frame_rate = 1.0 / local_current_time_in_seconds;
								double local_speed_factor = local_double_speed / local_current_speed;
								double local_common_factor = min(local_speed_factor, local_current_frame_rate);

								time_to_work = DWORD(local_common_factor*local_work_time);

								DWORD time_to_sleep = 1000 - time_to_work;

								//Sleep(time_to_sleep);
								Sleep(10);
							}
						}
					}

				}

				{
					int local_bytes_sent = 0;
					int local_error_number = 0;

					char send_buffer[CONST_MESSAGE_LENGTH];

					int send_buffer_data_length = service_signature_definition_length;

					ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH);

					memcpy(send_buffer, service_signature, service_signature_definition_length);

					CString send_data_string;

					int send_data_string_length = 0;

					send_data_string = command_web_camera_video_end;

					send_data_string_length = send_data_string.GetLength();

					memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

					send_buffer_data_length += send_data_string_length * sizeof(wchar_t);


					wchar_t zero_word = L'\0';
					memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
					send_buffer_data_length += sizeof(wchar_t);
					send_data_string_length++;


					memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
					send_buffer_data_length += sizeof(DWORD);
					send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);


					CString xor_code_string;

					//if(local_main_dialog->get_command_terminate_application())
					//{
					//	break;
					//}
					{
						xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
					}

					char xor_code = _wtoi(xor_code_string);

					encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer, send_buffer_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
					}
				}



				local_blocking_socket.Close();

				if (local_bytes_sent <= 0 && local_error_number == 0)
				{
					//			galaxy::MessageBox(CString(L"local_bytes_sent<=0"),CString(L"Ошибка"));
				}

				if (local_main_dialog->get_command_threads_web_camera_video_stop())
				{
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			//ULONGLONG local_end_time = GetTickCount64();

			//ULONGLONG local_work_time = local_end_time - local_start_time;

			//if(local_main_dialog->get_command_threads_web_camera_video_stop())
			//{
			//	break;
			//}
			//else
			//{
			//	CString local_string_speed;

			//	if(local_main_dialog->get_command_terminate_application())
			//	{
			//		break;
			//	}
			//	{
			//		local_string_speed = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_SPEED_state;				
			//	}
			//	double local_double_speed = _wtof(local_string_speed);
			//	if(local_data_size_send!=0.0)
			//	{
			//		DWORD time_to_work = 1000;
			//		double local_current_time_in_seconds = double(local_work_time)/1000.0;
			//		double local_current_bits_send = (local_data_size_send*8.0);
			//		double local_current_speed = local_current_bits_send/local_current_time_in_seconds;
			//		double local_current_frame_rate = 1.0/local_current_time_in_seconds;
			//		double local_speed_factor = local_double_speed/local_current_speed;
			//		double local_common_factor = min(local_speed_factor,local_current_frame_rate);

			//		time_to_work = DWORD(local_common_factor*local_work_time);

			//		DWORD time_to_sleep = 1000 - time_to_work;

			//		Sleep(time_to_sleep);
			//	}
			//}
		}

		//ULONGLONG local_end_time = GetTickCount64();

		//ULONGLONG local_work_time = local_end_time - local_start_time;

		//if(local_main_dialog->get_command_threads_web_camera_video_stop())
		//{
		//	break;
		//}
		//else
		//{
		//	CString local_string_speed;

		//	if(local_main_dialog->get_command_terminate_application())
		//	{
		//		break;
		//	}
		//	{
		//		local_string_speed = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_SPEED_state;				
		//	}
		//	double local_double_speed = _wtof(local_string_speed);
		//	if(local_data_size_send!=0.0)
		//	{
		//		DWORD time_to_work = 1000;
		//		double local_current_time_in_seconds = double(local_work_time)/1000.0;
		//		double local_current_bits_send = (local_data_size_send*8.0);
		//		double local_current_speed = local_current_bits_send/local_current_time_in_seconds;
		//		double local_current_frame_rate = 1.0/local_current_time_in_seconds;
		//		double local_speed_factor = local_double_speed/local_current_speed;
		//		double local_common_factor = min(local_speed_factor,local_current_frame_rate);

		//		time_to_work = DWORD(local_common_factor*local_work_time);

		//		DWORD time_to_sleep = 1000 - time_to_work;

		//		Sleep(time_to_sleep);
		//	}
		//}


		if (local_main_dialog->get_command_threads_web_camera_video_stop())
		{
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete[] send_buffer_this_time;

		delete local_send_web_camera_video_thread_parameters_structure;
		return 1;
	}
}

void Cstl_network_ip_4_ip_6_udp_engineDialog::PrepareWebCameraVideo(CString parameter_string, BYTE* parameter_data, size_t parameter_data_length)
{
	if (get_command_terminate_application())
	{
		if (parameter_data != NULL)
		{
			delete[]parameter_data;
		}

		return;
	}

	CSingleLock local_lock(&draw_web_camera_video_critical_section);

	local_lock.Lock();

	DWORD received_sequence = *(DWORD*)(parameter_data);

	DWORD received_part = ((DWORD*)(parameter_data))[1];
	DWORD received_total_parts = ((DWORD*)(parameter_data))[2];

	std::list<STREAM_INFORMATION>::iterator current_received_web_camera_video_stream = received_web_camera_video_stream.begin();

	for (; current_received_web_camera_video_stream != received_web_camera_video_stream.end(); current_received_web_camera_video_stream++)
	{
		if (current_received_web_camera_video_stream->sequence_number == received_sequence)
		{
			break;
		}
	}

	HRESULT local_create_IStream_result = S_OK;

	ULONG local_bytes_written = 0;
	if (current_received_web_camera_video_stream == received_web_camera_video_stream.end())
	{
		STREAM_INFORMATION local_new_stream;

		local_new_stream.sequence_number = received_sequence;

		local_create_IStream_result = CreateStreamOnHGlobal(0, TRUE, &local_new_stream.stream);

		received_web_camera_video_stream.push_front(local_new_stream);

		current_received_web_camera_video_stream = received_web_camera_video_stream.begin();
	}

	if (SUCCEEDED(local_create_IStream_result))
	{
		HRESULT local_write_result = current_received_web_camera_video_stream->stream->Write(parameter_data + sizeof(DWORD) + 2 * sizeof(DWORD), parameter_data_length - sizeof(DWORD) - 2 * sizeof(DWORD), &local_bytes_written);

		if (SUCCEEDED(local_write_result))
		{
			int local_IDC_CHECK_RETRANSLATE_WEB_CAMERA_state = 0;

			{
				CSingleLock lock(&GUI_update_critical_section);
				lock.Lock();

				local_IDC_CHECK_RETRANSLATE_WEB_CAMERA_state = GUI_CONTROLS_STATE_data.IDC_CHECK_RETRANSLATE_WEB_CAMERA_state;
			}

			if (local_IDC_CHECK_RETRANSLATE_WEB_CAMERA_state != 0)
			{

			}
		}
	}

	delete[] parameter_data;

	local_lock.Unlock();
}

void Cstl_network_ip_4_ip_6_udp_engineDialog::DrawWebCameraVideo(CString parameter_string, BYTE* parameter_data, size_t parameter_data_length)
{
	if (get_command_terminate_application())
	{
		if (parameter_data != NULL)
		{
			delete[]parameter_data;
		}

		return;
	}

	{
		CSingleLock local_lock(&draw_web_camera_video_critical_section);

		local_lock.Lock();

		DWORD received_sequence = *(DWORD*)(parameter_data);

		std::list<STREAM_INFORMATION>::iterator current_received_web_camera_video_stream = received_web_camera_video_stream.begin();

		for (; current_received_web_camera_video_stream != received_web_camera_video_stream.end(); current_received_web_camera_video_stream++)
		{
			if (current_received_web_camera_video_stream->sequence_number == received_sequence)
			{
				break;
			}
		}

		HRESULT local_create_IStream_result = S_OK;

		ULONG local_bytes_written = 0;
		if (current_received_web_camera_video_stream != received_web_camera_video_stream.end())
		{
			LARGE_INTEGER liBeggining = { 0 };

			current_received_web_camera_video_stream->stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

			STATSTG local_istream_statstg;
			HRESULT local_stat_result = current_received_web_camera_video_stream->stream->Stat(&local_istream_statstg, STATFLAG::STATFLAG_DEFAULT);

			WEB_CAMERA_FRAME current_received_web_camera_frame;

			current_received_web_camera_frame.arrival_time = CTime::GetCurrentTime();
			current_received_web_camera_frame.frame_data_size = UINT(local_istream_statstg.cbSize.QuadPart);
			current_received_web_camera_frame.frame_data = new BYTE[current_received_web_camera_frame.frame_data_size];

			if (current_received_web_camera_frame.frame_data != NULL)
			{
				ULONG bytes_read = 0;
				HRESULT local_Stream_Read_RESULT = current_received_web_camera_video_stream->stream->Read(current_received_web_camera_frame.frame_data, ULONG(local_istream_statstg.cbSize.QuadPart), &bytes_read);

				if (SUCCEEDED(local_Stream_Read_RESULT))
				{
					if (ULONG(local_istream_statstg.cbSize.QuadPart) != bytes_read)
					{
						CString Error(L"Error");

						delete[] current_received_web_camera_frame.frame_data;
						current_received_web_camera_frame.frame_data = NULL;
					}
					else
					{
						if (received_web_camera_video_frame_stream.size() == 0)
						{
							STREAM_WEB_CAMERA_FRAME_INFORMATION current_received_web_camera_video_frame;

							current_received_web_camera_video_frame.frames.push_back(current_received_web_camera_frame);
							current_received_web_camera_video_frame.sequence_source_number = received_sequence;
							current_received_web_camera_video_frame.source_address_string = parameter_string;

							received_web_camera_video_frame_stream.push_back(current_received_web_camera_video_frame);
						}
						else
						{
							auto current_received_web_camera_video_frame_stream_iterator = received_web_camera_video_frame_stream.begin();

							for (; current_received_web_camera_video_frame_stream_iterator != received_web_camera_video_frame_stream.end(); current_received_web_camera_video_frame_stream_iterator++)
							{
								if (current_received_web_camera_video_frame_stream_iterator->sequence_source_number == received_sequence)
								{
									current_received_web_camera_video_frame_stream_iterator->frames.push_back(current_received_web_camera_frame);
									current_received_web_camera_video_frame_stream_iterator->source_address_string = parameter_string;

									break;
								}
							}

							if (current_received_web_camera_video_frame_stream_iterator == received_web_camera_video_frame_stream.end())
							{
								STREAM_WEB_CAMERA_FRAME_INFORMATION current_received_web_camera_video_frame;

								current_received_web_camera_video_frame.frames.push_back(current_received_web_camera_frame);
								current_received_web_camera_video_frame.sequence_source_number = received_sequence;
								current_received_web_camera_video_frame.source_address_string = parameter_string;

								received_web_camera_video_frame_stream.push_back(current_received_web_camera_video_frame);
							}
						}
					}
				}
			}

			ULARGE_INTEGER zero_size = { 0,0 };
			current_received_web_camera_video_stream->stream->SetSize(zero_size);

			{
				LARGE_INTEGER liBeggining = { 0 };

				current_received_web_camera_video_stream->stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);
			}
		}
	}

	if (parameter_data != NULL)
	{
		delete[]parameter_data;
	}
}

void Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckEnableVideo()
{
	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();

	GUI_CONTROLS_STATE_data.IDC_CHECK_ENABLE_VIDEO_state = button_enable_draw_video.GetCheck();

	if (button_enable_draw_video.GetCheck() != 0)
	{
		if (received_video_dialog == NULL)
		{
			received_video_dialog = new CReceivedVideoDialog(this);
			received_video_dialog->main_dialog = this;
			received_video_dialog->Create(CReceivedVideoDialog::IDD);
			received_video_dialog->ShowWindow(SW_SHOWNORMAL);
		}

		{
			CSingleLock local_lock(&draw_video_critical_section);

			local_lock.Lock();

			std::list<STREAM_FRAME_INFORMATION>::iterator current_received_video_frame_stream = received_video_frame_stream.begin();

			for (; current_received_video_frame_stream != received_video_frame_stream.end(); current_received_video_frame_stream++)
			{
				for
					(
						std::list<FRAME>::iterator current_received_video_frame = current_received_video_frame_stream->frames.begin()
						;
						current_received_video_frame != current_received_video_frame_stream->frames.end()
						;
						current_received_video_frame++
						)
				{
					for (
						std::list<FRAME_PART>::iterator local_frame_parts_iterator = current_received_video_frame->frame_parts.begin()
						;
						local_frame_parts_iterator != current_received_video_frame->frame_parts.end()
						;
						local_frame_parts_iterator++
						)
					{
						delete[]local_frame_parts_iterator->frame_part_data;
					}
				}
			}

			received_video_frame_stream.clear();
		}
	}
	else
	{
		{
			if (received_video_dialog != NULL)
			{
				CSingleLock lock(&delete_received_video_dialog_critical_section);

				lock.Lock();

				CReceivedVideoDialog *local_received_video_dialog = received_video_dialog;

				received_video_dialog = NULL;

				lock.Unlock();

				//local_received_video_dialog->EndDialog(IDOK);
				local_received_video_dialog->SendMessage(WM_CLOSE);

				//delete local_received_video_dialog;
			}
		}
	}
}


void Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckEnableSound()
{
	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();

	GUI_CONTROLS_STATE_data.IDC_CHECK_ENABLE_SOUND_state = button_enable_play_audio.GetCheck();

	if (button_enable_play_audio.GetCheck() != 0)
	{
		/*	if(received_video_dialog==NULL)
		{
		received_video_dialog = new CReceivedVideoDialog(this);
		received_video_dialog->main_dialog = this;
		received_video_dialog->Create(CReceivedVideoDialog::IDD);
		received_video_dialog->ShowWindow(SW_SHOWNORMAL);
		}
		*/
	}
	else
	{
		{
			//if(received_video_dialog!=NULL)
			//{
			//	CSingleLock lock(&delete_received_video_dialog_critical_section);

			//	lock.Lock();

			//	CReceivedVideoDialog *local_received_video_dialog = received_video_dialog;

			//	received_video_dialog = NULL;

			//	lock.Unlock();

			//	local_received_video_dialog->EndDialog(IDOK);
			//	local_received_video_dialog->SendMessage(WM_CLOSE);

			//	//delete local_received_video_dialog;
			//}
		}
	}
}


//	send audio

UINT __cdecl datagram_send_audio_connection_thread(LPVOID parameter)
{
	thread_send_audio_parameters_structure_type *local_send_audio_thread_parameters_structure_source = (thread_send_audio_parameters_structure_type *)parameter;

	if (local_send_audio_thread_parameters_structure_source == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = local_send_audio_thread_parameters_structure_source->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete parameter;

		return 0;
	}

	{
		void *local_send_audio_thread_parameters_structure = new thread_send_audio_parameters_structure_type;

		((thread_send_audio_parameters_structure_type*)local_send_audio_thread_parameters_structure)->parameter_main_dialog = ((thread_send_audio_parameters_structure_type*)local_send_audio_thread_parameters_structure_source)->parameter_main_dialog;

		CWinThread *local_thread = AfxBeginThread(datagram_send_audio_connection_thread_ip_4, local_send_audio_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_send_audio_connection_thread_ip_4");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
		local_lock.Lock();

		local_main_dialog->threads_list.push_back(local_thread_information);
	}

	{
		void *local_send_audio_thread_parameters_structure = new thread_send_audio_parameters_structure_type;

		((thread_send_audio_parameters_structure_type*)local_send_audio_thread_parameters_structure)->parameter_main_dialog = ((thread_send_audio_parameters_structure_type*)local_send_audio_thread_parameters_structure_source)->parameter_main_dialog;

		CWinThread *local_thread = AfxBeginThread(datagram_send_audio_connection_thread_ip_6, local_send_audio_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_send_audio_connection_thread_ip_6");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
		local_lock.Lock();

		local_main_dialog->threads_list.push_back(local_thread_information);
	}

	{
		clear_thread_information(local_main_dialog);
	}

	delete parameter;

	return 1;
}



//	send audio

UINT __cdecl datagram_send_audio_connection_thread_ip_4(LPVOID parameter)
{
	DWORD sequence_number = DWORD(rand());

	thread_send_audio_parameters_structure_type *local_send_audio_thread_parameters_structure = (thread_send_audio_parameters_structure_type *)parameter;

	if (local_send_audio_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_send_audio_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_send_audio_thread_parameters_structure;
		return 0;
	}

	char *send_buffer = new char[CONST_MESSAGE_LENGTH_IMAGE];

	if (send_buffer == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_send_audio_thread_parameters_structure;
		return 0;
	}

	char *send_buffer_this_time = new char[CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME];

	if (send_buffer_this_time == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete local_send_audio_thread_parameters_structure;
		return 0;
	}

	for (;;)
	{
		ULONGLONG local_start_time = GetTickCount64();

		if (local_main_dialog->get_command_threads_audio_stop())
		{
			break;
		}

		CString local_address(localhost_definition);

		CStringA local_address_internet_address;

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}

		int peers_to_send_count = 0;
		{
			DWORD dwWaitResult;

			dwWaitResult = WaitForSingleObject(
				local_main_dialog->do_not_terminate_application_event, // event handle
				0);    // zero wait

			if (dwWaitResult == WAIT_OBJECT_0)
			{
				// Event object was signaled		

				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_audio_thread_parameters_structure;
					return 0;
				}

				for (
					std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
					current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
					current_item_iterator++
					)
				{
					if (current_item_iterator->is_checked > 0)
					{
						peers_to_send_count++;
					}
				}
			}
			else
			{
				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_audio_thread_parameters_structure;
					return 0;
				}
			}

		}

		CComPtr<IStream> local_stream;

		if (local_main_dialog->web_camera_dialog != NULL)
		{
			CSingleLock lock(&local_main_dialog->delete_web_camera_dialog_critical_section);

			lock.Lock();

			CaptureAudioSampleGetFromTheList_ip_4(local_main_dialog->web_camera_dialog, &local_stream);
		}

		if (local_stream == NULL)
		{
			Sleep(10);
			continue;
		}

		STATSTG local_istream_statstg;
		HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg, STATFLAG::STATFLAG_DEFAULT);

		double local_data_size_send = 0.0;

		for (int peers_to_send_count_counter = 0; peers_to_send_count_counter < peers_to_send_count; peers_to_send_count_counter++)
		{
			//double local_data_size_send = 0.0;

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

#ifdef use_istream_DEFINITION
			//IStream * local_stream = NULL;
#endif

			CString peer_to_send;

			int loop_counter = 0;
			for (
				std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
				current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
				current_item_iterator++
				)
			{
				Sleep(1);
				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
				if (current_item_iterator->is_checked > 0)
				{
					if (peers_to_send_count_counter == loop_counter)
					{
						peer_to_send = current_item_iterator->label;
						break;
					}
					loop_counter++;
				}

				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
			}

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

			local_address = peer_to_send;


			//	Здесь делаем снимок экрана

			//CSingleLock local_image_lock(&local_main_dialog->draw_audio_image_critical_section);

			//local_image_lock.Lock();
#ifdef use_istream_DEFINITION
#endif

			if (network::ip_4::domain_name_to_internet_4_name(local_address, local_address_internet_address) == false)
			{
				const int local_error_message_size = 10000;
				wchar_t local_error_message[local_error_message_size];

				const int local_system_error_message_size = local_error_message_size - 1000;
				wchar_t local_system_error_message[local_system_error_message_size];

				wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

				CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

				wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

				continue;
			}
#ifdef use_istream_DEFINITION
			//if(local_main_dialog->web_camera_dialog!=NULL)
			//{
			//	CSingleLock lock(&local_main_dialog->delete_web_camera_dialog_critical_section);

			//	lock.Lock();

			//	//local_stream = NULL;

			//	//ULARGE_INTEGER zero_size = {0,0};

			//	//local_stream->SetSize(zero_size);

			//	//LARGE_INTEGER liBeggining = { 0 };

			//	//local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

			//	//CaptureAudioSampleShot(local_main_dialog->web_camera_dialog,local_stream);
			//	CaptureAudioSampleGetFromTheList(local_main_dialog->web_camera_dialog,&local_stream);

			//	//if(local_stream==NULL)
			//	//{
			//	//	continue;
			//	//}
			//}

#endif
			//local_image_lock.Unlock();

			//if(local_stream==NULL)
			//{
			//	continue;
			//}


			//ULONGLONG local_start_time = GetTickCount64();

			//STATSTG local_istream_statstg;
			//HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg,STATFLAG::STATFLAG_DEFAULT);

			/*/
			//			Тест отображения из IStream
			{
			std::fstream local_file;
			local_file.open("c:\\temp\\file_source_2.wav",std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

			char *local_file_data = new char[CONST_MESSAGE_LENGTH_IMAGE];

			ULONG local_read_bytes = 0;

			LARGE_INTEGER liBeggining = { 0 };

			local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

			local_stream->Read(local_file_data,CONST_MESSAGE_LENGTH_IMAGE,&local_read_bytes);

			local_file.write(local_file_data, local_read_bytes);

			delete [] local_file_data;

			local_file.flush();

			local_file.close();
			}
			//*/

			for (UINT local_parameter_port_number = port_number_start_const; local_parameter_port_number <= port_number_end_const; local_parameter_port_number++)
			{
				network::ip_4::CBlockingSocket_ip_4 local_blocking_socket;

				if (network::ip_4::domain_name_to_internet_4_name(local_address, local_address_internet_address) == false)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					break;
				}

				network::ip_4::CSockAddr_ip_4 local_socket_address(local_address_internet_address, local_parameter_port_number);

				try
				{
					local_blocking_socket.Create(SOCK_DGRAM);
				}
				catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					local_blocking_socket_exception->Delete();

					delete local_send_audio_thread_parameters_structure;

					//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

					{
						clear_thread_information(local_main_dialog);
					}

					//if(local_stream!=NULL)
					//{
					//	local_stream->Release();
					//}

					delete[] send_buffer;
					delete[] send_buffer_this_time;

					return 0;
				}

				int local_bytes_sent = 0;
				int local_error_number = 0;


				int send_buffer_data_length = service_signature_definition_length;

				ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH_IMAGE);

				memcpy(send_buffer, service_signature, service_signature_definition_length);



				CString send_data_string = command_audio;

				int send_data_string_length = send_data_string.GetLength();

				memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

				send_buffer_data_length += send_data_string_length * sizeof(wchar_t);

				wchar_t zero_word = L'\0';
				memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
				send_buffer_data_length += sizeof(wchar_t);
				send_data_string_length++;


				memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
				send_buffer_data_length += sizeof(DWORD);
				send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);



				LARGE_INTEGER liBeggining = { 0 };
#ifdef use_istream_DEFINITION
				local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);
#endif


				ULONG local_read = 0;
#ifdef use_istream_DEFINITION
				local_stream->Read(send_buffer + send_buffer_data_length, CONST_MESSAGE_LENGTH_IMAGE - send_buffer_data_length, &local_read);
#else
				local_read = 100000;
#endif
				send_buffer_data_length += local_read;




				CString xor_code_string;

				//if(local_main_dialog->get_command_terminate_application())
				//{
				//	break;
				//}
				{
					xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
				}

				char xor_code = _wtoi(xor_code_string);

				encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

				int local_header_length = service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD);

				int local_part_counter = 0;
				int local_parts_count = local_read / (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));
				int local_last_part_size = local_read % (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));

				if (local_last_part_size != 0)
				{
					local_parts_count++;
				}

				for (; local_part_counter < local_parts_count; local_part_counter++)
				{
					int send_buffer_this_time_data_length = CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME;

					if (local_part_counter == local_parts_count - 1)
					{
						if (local_last_part_size != 0)
						{
							send_buffer_this_time_data_length = local_last_part_size + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD);
						}
					}

					ZeroMemory(send_buffer_this_time, CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME);

					memcpy(send_buffer_this_time, send_buffer, service_signature_definition_length + send_data_string_length * sizeof(wchar_t));

					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[0] = DWORD(local_part_counter + 1);
					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[1] = DWORD(local_parts_count);

					encrypt::encrypt_xor(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t), 2 * sizeof(DWORD), xor_code);

					int local_this_time_data_offset = local_part_counter*(CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));

					memcpy
					(
						send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD),
						send_buffer + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + local_this_time_data_offset,
						send_buffer_this_time_data_length - (service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD))
					);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer_this_time, send_buffer_this_time_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
						break;
					}

					//if(local_main_dialog->get_command_threads_audio_stop())
					//{
					//	break;
					//}
					//else
					{
						Sleep(10);
					}

				}

				{
					int local_bytes_sent = 0;
					int local_error_number = 0;

					char send_buffer[CONST_MESSAGE_LENGTH];

					int send_buffer_data_length = service_signature_definition_length;

					ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH);

					memcpy(send_buffer, service_signature, service_signature_definition_length);

					CString send_data_string;

					int send_data_string_length = 0;

					send_data_string = command_audio_end;

					send_data_string_length = send_data_string.GetLength();

					memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

					send_buffer_data_length += send_data_string_length * sizeof(wchar_t);


					wchar_t zero_word = L'\0';
					memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
					send_buffer_data_length += sizeof(wchar_t);
					send_data_string_length++;


					memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
					send_buffer_data_length += sizeof(DWORD);
					send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);


					CString xor_code_string;

					//if(local_main_dialog->get_command_terminate_application())
					//{
					//	break;
					//}
					{
						xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
					}

					char xor_code = _wtoi(xor_code_string);

					encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer, send_buffer_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
					}
				}



				local_blocking_socket.Close();

				if (local_bytes_sent <= 0 && local_error_number == 0)
				{
					//			galaxy::MessageBox(CString(L"local_bytes_sent<=0"),CString(L"Ошибка"));
				}

				if (local_main_dialog->get_command_threads_audio_stop())
				{
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			//if(local_stream!=NULL)
			//{
			//	local_stream->Release();
			//}

			//ULONGLONG local_end_time = GetTickCount64();

			//ULONGLONG local_work_time = local_end_time - local_start_time;

			//if(local_main_dialog->get_command_threads_audio_stop())
			//{
			//	break;
			//}
			//else
			//{
			//	if(local_work_time>=1000)
			//		Sleep(1);
			//	else
			//	{
			//		HRESULT hr;

			//		AM_MEDIA_TYPE mt;
			//		ZeroMemory(&mt, sizeof(mt));

			//		if(local_main_dialog->web_camera_dialog!=NULL)
			//		{
			//			CSingleLock lock(&local_main_dialog->delete_web_camera_dialog_critical_section);

			//			lock.Lock();

			//			hr = local_main_dialog->web_camera_dialog->pAudioGrabber->GetConnectedMediaType(&mt);

			//			lock.Unlock();

			//			if (SUCCEEDED(hr))
			//			{
			//				if(mt.formattype==FORMAT_WaveFormatEx)
			//				{
			//					WAVEFORMATEX *wave_format_ex = (WAVEFORMATEX *)mt.pbFormat;

			//					if(wave_format_ex!=NULL)
			//					{
			//						ULONGLONG chunk_playing_time = 
			//							1000 *
			//							wave_format_ex->nAvgBytesPerSec/
			//							wave_format_ex->nSamplesPerSec/
			//							wave_format_ex->nChannels/
			//							(wave_format_ex->wBitsPerSample>>3)/
			//							wave_format_ex->nBlockAlign
			//							;

			//						if(chunk_playing_time>local_work_time)
			//							Sleep(DWORD(chunk_playing_time-local_work_time));
			//						else
			//							Sleep(1);
			//					}
			//				}
			//				_FreeMediaType(mt);	
			//			}
			//			else
			//				Sleep(DWORD(1000.0/CONST_AUDIO_PACKETS_PER_SECOND));		//	CONST_AUDIO_PACKETS_PER_SECOND кадров в секунду
			//		}
			//	}
			//}
		}

		ULONGLONG local_end_time = GetTickCount64();

		ULONGLONG local_work_time = local_end_time - local_start_time;

		if (local_main_dialog->get_command_threads_audio_stop())
		{
			break;
		}
		else
		{
			if (local_work_time >= 1000)
				Sleep(1);
			else
			{
				HRESULT hr;

				AM_MEDIA_TYPE mt;
				ZeroMemory(&mt, sizeof(mt));

				if (local_main_dialog->web_camera_dialog != NULL)
				{
					CSingleLock lock(&local_main_dialog->delete_web_camera_dialog_critical_section);

					lock.Lock();

					hr = local_main_dialog->web_camera_dialog->pAudioGrabber->GetConnectedMediaType(&mt);

					lock.Unlock();

					if (SUCCEEDED(hr))
					{
						if (mt.formattype == FORMAT_WaveFormatEx)
						{
							WAVEFORMATEX *wave_format_ex = (WAVEFORMATEX *)mt.pbFormat;

							if (wave_format_ex != NULL)
							{
								ULONGLONG chunk_playing_time =
									1000 *
									wave_format_ex->nAvgBytesPerSec /
									wave_format_ex->nSamplesPerSec /
									wave_format_ex->nChannels /
									(wave_format_ex->wBitsPerSample >> 3) /
									wave_format_ex->nBlockAlign
									;

								if (chunk_playing_time > local_work_time)
									Sleep(DWORD(chunk_playing_time - local_work_time));
								else
									Sleep(1);
							}
						}
						_FreeMediaType(mt);
					}
					else
					{
						double local_factor = peers_to_send_count*(port_number_end_const - port_number_start_const + 1);
						if (local_factor == 0.0)
						{
							local_factor = 1.0;
						}
						Sleep(DWORD(1000.0 / CONST_AUDIO_PACKETS_PER_SECOND / local_factor));		//	CONST_AUDIO_PACKETS_PER_SECOND кадров в секунду
					}
				}
			}
		}

		if (local_main_dialog->get_command_threads_audio_stop())
		{
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete[] send_buffer_this_time;

		delete local_send_audio_thread_parameters_structure;
		return 1;
	}
}

UINT __cdecl datagram_send_audio_connection_thread_ip_6(LPVOID parameter)
{
	DWORD sequence_number = DWORD(rand());

	thread_send_audio_parameters_structure_type *local_send_audio_thread_parameters_structure = (thread_send_audio_parameters_structure_type *)parameter;

	if (local_send_audio_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_send_audio_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_send_audio_thread_parameters_structure;
		return 0;
	}

	char *send_buffer = new char[CONST_MESSAGE_LENGTH_IMAGE];

	if (send_buffer == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_send_audio_thread_parameters_structure;
		return 0;
	}

	char *send_buffer_this_time = new char[CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME];

	if (send_buffer_this_time == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete local_send_audio_thread_parameters_structure;
		return 0;
	}

	for (;;)
	{
		ULONGLONG local_start_time = GetTickCount64();

		if (local_main_dialog->get_command_threads_audio_stop())
		{
			break;
		}

		CString local_address(localhost_definition);

		CStringA local_address_internet_address;

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}

		int peers_to_send_count = 0;
		{
			DWORD dwWaitResult;

			dwWaitResult = WaitForSingleObject(
				local_main_dialog->do_not_terminate_application_event, // event handle
				0);    // zero wait

			if (dwWaitResult == WAIT_OBJECT_0)
			{
				// Event object was signaled		

				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_audio_thread_parameters_structure;
					return 0;
				}

				for (
					std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
					current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
					current_item_iterator++
					)
				{
					if (current_item_iterator->is_checked > 0)
					{
						peers_to_send_count++;
					}
				}
			}
			else
			{
				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_audio_thread_parameters_structure;
					return 0;
				}
			}

		}

		CComPtr<IStream> local_stream;

		if (local_main_dialog->web_camera_dialog != NULL)
		{
			CSingleLock lock(&local_main_dialog->delete_web_camera_dialog_critical_section);

			lock.Lock();

			CaptureAudioSampleGetFromTheList_ip_6(local_main_dialog->web_camera_dialog, &local_stream);
		}

		if (local_stream == NULL)
		{
			Sleep(10);
			continue;
		}

		STATSTG local_istream_statstg;
		HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg, STATFLAG::STATFLAG_DEFAULT);

		double local_data_size_send = 0.0;

		for (int peers_to_send_count_counter = 0; peers_to_send_count_counter < peers_to_send_count; peers_to_send_count_counter++)
		{
			//double local_data_size_send = 0.0;

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

#ifdef use_istream_DEFINITION
			//IStream *local_stream = NULL;
#endif

			CString peer_to_send;

			int loop_counter = 0;
			for (
				std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
				current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
				current_item_iterator++
				)
			{
				Sleep(1);
				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
				if (current_item_iterator->is_checked > 0)
				{
					if (peers_to_send_count_counter == loop_counter)
					{
						peer_to_send = current_item_iterator->label;
						break;
					}
					loop_counter++;
				}

				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
			}

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

			local_address = peer_to_send;


			//	Здесь делаем снимок экрана

			//CSingleLock local_image_lock(&local_main_dialog->draw_audio_image_critical_section);

			//local_image_lock.Lock();

			if (network::ip_6::domain_name_to_internet_6_name(local_address, local_address_internet_address) == false)
			{
				const int local_error_message_size = 10000;
				wchar_t local_error_message[local_error_message_size];

				const int local_system_error_message_size = local_error_message_size - 1000;
				wchar_t local_system_error_message[local_system_error_message_size];

				wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

				CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

				wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

				continue;
			}
#ifdef use_istream_DEFINITION
			//if(local_main_dialog->web_camera_dialog!=NULL)
			//{
			//	CSingleLock lock(&local_main_dialog->delete_web_camera_dialog_critical_section);

			//	lock.Lock();

			//	local_stream = NULL;

			//	//ULARGE_INTEGER zero_size = {0,0};

			//	//local_stream->SetSize(zero_size);

			//	//LARGE_INTEGER liBeggining = { 0 };

			//	//local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

			//	//CaptureAudioSampleShot(local_main_dialog->web_camera_dialog,local_stream);
			//	CaptureAudioSampleGetFromTheList(local_main_dialog->web_camera_dialog,&local_stream);

			//	if(local_stream==NULL)
			//	{
			//		continue;
			//	}
			//}

#endif
			//local_image_lock.Unlock();

			//if(local_stream==NULL)
			//{
			//	continue;
			//}


			//ULONGLONG local_start_time = GetTickCount64();

			//STATSTG local_istream_statstg;
			//HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg,STATFLAG::STATFLAG_DEFAULT);

			/*/
			//			Тест отображения из IStream
			{
			std::fstream local_file;
			local_file.open("c:\\temp\\file_source.wav",std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

			char *local_file_data = new char[CONST_MESSAGE_LENGTH_IMAGE];

			ULONG local_read_bytes = 0;

			LARGE_INTEGER liBeggining = { 0 };

			local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

			local_stream->Read(local_file_data,CONST_MESSAGE_LENGTH_IMAGE,&local_read_bytes);

			local_file.write(local_file_data, local_read_bytes);

			delete [] local_file_data;

			local_file.flush();

			local_file.close();
			}
			//*/



			for (UINT local_parameter_port_number = port_number_start_const; local_parameter_port_number <= port_number_end_const; local_parameter_port_number++)
			{
				network::ip_6::CBlockingSocket_ip_6 local_blocking_socket;

				if (network::ip_6::domain_name_to_internet_6_name(local_address, local_address_internet_address) == false)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					break;
				}

				network::ip_6::CSockAddr_ip_6 local_socket_address(local_address_internet_address, local_parameter_port_number);

				try
				{
					local_blocking_socket.Create(SOCK_DGRAM);
				}
				catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					local_blocking_socket_exception->Delete();

					delete local_send_audio_thread_parameters_structure;

					//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

					{
						clear_thread_information(local_main_dialog);
					}

					//if(local_stream!=NULL)
					//{
					//	local_stream->Release();
					//}

					delete[] send_buffer;
					delete[] send_buffer_this_time;

					return 0;
				}

				int local_bytes_sent = 0;
				int local_error_number = 0;


				int send_buffer_data_length = service_signature_definition_length;

				ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH_IMAGE);

				memcpy(send_buffer, service_signature, service_signature_definition_length);



				CString send_data_string = command_audio;

				int send_data_string_length = send_data_string.GetLength();

				memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

				send_buffer_data_length += send_data_string_length * sizeof(wchar_t);

				wchar_t zero_word = L'\0';
				memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
				send_buffer_data_length += sizeof(wchar_t);
				send_data_string_length++;


				memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
				send_buffer_data_length += sizeof(DWORD);
				send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);



				LARGE_INTEGER liBeggining = { 0 };
#ifdef use_istream_DEFINITION
				local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);
#endif


				ULONG local_read = 0;
#ifdef use_istream_DEFINITION
				local_stream->Read(send_buffer + send_buffer_data_length, CONST_MESSAGE_LENGTH_IMAGE - send_buffer_data_length, &local_read);
#else
				local_read = 100000;
#endif
				send_buffer_data_length += local_read;




				CString xor_code_string;

				//if(local_main_dialog->get_command_terminate_application())
				//{
				//	break;
				//}
				{
					xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
				}

				char xor_code = _wtoi(xor_code_string);

				encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

				int local_header_length = service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD);

				int local_part_counter = 0;
				int local_parts_count = local_read / (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));
				int local_last_part_size = local_read % (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));

				if (local_last_part_size != 0)
				{
					local_parts_count++;
				}

				for (; local_part_counter < local_parts_count; local_part_counter++)
				{
					int send_buffer_this_time_data_length = CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME;

					if (local_part_counter == local_parts_count - 1)
					{
						if (local_last_part_size != 0)
						{
							send_buffer_this_time_data_length = local_last_part_size + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD);
						}
					}

					ZeroMemory(send_buffer_this_time, CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME);

					memcpy(send_buffer_this_time, send_buffer, service_signature_definition_length + send_data_string_length * sizeof(wchar_t));

					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[0] = DWORD(local_part_counter + 1);
					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[1] = DWORD(local_parts_count);

					encrypt::encrypt_xor(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t), 2 * sizeof(DWORD), xor_code);

					int local_this_time_data_offset = local_part_counter*(CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));

					memcpy
					(
						send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD),
						send_buffer + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + local_this_time_data_offset,
						send_buffer_this_time_data_length - (service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD))
					);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer_this_time, send_buffer_this_time_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
						break;
					}

					//if(local_main_dialog->get_command_threads_audio_stop())
					//{
					//	break;
					//}
					//else
					{
						Sleep(10);
					}

				}

				{
					int local_bytes_sent = 0;
					int local_error_number = 0;

					char send_buffer[CONST_MESSAGE_LENGTH];

					int send_buffer_data_length = service_signature_definition_length;

					ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH);

					memcpy(send_buffer, service_signature, service_signature_definition_length);

					CString send_data_string;

					int send_data_string_length = 0;

					send_data_string = command_audio_end;

					send_data_string_length = send_data_string.GetLength();

					memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

					send_buffer_data_length += send_data_string_length * sizeof(wchar_t);


					wchar_t zero_word = L'\0';
					memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
					send_buffer_data_length += sizeof(wchar_t);
					send_data_string_length++;


					memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
					send_buffer_data_length += sizeof(DWORD);
					send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);


					CString xor_code_string;

					//if(local_main_dialog->get_command_terminate_application())
					//{
					//	break;
					//}
					{
						xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
					}

					char xor_code = _wtoi(xor_code_string);

					encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer, send_buffer_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
					}
				}



				local_blocking_socket.Close();

				if (local_bytes_sent <= 0 && local_error_number == 0)
				{
					//			galaxy::MessageBox(CString(L"local_bytes_sent<=0"),CString(L"Ошибка"));
				}

				if (local_main_dialog->get_command_threads_web_camera_video_stop())
				{
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			//if(local_stream!=NULL)
			//{
			//	local_stream->Release();
			//}

			//ULONGLONG local_end_time = GetTickCount64();

			//ULONGLONG local_work_time = local_end_time - local_start_time;

			//if(local_main_dialog->get_command_threads_audio_stop())
			//{
			//	break;
			//}
			//else
			//{
			//	if(local_work_time>=1000)
			//		Sleep(1);
			//	else
			//	{
			//		HRESULT hr;

			//		AM_MEDIA_TYPE mt;
			//		ZeroMemory(&mt, sizeof(mt));

			//		if(local_main_dialog->web_camera_dialog!=NULL)
			//		{
			//			CSingleLock lock(&local_main_dialog->delete_web_camera_dialog_critical_section);

			//			lock.Lock();

			//			hr = local_main_dialog->web_camera_dialog->pAudioGrabber->GetConnectedMediaType(&mt);

			//			lock.Unlock();

			//			if (SUCCEEDED(hr))
			//			{
			//				if(mt.formattype==FORMAT_WaveFormatEx)
			//				{
			//					WAVEFORMATEX *wave_format_ex = (WAVEFORMATEX *)mt.pbFormat;

			//					if(wave_format_ex!=NULL)
			//					{
			//						ULONGLONG chunk_playing_time = 
			//							1000 *
			//							wave_format_ex->nAvgBytesPerSec/
			//							wave_format_ex->nSamplesPerSec/
			//							wave_format_ex->nChannels/
			//							(wave_format_ex->wBitsPerSample>>3)/
			//							wave_format_ex->nBlockAlign
			//							;

			//						if(chunk_playing_time>local_work_time)
			//							Sleep(DWORD(chunk_playing_time-local_work_time));
			//						else
			//							Sleep(1);
			//					}
			//				}
			//				_FreeMediaType(mt);	
			//			}
			//			else
			//				Sleep(DWORD(1000.0/CONST_AUDIO_PACKETS_PER_SECOND));		//	CONST_AUDIO_PACKETS_PER_SECOND кадров в секунду
			//		}
			//	}
			//}
		}

		ULONGLONG local_end_time = GetTickCount64();

		ULONGLONG local_work_time = local_end_time - local_start_time;

		if (local_main_dialog->get_command_threads_audio_stop())
		{
			break;
		}
		else
		{
			if (local_work_time >= 1000)
				Sleep(1);
			else
			{
				HRESULT hr;

				AM_MEDIA_TYPE mt;
				ZeroMemory(&mt, sizeof(mt));

				if (local_main_dialog->web_camera_dialog != NULL)
				{
					CSingleLock lock(&local_main_dialog->delete_web_camera_dialog_critical_section);

					lock.Lock();

					hr = local_main_dialog->web_camera_dialog->pAudioGrabber->GetConnectedMediaType(&mt);

					lock.Unlock();

					if (SUCCEEDED(hr))
					{
						if (mt.formattype == FORMAT_WaveFormatEx)
						{
							WAVEFORMATEX *wave_format_ex = (WAVEFORMATEX *)mt.pbFormat;

							if (wave_format_ex != NULL)
							{
								ULONGLONG chunk_playing_time =
									1000 *
									wave_format_ex->nAvgBytesPerSec /
									wave_format_ex->nSamplesPerSec /
									wave_format_ex->nChannels /
									(wave_format_ex->wBitsPerSample >> 3) /
									wave_format_ex->nBlockAlign
									;

								if (chunk_playing_time > local_work_time)
									Sleep(DWORD(chunk_playing_time - local_work_time));
								else
									Sleep(1);
							}
						}
						_FreeMediaType(mt);
					}
					else
					{
						double local_factor = peers_to_send_count*(port_number_end_const - port_number_start_const + 1);
						if (local_factor == 0.0)
						{
							local_factor = 1.0;
						}
						Sleep(DWORD(1000.0 / CONST_AUDIO_PACKETS_PER_SECOND / local_factor));		//	CONST_AUDIO_PACKETS_PER_SECOND кадров в секунду
					}
				}
			}
		}

		if (local_main_dialog->get_command_threads_audio_stop())
		{
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete[] send_buffer_this_time;

		delete local_send_audio_thread_parameters_structure;
		return 1;
	}
}


void Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckSendMicrophoneAudio()
{
	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();

	GUI_CONTROLS_STATE_data.IDC_CHECK_SEND_MICROPHONE_AUDIO_state = button_send_audio.GetCheck();

	if (button_send_audio.GetCheck() != 0)
	{
		button_retranslate_microphone.SetCheck(0);
		GUI_CONTROLS_STATE_data.IDC_CHECK_RETRANSLATE_MICROPHONE_state = 0;

		set_command_threads_audio_stop(false);

		if (web_camera_dialog == NULL)
		{
			web_camera_dialog = new CWebCameraPictureDialog(this);
			web_camera_dialog->main_dialog = this;
			web_camera_dialog->Create(CWebCameraPictureDialog::IDD);
			web_camera_dialog->ShowWindow(SW_SHOWNORMAL);
		}

		{
			void *local_send_audio_thread_parameters_structure = new thread_send_audio_parameters_structure_type;

			((thread_send_audio_parameters_structure_type*)local_send_audio_thread_parameters_structure)->parameter_main_dialog = this;

			CWinThread *local_thread = AfxBeginThread(datagram_send_audio_connection_thread, local_send_audio_thread_parameters_structure);

			THREADS_INFORMATION local_thread_information;
			local_thread_information.thread_name = CString(L"datagram_send_audio_connection_thread");
			local_thread_information.WinThread = local_thread->m_hThread;

			CSingleLock local_lock(&threads_list_critical_section);
			local_lock.Lock();

			threads_list.push_back(local_thread_information);
		}
	}
	else
	{
		set_command_threads_audio_stop(true);

		{
			if (web_camera_dialog != NULL)
			{
				CSingleLock lock(&delete_web_camera_dialog_critical_section);

				lock.Lock();

				CWebCameraPictureDialog *local_web_camera_dialog = web_camera_dialog;

				lock.Unlock();

				if (button_send_web_camera_video.GetCheck() == 0)
				{
					web_camera_dialog = NULL;

					local_web_camera_dialog->StopMedia();

					//local_web_camera_dialog->EndDialog(IDOK);
					local_web_camera_dialog->SendMessage(WM_CLOSE);

					//delete local_web_camera_dialog;
				}
			}
		}
	}
}

UINT __cdecl datagram_play_audio_connection_thread(LPVOID parameter)
{

	thread_play_audio_parameters_structure_type *local_play_audio_thread_parameters_structure = (thread_play_audio_parameters_structure_type *)parameter;

	if (local_play_audio_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_play_audio_thread_parameters_structure)->parameter_main_dialog;
	CWave *local_wave = (local_play_audio_thread_parameters_structure)->parameter_wave;

	if (local_main_dialog == NULL)
	{
		if (local_wave != NULL)
		{
			delete local_wave;
		}
		delete local_play_audio_thread_parameters_structure;
		return 0;
	}

	CSingleLock local_lock(&local_main_dialog->play_audio_critical_section);

	local_lock.Lock();

	if (local_wave == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_play_audio_thread_parameters_structure;
		return 0;
	}

	if (local_main_dialog->get_command_terminate_application())
	{
		{
			clear_thread_information(local_main_dialog);
		}

		if (local_wave != NULL)
		{
			delete local_wave;
		}
		delete local_play_audio_thread_parameters_structure;
		return 0;
	}

	int local_int = 0;
	{
		DWORD dwWaitResult;

		dwWaitResult = WaitForSingleObject(
			local_main_dialog->do_not_terminate_application_event, // event handle
			0);    // zero wait

		if (dwWaitResult == WAIT_OBJECT_0)
		{
			// Event object was signaled		

			if (local_main_dialog->get_command_terminate_application())
			{
				{
					clear_thread_information(local_main_dialog);
				}

				if (local_wave != NULL)
				{
					delete local_wave;
				}
				delete local_play_audio_thread_parameters_structure;
				return 0;
			}
			local_int = local_main_dialog->button_enable_play_audio.GetCheck();
		}
		else
		{
			if (local_main_dialog->get_command_terminate_application())
			{
				{
					clear_thread_information(local_main_dialog);
				}

				if (local_wave != NULL)
				{
					delete local_wave;
				}
				delete local_play_audio_thread_parameters_structure;
				return 0;
			}
		}
	}
	if (local_int != 0)
	{
		if (local_wave->IsValid() == TRUE)
		{
			DWORD chunk_playing_time =
				1000 *
				local_wave->GetSize() /
				local_wave->GetSampleRate() /
				local_wave->GetChannels() /
				(local_wave->GetBitsPerSample() >> 3);

			local_wave->Play();

			Sleep(chunk_playing_time);
		}
	}

	{
		clear_thread_information(local_main_dialog);
	}

	delete local_wave;

	delete local_play_audio_thread_parameters_structure;

	return 1;
}

void Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckEnableShowWebCamera()
{
	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();

	GUI_CONTROLS_STATE_data.IDC_CHECK_ENABLE_SHOW_WEB_CAMERA_state = button_enable_web_camera.GetCheck();

	if (button_enable_web_camera.GetCheck() != 0)
	{
		if (web_camera_dialog == NULL)
		{
			web_camera_dialog = new CWebCameraPictureDialog(this);
			web_camera_dialog->main_dialog = this;
			web_camera_dialog->Create(CWebCameraPictureDialog::IDD);
			web_camera_dialog->ShowWindow(SW_SHOWNORMAL);
		}
	}
	else
	{
		set_command_threads_web_camera_video_stop(true);
		set_command_threads_audio_stop(true);

		{
			if (web_camera_dialog != NULL)
			{
				CSingleLock lock(&delete_web_camera_dialog_critical_section);

				lock.Lock();

				CWebCameraPictureDialog *local_web_camera_dialog = web_camera_dialog;

				lock.Unlock();

				if (button_enable_web_camera.GetCheck() == 0)
				{
					web_camera_dialog = NULL;

					local_web_camera_dialog->StopMedia();

					//local_web_camera_dialog->EndDialog(IDOK);
					local_web_camera_dialog->SendMessage(WM_CLOSE);

					//delete local_web_camera_dialog;
				}
			}
		}
	}
}


void Cstl_network_ip_4_ip_6_udp_engineDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного

	if (Cstl_network_ip_4_ip_6_udp_engineDialog_timer_nIDEvent == nIDEvent)
	{
		if (get_command_terminate_application())
		{
			return;
		}

		CSingleLock lock(&GUI_update_critical_section);
		lock.Lock();

		{
			for (; GUI_CONTROLS_STATE_data.IDC_LIST_CHAT_state.begin() != GUI_CONTROLS_STATE_data.IDC_LIST_CHAT_state.end();)
			{
				std::list<GUI_LIST_CONTROL>::iterator list_chat_items_counter = GUI_CONTROLS_STATE_data.IDC_LIST_CHAT_state.begin();

				if (list_chat_items_counter != GUI_CONTROLS_STATE_data.IDC_LIST_CHAT_state.end())
				{
					CString list_chat_current_item_label = list_chat_items_counter->label;
					list_chat.AddString(list_chat_current_item_label);																//DDX_Control(pDX, IDC_LIST_CHAT, list_chat);
					list_chat.SetSel(list_nodes.FindStringExact(0, list_chat_current_item_label), list_chat_items_counter->is_checked);

					GUI_CONTROLS_STATE_data.IDC_LIST_CHAT_state.pop_front();
				}
				else
				{
					break;
				}
			}
			if (!edit_text.GetFocus()) edit_text.SetWindowTextW(GUI_CONTROLS_STATE_data.IDC_EDIT_TEXT_state);				//DDX_Control(pDX, IDC_EDIT_TEXT, edit_text);
			{
				list_nodes.ResetContent();
				for (
					std::list<GUI_LIST_CONTROL>::iterator list_nodes_items_counter = GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
					list_nodes_items_counter != GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
					list_nodes_items_counter++
					)
				{
					CString list_nodes_current_item_label = list_nodes_items_counter->label;
					list_nodes.AddString(list_nodes_current_item_label);																//DDX_Control(pDX, IDC_LIST_NODES, list_nodes);
					list_nodes.SetSel(list_nodes.FindStringExact(0, list_nodes_current_item_label), list_nodes_items_counter->is_checked);
				}
			}
			if (!edit_xor_code.GetFocus()) edit_xor_code.SetWindowTextW(GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state);	//DDX_Control(pDX, IDC_EDIT_XOR_CODE, edit_xor_code);
			if (!edit_server.GetFocus()) edit_server.SetWindowTextW(GUI_CONTROLS_STATE_data.IDC_EDIT_SERVER_state);			//DDX_Control(pDX, IDC_EDIT_SERVER, edit_server);
			{
				list_external_ip_4.ResetContent();
				for (
					std::list<GUI_LIST_CONTROL>::iterator list_external_ip_4_items_counter = GUI_CONTROLS_STATE_data.IDC_LIST_EXTERNAL_IP_4_state.begin();
					list_external_ip_4_items_counter != GUI_CONTROLS_STATE_data.IDC_LIST_EXTERNAL_IP_4_state.end();
					list_external_ip_4_items_counter++
					)
				{
					CString list_external_ip_4_current_item_label = list_external_ip_4_items_counter->label;
					list_external_ip_4.AddString(list_external_ip_4_current_item_label);												//DDX_Control(pDX, IDC_LIST_EXTERNAL_IP_4, list_external_ip_4);
					list_external_ip_4.SetSel(list_nodes.FindStringExact(0, list_external_ip_4_current_item_label), list_external_ip_4_items_counter->is_checked);
				}
			}
			button_enable_chat.SetCheck(GUI_CONTROLS_STATE_data.IDC_CHECK_ENABLE_CHAT_state);								//DDX_Control(pDX, IDC_CHECK_ENABLE_CHAT, button_enable_chat);
			button_enable_draw_video.SetCheck(GUI_CONTROLS_STATE_data.IDC_CHECK_ENABLE_VIDEO_state);						//DDX_Control(pDX, IDC_CHECK_ENABLE_VIDEO, button_enable_draw_video);
			button_enable_play_audio.SetCheck(GUI_CONTROLS_STATE_data.IDC_CHECK_ENABLE_SOUND_state);						//DDX_Control(pDX, IDC_CHECK_ENABLE_SOUND, button_enable_play_audio);
			button_send_video.SetCheck(GUI_CONTROLS_STATE_data.IDC_CHECK_SEND_VIDEO_state);									//DDX_Control(pDX, IDC_CHECK_SEND_VIDEO, button_send_video);
			button_send_web_camera_video.SetCheck(GUI_CONTROLS_STATE_data.IDC_CHECK_SEND_WEB_CAMERA_VIDEO_state);			//DDX_Control(pDX, IDC_CHECK_SEND_WEB_CAMERA_VIDEO, button_send_web_camera_video);
			button_send_audio.SetCheck(GUI_CONTROLS_STATE_data.IDC_CHECK_SEND_MICROPHONE_AUDIO_state);						//DDX_Control(pDX, IDC_CHECK_SEND_MICROPHONE_AUDIO, button_send_audio);
			if (!edit_speed.GetFocus()) edit_speed.SetWindowTextW(GUI_CONTROLS_STATE_data.IDC_EDIT_SPEED_state);				//DDX_Control(pDX, IDC_EDIT_SPEED, edit_speed);
			button_enable_web_camera.SetCheck(GUI_CONTROLS_STATE_data.IDC_CHECK_ENABLE_SHOW_WEB_CAMERA_state);				//DDX_Control(pDX, IDC_CHECK_ENABLE_SHOW_WEB_CAMERA, button_enable_web_camera);

		}

	}
	else if (Cstl_network_ip_4_ip_6_udp_engineDialog_draw_video_timer_nIDEvent == nIDEvent)
	{
		CSingleLock lock(&draw_video_critical_section);
		lock.Lock();

		//	Сборка и отрисовка
		std::list<STREAM_FRAME_INFORMATION>::iterator current_video_frame_stream = received_video_frame_stream.begin();

		for (; current_video_frame_stream != received_video_frame_stream.end(); current_video_frame_stream++)
		{
			if (received_video_frame_stream.size() != 0)
			{
				if (current_video_frame_stream->frames.size() != 0)
				{
					CString source_address_string = current_video_frame_stream->source_address_string;

					for
						(
							std::list<FRAME>::iterator current_video_frame = current_video_frame_stream->frames.begin()
							;
							current_video_frame != current_video_frame_stream->frames.end()
							;
							)
					{
						if (current_video_frame->frame_parts_number == current_video_frame->frame_parts.size())
						{
							//	Количество частей равно ожидаемому количеству частей - собираем части и отрисовываем, очищаем память, первого кадра в очереди текущей последовательности.

							CComPtr<IStream> frame_stream;

							HRESULT local_create_frame_IStream_result = CreateStreamOnHGlobal(0, TRUE, &frame_stream);

							{
								LARGE_INTEGER liBeggining = { 0 };

								frame_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);
							}


							UINT frame_size = 0;

							for (
								std::list<FRAME_PART>::iterator local_frame_parts_iterator = current_video_frame->frame_parts.begin()
								;
								local_frame_parts_iterator != current_video_frame->frame_parts.end()
								;
								local_frame_parts_iterator++
								)
							{
								frame_size += local_frame_parts_iterator->frame_part_data_size;
							}

							BYTE *local_frame_buffer = new BYTE[frame_size];

							UINT current_buffer_offset = 0;

							for
								(
									UINT local_parts_counter = 1
									;
									local_parts_counter <= current_video_frame->frame_parts_number
									;
									local_parts_counter++
									)
							{
								for (
									std::list<FRAME_PART>::iterator local_frame_parts_iterator = current_video_frame->frame_parts.begin()
									;
									local_frame_parts_iterator != current_video_frame->frame_parts.end()
									;
									local_frame_parts_iterator++
									)
								{
									if (local_parts_counter == local_frame_parts_iterator->frame_part_number)
									{
										if (local_frame_buffer != NULL)
										{
											memcpy(local_frame_buffer + current_buffer_offset, local_frame_parts_iterator->frame_part_data, local_frame_parts_iterator->frame_part_data_size);
											current_buffer_offset += local_frame_parts_iterator->frame_part_data_size;
										}

										delete[]local_frame_parts_iterator->frame_part_data;

										local_frame_parts_iterator->frame_part_data = NULL;
										local_frame_parts_iterator->frame_part_data_size = 0;

										//local_frame_parts_iterator = current_video_frame->frame_parts.erase(local_frame_parts_iterator);

										break;
									}
								}
							}

							if (local_frame_buffer != NULL)
							{
								ULONG local_bytes_written = 0;
								frame_stream->Write(local_frame_buffer, frame_size, &local_bytes_written);

								if (frame_size != local_bytes_written)
								{
									CString local_error(L"Error");
								}

								delete[]local_frame_buffer;

								local_frame_buffer = NULL;
							}

							ATL::CImage current_received_video_image;

							LARGE_INTEGER liBeggining = { 0 };

							frame_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

							STATSTG local_istream_statstg;
							HRESULT local_stat_result = frame_stream->Stat(&local_istream_statstg, STATFLAG::STATFLAG_DEFAULT);

							HRESULT local_load_result = current_received_video_image.Load(frame_stream);

							if (SUCCEEDED(local_load_result))
							{
								int local_IDC_CHECK_SAVE_PICTURES_state = 0;

								{
									CSingleLock lock(&GUI_update_critical_section);
									lock.Lock();

									local_IDC_CHECK_SAVE_PICTURES_state = GUI_CONTROLS_STATE_data.IDC_CHECK_SAVE_PICTURES_state;
								}

								if (local_IDC_CHECK_SAVE_PICTURES_state != 0)
								{
									CString new_picture_file_name;

									SYSTEMTIME local_system_time;

									GetLocalTime(&local_system_time);

									new_picture_file_name.Format
									(
										L"%04d-%02d-%02d %02d-%02d-%02d.%03d.PNG",
										local_system_time.wYear,
										local_system_time.wMonth,
										local_system_time.wDay,
										local_system_time.wHour,
										local_system_time.wMinute,
										local_system_time.wSecond,
										local_system_time.wMilliseconds
									);

									try
									{
										HRESULT local_save_result = current_received_video_image.Save(new_picture_file_name, Gdiplus::ImageFormatPNG);
									}
									catch (...)
									{
										CString local_fail(L"Fail");
									}
								}

								int local_IDC_CHECK_RETRANSLATE_VIDEO_state = 0;

								{
									CSingleLock lock(&GUI_update_critical_section);
									lock.Lock();

									local_IDC_CHECK_RETRANSLATE_VIDEO_state = GUI_CONTROLS_STATE_data.IDC_CHECK_RETRANSLATE_VIDEO_state;
								}

								if (local_IDC_CHECK_RETRANSLATE_VIDEO_state != 0)
								{
									{
										STREAM_INFORMATION local_retranslate_stream_information_ip_4;

										LARGE_INTEGER liBeggining = { 0 };

										frame_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

										ULONG local_read = 0;
										HRESULT local_clone_IStream_result = frame_stream->Clone(&local_retranslate_stream_information_ip_4.stream);
										local_retranslate_stream_information_ip_4.sequence_number = current_video_frame_stream->sequence_source_number;

										retranslate_video_frames_ip_4.push_back(local_retranslate_stream_information_ip_4);
									}

									{
										STREAM_INFORMATION local_retranslate_stream_information_ip_6;

										LARGE_INTEGER liBeggining = { 0 };

										frame_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

										ULONG local_read = 0;
										HRESULT local_clone_IStream_result = frame_stream->Clone(&local_retranslate_stream_information_ip_6.stream);
										local_retranslate_stream_information_ip_6.sequence_number = current_video_frame_stream->sequence_source_number;

										retranslate_video_frames_ip_6.push_back(local_retranslate_stream_information_ip_6);
									}

								}

								{
									CRect local_window_rectangle;

									if (received_video_dialog != NULL)
									{
										CSingleLock lock(&delete_received_video_dialog_critical_section);

										lock.Lock();

										if (get_command_terminate_application())
										{
											//if (parameter_data != NULL)
											//{
											//	delete[]parameter_data;
											//}

											return;
										}
										received_video_dialog->static_image.GetClientRect(&local_window_rectangle);

										if (get_command_terminate_application())
										{
											//if (parameter_data != NULL)
											//{
											//	delete[]parameter_data;
											//}

											return;
										}
										HDC local_HDC = *received_video_dialog->static_image.GetDC();

										if (get_command_terminate_application())
										{
											//if (parameter_data != NULL)
											//{
											//	delete[]parameter_data;
											//}

											return;
										}

										try
										{
											current_received_video_image.Draw(local_HDC, local_window_rectangle, Gdiplus::InterpolationMode::InterpolationModeDefault);
										}
										catch (...)
										{
											//if (parameter_data != NULL)
											//{
											//	delete[]parameter_data;
											//}

											return;
										}

										if (get_command_terminate_application())
										{
											//if (parameter_data != NULL)
											//{
											//	delete[]parameter_data;
											//}

											return;
										}
										received_video_dialog->SetWindowTextW(CString(L"Видео от ") + source_address_string);
									}
								}
							}
							current_video_frame = current_video_frame_stream->frames.erase(current_video_frame);
							continue;
						}
						else
						{
							bool local_data_has_expired = false;

							CTime local_current_time_to_check_if_expired_frame = CTime::GetCurrentTime();

							for (
								std::list<FRAME_PART>::iterator local_frame_parts_iterator = current_video_frame->frame_parts.begin()
								;
								local_frame_parts_iterator != current_video_frame->frame_parts.end()
								;
								local_frame_parts_iterator++
								)
							{
								CTime local_current_time = CTime::GetCurrentTime();

								CTime local_arrival_time = local_frame_parts_iterator->arrival_time;

								CTimeSpan local_time_difference = local_current_time - local_arrival_time;

								if (local_time_difference.GetTotalSeconds() >= CONST_EXPIRATION_DATA_LIMIT_SECONDS)
								{
									local_data_has_expired = true;

									delete[]local_frame_parts_iterator->frame_part_data;

									local_frame_parts_iterator->frame_part_data = NULL;
									local_frame_parts_iterator->frame_part_data_size = 0;

									//local_frame_parts_iterator = current_video_frame->frame_parts.erase(local_frame_parts_iterator);
								}
							}

							if (local_data_has_expired)
							{
								current_video_frame = current_video_frame_stream->frames.erase(current_video_frame);
								continue;
							}
							else
							{
								current_video_frame++;
							}
						}
					}
				}
			}
		}
	}
	else if (Cstl_network_ip_4_ip_6_udp_engineDialog_draw_web_camera_timer_nIDEvent == nIDEvent)
	{
		CSingleLock lock(&draw_web_camera_video_critical_section);
		lock.Lock();

		CRect local_window_rectangle;

		if (received_video_dialog != NULL)
		{
			CSingleLock lock(&delete_received_video_dialog_critical_section);

			lock.Lock();

			if (get_command_terminate_application())
			{
				//if (parameter_data != NULL)
				//{
				//	delete[]parameter_data;
				//}

				return;
			}
			received_video_dialog->static_image.GetClientRect(&local_window_rectangle);

			if (get_command_terminate_application())
			{
				//if (parameter_data != NULL)
				//{
				//	delete[]parameter_data;
				//}

				return;
			}
			HDC local_HDC = *received_video_dialog->static_image.GetDC();

			if (get_command_terminate_application())
			{
				//if (parameter_data != NULL)
				//{
				//	delete[]parameter_data;
				//}

				return;
			}


			if (received_web_camera_video_frame_stream.size() != 0)
			{
				auto current_received_web_camera_video_frame_stream = received_web_camera_video_frame_stream.begin();

				if (current_received_web_camera_video_frame_stream->frames.size() != 0)
				{
					auto &curren_received_web_camera_frame = current_received_web_camera_video_frame_stream->frames.begin();

					CComPtr<IStream> frame_stream;

					HRESULT local_create_frame_IStream_result = CreateStreamOnHGlobal(0, TRUE, &frame_stream);

					{
						LARGE_INTEGER liBeggining = { 0 };

						frame_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);
					}

					auto frame_size = curren_received_web_camera_frame->frame_data_size;

					if(curren_received_web_camera_frame->frame_data!=NULL)
					{
						ULONG local_bytes_written = 0;
						frame_stream->Write(curren_received_web_camera_frame->frame_data, frame_size, &local_bytes_written);

						if (frame_size != local_bytes_written)
						{
							CString local_error(L"Error");
						}
					}

					ATL::CImage current_received_web_camera_video_image;

					LARGE_INTEGER liBeggining = { 0 };

					frame_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

					STATSTG local_istream_statstg;
					HRESULT local_stat_result = frame_stream->Stat(&local_istream_statstg, STATFLAG::STATFLAG_DEFAULT);

					HRESULT local_load_result = current_received_web_camera_video_image.Load(frame_stream);

					auto sequence_source_number = current_received_web_camera_video_frame_stream->sequence_source_number;
					auto source_address_string = current_received_web_camera_video_frame_stream->source_address_string;

					current_received_web_camera_video_frame_stream->frames.pop_front();

					if (SUCCEEDED(local_load_result))
					{
						int local_IDC_CHECK_SAVE_WEB_CAMERA_PICTURES_state = 0;

						{
							CSingleLock lock(&GUI_update_critical_section);
							lock.Lock();

							local_IDC_CHECK_SAVE_WEB_CAMERA_PICTURES_state = GUI_CONTROLS_STATE_data.IDC_CHECK_SAVE_WEB_CAMERA_PICTURES_state;
						}

						if (local_IDC_CHECK_SAVE_WEB_CAMERA_PICTURES_state != 0)
						{
							CString new_picture_file_name;

							SYSTEMTIME local_system_time;

							GetLocalTime(&local_system_time);

							new_picture_file_name.Format
							(
								L"%04d-%02d-%02d %02d-%02d-%02d.%03d.JPEG",
								local_system_time.wYear,
								local_system_time.wMonth,
								local_system_time.wDay,
								local_system_time.wHour,
								local_system_time.wMinute,
								local_system_time.wSecond,
								local_system_time.wMilliseconds
							);

							try
							{
								HRESULT local_save_result = current_received_web_camera_video_image.Save(new_picture_file_name, Gdiplus::ImageFormatJPEG);
							}
							catch (...)
							{
								CString local_fail(L"Fail");
							}
						}




						try
						{
							current_received_web_camera_video_image.Draw(local_HDC, local_window_rectangle, Gdiplus::InterpolationMode::InterpolationModeDefault);
						}
						catch (...)
						{
							//if (parameter_data != NULL)
							//{
							//	delete[]parameter_data;
							//}

							return;
						}

						if (get_command_terminate_application())
						{
							//if (parameter_data != NULL)
							//{
							//	delete[]parameter_data;
							//}

							return;
						}
						received_video_dialog->SetWindowTextW(CString(L"Видео с веб камеры от ") + source_address_string);

						{
							int local_IDC_CHECK_RETRANSLATE_WEB_CAMERA_state = 0;

							{
								CSingleLock lock(&GUI_update_critical_section);
								lock.Lock();

								local_IDC_CHECK_RETRANSLATE_WEB_CAMERA_state = GUI_CONTROLS_STATE_data.IDC_CHECK_RETRANSLATE_WEB_CAMERA_state;
							}

							if (local_IDC_CHECK_RETRANSLATE_WEB_CAMERA_state != 0)
							{
								{
									STREAM_INFORMATION local_retranslate_stream_information_ip_4;

									LARGE_INTEGER liBeggining = { 0 };

									frame_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

									ULONG local_read = 0;
									HRESULT local_clone_IStream_result = frame_stream->Clone(&local_retranslate_stream_information_ip_4.stream);
									local_retranslate_stream_information_ip_4.sequence_number = sequence_source_number;

									retranslate_web_camera_video_frames_ip_4.push_back(local_retranslate_stream_information_ip_4);
								}

								{
									STREAM_INFORMATION local_retranslate_stream_information_ip_6;

									LARGE_INTEGER liBeggining = { 0 };

									frame_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

									ULONG local_read = 0;
									HRESULT local_clone_IStream_result = frame_stream->Clone(&local_retranslate_stream_information_ip_6.stream);
									local_retranslate_stream_information_ip_6.sequence_number = sequence_source_number;

									retranslate_web_camera_video_frames_ip_6.push_back(local_retranslate_stream_information_ip_6);
								}
							}
						}
					}
				}
			}
		}
	}
	else if (Cstl_network_ip_4_ip_6_udp_engineDialog_play_sound_timer_nIDEvent == nIDEvent)
	{

	}

	CDialogEx::OnTimer(nIDEvent);
}


void Cstl_network_ip_4_ip_6_udp_engineDialog::OnClickedCheckEnableChat()
{
	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();

	GUI_CONTROLS_STATE_data.IDC_CHECK_ENABLE_CHAT_state = button_enable_chat.GetCheck();
}


void Cstl_network_ip_4_ip_6_udp_engineDialog::OnChangeEditServer()
{
	// TODO:  Если это элемент управления RICHEDIT, то элемент управления не будет
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Добавьте код элемента управления

	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();
	edit_server.GetWindowTextW(GUI_CONTROLS_STATE_data.IDC_EDIT_SERVER_state);
}


void Cstl_network_ip_4_ip_6_udp_engineDialog::OnChangeEditText()
{
	// TODO:  Если это элемент управления RICHEDIT, то элемент управления не будет
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Добавьте код элемента управления

	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();
	edit_text.GetWindowTextW(GUI_CONTROLS_STATE_data.IDC_EDIT_TEXT_state);
}


void Cstl_network_ip_4_ip_6_udp_engineDialog::OnChangeEditXorCode()
{
	// TODO:  Если это элемент управления RICHEDIT, то элемент управления не будет
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Добавьте код элемента управления

	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();
	edit_xor_code.GetWindowTextW(GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state);
}


void Cstl_network_ip_4_ip_6_udp_engineDialog::OnChangeEditSpeed()
{
	// TODO:  Если это элемент управления RICHEDIT, то элемент управления не будет
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Добавьте код элемента управления

	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();
	edit_speed.GetWindowTextW(GUI_CONTROLS_STATE_data.IDC_EDIT_SPEED_state);
}


void Cstl_network_ip_4_ip_6_udp_engineDialog::OnSelchangeListChat()
{
	// TODO: добавьте свой код обработчика уведомлений
	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();
	{
		int list_chat_items_index = list_chat.GetCurSel();
		CString list_chat_current_item;
		list_chat.GetText(list_chat_items_index, list_chat_current_item);
		GUI_LIST_CONTROL list_chat_current_item_state;
		list_chat_current_item_state.label = list_chat_current_item;
		list_chat_current_item_state.is_checked = 0;

		for (
			std::list<GUI_LIST_CONTROL>::iterator list_chat_items_counter = GUI_CONTROLS_STATE_data.IDC_LIST_CHAT_state.begin();
			list_chat_items_counter != GUI_CONTROLS_STATE_data.IDC_LIST_CHAT_state.end();
			list_chat_items_counter++
			)
		{
			if (list_chat_items_counter->label == list_chat_current_item_state.label)
			{
				list_chat_items_counter->is_checked = !list_chat_items_counter->is_checked;
			}
		}
	}
}


void Cstl_network_ip_4_ip_6_udp_engineDialog::OnSelchangeListExternalIp4()
{
	// TODO: добавьте свой код обработчика уведомлений
	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();
	{
		int list_external_ip_4_items_index = list_external_ip_4.GetCurSel();
		CString list_external_ip_4_current_item;
		list_external_ip_4.GetText(list_external_ip_4_items_index, list_external_ip_4_current_item);
		GUI_LIST_CONTROL list_external_ip_4_current_item_state;
		list_external_ip_4_current_item_state.label = list_external_ip_4_current_item;
		list_external_ip_4_current_item_state.is_checked = 0;

		for (
			std::list<GUI_LIST_CONTROL>::iterator list_external_ip_4_items_counter = GUI_CONTROLS_STATE_data.IDC_LIST_EXTERNAL_IP_4_state.begin();
			list_external_ip_4_items_counter != GUI_CONTROLS_STATE_data.IDC_LIST_EXTERNAL_IP_4_state.end();
			list_external_ip_4_items_counter++
			)
		{
			if (list_external_ip_4_items_counter->label == list_external_ip_4_current_item_state.label)
			{
				list_external_ip_4_items_counter->is_checked = !list_external_ip_4_items_counter->is_checked;
			}
		}
	}
}


void Cstl_network_ip_4_ip_6_udp_engineDialog::OnSelchangeListNodes()
{
	// TODO: добавьте свой код обработчика уведомлений
	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();
	{
		int list_nodes_items_index = list_nodes.GetCurSel();
		CString list_nodes_current_item;
		list_nodes.GetText(list_nodes_items_index, list_nodes_current_item);
		GUI_LIST_CONTROL list_nodes_current_item_state;
		list_nodes_current_item_state.label = list_nodes_current_item;
		list_nodes_current_item_state.is_checked = 0;

		for (
			std::list<GUI_LIST_CONTROL>::iterator list_nodes_items_counter = GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
			list_nodes_items_counter != GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
			list_nodes_items_counter++
			)
		{
			if (list_nodes_items_counter->label == list_nodes_current_item_state.label)
			{
				list_nodes_items_counter->is_checked = !list_nodes_items_counter->is_checked;
			}
		}
	}
}

UINT _cdecl upnp_external_ipv4_detection_thread(LPVOID parameter)
{
	thread_upnp_external_ipv4_detection_thread_parameters_structure_type *local_upnp_external_ipv4_detection_thread_parameters_structure = (thread_upnp_external_ipv4_detection_thread_parameters_structure_type *)parameter;

	if (local_upnp_external_ipv4_detection_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = local_upnp_external_ipv4_detection_thread_parameters_structure->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete parameter;

		return 0;
	}

	try
	{
		IGDDiscoverProcess discoverer(500000);

		tg.create_thread(boost::ref(discoverer));
		tg.join_all();

		IGDDiscoverProcess::IGDControlInformations cinfos =
			discoverer.GetAllIGDControlInformations();
		boost_foreach(IGDDiscoverProcess::ControlInfo info, cinfos)
		{
			if (info.has_problem)
			{
				continue;
			}

			if (info.service_type == UPNPSERVICE_LAYER3FORWARDING1)
			{
				UPNPNATHTTPClient soap(info.ip, info.port);

				soap.GetDefaultConnectionService(info.control_url, info.service_type);
			}
			else if (info.service_type == UPNPSERVICE_WANIPCONNECTION1)
			{
				UPNPNATHTTPClient soap(info.ip, info.port);

				std::string ip;
				UPNPNATHTTPClient::SoapResult res = soap.GetWANIPAddress(
					ip, info.control_url, info.service_type
				);

				if (ip.length() != 0)
				{
					CSingleLock lock(&local_main_dialog->GUI_update_critical_section);
					lock.Lock();

					CString local_external_ip_4(ip.c_str());

					GUI_LIST_CONTROL list_external_ip_4_item_state;
					list_external_ip_4_item_state.label = local_external_ip_4;
					list_external_ip_4_item_state.is_checked = FALSE;
					local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_EXTERNAL_IP_4_state.push_back(list_external_ip_4_item_state);
				}
			}
			else if (info.service_type == UPNPSERVICE_WANCOMMONINTERFACECONFIG1)
			{
			}
			else if (info.service_type == UPNPSERVICE_WANPPPCONNECTION1)
			{
				UPNPNATHTTPClient soap(info.ip, info.port);

				std::string ip;
				UPNPNATHTTPClient::SoapResult res = soap.GetWANIPAddress(
					ip, info.control_url, info.service_type
				);

				if (ip.length() != 0)
				{
					CSingleLock lock(&local_main_dialog->GUI_update_critical_section);
					lock.Lock();

					CString local_external_ip_4(ip.c_str());

					GUI_LIST_CONTROL list_external_ip_4_item_state;
					list_external_ip_4_item_state.label = local_external_ip_4;
					list_external_ip_4_item_state.is_checked = FALSE;
					local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_EXTERNAL_IP_4_state.push_back(list_external_ip_4_item_state);
				}
			}
		}
	}
	catch (IGDDiscoverProcess::exception &e)
	{
		CString error(e.what());

		const int local_error_message_size = 10000;
		wchar_t local_error_message[local_error_message_size];

		const int local_system_error_message_size = local_error_message_size - 1000;
		wchar_t local_system_error_message[local_system_error_message_size];

		CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

		wcscpy_s(local_system_error_message, local_error_message_size, error);

		wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

		galaxy::MessageBox(local_error_message, CString(L"Ошибка"));
	}

	{
		clear_thread_information(local_main_dialog);
	}

	delete parameter;

	return 1;
}

void Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckSavePictures()
{
	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();

	GUI_CONTROLS_STATE_data.IDC_CHECK_SAVE_PICTURES_state = button_enable_save_video.GetCheck();
}

void Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckSaveWebCameraPictures()
{
	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();

	GUI_CONTROLS_STATE_data.IDC_CHECK_SAVE_WEB_CAMERA_PICTURES_state = button_enable_save_web_camera_video.GetCheck();
}

void Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckSaveMicrophoneSound()
{
	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();

	GUI_CONTROLS_STATE_data.IDC_CHECK_SAVE_MICROPHONE_SOUND_state = button_enable_save_microphone_audio.GetCheck();
}

void Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckRetranslateVideo()
{
	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();

	GUI_CONTROLS_STATE_data.IDC_CHECK_RETRANSLATE_VIDEO_state = button_retranslate_video.GetCheck();

	if (GUI_CONTROLS_STATE_data.IDC_CHECK_RETRANSLATE_VIDEO_state != 0)
	{
		button_send_video.SetCheck(0);
		GUI_CONTROLS_STATE_data.IDC_CHECK_SEND_VIDEO_state = 0;

		set_command_threads_retranslate_video_stop(false);

		{
			void *local_send_video_thread_parameters_structure = new thread_send_video_parameters_structure_type;

			((thread_send_video_parameters_structure_type*)local_send_video_thread_parameters_structure)->parameter_main_dialog = this;

			CWinThread *local_thread = AfxBeginThread(datagram_retranslate_video_connection_thread, local_send_video_thread_parameters_structure);

			THREADS_INFORMATION local_thread_information;
			local_thread_information.thread_name = CString(L"datagram_retranslate_video_connection_thread");
			local_thread_information.WinThread = local_thread->m_hThread;

			CSingleLock local_lock(&threads_list_critical_section);
			local_lock.Lock();

			threads_list.push_back(local_thread_information);
		}
	}
	else
	{
		set_command_threads_retranslate_video_stop(true);
	}
}

void Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckRetranslateWebCamera()
{
	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();

	GUI_CONTROLS_STATE_data.IDC_CHECK_RETRANSLATE_WEB_CAMERA_state = button_retranslate_web_camera.GetCheck();

	if (GUI_CONTROLS_STATE_data.IDC_CHECK_RETRANSLATE_WEB_CAMERA_state != 0)
	{
		button_send_web_camera_video.SetCheck(0);
		GUI_CONTROLS_STATE_data.IDC_CHECK_SEND_WEB_CAMERA_VIDEO_state = 0;

		set_command_threads_retranslate_web_camera_video_stop(false);

		{
			void *local_send_video_thread_parameters_structure = new thread_send_video_parameters_structure_type;

			((thread_send_video_parameters_structure_type*)local_send_video_thread_parameters_structure)->parameter_main_dialog = this;

			CWinThread *local_thread = AfxBeginThread(datagram_retranslate_web_camera_video_connection_thread, local_send_video_thread_parameters_structure);

			THREADS_INFORMATION local_thread_information;
			local_thread_information.thread_name = CString(L"datagram_retranslate_web_camera_video_connection_thread");
			local_thread_information.WinThread = local_thread->m_hThread;

			CSingleLock local_lock(&threads_list_critical_section);
			local_lock.Lock();

			threads_list.push_back(local_thread_information);
		}
	}
	else
	{
		set_command_threads_retranslate_web_camera_video_stop(true);
	}
}

void Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckRetranslateMicrophone()
{
	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();

	GUI_CONTROLS_STATE_data.IDC_CHECK_RETRANSLATE_MICROPHONE_state = button_retranslate_microphone.GetCheck();
	if (GUI_CONTROLS_STATE_data.IDC_CHECK_RETRANSLATE_MICROPHONE_state != 0)
	{
		button_send_audio.SetCheck(0);
		GUI_CONTROLS_STATE_data.IDC_CHECK_SEND_MICROPHONE_AUDIO_state = 0;

		set_command_threads_retranslate_audio_stop(false);

		{
			void *local_send_audio_thread_parameters_structure = new thread_send_audio_parameters_structure_type;

			((thread_send_audio_parameters_structure_type*)local_send_audio_thread_parameters_structure)->parameter_main_dialog = this;

			CWinThread *local_thread = AfxBeginThread(datagram_retranslate_audio_connection_thread, local_send_audio_thread_parameters_structure);

			THREADS_INFORMATION local_thread_information;
			local_thread_information.thread_name = CString(L"datagram_retranslate_audio_connection_thread");
			local_thread_information.WinThread = local_thread->m_hThread;

			CSingleLock local_lock(&threads_list_critical_section);
			local_lock.Lock();

			threads_list.push_back(local_thread_information);
		}
	}
	else
	{
		set_command_threads_retranslate_audio_stop(true);
	}
}

//	retranslate

UINT __cdecl datagram_retranslate_video_connection_thread(LPVOID parameter)
{
	thread_send_video_parameters_structure_type *local_send_video_thread_parameters_structure_source = (thread_send_video_parameters_structure_type *)parameter;

	if (local_send_video_thread_parameters_structure_source == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = local_send_video_thread_parameters_structure_source->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete parameter;

		return 0;
	}

	{
		void *local_send_video_thread_parameters_structure = new thread_send_video_parameters_structure_type;

		((thread_send_video_parameters_structure_type*)local_send_video_thread_parameters_structure)->parameter_main_dialog = ((thread_send_video_parameters_structure_type*)local_send_video_thread_parameters_structure_source)->parameter_main_dialog;

		CWinThread *local_thread = AfxBeginThread(datagram_retranslate_video_connection_thread_ip_4, local_send_video_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_retranslate_video_connection_thread_ip_4");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
		local_lock.Lock();

		local_main_dialog->threads_list.push_back(local_thread_information);
	}

	{
		void *local_send_video_thread_parameters_structure = new thread_send_video_parameters_structure_type;

		((thread_send_video_parameters_structure_type*)local_send_video_thread_parameters_structure)->parameter_main_dialog = ((thread_send_video_parameters_structure_type*)local_send_video_thread_parameters_structure_source)->parameter_main_dialog;

		CWinThread *local_thread = AfxBeginThread(datagram_retranslate_video_connection_thread_ip_6, local_send_video_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_retranslate_video_connection_thread_ip_6");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
		local_lock.Lock();

		local_main_dialog->threads_list.push_back(local_thread_information);
	}

	{
		clear_thread_information(local_main_dialog);
	}

	delete parameter;

	return 1;
}


//	retranslate video

UINT __cdecl datagram_retranslate_video_connection_thread_ip_4(LPVOID parameter)
{
	DWORD sequence_number = DWORD(rand());

	thread_send_video_parameters_structure_type *local_send_video_thread_parameters_structure = (thread_send_video_parameters_structure_type *)parameter;

	if (local_send_video_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_send_video_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_send_video_thread_parameters_structure;
		return 0;
	}

	char *send_buffer = new char[CONST_MESSAGE_LENGTH_IMAGE];

	if (send_buffer == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_send_video_thread_parameters_structure;
		return 0;
	}

	char *send_buffer_this_time = new char[CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME];

	if (send_buffer_this_time == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete local_send_video_thread_parameters_structure;
		return 0;
	}

	for (;;)
	{
		ULONGLONG local_start_capture_time = GetTickCount64();

		if (local_main_dialog->get_command_threads_retranslate_video_stop())
		{
			break;
		}

		CString local_address(localhost_definition);

		CStringA local_address_internet_address;

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}

		int peers_to_send_count = 0;
		{
			DWORD dwWaitResult;

			dwWaitResult = WaitForSingleObject(
				local_main_dialog->do_not_terminate_application_event, // event handle
				0);    // zero wait

			if (dwWaitResult == WAIT_OBJECT_0)
			{
				// Event object was signaled		

				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_video_thread_parameters_structure;
					return 0;
				}

				for (
					std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
					current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
					current_item_iterator++
					)
				{
					if (current_item_iterator->is_checked > 0)
					{
						peers_to_send_count++;
					}
				}
			}
			else
			{
				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_video_thread_parameters_structure;
					return 0;
				}
			}
		}

		CComPtr<IStream> local_stream;

		if (local_main_dialog->retranslate_video_frames_ip_4.size() == 0)
		{
			Sleep(10);
			continue;
		}
		else
		{
			std::list<STREAM_INFORMATION>::iterator local_frames_iterator = local_main_dialog->retranslate_video_frames_ip_4.begin();

			local_frames_iterator->stream->Clone(&local_stream);

			local_main_dialog->retranslate_video_frames_ip_4.pop_front();
		}

		if (local_stream == NULL)
		{
			Sleep(10);
			continue;
		}

		STATSTG local_istream_statstg;
		HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg, STATFLAG::STATFLAG_DEFAULT);

		double local_data_size_send = 0.0;

		ULONGLONG local_end_capture_time = GetTickCount64();

		ULONGLONG local_capture_time = local_end_capture_time - local_start_capture_time;

		for (int peers_to_send_count_counter = 0; peers_to_send_count_counter < peers_to_send_count; peers_to_send_count_counter++)
		{
			//double local_data_size_send = 0.0;

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

#ifdef use_istream_DEFINITION
			//CImage local_image;
#endif

			CString peer_to_send;

			int loop_counter = 0;
			for (
				std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
				current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
				current_item_iterator++
				)
			{
				Sleep(1);
				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
				if (current_item_iterator->is_checked > 0)
				{
					if (peers_to_send_count_counter == loop_counter)
					{
						peer_to_send = current_item_iterator->label;
						break;
					}
					loop_counter++;
				}

				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
			}

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

			local_address = peer_to_send;


			//	Здесь делаем снимок экрана

			//CSingleLock local_image_lock(&local_main_dialog->draw_video_image_critical_section);

			//local_image_lock.Lock();
#ifdef use_istream_DEFINITION
			//if(!local_image.IsNull())
			//{
			//	local_image.Destroy();
			//}
#endif

			if (network::ip_4::domain_name_to_internet_4_name(local_address, local_address_internet_address) == false)
			{
				const int local_error_message_size = 10000;
				wchar_t local_error_message[local_error_message_size];

				const int local_system_error_message_size = local_error_message_size - 1000;
				wchar_t local_system_error_message[local_system_error_message_size];

				wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

				CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

				wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

				continue;
			}
#ifdef use_istream_DEFINITION
			//CaptureScreenShot(&local_image);

			//CComPtr<IStream> local_stream;

			//HRESULT local_create_IStream_result = CreateStreamOnHGlobal ( 0 , TRUE , &local_stream );

			//try
			//{
			//	HRESULT local_save_result = local_image.Save(local_stream, Gdiplus::ImageFormatPNG);
			//}
			//catch(...)
			//{
			//	continue;
			//}

#endif
			//local_image_lock.Unlock();

			//ULONGLONG local_start_time = GetTickCount64();

			//STATSTG local_istream_statstg;
			//HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg,STATFLAG::STATFLAG_DEFAULT);

			/*/
			//			Тест отображения из IStream
			{
			CString new_picture_file_name;

			SYSTEMTIME local_system_time;

			GetLocalTime(&local_system_time);

			new_picture_file_name.Format
			(
			L"%04d-%02d-%02d %02d-%02d-%02d.%03d.png",
			local_system_time.wYear,
			local_system_time.wMonth,
			local_system_time.wDay,
			local_system_time.wHour,
			local_system_time.wMinute,
			local_system_time.wSecond,
			local_system_time.wMilliseconds
			);

			std::fstream local_file;
			local_file.open(new_picture_file_name.GetBuffer(),std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

			char *local_file_data = new char[CONST_MESSAGE_LENGTH_IMAGE];

			ULONG local_read_bytes = 0;

			LARGE_INTEGER liBeggining = { 0 };

			local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

			local_stream->Read(local_file_data,CONST_MESSAGE_LENGTH_IMAGE,&local_read_bytes);

			local_file.write(local_file_data, local_read_bytes);

			delete [] local_file_data;

			local_file.flush();

			local_file.close();
			}
			//*/



			for (UINT local_parameter_port_number = port_number_start_const; local_parameter_port_number <= port_number_end_const; local_parameter_port_number++)
			{
				network::ip_4::CBlockingSocket_ip_4 local_blocking_socket;

				if (network::ip_4::domain_name_to_internet_4_name(local_address, local_address_internet_address) == false)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					break;
				}

				network::ip_4::CSockAddr_ip_4 local_socket_address(local_address_internet_address, local_parameter_port_number);

				try
				{
					local_blocking_socket.Create(SOCK_DGRAM);
				}
				catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					local_blocking_socket_exception->Delete();

					delete local_send_video_thread_parameters_structure;

					//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete[] send_buffer_this_time;

					return 0;
				}

				int local_bytes_sent = 0;
				int local_error_number = 0;


				int send_buffer_data_length = service_signature_definition_length;

				ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH_IMAGE);

				memcpy(send_buffer, service_signature, service_signature_definition_length);



				CString send_data_string = command_video;

				int send_data_string_length = send_data_string.GetLength();

				memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

				send_buffer_data_length += send_data_string_length * sizeof(wchar_t);

				wchar_t zero_word = L'\0';
				memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
				send_buffer_data_length += sizeof(wchar_t);
				send_data_string_length++;


				memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
				send_buffer_data_length += sizeof(DWORD);
				send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);


				LARGE_INTEGER liBeggining = { 0 };
#ifdef use_istream_DEFINITION
				local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);
#endif


				ULONG local_read = 0;
#ifdef use_istream_DEFINITION
				local_stream->Read(send_buffer + send_buffer_data_length, CONST_MESSAGE_LENGTH_IMAGE - send_buffer_data_length, &local_read);
#else
				local_read = 100000;
#endif
				send_buffer_data_length += local_read;




				CString xor_code_string;

				//if(local_main_dialog->get_command_terminate_application())
				//{
				//	break;
				//}
				{
					xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
				}

				char xor_code = _wtoi(xor_code_string);

				encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

				int local_header_length = service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 3 * sizeof(DWORD);

				int local_part_counter = 0;
				int local_parts_count = local_read / (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 3 * sizeof(DWORD));
				int local_last_part_size = local_read % (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 3 * sizeof(DWORD));

				if (local_last_part_size != 0)
				{
					local_parts_count++;
				}

				int local_sequence_frame_number = rand();

				for (; local_part_counter < local_parts_count; local_part_counter++)
				{
					ULONGLONG local_start_time = GetTickCount64();

					int send_buffer_this_time_data_length = CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME;

					if (local_part_counter == local_parts_count - 1)
					{
						if (local_last_part_size != 0)
						{
							send_buffer_this_time_data_length = local_last_part_size + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 3 * sizeof(DWORD);
						}
					}

					ZeroMemory(send_buffer_this_time, CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME);

					memcpy(send_buffer_this_time, send_buffer, service_signature_definition_length + send_data_string_length * sizeof(wchar_t));

					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[0] = DWORD(local_part_counter + 1);
					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[1] = DWORD(local_parts_count);

					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[2] = DWORD(local_sequence_frame_number);

					encrypt::encrypt_xor(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t), 3 * sizeof(DWORD), xor_code);

					int local_this_time_data_offset = local_part_counter*(CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 3 * sizeof(DWORD));

					memcpy
					(
						send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 3 * sizeof(DWORD),
						send_buffer + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + local_this_time_data_offset,
						send_buffer_this_time_data_length - (service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 3 * sizeof(DWORD))
					);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer_this_time, send_buffer_this_time_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
						break;
					}

					//if(local_main_dialog->get_command_threads_retranslate_video_stop())
					//{
					//	break;
					//}
					//else
					{
						ULONGLONG local_end_time = GetTickCount64();

						ULONGLONG local_work_time = local_end_time - local_start_time;

						CString local_string_speed;

						if (local_main_dialog->get_command_terminate_application())
						{
							break;
						}
						{
							local_string_speed = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_SPEED_state;
						}
						double local_double_speed = _wtof(local_string_speed);
						if (local_data_size_send != 0.0)
						{
							double local_current_time_in_seconds = double(local_work_time + 1) / 1000.0;
							double local_current_bits_send = (local_bytes_sent*8.0);
							double local_current_speed = local_current_bits_send / local_current_time_in_seconds;

							double local_test_rate_1 = double((local_double_speed / 8) / (local_parts_count*local_bytes_sent));		//	Требуемая частота кадров
							double local_test_rate_2 = double((local_current_speed / 8) / (local_bytes_sent));						//	Текущая частота отдачи кадров
							double local_test_rate_3 =
								double
								(
									1000.0
									*
									local_parts_count
									*
									port_count_const
									*
									peers_to_send_count
									/
									local_capture_time
									);
							//	Текущая частота съёма кадров на часть на порт на пару

							double local_test_rate_4 = local_test_rate_3;

							DWORD time_to_sleep = 0;

							if (local_test_rate_1 >= local_test_rate_4)
							{
								time_to_sleep = 0;
							}
							else
							{
								time_to_sleep = DWORD((1000.0 / local_test_rate_1 - 1000.0 / local_test_rate_4) /
									(
										local_parts_count
										*
										port_count_const
										*
										peers_to_send_count)
								);
							}

							Sleep(time_to_sleep);
						}
					}

				}

				{
					int local_bytes_sent = 0;
					int local_error_number = 0;

					char send_buffer[CONST_MESSAGE_LENGTH];

					int send_buffer_data_length = service_signature_definition_length;

					ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH);

					memcpy(send_buffer, service_signature, service_signature_definition_length);

					CString send_data_string;

					int send_data_string_length = 0;

					send_data_string = command_video_end;

					send_data_string_length = send_data_string.GetLength();

					memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

					send_buffer_data_length += send_data_string_length * sizeof(wchar_t);


					wchar_t zero_word = L'\0';
					memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
					send_buffer_data_length += sizeof(wchar_t);
					send_data_string_length++;


					memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
					send_buffer_data_length += sizeof(DWORD);
					send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);


					CString xor_code_string;

					//if(local_main_dialog->get_command_terminate_application())
					//{
					//	break;
					//}
					{
						xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
					}

					char xor_code = _wtoi(xor_code_string);

					encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer, send_buffer_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
					}
				}



				local_blocking_socket.Close();

				if (local_bytes_sent <= 0 && local_error_number == 0)
				{
					//			galaxy::MessageBox(CString(L"local_bytes_sent<=0"),CString(L"Ошибка"));
				}

				if (local_main_dialog->get_command_threads_retranslate_video_stop())
				{
					break;
				}
				else
				{
					Sleep(10);
				}
			}
		}

		if (local_main_dialog->get_command_threads_retranslate_video_stop())
		{
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete[] send_buffer_this_time;

		delete local_send_video_thread_parameters_structure;
		return 1;
	}
}

UINT __cdecl datagram_retranslate_video_connection_thread_ip_6(LPVOID parameter)
{
	DWORD sequence_number = DWORD(rand());

	thread_send_video_parameters_structure_type *local_send_video_thread_parameters_structure = (thread_send_video_parameters_structure_type *)parameter;

	if (local_send_video_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_send_video_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_send_video_thread_parameters_structure;
		return 0;
	}

	char *send_buffer = new char[CONST_MESSAGE_LENGTH_IMAGE];

	if (send_buffer == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_send_video_thread_parameters_structure;
		return 0;
	}

	char *send_buffer_this_time = new char[CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME];

	if (send_buffer_this_time == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete local_send_video_thread_parameters_structure;
		return 0;
	}

	for (;;)
	{
		ULONGLONG local_start_capture_time = GetTickCount64();

		if (local_main_dialog->get_command_threads_retranslate_video_stop())
		{
			break;
		}

		CString local_address(localhost_definition);

		CStringA local_address_internet_address;

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}

		int peers_to_send_count = 0;
		{
			DWORD dwWaitResult;

			dwWaitResult = WaitForSingleObject(
				local_main_dialog->do_not_terminate_application_event, // event handle
				0);    // zero wait

			if (dwWaitResult == WAIT_OBJECT_0)
			{
				// Event object was signaled		

				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_video_thread_parameters_structure;
					return 0;
				}

				for (
					std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
					current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
					current_item_iterator++
					)
				{
					if (current_item_iterator->is_checked > 0)
					{
						peers_to_send_count++;
					}
				}
			}
			else
			{
				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_video_thread_parameters_structure;
					return 0;
				}
			}

		}

		CComPtr<IStream> local_stream;

		if (local_main_dialog->retranslate_video_frames_ip_6.size() == 0)
		{
			Sleep(10);
			continue;
		}
		else
		{
			std::list<STREAM_INFORMATION>::iterator local_frames_iterator = local_main_dialog->retranslate_video_frames_ip_6.begin();

			local_frames_iterator->stream->Clone(&local_stream);

			local_main_dialog->retranslate_video_frames_ip_6.pop_front();
		}

		if (local_stream == NULL)
		{
			Sleep(10);
			continue;
		}

		STATSTG local_istream_statstg;
		HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg, STATFLAG::STATFLAG_DEFAULT);

		double local_data_size_send = 0.0;

		ULONGLONG local_end_capture_time = GetTickCount64();

		ULONGLONG local_capture_time = local_end_capture_time - local_start_capture_time;

		for (int peers_to_send_count_counter = 0; peers_to_send_count_counter < peers_to_send_count; peers_to_send_count_counter++)
		{
			//double local_data_size_send = 0.0;

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

#ifdef use_istream_DEFINITION
			//CImage local_image;
#endif

			CString peer_to_send;

			int loop_counter = 0;
			for (
				std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
				current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
				current_item_iterator++
				)
			{
				Sleep(1);
				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
				if (current_item_iterator->is_checked > 0)
				{
					if (peers_to_send_count_counter == loop_counter)
					{
						peer_to_send = current_item_iterator->label;
						break;
					}
					loop_counter++;
				}

				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
			}

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

			local_address = peer_to_send;


			//	Здесь делаем снимок экрана

			//CSingleLock local_image_lock(&local_main_dialog->draw_video_image_critical_section);

			//local_image_lock.Lock();
#ifdef use_istream_DEFINITION
			//if(!local_image.IsNull())
			//{
			//	local_image.Destroy();
			//}
#endif

			if (network::ip_6::domain_name_to_internet_6_name(local_address, local_address_internet_address) == false)
			{
				const int local_error_message_size = 10000;
				wchar_t local_error_message[local_error_message_size];

				const int local_system_error_message_size = local_error_message_size - 1000;
				wchar_t local_system_error_message[local_system_error_message_size];

				wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

				CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

				wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

				continue;
			}
#ifdef use_istream_DEFINITION
			//CaptureScreenShot(&local_image);

			//CComPtr<IStream> local_stream;

			//HRESULT local_create_IStream_result = CreateStreamOnHGlobal ( 0 , TRUE , &local_stream );

			//try
			//{
			//	HRESULT local_save_result = local_image.Save(local_stream, Gdiplus::ImageFormatPNG);
			//}
			//catch(...)
			//{
			//	continue;
			//}
#endif
			//local_image_lock.Unlock();

			//ULONGLONG local_start_time = GetTickCount64();

			//STATSTG local_istream_statstg;
			//HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg,STATFLAG::STATFLAG_DEFAULT);

			/*/
			//			Тест отображения из IStream
			{
			std::fstream local_file;
			local_file.open("c:\\temp\\file_source.png",std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

			char *local_file_data = new char[CONST_MESSAGE_LENGTH_IMAGE];

			ULONG local_read_bytes = 0;

			LARGE_INTEGER liBeggining = { 0 };

			local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

			local_stream->Read(local_file_data,CONST_MESSAGE_LENGTH_IMAGE,&local_read_bytes);

			local_file.write(local_file_data, local_read_bytes);

			delete [] local_file_data;

			local_file.flush();

			local_file.close();
			}
			//*/



			for (UINT local_parameter_port_number = port_number_start_const; local_parameter_port_number <= port_number_end_const; local_parameter_port_number++)
			{
				network::ip_6::CBlockingSocket_ip_6 local_blocking_socket;

				if (network::ip_6::domain_name_to_internet_6_name(local_address, local_address_internet_address) == false)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					break;
				}

				network::ip_6::CSockAddr_ip_6 local_socket_address(local_address_internet_address, local_parameter_port_number);

				try
				{
					local_blocking_socket.Create(SOCK_DGRAM);
				}
				catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					local_blocking_socket_exception->Delete();

					delete local_send_video_thread_parameters_structure;

					//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete[] send_buffer_this_time;

					return 0;
				}

				int local_bytes_sent = 0;
				int local_error_number = 0;


				int send_buffer_data_length = service_signature_definition_length;

				ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH_IMAGE);

				memcpy(send_buffer, service_signature, service_signature_definition_length);



				CString send_data_string = command_video;

				int send_data_string_length = send_data_string.GetLength();

				memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

				send_buffer_data_length += send_data_string_length * sizeof(wchar_t);

				wchar_t zero_word = L'\0';
				memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
				send_buffer_data_length += sizeof(wchar_t);
				send_data_string_length++;


				memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
				send_buffer_data_length += sizeof(DWORD);
				send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);



				LARGE_INTEGER liBeggining = { 0 };
#ifdef use_istream_DEFINITION
				local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);
#endif


				ULONG local_read = 0;
#ifdef use_istream_DEFINITION
				local_stream->Read(send_buffer + send_buffer_data_length, CONST_MESSAGE_LENGTH_IMAGE - send_buffer_data_length, &local_read);
#else
				local_read = 100000;
#endif
				send_buffer_data_length += local_read;




				CString xor_code_string;

				//if(local_main_dialog->get_command_terminate_application())
				//{
				//	break;
				//}
				{
					xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
				}

				char xor_code = _wtoi(xor_code_string);

				encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

				int local_header_length = service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 3 * sizeof(DWORD);

				int local_part_counter = 0;
				int local_parts_count = local_read / (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 3 * sizeof(DWORD));
				int local_last_part_size = local_read % (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 3 * sizeof(DWORD));

				if (local_last_part_size != 0)
				{
					local_parts_count++;
				}

				int local_sequence_frame_number = rand();

				for (; local_part_counter < local_parts_count; local_part_counter++)
				{
					ULONGLONG local_start_time = GetTickCount64();

					int send_buffer_this_time_data_length = CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME;

					if (local_part_counter == local_parts_count - 1)
					{
						if (local_last_part_size != 0)
						{
							send_buffer_this_time_data_length = local_last_part_size + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 3 * sizeof(DWORD);
						}
					}

					ZeroMemory(send_buffer_this_time, CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME);

					memcpy(send_buffer_this_time, send_buffer, service_signature_definition_length + send_data_string_length * sizeof(wchar_t));

					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[0] = DWORD(local_part_counter + 1);
					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[1] = DWORD(local_parts_count);

					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[2] = DWORD(local_sequence_frame_number);

					encrypt::encrypt_xor(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t), 3 * sizeof(DWORD), xor_code);

					int local_this_time_data_offset = local_part_counter*(CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 3 * sizeof(DWORD));

					memcpy
					(
						send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 3 * sizeof(DWORD),
						send_buffer + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + local_this_time_data_offset,
						send_buffer_this_time_data_length - (service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 3 * sizeof(DWORD))
					);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer_this_time, send_buffer_this_time_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
						break;
					}

					//if(local_main_dialog->get_command_threads_retranslate_video_stop())
					//{
					//	break;
					//}
					//else
					{
						ULONGLONG local_end_time = GetTickCount64();

						ULONGLONG local_work_time = local_end_time - local_start_time;

						CString local_string_speed;

						if (local_main_dialog->get_command_terminate_application())
						{
							break;
						}
						{
							local_string_speed = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_SPEED_state;
						}
						double local_double_speed = _wtof(local_string_speed);
						if (local_data_size_send != 0.0)
						{
							double local_current_time_in_seconds = double(local_work_time + 1) / 1000.0;
							double local_current_bits_send = (local_bytes_sent*8.0);
							double local_current_speed = local_current_bits_send / local_current_time_in_seconds;

							double local_test_rate_1 = double((local_double_speed / 8) / (local_parts_count*local_bytes_sent));		//	Требуемая частота кадров
							double local_test_rate_2 = double((local_current_speed / 8) / (local_bytes_sent));						//	Текущая частота отдачи кадров
							double local_test_rate_3 =
								double
								(
									1000.0
									*
									local_parts_count
									*
									port_count_const
									*
									peers_to_send_count
									/
									local_capture_time
									);
							//	Текущая частота съёма кадров на часть на порт на пару

							double local_test_rate_4 = local_test_rate_3;

							DWORD time_to_sleep = 0;

							if (local_test_rate_1 >= local_test_rate_4)
							{
								time_to_sleep = 0;
							}
							else
							{
								time_to_sleep = DWORD((1000.0 / local_test_rate_1 - 1000.0 / local_test_rate_4) /
									(
										local_parts_count
										*
										port_count_const
										*
										peers_to_send_count)
								);
							}

							Sleep(time_to_sleep);
						}
					}

				}

				{
					int local_bytes_sent = 0;
					int local_error_number = 0;

					char send_buffer[CONST_MESSAGE_LENGTH];

					int send_buffer_data_length = service_signature_definition_length;

					ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH);

					memcpy(send_buffer, service_signature, service_signature_definition_length);

					CString send_data_string;

					int send_data_string_length = 0;

					send_data_string = command_video_end;

					send_data_string_length = send_data_string.GetLength();

					memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

					send_buffer_data_length += send_data_string_length * sizeof(wchar_t);


					wchar_t zero_word = L'\0';
					memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
					send_buffer_data_length += sizeof(wchar_t);
					send_data_string_length++;


					memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
					send_buffer_data_length += sizeof(DWORD);
					send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);

					CString xor_code_string;

					//if(local_main_dialog->get_command_terminate_application())
					//{
					//	break;
					//}
					{
						xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
					}

					char xor_code = _wtoi(xor_code_string);

					encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer, send_buffer_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
					}
				}



				local_blocking_socket.Close();

				if (local_bytes_sent <= 0 && local_error_number == 0)
				{
					//			galaxy::MessageBox(CString(L"local_bytes_sent<=0"),CString(L"Ошибка"));
				}

				if (local_main_dialog->get_command_threads_retranslate_video_stop())
				{
					break;
				}
				else
				{
					Sleep(10);
				}
			}
		}


		if (local_main_dialog->get_command_threads_retranslate_video_stop())
		{
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete[] send_buffer_this_time;

		delete local_send_video_thread_parameters_structure;
		return 1;
	}
}





UINT __cdecl datagram_retranslate_web_camera_video_connection_thread(LPVOID parameter)
{
	thread_send_web_camera_video_parameters_structure_type *local_send_web_camera_video_thread_parameters_structure_source = (thread_send_web_camera_video_parameters_structure_type *)parameter;

	if (local_send_web_camera_video_thread_parameters_structure_source == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = local_send_web_camera_video_thread_parameters_structure_source->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete parameter;

		return 0;
	}

	{
		void *local_send_web_camera_video_thread_parameters_structure = new thread_send_web_camera_video_parameters_structure_type;

		((thread_send_web_camera_video_parameters_structure_type*)local_send_web_camera_video_thread_parameters_structure)->parameter_main_dialog = ((thread_send_web_camera_video_parameters_structure_type*)local_send_web_camera_video_thread_parameters_structure_source)->parameter_main_dialog;

		CWinThread *local_thread = AfxBeginThread(datagram_retranslate_web_camera_video_connection_thread_ip_4, local_send_web_camera_video_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_retranslate_web_camera_video_connection_thread_ip_4");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
		local_lock.Lock();

		local_main_dialog->threads_list.push_back(local_thread_information);
	}

	{
		void *local_send_web_camera_video_thread_parameters_structure = new thread_send_web_camera_video_parameters_structure_type;

		((thread_send_web_camera_video_parameters_structure_type*)local_send_web_camera_video_thread_parameters_structure)->parameter_main_dialog = ((thread_send_web_camera_video_parameters_structure_type*)local_send_web_camera_video_thread_parameters_structure_source)->parameter_main_dialog;

		CWinThread *local_thread = AfxBeginThread(datagram_retranslate_web_camera_video_connection_thread_ip_6, local_send_web_camera_video_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_retranslate_web_camera_video_connection_thread_ip_6");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
		local_lock.Lock();

		local_main_dialog->threads_list.push_back(local_thread_information);
	}

	{
		clear_thread_information(local_main_dialog);
	}

	delete parameter;

	return 1;
}

//	retranslate web camera video

UINT __cdecl datagram_retranslate_web_camera_video_connection_thread_ip_4(LPVOID parameter)
{
	DWORD sequence_number = DWORD(rand());

	thread_send_web_camera_video_parameters_structure_type *local_send_web_camera_video_thread_parameters_structure = (thread_send_web_camera_video_parameters_structure_type *)parameter;

	if (local_send_web_camera_video_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_send_web_camera_video_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_send_web_camera_video_thread_parameters_structure;
		return 0;
	}

	char *send_buffer = new char[CONST_MESSAGE_LENGTH_IMAGE];

	if (send_buffer == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_send_web_camera_video_thread_parameters_structure;
		return 0;
	}

	char *send_buffer_this_time = new char[CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME];

	if (send_buffer_this_time == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete local_send_web_camera_video_thread_parameters_structure;
		return 0;
	}

	for (;;)
	{
		ULONGLONG local_start_time = GetTickCount64();

		if (local_main_dialog->get_command_threads_retranslate_web_camera_video_stop())
		{
			break;
		}

		CString local_address(localhost_definition);

		CStringA local_address_internet_address;

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}

		int peers_to_send_count = 0;
		{
			DWORD dwWaitResult;

			dwWaitResult = WaitForSingleObject(
				local_main_dialog->do_not_terminate_application_event, // event handle
				0);    // zero wait

			if (dwWaitResult == WAIT_OBJECT_0)
			{
				// Event object was signaled		

				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_web_camera_video_thread_parameters_structure;
					return 0;
				}

				for (
					std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
					current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
					current_item_iterator++
					)
				{
					if (current_item_iterator->is_checked > 0)
					{
						peers_to_send_count++;
					}
				}
			}
			else
			{
				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_web_camera_video_thread_parameters_structure;
					return 0;
				}
			}

		}

		CComPtr<IStream> local_stream;

		if (local_main_dialog->retranslate_web_camera_video_frames_ip_4.size() == 0)
		{
			Sleep(10);
			continue;
		}
		else
		{
			std::list<STREAM_INFORMATION>::iterator local_frames_iterator = local_main_dialog->retranslate_web_camera_video_frames_ip_4.begin();

			local_frames_iterator->stream->Clone(&local_stream);

			local_main_dialog->retranslate_web_camera_video_frames_ip_4.pop_front();
		}

		if (local_stream == NULL)
		{
			Sleep(10);
			continue;
		}

		STATSTG local_istream_statstg;
		HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg, STATFLAG::STATFLAG_DEFAULT);

		double local_data_size_send = 0.0;

		for (int peers_to_send_count_counter = 0; peers_to_send_count_counter < peers_to_send_count; peers_to_send_count_counter++)
		{
			//double local_data_size_send = 0.0;

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

#ifdef use_istream_DEFINITION
			//CImage local_image;
#endif

			CString peer_to_send;

			int loop_counter = 0;
			for (
				std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
				current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
				current_item_iterator++
				)
			{
				Sleep(1);
				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
				if (current_item_iterator->is_checked > 0)
				{
					if (peers_to_send_count_counter == loop_counter)
					{
						peer_to_send = current_item_iterator->label;
						break;
					}
					loop_counter++;
				}

				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
			}

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

			local_address = peer_to_send;


			//	Здесь делаем снимок экрана

			//CSingleLock local_image_lock(&local_main_dialog->draw_video_image_critical_section);

			//local_image_lock.Lock();
#ifdef use_istream_DEFINITION
			//if(!local_image.IsNull())
			//{
			//	local_image.Destroy();
			//}
#endif


			if (network::ip_4::domain_name_to_internet_4_name(local_address, local_address_internet_address) == false)
			{
				const int local_error_message_size = 10000;
				wchar_t local_error_message[local_error_message_size];

				const int local_system_error_message_size = local_error_message_size - 1000;
				wchar_t local_system_error_message[local_system_error_message_size];

				wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

				CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

				wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

				continue;
			}

#ifdef use_istream_DEFINITION
			//if(local_main_dialog->web_camera_dialog!=NULL)
			//{
			//	CSingleLock lock(&local_main_dialog->delete_web_camera_dialog_critical_section);

			//	lock.Lock();

			//	//CaptureWndShot(local_main_dialog->web_camera_dialog->m_hWnd,&local_image,ENGINE_WINDOWS_SIZE_CX/2,ENGINE_WINDOWS_SIZE_CY/2);
			//	CaptureVideoSampleShot(local_main_dialog->web_camera_dialog,&local_image);
			//}

			//CComPtr<IStream> local_stream;

			//HRESULT local_create_IStream_result = CreateStreamOnHGlobal ( 0 , TRUE , &local_stream );

			//try
			//{
			//	HRESULT local_save_result = local_image.Save(local_stream, Gdiplus::ImageFormatJPEG);
			//}
			//catch(...)
			//{
			//	continue;
			//}
#endif
			//local_image_lock.Unlock();


			//ULONGLONG local_start_time = GetTickCount64();

			//STATSTG local_istream_statstg;
			//HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg,STATFLAG::STATFLAG_DEFAULT);

			/*/
			//			Тест отображения из IStream
			{
			std::fstream local_file;
			local_file.open("c:\\temp\\file_source.jpg",std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

			char *local_file_data = new char[CONST_MESSAGE_LENGTH_IMAGE];

			ULONG local_read_bytes = 0;

			LARGE_INTEGER liBeggining = { 0 };

			local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

			local_stream->Read(local_file_data,CONST_MESSAGE_LENGTH_IMAGE,&local_read_bytes);

			local_file.write(local_file_data, local_read_bytes);

			delete [] local_file_data;

			local_file.flush();

			local_file.close();
			}
			//*/



			for (UINT local_parameter_port_number = port_number_start_const; local_parameter_port_number <= port_number_end_const; local_parameter_port_number++)
			{
				network::ip_4::CBlockingSocket_ip_4 local_blocking_socket;

				if (network::ip_4::domain_name_to_internet_4_name(local_address, local_address_internet_address) == false)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					break;
				}

				network::ip_4::CSockAddr_ip_4 local_socket_address(local_address_internet_address, local_parameter_port_number);

				try
				{
					local_blocking_socket.Create(SOCK_DGRAM);
				}
				catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					local_blocking_socket_exception->Delete();

					delete local_send_web_camera_video_thread_parameters_structure;

					//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete[] send_buffer_this_time;

					return 0;
				}

				int local_bytes_sent = 0;
				int local_error_number = 0;


				int send_buffer_data_length = service_signature_definition_length;

				ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH_IMAGE);

				memcpy(send_buffer, service_signature, service_signature_definition_length);



				CString send_data_string = command_web_camera_video;

				int send_data_string_length = send_data_string.GetLength();

				memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

				send_buffer_data_length += send_data_string_length * sizeof(wchar_t);

				wchar_t zero_word = L'\0';
				memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
				send_buffer_data_length += sizeof(wchar_t);
				send_data_string_length++;


				memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
				send_buffer_data_length += sizeof(DWORD);
				send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);



				LARGE_INTEGER liBeggining = { 0 };
#ifdef use_istream_DEFINITION
				local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);
#endif


				ULONG local_read = 0;
#ifdef use_istream_DEFINITION
				local_stream->Read(send_buffer + send_buffer_data_length, CONST_MESSAGE_LENGTH_IMAGE - send_buffer_data_length, &local_read);
#else
				local_read = 100000;
#endif
				send_buffer_data_length += local_read;




				CString xor_code_string;

				//if(local_main_dialog->get_command_terminate_application())
				//{
				//	break;
				//}
				{
					xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
				}

				char xor_code = _wtoi(xor_code_string);

				encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

				int local_header_length = service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD);

				int local_part_counter = 0;
				int local_parts_count = local_read / (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));
				int local_last_part_size = local_read % (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));

				if (local_last_part_size != 0)
				{
					local_parts_count++;
				}

				for (; local_part_counter < local_parts_count; local_part_counter++)
				{
					int send_buffer_this_time_data_length = CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME;

					if (local_part_counter == local_parts_count - 1)
					{
						if (local_last_part_size != 0)
						{
							send_buffer_this_time_data_length = local_last_part_size + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD);
						}
					}

					ZeroMemory(send_buffer_this_time, CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME);

					memcpy(send_buffer_this_time, send_buffer, service_signature_definition_length + send_data_string_length * sizeof(wchar_t));

					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[0] = DWORD(local_part_counter + 1);
					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[1] = DWORD(local_parts_count);

					encrypt::encrypt_xor(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t), 2 * sizeof(DWORD), xor_code);

					int local_this_time_data_offset = local_part_counter*(CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));

					memcpy
					(
						send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD),
						send_buffer + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + local_this_time_data_offset,
						send_buffer_this_time_data_length - (service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD))
					);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer_this_time, send_buffer_this_time_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
						break;
					}

					//if(local_main_dialog->get_command_threads_retranslate_web_camera_video_stop())
					//{
					//	break;
					//}
					//else
					{
						ULONGLONG local_end_time = GetTickCount64();

						ULONGLONG local_work_time = local_end_time - local_start_time;

						if (local_main_dialog->get_command_threads_retranslate_web_camera_video_stop())
						{
							break;
						}
						else
						{
							CString local_string_speed;

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								local_string_speed = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_SPEED_state;
							}
							double local_double_speed = _wtof(local_string_speed);
							if (local_data_size_send != 0.0)
							{
								DWORD time_to_work = 1000;
								double local_current_time_in_seconds = double(local_work_time) / 1000.0;
								double local_current_bits_send = (local_data_size_send*8.0);
								double local_current_speed = local_current_bits_send / local_current_time_in_seconds;
								double local_current_frame_rate = 1.0 / local_current_time_in_seconds;
								double local_speed_factor = local_double_speed / local_current_speed;
								double local_common_factor = min(local_speed_factor, local_current_frame_rate);

								time_to_work = DWORD(local_common_factor*local_work_time);

								DWORD time_to_sleep = 1000 - time_to_work;

								//Sleep(time_to_sleep);
								Sleep(10);
							}
						}
					}

				}

				{
					int local_bytes_sent = 0;
					int local_error_number = 0;

					char send_buffer[CONST_MESSAGE_LENGTH];

					int send_buffer_data_length = service_signature_definition_length;

					ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH);

					memcpy(send_buffer, service_signature, service_signature_definition_length);

					CString send_data_string;

					int send_data_string_length = 0;

					send_data_string = command_web_camera_video_end;

					send_data_string_length = send_data_string.GetLength();

					memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

					send_buffer_data_length += send_data_string_length * sizeof(wchar_t);


					wchar_t zero_word = L'\0';
					memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
					send_buffer_data_length += sizeof(wchar_t);
					send_data_string_length++;


					memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
					send_buffer_data_length += sizeof(DWORD);
					send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);


					CString xor_code_string;

					//if(local_main_dialog->get_command_terminate_application())
					//{
					//	break;
					//}
					{
						xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
					}

					char xor_code = _wtoi(xor_code_string);

					encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer, send_buffer_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
					}
				}



				local_blocking_socket.Close();

				if (local_bytes_sent <= 0 && local_error_number == 0)
				{
					//			galaxy::MessageBox(CString(L"local_bytes_sent<=0"),CString(L"Ошибка"));
				}

				if (local_main_dialog->get_command_threads_retranslate_web_camera_video_stop())
				{
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			//ULONGLONG local_end_time = GetTickCount64();

			//ULONGLONG local_work_time = local_end_time - local_start_time;

			//if(local_main_dialog->get_command_threads_retranslate_web_camera_video_stop())
			//{
			//	break;
			//}
			//else
			//{
			//	CString local_string_speed;

			//	if(local_main_dialog->get_command_terminate_application())
			//	{
			//		break;
			//	}
			//	{
			//		local_string_speed = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_SPEED_state;
			//	}
			//	double local_double_speed = _wtof(local_string_speed);
			//	if(local_data_size_send!=0.0)
			//	{
			//		DWORD time_to_work = 1000;
			//		double local_current_time_in_seconds = double(local_work_time)/1000.0;
			//		double local_current_bits_send = (local_data_size_send*8.0);
			//		double local_current_speed = local_current_bits_send/local_current_time_in_seconds;
			//		double local_current_frame_rate = 1.0/local_current_time_in_seconds;
			//		double local_speed_factor = local_double_speed/local_current_speed;
			//		double local_common_factor = min(local_speed_factor,local_current_frame_rate);

			//		time_to_work = DWORD(local_common_factor*local_work_time);

			//		DWORD time_to_sleep = 1000 - time_to_work;

			//		Sleep(time_to_sleep);
			//	}
			//}
		}

		//ULONGLONG local_end_time = GetTickCount64();

		//ULONGLONG local_work_time = local_end_time - local_start_time;

		//if(local_main_dialog->get_command_threads_retranslate_web_camera_video_stop())
		//{
		//	break;
		//}
		//else
		//{
		//	CString local_string_speed;

		//	if(local_main_dialog->get_command_terminate_application())
		//	{
		//		break;
		//	}
		//	{
		//		local_string_speed = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_SPEED_state;
		//	}
		//	double local_double_speed = _wtof(local_string_speed);
		//	if(local_data_size_send!=0.0)
		//	{
		//		DWORD time_to_work = 1000;
		//		double local_current_time_in_seconds = double(local_work_time)/1000.0;
		//		double local_current_bits_send = (local_data_size_send*8.0);
		//		double local_current_speed = local_current_bits_send/local_current_time_in_seconds;
		//		double local_current_frame_rate = 1.0/local_current_time_in_seconds;
		//		double local_speed_factor = local_double_speed/local_current_speed;
		//		double local_common_factor = min(local_speed_factor,local_current_frame_rate);

		//		time_to_work = DWORD(local_common_factor*local_work_time);

		//		DWORD time_to_sleep = 1000 - time_to_work;

		//		Sleep(time_to_sleep);
		//	}
		//}

		if (local_main_dialog->get_command_threads_retranslate_web_camera_video_stop())
		{
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete[] send_buffer_this_time;

		delete local_send_web_camera_video_thread_parameters_structure;
		return 1;
	}
}

UINT __cdecl datagram_retranslate_web_camera_video_connection_thread_ip_6(LPVOID parameter)
{
	DWORD sequence_number = DWORD(rand());

	thread_send_web_camera_video_parameters_structure_type *local_send_web_camera_video_thread_parameters_structure = (thread_send_web_camera_video_parameters_structure_type *)parameter;

	if (local_send_web_camera_video_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_send_web_camera_video_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_send_web_camera_video_thread_parameters_structure;
		return 0;
	}

	char *send_buffer = new char[CONST_MESSAGE_LENGTH_IMAGE];

	if (send_buffer == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_send_web_camera_video_thread_parameters_structure;
		return 0;
	}

	char *send_buffer_this_time = new char[CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME];

	if (send_buffer_this_time == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete local_send_web_camera_video_thread_parameters_structure;
		return 0;
	}

	for (;;)
	{
		ULONGLONG local_start_time = GetTickCount64();

		if (local_main_dialog->get_command_threads_retranslate_web_camera_video_stop())
		{
			break;
		}

		CString local_address(localhost_definition);

		CStringA local_address_internet_address;

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}
		int peers_to_send_count = 0;
		{
			DWORD dwWaitResult;

			dwWaitResult = WaitForSingleObject(
				local_main_dialog->do_not_terminate_application_event, // event handle
				0);    // zero wait

			if (dwWaitResult == WAIT_OBJECT_0)
			{
				// Event object was signaled		

				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_web_camera_video_thread_parameters_structure;
					return 0;
				}

				for (
					std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
					current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
					current_item_iterator++
					)
				{
					if (current_item_iterator->is_checked > 0)
					{
						peers_to_send_count++;
					}
				}
			}
			else
			{
				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_web_camera_video_thread_parameters_structure;
					return 0;
				}
			}

		}

		CComPtr<IStream> local_stream;

		if (local_main_dialog->retranslate_web_camera_video_frames_ip_6.size() == 0)
		{
			Sleep(10);
			continue;
		}
		else
		{
			std::list<STREAM_INFORMATION>::iterator local_frames_iterator = local_main_dialog->retranslate_web_camera_video_frames_ip_6.begin();

			local_frames_iterator->stream->Clone(&local_stream);

			local_main_dialog->retranslate_web_camera_video_frames_ip_6.pop_front();
		}

		if (local_stream == NULL)
		{
			Sleep(10);
			continue;
		}

		STATSTG local_istream_statstg;
		HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg, STATFLAG::STATFLAG_DEFAULT);

		double local_data_size_send = 0.0;

		for (int peers_to_send_count_counter = 0; peers_to_send_count_counter < peers_to_send_count; peers_to_send_count_counter++)
		{
			//double local_data_size_send = 0.0;

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

#ifdef use_istream_DEFINITION
			//CImage local_image;
#endif

			CString peer_to_send;

			int loop_counter = 0;
			for (
				std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
				current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
				current_item_iterator++
				)
			{
				Sleep(1);
				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
				if (current_item_iterator->is_checked > 0)
				{
					if (peers_to_send_count_counter == loop_counter)
					{
						peer_to_send = current_item_iterator->label;
						break;
					}
					loop_counter++;
				}

				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
			}

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

			local_address = peer_to_send;


			//	Здесь делаем снимок экрана

			//CSingleLock local_image_lock(&local_main_dialog->draw_video_image_critical_section);

			//local_image_lock.Lock();
#ifdef use_istream_DEFINITION
			//if(!local_image.IsNull())
			//{
			//	local_image.Destroy();
			//}
#endif

			if (network::ip_6::domain_name_to_internet_6_name(local_address, local_address_internet_address) == false)
			{
				const int local_error_message_size = 10000;
				wchar_t local_error_message[local_error_message_size];

				const int local_system_error_message_size = local_error_message_size - 1000;
				wchar_t local_system_error_message[local_system_error_message_size];

				wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

				CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

				wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

				continue;
			}

#ifdef use_istream_DEFINITION
			//if(local_main_dialog->web_camera_dialog!=NULL)
			//{
			//	CSingleLock lock(&local_main_dialog->delete_web_camera_dialog_critical_section);
			//
			//	lock.Lock();
			//
			//	//CaptureWndShot(local_main_dialog->web_camera_dialog->m_hWnd,&local_image,ENGINE_WINDOWS_SIZE_CX/2,ENGINE_WINDOWS_SIZE_CY/2);
			//	CaptureVideoSampleShot(local_main_dialog->web_camera_dialog,&local_image);
			//}

			//CComPtr<IStream> local_stream;

			//HRESULT local_create_IStream_result = CreateStreamOnHGlobal ( 0 , TRUE , &local_stream );

			//try
			//{
			//	HRESULT local_save_result = local_image.Save(local_stream, Gdiplus::ImageFormatJPEG);
			//}
			//catch(...)
			//{
			//	continue;
			//}
#endif
			//local_image_lock.Unlock();


			//ULONGLONG local_start_time = GetTickCount64();

			//STATSTG local_istream_statstg;
			//HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg,STATFLAG::STATFLAG_DEFAULT);

			/*/
			//			Тест отображения из IStream
			{
			std::fstream local_file;
			local_file.open("c:\\temp\\file_source.jpg",std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

			char *local_file_data = new char[CONST_MESSAGE_LENGTH_IMAGE];

			ULONG local_read_bytes = 0;

			LARGE_INTEGER liBeggining = { 0 };

			local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

			local_stream->Read(local_file_data,CONST_MESSAGE_LENGTH_IMAGE,&local_read_bytes);

			local_file.write(local_file_data, local_read_bytes);

			delete [] local_file_data;

			local_file.flush();

			local_file.close();
			}
			//*/



			for (UINT local_parameter_port_number = port_number_start_const; local_parameter_port_number <= port_number_end_const; local_parameter_port_number++)
			{
				network::ip_6::CBlockingSocket_ip_6 local_blocking_socket;

				if (network::ip_6::domain_name_to_internet_6_name(local_address, local_address_internet_address) == false)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					break;
				}

				network::ip_6::CSockAddr_ip_6 local_socket_address(local_address_internet_address, local_parameter_port_number);

				try
				{
					local_blocking_socket.Create(SOCK_DGRAM);
				}
				catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					local_blocking_socket_exception->Delete();

					delete local_send_web_camera_video_thread_parameters_structure;

					//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete[] send_buffer_this_time;

					return 0;
				}

				int local_bytes_sent = 0;
				int local_error_number = 0;


				int send_buffer_data_length = service_signature_definition_length;

				ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH_IMAGE);

				memcpy(send_buffer, service_signature, service_signature_definition_length);



				CString send_data_string = command_web_camera_video;

				int send_data_string_length = send_data_string.GetLength();

				memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

				send_buffer_data_length += send_data_string_length * sizeof(wchar_t);

				wchar_t zero_word = L'\0';
				memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
				send_buffer_data_length += sizeof(wchar_t);
				send_data_string_length++;


				memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
				send_buffer_data_length += sizeof(DWORD);
				send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);



				LARGE_INTEGER liBeggining = { 0 };
#ifdef use_istream_DEFINITION
				local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);
#endif


				ULONG local_read = 0;
#ifdef use_istream_DEFINITION
				local_stream->Read(send_buffer + send_buffer_data_length, CONST_MESSAGE_LENGTH_IMAGE - send_buffer_data_length, &local_read);
#else
				local_read = 100000;
#endif
				send_buffer_data_length += local_read;




				CString xor_code_string;

				//if(local_main_dialog->get_command_terminate_application())
				//{
				//	break;
				//}
				{
					xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
				}

				char xor_code = _wtoi(xor_code_string);

				encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

				int local_header_length = service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD);

				int local_part_counter = 0;
				int local_parts_count = local_read / (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));
				int local_last_part_size = local_read % (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));

				if (local_last_part_size != 0)
				{
					local_parts_count++;
				}

				for (; local_part_counter < local_parts_count; local_part_counter++)
				{
					int send_buffer_this_time_data_length = CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME;

					if (local_part_counter == local_parts_count - 1)
					{
						if (local_last_part_size != 0)
						{
							send_buffer_this_time_data_length = local_last_part_size + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD);
						}
					}

					ZeroMemory(send_buffer_this_time, CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME);

					memcpy(send_buffer_this_time, send_buffer, service_signature_definition_length + send_data_string_length * sizeof(wchar_t));

					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[0] = DWORD(local_part_counter + 1);
					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[1] = DWORD(local_parts_count);

					encrypt::encrypt_xor(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t), 2 * sizeof(DWORD), xor_code);

					int local_this_time_data_offset = local_part_counter*(CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));

					memcpy
					(
						send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD),
						send_buffer + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + local_this_time_data_offset,
						send_buffer_this_time_data_length - (service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD))
					);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer_this_time, send_buffer_this_time_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
						break;
					}

					//if(local_main_dialog->get_command_threads_retranslate_web_camera_video_stop())
					//{
					//	break;
					//}
					//else
					{
						ULONGLONG local_end_time = GetTickCount64();

						ULONGLONG local_work_time = local_end_time - local_start_time;

						if (local_main_dialog->get_command_threads_retranslate_web_camera_video_stop())
						{
							break;
						}
						else
						{
							CString local_string_speed;

							if (local_main_dialog->get_command_terminate_application())
							{
								break;
							}
							{
								local_string_speed = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_SPEED_state;
							}
							double local_double_speed = _wtof(local_string_speed);
							if (local_data_size_send != 0.0)
							{
								DWORD time_to_work = 1000;
								double local_current_time_in_seconds = double(local_work_time) / 1000.0;
								double local_current_bits_send = (local_data_size_send*8.0);
								double local_current_speed = local_current_bits_send / local_current_time_in_seconds;
								double local_current_frame_rate = 1.0 / local_current_time_in_seconds;
								double local_speed_factor = local_double_speed / local_current_speed;
								double local_common_factor = min(local_speed_factor, local_current_frame_rate);

								time_to_work = DWORD(local_common_factor*local_work_time);

								DWORD time_to_sleep = 1000 - time_to_work;

								//Sleep(time_to_sleep);
								Sleep(10);
							}
						}
					}

				}

				{
					int local_bytes_sent = 0;
					int local_error_number = 0;

					char send_buffer[CONST_MESSAGE_LENGTH];

					int send_buffer_data_length = service_signature_definition_length;

					ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH);

					memcpy(send_buffer, service_signature, service_signature_definition_length);

					CString send_data_string;

					int send_data_string_length = 0;

					send_data_string = command_web_camera_video_end;

					send_data_string_length = send_data_string.GetLength();

					memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

					send_buffer_data_length += send_data_string_length * sizeof(wchar_t);


					wchar_t zero_word = L'\0';
					memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
					send_buffer_data_length += sizeof(wchar_t);
					send_data_string_length++;


					memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
					send_buffer_data_length += sizeof(DWORD);
					send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);


					CString xor_code_string;

					//if(local_main_dialog->get_command_terminate_application())
					//{
					//	break;
					//}
					{
						xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
					}

					char xor_code = _wtoi(xor_code_string);

					encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer, send_buffer_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
					}
				}



				local_blocking_socket.Close();

				if (local_bytes_sent <= 0 && local_error_number == 0)
				{
					//			galaxy::MessageBox(CString(L"local_bytes_sent<=0"),CString(L"Ошибка"));
				}

				if (local_main_dialog->get_command_threads_retranslate_web_camera_video_stop())
				{
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			//ULONGLONG local_end_time = GetTickCount64();

			//ULONGLONG local_work_time = local_end_time - local_start_time;

			//if(local_main_dialog->get_command_threads_retranslate_web_camera_video_stop())
			//{
			//	break;
			//}
			//else
			//{
			//	CString local_string_speed;

			//	if(local_main_dialog->get_command_terminate_application())
			//	{
			//		break;
			//	}
			//	{
			//		local_string_speed = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_SPEED_state;				
			//	}
			//	double local_double_speed = _wtof(local_string_speed);
			//	if(local_data_size_send!=0.0)
			//	{
			//		DWORD time_to_work = 1000;
			//		double local_current_time_in_seconds = double(local_work_time)/1000.0;
			//		double local_current_bits_send = (local_data_size_send*8.0);
			//		double local_current_speed = local_current_bits_send/local_current_time_in_seconds;
			//		double local_current_frame_rate = 1.0/local_current_time_in_seconds;
			//		double local_speed_factor = local_double_speed/local_current_speed;
			//		double local_common_factor = min(local_speed_factor,local_current_frame_rate);

			//		time_to_work = DWORD(local_common_factor*local_work_time);

			//		DWORD time_to_sleep = 1000 - time_to_work;

			//		Sleep(time_to_sleep);
			//	}
			//}
		}

		//ULONGLONG local_end_time = GetTickCount64();

		//ULONGLONG local_work_time = local_end_time - local_start_time;

		//if(local_main_dialog->get_command_threads_retranslate_web_camera_video_stop())
		//{
		//	break;
		//}
		//else
		//{
		//	CString local_string_speed;

		//	if(local_main_dialog->get_command_terminate_application())
		//	{
		//		break;
		//	}
		//	{
		//		local_string_speed = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_SPEED_state;				
		//	}
		//	double local_double_speed = _wtof(local_string_speed);
		//	if(local_data_size_send!=0.0)
		//	{
		//		DWORD time_to_work = 1000;
		//		double local_current_time_in_seconds = double(local_work_time)/1000.0;
		//		double local_current_bits_send = (local_data_size_send*8.0);
		//		double local_current_speed = local_current_bits_send/local_current_time_in_seconds;
		//		double local_current_frame_rate = 1.0/local_current_time_in_seconds;
		//		double local_speed_factor = local_double_speed/local_current_speed;
		//		double local_common_factor = min(local_speed_factor,local_current_frame_rate);

		//		time_to_work = DWORD(local_common_factor*local_work_time);

		//		DWORD time_to_sleep = 1000 - time_to_work;

		//		Sleep(time_to_sleep);
		//	}
		//}


		if (local_main_dialog->get_command_threads_retranslate_web_camera_video_stop())
		{
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete[] send_buffer_this_time;

		delete local_send_web_camera_video_thread_parameters_structure;
		return 1;
	}
}


//	retranslate audio

UINT __cdecl datagram_retranslate_audio_connection_thread(LPVOID parameter)
{
	thread_send_audio_parameters_structure_type *local_send_audio_thread_parameters_structure_source = (thread_send_audio_parameters_structure_type *)parameter;

	if (local_send_audio_thread_parameters_structure_source == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = local_send_audio_thread_parameters_structure_source->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete parameter;

		return 0;
	}

	{
		void *local_send_audio_thread_parameters_structure = new thread_send_audio_parameters_structure_type;

		((thread_send_audio_parameters_structure_type*)local_send_audio_thread_parameters_structure)->parameter_main_dialog = ((thread_send_audio_parameters_structure_type*)local_send_audio_thread_parameters_structure_source)->parameter_main_dialog;

		CWinThread *local_thread = AfxBeginThread(datagram_retranslate_audio_connection_thread_ip_4, local_send_audio_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_retranslate_audio_connection_thread_ip_4");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
		local_lock.Lock();

		local_main_dialog->threads_list.push_back(local_thread_information);
	}

	{
		void *local_send_audio_thread_parameters_structure = new thread_send_audio_parameters_structure_type;

		((thread_send_audio_parameters_structure_type*)local_send_audio_thread_parameters_structure)->parameter_main_dialog = ((thread_send_audio_parameters_structure_type*)local_send_audio_thread_parameters_structure_source)->parameter_main_dialog;

		CWinThread *local_thread = AfxBeginThread(datagram_retranslate_audio_connection_thread_ip_6, local_send_audio_thread_parameters_structure);

		THREADS_INFORMATION local_thread_information;
		local_thread_information.thread_name = CString(L"datagram_retranslate_audio_connection_thread_ip_6");
		local_thread_information.WinThread = local_thread->m_hThread;

		CSingleLock local_lock(&local_main_dialog->threads_list_critical_section);
		local_lock.Lock();

		local_main_dialog->threads_list.push_back(local_thread_information);
	}

	{
		clear_thread_information(local_main_dialog);
	}

	delete parameter;

	return 1;
}



//	retranslate audio

UINT __cdecl datagram_retranslate_audio_connection_thread_ip_4(LPVOID parameter)
{
	DWORD sequence_number = DWORD(rand());

	thread_send_audio_parameters_structure_type *local_send_audio_thread_parameters_structure = (thread_send_audio_parameters_structure_type *)parameter;

	if (local_send_audio_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_send_audio_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_send_audio_thread_parameters_structure;
		return 0;
	}

	char *send_buffer = new char[CONST_MESSAGE_LENGTH_IMAGE];

	if (send_buffer == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_send_audio_thread_parameters_structure;
		return 0;
	}

	char *send_buffer_this_time = new char[CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME];

	if (send_buffer_this_time == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete local_send_audio_thread_parameters_structure;
		return 0;
	}

	for (;;)
	{
		ULONGLONG local_start_time = GetTickCount64();

		if (local_main_dialog->get_command_threads_retranslate_audio_stop())
		{
			break;
		}

		CString local_address(localhost_definition);

		CStringA local_address_internet_address;

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}

		int peers_to_send_count = 0;
		{
			DWORD dwWaitResult;

			dwWaitResult = WaitForSingleObject(
				local_main_dialog->do_not_terminate_application_event, // event handle
				0);    // zero wait

			if (dwWaitResult == WAIT_OBJECT_0)
			{
				// Event object was signaled		

				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_audio_thread_parameters_structure;
					return 0;
				}

				for (
					std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
					current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
					current_item_iterator++
					)
				{
					if (current_item_iterator->is_checked > 0)
					{
						peers_to_send_count++;
					}
				}
			}
			else
			{
				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_audio_thread_parameters_structure;
					return 0;
				}
			}

		}

		CComPtr<IStream> local_stream;

		if (local_main_dialog->retranslate_microphone_frames_ip_4.size() == 0)
		{
			Sleep(10);
			continue;
		}
		else
		{
			std::list<STREAM_INFORMATION>::iterator local_frames_iterator = local_main_dialog->retranslate_microphone_frames_ip_4.begin();

			local_frames_iterator->stream->Clone(&local_stream);

			local_main_dialog->retranslate_microphone_frames_ip_4.pop_front();
		}

		if (local_stream == NULL)
		{
			Sleep(10);
			continue;
		}

		STATSTG local_istream_statstg;
		HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg, STATFLAG::STATFLAG_DEFAULT);

		double local_data_size_send = 0.0;

		for (int peers_to_send_count_counter = 0; peers_to_send_count_counter < peers_to_send_count; peers_to_send_count_counter++)
		{
			//double local_data_size_send = 0.0;

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

#ifdef use_istream_DEFINITION
			//IStream * local_stream = NULL;
#endif

			CString peer_to_send;

			int loop_counter = 0;
			for (
				std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
				current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
				current_item_iterator++
				)
			{
				Sleep(1);
				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
				if (current_item_iterator->is_checked > 0)
				{
					if (peers_to_send_count_counter == loop_counter)
					{
						peer_to_send = current_item_iterator->label;
						break;
					}
					loop_counter++;
				}

				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
			}

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

			local_address = peer_to_send;


			//	Здесь делаем снимок экрана

			//CSingleLock local_image_lock(&local_main_dialog->draw_audio_image_critical_section);

			//local_image_lock.Lock();
#ifdef use_istream_DEFINITION
#endif

			if (network::ip_4::domain_name_to_internet_4_name(local_address, local_address_internet_address) == false)
			{
				const int local_error_message_size = 10000;
				wchar_t local_error_message[local_error_message_size];

				const int local_system_error_message_size = local_error_message_size - 1000;
				wchar_t local_system_error_message[local_system_error_message_size];

				wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

				CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

				wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

				continue;
			}
#ifdef use_istream_DEFINITION
			//if(local_main_dialog->web_camera_dialog!=NULL)
			//{
			//	CSingleLock lock(&local_main_dialog->delete_web_camera_dialog_critical_section);

			//	lock.Lock();

			//	local_stream = NULL;

			//	//ULARGE_INTEGER zero_size = {0,0};

			//	//local_stream->SetSize(zero_size);

			//	//LARGE_INTEGER liBeggining = { 0 };

			//	//local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

			//	//CaptureAudioSampleShot(local_main_dialog->web_camera_dialog,local_stream);
			//	CaptureAudioSampleGetFromTheList(local_main_dialog->web_camera_dialog,&local_stream);

			//	if(local_stream==NULL)
			//	{
			//		continue;
			//	}
			//}
#endif
			//local_image_lock.Unlock();

			//if(local_stream==NULL)
			//{
			//	continue;
			//}


			//ULONGLONG local_start_time = GetTickCount64();

			//STATSTG local_istream_statstg;
			//HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg,STATFLAG::STATFLAG_DEFAULT);

			/*/
			//			Тест отображения из IStream
			{
			std::fstream local_file;
			local_file.open("c:\\temp\\file_source_2.wav",std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

			char *local_file_data = new char[CONST_MESSAGE_LENGTH_IMAGE];

			ULONG local_read_bytes = 0;

			LARGE_INTEGER liBeggining = { 0 };

			local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

			local_stream->Read(local_file_data,CONST_MESSAGE_LENGTH_IMAGE,&local_read_bytes);

			local_file.write(local_file_data, local_read_bytes);

			delete [] local_file_data;

			local_file.flush();

			local_file.close();
			}
			//*/



			for (UINT local_parameter_port_number = port_number_start_const; local_parameter_port_number <= port_number_end_const; local_parameter_port_number++)
			{
				network::ip_4::CBlockingSocket_ip_4 local_blocking_socket;

				if (network::ip_4::domain_name_to_internet_4_name(local_address, local_address_internet_address) == false)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					break;
				}

				network::ip_4::CSockAddr_ip_4 local_socket_address(local_address_internet_address, local_parameter_port_number);

				try
				{
					local_blocking_socket.Create(SOCK_DGRAM);
				}
				catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					local_blocking_socket_exception->Delete();

					delete local_send_audio_thread_parameters_structure;

					//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

					{
						clear_thread_information(local_main_dialog);
					}

					//if(local_stream!=NULL)
					//{
					//	local_stream->Release();
					//}

					delete[] send_buffer;
					delete[] send_buffer_this_time;

					return 0;
				}

				int local_bytes_sent = 0;
				int local_error_number = 0;


				int send_buffer_data_length = service_signature_definition_length;

				ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH_IMAGE);

				memcpy(send_buffer, service_signature, service_signature_definition_length);



				CString send_data_string = command_audio;

				int send_data_string_length = send_data_string.GetLength();

				memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

				send_buffer_data_length += send_data_string_length * sizeof(wchar_t);

				wchar_t zero_word = L'\0';
				memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
				send_buffer_data_length += sizeof(wchar_t);
				send_data_string_length++;


				memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
				send_buffer_data_length += sizeof(DWORD);
				send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);



				LARGE_INTEGER liBeggining = { 0 };
#ifdef use_istream_DEFINITION
				local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);
#endif


				ULONG local_read = 0;
#ifdef use_istream_DEFINITION
				local_stream->Read(send_buffer + send_buffer_data_length, CONST_MESSAGE_LENGTH_IMAGE - send_buffer_data_length, &local_read);
#else
				local_read = 100000;
#endif
				send_buffer_data_length += local_read;




				CString xor_code_string;

				//if(local_main_dialog->get_command_terminate_application())
				//{
				//	break;
				//}
				{
					xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
				}

				char xor_code = _wtoi(xor_code_string);

				encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

				int local_header_length = service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD);

				int local_part_counter = 0;
				int local_parts_count = local_read / (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));
				int local_last_part_size = local_read % (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));

				if (local_last_part_size != 0)
				{
					local_parts_count++;
				}

				for (; local_part_counter < local_parts_count; local_part_counter++)
				{
					int send_buffer_this_time_data_length = CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME;

					if (local_part_counter == local_parts_count - 1)
					{
						if (local_last_part_size != 0)
						{
							send_buffer_this_time_data_length = local_last_part_size + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD);
						}
					}

					ZeroMemory(send_buffer_this_time, CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME);

					memcpy(send_buffer_this_time, send_buffer, service_signature_definition_length + send_data_string_length * sizeof(wchar_t));

					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[0] = DWORD(local_part_counter + 1);
					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[1] = DWORD(local_parts_count);

					encrypt::encrypt_xor(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t), 2 * sizeof(DWORD), xor_code);

					int local_this_time_data_offset = local_part_counter*(CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));

					memcpy
					(
						send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD),
						send_buffer + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + local_this_time_data_offset,
						send_buffer_this_time_data_length - (service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD))
					);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer_this_time, send_buffer_this_time_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
						break;
					}

					//if(local_main_dialog->get_command_threads_retranslate_audio_stop())
					//{
					//	break;
					//}
					//else
					{
						Sleep(10);
					}

				}

				{
					int local_bytes_sent = 0;
					int local_error_number = 0;

					char send_buffer[CONST_MESSAGE_LENGTH];

					int send_buffer_data_length = service_signature_definition_length;

					ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH);

					memcpy(send_buffer, service_signature, service_signature_definition_length);

					CString send_data_string;

					int send_data_string_length = 0;

					send_data_string = command_audio_end;

					send_data_string_length = send_data_string.GetLength();

					memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

					send_buffer_data_length += send_data_string_length * sizeof(wchar_t);


					wchar_t zero_word = L'\0';
					memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
					send_buffer_data_length += sizeof(wchar_t);
					send_data_string_length++;


					memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
					send_buffer_data_length += sizeof(DWORD);
					send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);


					CString xor_code_string;

					//if(local_main_dialog->get_command_terminate_application())
					//{
					//	break;
					//}
					{
						xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
					}

					char xor_code = _wtoi(xor_code_string);

					encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer, send_buffer_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_4::CBlockingSocketException_ip_4 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
					}
				}



				local_blocking_socket.Close();

				if (local_bytes_sent <= 0 && local_error_number == 0)
				{
					//			galaxy::MessageBox(CString(L"local_bytes_sent<=0"),CString(L"Ошибка"));
				}

				if (local_main_dialog->get_command_threads_retranslate_audio_stop())
				{
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			//if(local_stream!=NULL)
			//{
			//	local_stream->Release();
			//}

			//ULONGLONG local_end_time = GetTickCount64();

			//ULONGLONG local_work_time = local_end_time - local_start_time;

			//if(local_main_dialog->get_command_threads_retranslate_audio_stop())
			//{
			//	break;
			//}
			//else
			//{
			//	Sleep(DWORD(1000.0/CONST_AUDIO_PACKETS_PER_SECOND));		//	CONST_AUDIO_PACKETS_PER_SECOND кадров в секунду

			//	/*/
			//	if(local_work_time>=1000)
			//		Sleep(1);
			//	else
			//	{
			//		HRESULT hr;

			//		AM_MEDIA_TYPE mt;
			//		ZeroMemory(&mt, sizeof(mt));

			//		if(local_main_dialog->web_camera_dialog!=NULL)
			//		{
			//			CSingleLock lock(&local_main_dialog->delete_web_camera_dialog_critical_section);

			//			lock.Lock();

			//			hr = local_main_dialog->web_camera_dialog->pAudioGrabber->GetConnectedMediaType(&mt);

			//			lock.Unlock();

			//			if (SUCCEEDED(hr))
			//			{
			//				if(mt.formattype==FORMAT_WaveFormatEx)
			//				{
			//					WAVEFORMATEX *wave_format_ex = (WAVEFORMATEX *)mt.pbFormat;

			//					if(wave_format_ex!=NULL)
			//					{
			//						ULONGLONG chunk_playing_time = 
			//							1000 *
			//							wave_format_ex->nAvgBytesPerSec/
			//							wave_format_ex->nSamplesPerSec/
			//							wave_format_ex->nChannels/
			//							(wave_format_ex->wBitsPerSample>>3)/
			//							wave_format_ex->nBlockAlign
			//							;

			//						if(chunk_playing_time>local_work_time)
			//							Sleep(DWORD(chunk_playing_time-local_work_time));
			//						else
			//							Sleep(1);
			//					}
			//				}
			//				_FreeMediaType(mt);	
			//			}
			//			else
			//				Sleep(DWORD(1000.0/CONST_AUDIO_PACKETS_PER_SECOND));		//	CONST_AUDIO_PACKETS_PER_SECOND кадров в секунду
			//		}
			//	}
			//	/*/
			//}
		}

		ULONGLONG local_end_time = GetTickCount64();

		ULONGLONG local_work_time = local_end_time - local_start_time;

		if (local_main_dialog->get_command_threads_retranslate_audio_stop())
		{
			break;
		}
		else
		{
			{
				double local_factor = peers_to_send_count*(port_number_end_const - port_number_start_const + 1);
				if (local_factor == 0.0)
				{
					local_factor = 1.0;
				}
				Sleep(DWORD(1000.0 / CONST_AUDIO_PACKETS_PER_SECOND / local_factor));		//	CONST_AUDIO_PACKETS_PER_SECOND кадров в секунду
			}

			/*/
			if(local_work_time>=1000)
			Sleep(1);
			else
			{
			HRESULT hr;

			AM_MEDIA_TYPE mt;
			ZeroMemory(&mt, sizeof(mt));

			if(local_main_dialog->web_camera_dialog!=NULL)
			{
			CSingleLock lock(&local_main_dialog->delete_web_camera_dialog_critical_section);

			lock.Lock();

			hr = local_main_dialog->web_camera_dialog->pAudioGrabber->GetConnectedMediaType(&mt);

			lock.Unlock();

			if (SUCCEEDED(hr))
			{
			if(mt.formattype==FORMAT_WaveFormatEx)
			{
			WAVEFORMATEX *wave_format_ex = (WAVEFORMATEX *)mt.pbFormat;

			if(wave_format_ex!=NULL)
			{
			ULONGLONG chunk_playing_time =
			1000 *
			wave_format_ex->nAvgBytesPerSec/
			wave_format_ex->nSamplesPerSec/
			wave_format_ex->nChannels/
			(wave_format_ex->wBitsPerSample>>3)/
			wave_format_ex->nBlockAlign
			;

			if(chunk_playing_time>local_work_time)
			Sleep(DWORD(chunk_playing_time-local_work_time));
			else
			Sleep(1);
			}
			}
			_FreeMediaType(mt);
			}
			else
			Sleep(DWORD(1000.0/CONST_AUDIO_PACKETS_PER_SECOND));		//	CONST_AUDIO_PACKETS_PER_SECOND кадров в секунду
			}
			}
			/*/
		}

		if (local_main_dialog->get_command_threads_retranslate_audio_stop())
		{
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete[] send_buffer_this_time;

		delete local_send_audio_thread_parameters_structure;
		return 1;
	}
}

UINT __cdecl datagram_retranslate_audio_connection_thread_ip_6(LPVOID parameter)
{
	DWORD sequence_number = DWORD(rand());

	thread_send_audio_parameters_structure_type *local_send_audio_thread_parameters_structure = (thread_send_audio_parameters_structure_type *)parameter;

	if (local_send_audio_thread_parameters_structure == NULL)
	{
		return 0;
	}

	Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = (local_send_audio_thread_parameters_structure)->parameter_main_dialog;

	if (local_main_dialog == NULL)
	{
		delete local_send_audio_thread_parameters_structure;
		return 0;
	}

	char *send_buffer = new char[CONST_MESSAGE_LENGTH_IMAGE];

	if (send_buffer == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete local_send_audio_thread_parameters_structure;
		return 0;
	}

	char *send_buffer_this_time = new char[CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME];

	if (send_buffer_this_time == NULL)
	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete local_send_audio_thread_parameters_structure;
		return 0;
	}

	for (;;)
	{
		ULONGLONG local_start_time = GetTickCount64();

		if (local_main_dialog->get_command_threads_retranslate_audio_stop())
		{
			break;
		}

		CString local_address(localhost_definition);

		CStringA local_address_internet_address;

		if (local_main_dialog->get_command_terminate_application())
		{
			break;
		}

		int peers_to_send_count = 0;
		{
			DWORD dwWaitResult;

			dwWaitResult = WaitForSingleObject(
				local_main_dialog->do_not_terminate_application_event, // event handle
				0);    // zero wait

			if (dwWaitResult == WAIT_OBJECT_0)
			{
				// Event object was signaled		

				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_audio_thread_parameters_structure;
					return 0;
				}

				for (
					std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
					current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
					current_item_iterator++
					)
				{
					if (current_item_iterator->is_checked > 0)
					{
						peers_to_send_count++;
					}
				}
			}
			else
			{
				if (local_main_dialog->get_command_terminate_application())
				{
					{
						clear_thread_information(local_main_dialog);
					}

					delete[] send_buffer;
					delete local_send_audio_thread_parameters_structure;
					return 0;
				}
			}

		}

		CComPtr<IStream> local_stream;

		if (local_main_dialog->retranslate_microphone_frames_ip_6.size() == 0)
		{
			Sleep(10);
			continue;
		}
		else
		{
			std::list<STREAM_INFORMATION>::iterator local_frames_iterator = local_main_dialog->retranslate_microphone_frames_ip_6.begin();

			local_frames_iterator->stream->Clone(&local_stream);

			local_main_dialog->retranslate_microphone_frames_ip_6.pop_front();
		}

		if (local_stream == NULL)
		{
			Sleep(10);
			continue;
		}

		STATSTG local_istream_statstg;
		HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg, STATFLAG::STATFLAG_DEFAULT);

		double local_data_size_send = 0.0;

		for (int peers_to_send_count_counter = 0; peers_to_send_count_counter < peers_to_send_count; peers_to_send_count_counter++)
		{
			//double local_data_size_send = 0.0;

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

#ifdef use_istream_DEFINITION
			//IStream *local_stream = NULL;
#endif

			CString peer_to_send;

			int loop_counter = 0;
			for (
				std::list<GUI_LIST_CONTROL>::iterator current_item_iterator = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.begin();
				current_item_iterator != local_main_dialog->GUI_CONTROLS_STATE_data.IDC_LIST_NODES_state.end();
				current_item_iterator++
				)
			{
				Sleep(1);
				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
				if (current_item_iterator->is_checked > 0)
				{
					if (peers_to_send_count_counter == loop_counter)
					{
						peer_to_send = current_item_iterator->label;
						break;
					}
					loop_counter++;
				}

				if (local_main_dialog->get_command_terminate_application())
				{
					break;
				}
			}

			if (local_main_dialog->get_command_terminate_application())
			{
				break;
			}

			local_address = peer_to_send;


			//	Здесь делаем снимок экрана

			//CSingleLock local_image_lock(&local_main_dialog->draw_audio_image_critical_section);

			//local_image_lock.Lock();

			if (network::ip_6::domain_name_to_internet_6_name(local_address, local_address_internet_address) == false)
			{
				const int local_error_message_size = 10000;
				wchar_t local_error_message[local_error_message_size];

				const int local_system_error_message_size = local_error_message_size - 1000;
				wchar_t local_system_error_message[local_system_error_message_size];

				wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

				CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

				wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

				continue;
			}
#ifdef use_istream_DEFINITION
			//if(local_main_dialog->web_camera_dialog!=NULL)
			//{
			//	CSingleLock lock(&local_main_dialog->delete_web_camera_dialog_critical_section);

			//	lock.Lock();

			//	local_stream = NULL;

			//	//ULARGE_INTEGER zero_size = {0,0};

			//	//local_stream->SetSize(zero_size);

			//	//LARGE_INTEGER liBeggining = { 0 };

			//	//local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

			//	//CaptureAudioSampleShot(local_main_dialog->web_camera_dialog,local_stream);
			//	CaptureAudioSampleGetFromTheList(local_main_dialog->web_camera_dialog,&local_stream);

			//	if(local_stream==NULL)
			//	{
			//		continue;
			//	}
			//}
#endif
			//local_image_lock.Unlock();

			//if(local_stream==NULL)
			//{
			//	continue;
			//}


			//ULONGLONG local_start_time = GetTickCount64();

			//STATSTG local_istream_statstg;
			//HRESULT local_stat_result = local_stream->Stat(&local_istream_statstg,STATFLAG::STATFLAG_DEFAULT);

			/*/
			//			Тест отображения из IStream
			{
			std::fstream local_file;
			local_file.open("c:\\temp\\file_source.wav",std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

			char *local_file_data = new char[CONST_MESSAGE_LENGTH_IMAGE];

			ULONG local_read_bytes = 0;

			LARGE_INTEGER liBeggining = { 0 };

			local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);

			local_stream->Read(local_file_data,CONST_MESSAGE_LENGTH_IMAGE,&local_read_bytes);

			local_file.write(local_file_data, local_read_bytes);

			delete [] local_file_data;

			local_file.flush();

			local_file.close();
			}
			//*/



			for (UINT local_parameter_port_number = port_number_start_const; local_parameter_port_number <= port_number_end_const; local_parameter_port_number++)
			{
				network::ip_6::CBlockingSocket_ip_6 local_blocking_socket;

				if (network::ip_6::domain_name_to_internet_6_name(local_address, local_address_internet_address) == false)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					wcscpy_s(local_system_error_message, local_system_error_message_size, L"domain_name_to_internet_6_name завершилась неудачей");

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					break;
				}

				network::ip_6::CSockAddr_ip_6 local_socket_address(local_address_internet_address, local_parameter_port_number);

				try
				{
					local_blocking_socket.Create(SOCK_DGRAM);
				}
				catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
				{
					const int local_error_message_size = 10000;
					wchar_t local_error_message[local_error_message_size];

					const int local_system_error_message_size = local_error_message_size - 1000;
					wchar_t local_system_error_message[local_system_error_message_size];

					CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

					local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

					wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

					local_blocking_socket_exception->Delete();

					delete local_send_audio_thread_parameters_structure;

					//			galaxy::MessageBox(local_error_message,CString(L"Ошибка"));

					{
						clear_thread_information(local_main_dialog);
					}

					//if(local_stream!=NULL)
					//{
					//	local_stream->Release();
					//}

					delete[] send_buffer;
					delete[] send_buffer_this_time;

					return 0;
				}

				int local_bytes_sent = 0;
				int local_error_number = 0;


				int send_buffer_data_length = service_signature_definition_length;

				ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH_IMAGE);

				memcpy(send_buffer, service_signature, service_signature_definition_length);



				CString send_data_string = command_audio;

				int send_data_string_length = send_data_string.GetLength();

				memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

				send_buffer_data_length += send_data_string_length * sizeof(wchar_t);

				wchar_t zero_word = L'\0';
				memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
				send_buffer_data_length += sizeof(wchar_t);
				send_data_string_length++;


				memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
				send_buffer_data_length += sizeof(DWORD);
				send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);



				LARGE_INTEGER liBeggining = { 0 };
#ifdef use_istream_DEFINITION
				local_stream->Seek(liBeggining, STREAM_SEEK_SET, NULL);
#endif


				ULONG local_read = 0;
#ifdef use_istream_DEFINITION
				local_stream->Read(send_buffer + send_buffer_data_length, CONST_MESSAGE_LENGTH_IMAGE - send_buffer_data_length, &local_read);
#else
				local_read = 100000;
#endif
				send_buffer_data_length += local_read;




				CString xor_code_string;

				//if(local_main_dialog->get_command_terminate_application())
				//{
				//	break;
				//}
				{
					xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
				}

				char xor_code = _wtoi(xor_code_string);

				encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

				int local_header_length = service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD);

				int local_part_counter = 0;
				int local_parts_count = local_read / (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));
				int local_last_part_size = local_read % (CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));

				if (local_last_part_size != 0)
				{
					local_parts_count++;
				}

				for (; local_part_counter < local_parts_count; local_part_counter++)
				{
					int send_buffer_this_time_data_length = CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME;

					if (local_part_counter == local_parts_count - 1)
					{
						if (local_last_part_size != 0)
						{
							send_buffer_this_time_data_length = local_last_part_size + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD);
						}
					}

					ZeroMemory(send_buffer_this_time, CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME);

					memcpy(send_buffer_this_time, send_buffer, service_signature_definition_length + send_data_string_length * sizeof(wchar_t));

					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[0] = DWORD(local_part_counter + 1);
					((DWORD*)(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t)))[1] = DWORD(local_parts_count);

					encrypt::encrypt_xor(send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t), 2 * sizeof(DWORD), xor_code);

					int local_this_time_data_offset = local_part_counter*(CONST_MESSAGE_LENGTH_IMAGE_EVERY_TIME - service_signature_definition_length - send_data_string_length * sizeof(wchar_t) - 2 * sizeof(DWORD));

					memcpy
					(
						send_buffer_this_time + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD),
						send_buffer + service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + local_this_time_data_offset,
						send_buffer_this_time_data_length - (service_signature_definition_length + send_data_string_length * sizeof(wchar_t) + 2 * sizeof(DWORD))
					);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer_this_time, send_buffer_this_time_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
						break;
					}

					//if(local_main_dialog->get_command_threads_retranslate_audio_stop())
					//{
					//	break;
					//}
					//else
					{
						Sleep(10);
					}

				}

				{
					int local_bytes_sent = 0;
					int local_error_number = 0;

					char send_buffer[CONST_MESSAGE_LENGTH];

					int send_buffer_data_length = service_signature_definition_length;

					ZeroMemory(send_buffer, CONST_MESSAGE_LENGTH);

					memcpy(send_buffer, service_signature, service_signature_definition_length);

					CString send_data_string;

					int send_data_string_length = 0;

					send_data_string = command_audio_end;

					send_data_string_length = send_data_string.GetLength();

					memcpy(send_buffer + send_buffer_data_length, send_data_string.GetBuffer(), send_data_string_length * sizeof(wchar_t));

					send_buffer_data_length += send_data_string_length * sizeof(wchar_t);


					wchar_t zero_word = L'\0';
					memcpy(send_buffer + send_buffer_data_length, &zero_word, sizeof(wchar_t));
					send_buffer_data_length += sizeof(wchar_t);
					send_data_string_length++;


					memcpy(send_buffer + send_buffer_data_length, &sequence_number, sizeof(DWORD));
					send_buffer_data_length += sizeof(DWORD);
					send_data_string_length += sizeof(DWORD) / sizeof(wchar_t);


					CString xor_code_string;

					//if(local_main_dialog->get_command_terminate_application())
					//{
					//	break;
					//}
					{
						xor_code_string = local_main_dialog->GUI_CONTROLS_STATE_data.IDC_EDIT_XOR_CODE_state;
					}

					char xor_code = _wtoi(xor_code_string);

					encrypt::encrypt_xor(send_buffer + service_signature_definition_length, send_buffer_data_length - service_signature_definition_length, xor_code);

					try
					{
						local_bytes_sent = local_blocking_socket.SendDatagram(send_buffer, send_buffer_data_length, local_socket_address, CONST_WAIT_TIME_SEND);

						local_data_size_send += local_bytes_sent;
					}
					catch (network::ip_6::CBlockingSocketException_ip_6 *local_blocking_socket_exception)
					{
						const int local_error_message_size = 10000;
						wchar_t local_error_message[local_error_message_size];

						const int local_system_error_message_size = local_error_message_size;
						wchar_t local_system_error_message[local_system_error_message_size];

						CString local_time_string = CTime::GetCurrentTime().FormatGmt("%d/%m/%y %H:%M:%S GMT");

						local_blocking_socket_exception->GetErrorMessage(local_system_error_message, local_system_error_message_size);

						wsprintf((wchar_t*)local_error_message, L"Сетевая ошибка -- %s -- %s\r\n", local_system_error_message, local_time_string.GetBuffer());

						local_error_number = local_blocking_socket_exception->GetErrorNumber();

						local_blocking_socket_exception->Delete();

						if (local_error_number != 0)
						{
							//				galaxy::MessageBox(local_error_message,CString(L"Ошибка"));
						}
					}
				}



				local_blocking_socket.Close();

				if (local_bytes_sent <= 0 && local_error_number == 0)
				{
					//			galaxy::MessageBox(CString(L"local_bytes_sent<=0"),CString(L"Ошибка"));
				}

				if (local_main_dialog->get_command_threads_web_camera_video_stop())
				{
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			//if(local_stream!=NULL)
			//{
			//	local_stream->Release();
			//}

			//ULONGLONG local_end_time = GetTickCount64();

			//ULONGLONG local_work_time = local_end_time - local_start_time;

			//if(local_main_dialog->get_command_threads_retranslate_audio_stop())
			//{
			//	break;
			//}
			//else
			//{
			//	Sleep(DWORD(1000.0/CONST_AUDIO_PACKETS_PER_SECOND));		//	CONST_AUDIO_PACKETS_PER_SECOND кадров в секунду

			//	/*/
			//	if(local_work_time>=1000)
			//		Sleep(1);
			//	else
			//	{
			//		HRESULT hr;

			//		AM_MEDIA_TYPE mt;
			//		ZeroMemory(&mt, sizeof(mt));

			//		if(local_main_dialog->web_camera_dialog!=NULL)
			//		{
			//			CSingleLock lock(&local_main_dialog->delete_web_camera_dialog_critical_section);

			//			lock.Lock();

			//			hr = local_main_dialog->web_camera_dialog->pAudioGrabber->GetConnectedMediaType(&mt);

			//			lock.Unlock();

			//			if (SUCCEEDED(hr))
			//			{
			//				if(mt.formattype==FORMAT_WaveFormatEx)
			//				{
			//					WAVEFORMATEX *wave_format_ex = (WAVEFORMATEX *)mt.pbFormat;

			//					if(wave_format_ex!=NULL)
			//					{
			//						ULONGLONG chunk_playing_time = 
			//							1000 *
			//							wave_format_ex->nAvgBytesPerSec/
			//							wave_format_ex->nSamplesPerSec/
			//							wave_format_ex->nChannels/
			//							(wave_format_ex->wBitsPerSample>>3)/
			//							wave_format_ex->nBlockAlign
			//							;

			//						if(chunk_playing_time>local_work_time)
			//							Sleep(DWORD(chunk_playing_time-local_work_time));
			//						else
			//							Sleep(1);
			//					}
			//				}
			//				_FreeMediaType(mt);	
			//			}
			//			else
			//				Sleep(DWORD(1000.0/CONST_AUDIO_PACKETS_PER_SECOND));		//	CONST_AUDIO_PACKETS_PER_SECOND кадров в секунду
			//		}
			//	}
			//	/*/
			//}
		}

		ULONGLONG local_end_time = GetTickCount64();

		ULONGLONG local_work_time = local_end_time - local_start_time;

		if (local_main_dialog->get_command_threads_retranslate_audio_stop())
		{
			break;
		}
		else
		{
			{
				double local_factor = peers_to_send_count*(port_number_end_const - port_number_start_const + 1);
				if (local_factor == 0.0)
				{
					local_factor = 1.0;
				}
				Sleep(DWORD(1000.0 / CONST_AUDIO_PACKETS_PER_SECOND / local_factor));		//	CONST_AUDIO_PACKETS_PER_SECOND кадров в секунду
			}

			/*/
			if(local_work_time>=1000)
			Sleep(1);
			else
			{
			HRESULT hr;

			AM_MEDIA_TYPE mt;
			ZeroMemory(&mt, sizeof(mt));

			if(local_main_dialog->web_camera_dialog!=NULL)
			{
			CSingleLock lock(&local_main_dialog->delete_web_camera_dialog_critical_section);

			lock.Lock();

			hr = local_main_dialog->web_camera_dialog->pAudioGrabber->GetConnectedMediaType(&mt);

			lock.Unlock();

			if (SUCCEEDED(hr))
			{
			if(mt.formattype==FORMAT_WaveFormatEx)
			{
			WAVEFORMATEX *wave_format_ex = (WAVEFORMATEX *)mt.pbFormat;

			if(wave_format_ex!=NULL)
			{
			ULONGLONG chunk_playing_time =
			1000 *
			wave_format_ex->nAvgBytesPerSec/
			wave_format_ex->nSamplesPerSec/
			wave_format_ex->nChannels/
			(wave_format_ex->wBitsPerSample>>3)/
			wave_format_ex->nBlockAlign
			;

			if(chunk_playing_time>local_work_time)
			Sleep(DWORD(chunk_playing_time-local_work_time));
			else
			Sleep(1);
			}
			}
			_FreeMediaType(mt);
			}
			else
			Sleep(DWORD(1000.0/CONST_AUDIO_PACKETS_PER_SECOND));		//	CONST_AUDIO_PACKETS_PER_SECOND кадров в секунду
			}
			}
			/*/
		}

		if (local_main_dialog->get_command_threads_retranslate_audio_stop())
		{
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	{
		{
			clear_thread_information(local_main_dialog);
		}

		delete[] send_buffer;
		delete[] send_buffer_this_time;

		delete local_send_audio_thread_parameters_structure;
		return 1;
	}
}



void Cstl_network_ip_4_ip_6_udp_engineDialog::OnBnClickedCheckEnableChatServiceInformation()
{
	CSingleLock lock(&GUI_update_critical_section);
	lock.Lock();

	GUI_CONTROLS_STATE_data.IDC_CHECK_ENABLE_CHAT_SERVICE_INFORMATION_state = button_enable_showing_service_information_in_chat.GetCheck();
}
