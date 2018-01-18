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

	BOOL CLogFile::Create(CString FileName)
	{
		LogFileName = FileName;
		CFile file(LogFileName, CFile::modeCreate | CFile::modeWrite);
		unsigned char local_signature[] = { 0xFF, 0xFE };
		file.Write(local_signature, 2);
		file.Close();
		return TRUE;
	}

	CLogFile::CLogFile()
	{
	}

	CLogFile::~CLogFile()
	{
	}

	BOOL CLogFile::Write(CString StringToWrite, bool enable_write)
	{
		if (enable_write)
		{
			CSingleLock Lock(&LogFileCriticalSection);
			Lock.Lock();
			try
			{
				CFile file(LogFileName, CFile::modeNoTruncate | CFile::modeWrite);
				file.SeekToEnd();
				StringToWrite = StringToWrite + CString(L"\r\n");
				file.Write(StringToWrite.GetBufferSetLength(
					StringToWrite.GetLength() * sizeof(WCHAR)), StringToWrite.GetLength() * sizeof(WCHAR));
				file.Close();
			}
			catch (CFileException* e)
			{
				e->Delete();
				return FALSE;
			}
		}
		return TRUE;
	}
	BOOL CLogFile::Write(BYTE *DataToWrite, UINT DataToWriteCount, bool enable_write)
	{
		if (enable_write)
		{
			try
			{
				CSingleLock Lock(&LogFileCriticalSection);
				Lock.Lock();
				CFile file(LogFileName, CFile::modeNoTruncate | CFile::modeWrite);
				file.SeekToEnd();
				file.Write(DataToWrite, DataToWriteCount);
				file.Close();
			}
			catch (CFileException* e)
			{
				e->Delete();
				return FALSE;
			}
		}
		return TRUE;
	}
}