#include "stdafx.h"
#include "UPNPSOAPAction.h"

#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;


struct UPNPSOAPAction::Impl
{
	Impl()
		: httpsock(INVALID_SOCKET)
	{
	}

	~Impl()
	{
	}

	int httpsock; // tcp
};


UPNPSOAPAction::UPNPSOAPAction()
	: pimpl( new Impl )
{

}


UPNPSOAPAction::~UPNPSOAPAction()
{

}

