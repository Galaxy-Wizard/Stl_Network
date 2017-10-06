// ReceivedVideoDialog.cpp: файл реализации
//

#include "stdafx.h"
#include "stl_network_ip_4_ip_6_udp_engine.h"
#include "ReceivedVideoDialog.h"
#include "afxdialogex.h"

#include "stl_network_ip_4_ip_6_udp_engineDialog.h"

// диалоговое окно CReceivedVideoDialog

IMPLEMENT_DYNAMIC(CReceivedVideoDialog, CDialogEx)

	CReceivedVideoDialog::CReceivedVideoDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CReceivedVideoDialog::IDD, pParent)
{
	main_dialog = NULL;
}

CReceivedVideoDialog::~CReceivedVideoDialog()
{
}

void CReceivedVideoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_IMAGE, static_image);
}


BEGIN_MESSAGE_MAP(CReceivedVideoDialog, CDialogEx)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// обработчики сообщений CReceivedVideoDialog


BOOL CReceivedVideoDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: добавьте дополнительную инициализацию

	CRect rcClient(0, 0, ENGINE_WINDOWS_SIZE_CX, ENGINE_WINDOWS_SIZE_CY);

	if(
		SetWindowPos(NULL, 0, 0, rcClient.Width(), rcClient.Height(), SWP_NOZORDER | SWP_NOREDRAW)
		==
		TRUE
		)
	{
		CString dumme_string("success");
		RedrawWindow();
	}

	return TRUE;
}

void CReceivedVideoDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: добавьте свой код обработчика сообщений
	if(static_image.m_hWnd!=NULL)
	{
		CRect rcClient;
		GetClientRect (&rcClient);

		if(
			static_image.SetWindowPos(NULL, 0, 0, rcClient.Width(), rcClient.Height(), SWP_NOZORDER | SWP_NOREDRAW)
			==
			TRUE
			)
		{
			CString dumme_string("success");
			static_image.RedrawWindow();
		}
	}
}


void CReceivedVideoDialog::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnSizing(fwSide, pRect);

	// TODO: добавьте свой код обработчика сообщений
	if(static_image.m_hWnd!=NULL)
	{
		CRect rcClient;
		GetClientRect (&rcClient);

		if(
			static_image.SetWindowPos(NULL, 0, 0, rcClient.Width(), rcClient.Height(), SWP_NOZORDER | SWP_NOREDRAW)
			==
			TRUE
			)
		{
			CString dumme_string("success");
			static_image.RedrawWindow();
		}
	}
}


void CReceivedVideoDialog::OnClose()
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного
	{
		Cstl_network_ip_4_ip_6_udp_engineDialog *local_main_dialog = main_dialog;

		if(local_main_dialog!=NULL)
		{
			if(local_main_dialog->received_video_dialog!=NULL)
			{
				CSingleLock lock(&local_main_dialog->delete_received_video_dialog_critical_section);

				lock.Lock();

				local_main_dialog->received_video_dialog = NULL;
			}
		}
	}

	CDialogEx::OnClose();

	delete this;
}
