#pragma once

namespace galaxy
{
	INT MessageBox(LPCWSTR lpszText, LPCWSTR lpszCaption);

	class CLogFile
	{
	public:
		BOOL Create(CString FileName);
		CString LogFileName;
		BOOL Write(CString  StringToWrite, bool enable_write);
		BOOL Write(BYTE *DataToWrite, UINT DataToWriteCount, bool enable_write);
		CLogFile();
		virtual ~CLogFile();
	protected:
		CCriticalSection LogFileCriticalSection;
	};
}