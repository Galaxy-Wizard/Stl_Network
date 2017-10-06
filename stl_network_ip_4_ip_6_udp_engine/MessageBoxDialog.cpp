// MessageBoxDialog.cpp: файл реализации
//

#include "stdafx.h"
#include "stl_network_ip_4_ip_6_udp_engine.h"
#include "MessageBoxDialog.h"
#include "afxdialogex.h"


// диалоговое окно CMessageBoxDialog

IMPLEMENT_DYNAMIC(CMessageBoxDialog, CDialogEx)

CMessageBoxDialog::CMessageBoxDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMessageBoxDialog::IDD, pParent)
{

	EnableAutomation();

}

CMessageBoxDialog::~CMessageBoxDialog()
{
}

void CMessageBoxDialog::OnFinalRelease()
{
	// Когда будет освобождена последняя ссылка на объект автоматизации,
	// вызывается OnFinalRelease. Базовый класс автоматически
	// удалит объект. Перед вызовом базового класса добавьте
	// дополнительную очистку, необходимую вашему объекту.

	CDialogEx::OnFinalRelease();
}

void CMessageBoxDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MESSAGE, message_text);
}


BEGIN_MESSAGE_MAP(CMessageBoxDialog, CDialogEx)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CMessageBoxDialog, CDialogEx)
END_DISPATCH_MAP()

// Примечание: мы добавили поддержку для IID_IMessageBoxDialog, чтобы обеспечить безопасную с точки зрения типов привязку
//  из VBA. Этот IID должен соответствовать GUID, связанному с 
//  disp-интерфейсом в файле .IDL.

// {AEE04BD7-E7D1-48DB-907F-16F11B1C3486}
static const IID IID_IMessageBoxDialog =
{ 0xAEE04BD7, 0xE7D1, 0x48DB, { 0x90, 0x7F, 0x16, 0xF1, 0x1B, 0x1C, 0x34, 0x86 } };

BEGIN_INTERFACE_MAP(CMessageBoxDialog, CDialogEx)
	INTERFACE_PART(CMessageBoxDialog, IID_IMessageBoxDialog, Dispatch)
END_INTERFACE_MAP()


// обработчики сообщений CMessageBoxDialog


void CMessageBoxDialog::SetParameters(LPCWSTR lpszText, LPCWSTR lpszCaption)
{
	member_caption = lpszCaption;
	member_text = lpszText;
}


BOOL CMessageBoxDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowTextW(member_caption);
	message_text.SetWindowTextW(member_text);

	// TODO:  Добавить дополнительную инициализацию

	return TRUE;  // return TRUE unless you set the focus to a control
	// Исключение: страница свойств OCX должна возвращать значение FALSE
}
