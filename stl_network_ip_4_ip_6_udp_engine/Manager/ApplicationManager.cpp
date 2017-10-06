#include "stdafx.h"
#include "ApplicationManager.h"

#include "../util/xerces-helper.h"

struct ApplicationManager::Impl
{
	Impl()
	{
	}

	~Impl()
	{
	}


	XercesInitializer init;
};


ApplicationManager::ApplicationManager()
: pimpl( new Impl )
{
	HRESULT res = ::CoInitialize(NULL);
	BHDebug( S_OK == res, L"COM initialization failed.\n" );

}


ApplicationManager::~ApplicationManager()
{
	::CoUninitialize();

#ifdef _CONSOLE_APP
	getchar();
#endif
}

