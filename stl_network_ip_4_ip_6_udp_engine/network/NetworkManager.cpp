#include "stdafx.h"
#include "NetworkManager.h"


struct NetworkManager::Impl
{
	Impl()
	{
	}

	~Impl()
	{
	}

	WSAData wsadata;
};


NetworkManager::NetworkManager()
: pimpl( new Impl )
{
	/*
	WSASYSNOTREADY 	ネットワークサブシステムが
	ネットワークへの接続準備ができていない
	WSAVERNOTSUPPORTED 	要求した WinSock のバージョンは
	このシステムで提供されていない
	WSAEINPROGRESS 	ブロッキング操作の実行中である
	またはサービスプロバイダがコールバック関数を処理している
	WSAEPROCLIM 	WinSock が同時に処理できる最大プロセスに達成した
	WSAEFAULT 	lpWSAData は有効なポインタではない 
	*/
	int res = WSAStartup( MAKEWORD(2,0), &pimpl->wsadata );
	switch( res )
	{
	case WSASYSNOTREADY:
		break;
	case WSAVERNOTSUPPORTED:
		break;
	case WSAEINPROGRESS:
		break;
	case WSAEPROCLIM:
		break;
	case WSAEFAULT:
		break;
	}
}


NetworkManager::~NetworkManager()
{
	int res = WSACleanup();
	BHDebug( 0 != res, L"WSACleanup error.\n" );
}

