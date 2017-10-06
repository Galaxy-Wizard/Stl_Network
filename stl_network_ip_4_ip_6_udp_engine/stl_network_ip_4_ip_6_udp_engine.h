
// stl_network_ip_4_ip_6_udp_engine.h : главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// Cstl_network_ip_4_ip_6_udp_engineApp:
// О реализации данного класса см. stl_network_ip_4_ip_6_udp_engine.cpp
//

class Cstl_network_ip_4_ip_6_udp_engineApp : public CWinApp
{
public:
	Cstl_network_ip_4_ip_6_udp_engineApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern Cstl_network_ip_4_ip_6_udp_engineApp theApp;