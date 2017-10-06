#include "stdafx.h"
#include "COMManager.h"

COMManager::COMManager()
{
	HRESULT res = ::CoInitialize(NULL);
	if( res != S_OK )
	{
		BHDebug( false, L"CoInitialize failed." );
	}
}


COMManager::~COMManager()
{
	::CoUninitialize();
}