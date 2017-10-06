#include "stdafx.h"

#include "galaxy_namespace.h"

#include "MessageBoxDialog.h"

namespace galaxy
{
	INT MessageBox(LPCWSTR lpszText, LPCWSTR lpszCaption)
	{
		CMessageBoxDialog local_message_box_dialog;

		local_message_box_dialog.SetParameters(lpszText, lpszCaption);

		return local_message_box_dialog.DoModal();
	}
}