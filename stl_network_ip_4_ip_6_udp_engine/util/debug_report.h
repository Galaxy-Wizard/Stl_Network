#ifndef __DEBUG_REPORT_H__
#define __DEBUG_REPORT_H__

#ifdef _DEBUG
static boost::mutex debug_mutex;
static std::wofstream global_debug_file(L"debug_info.txt");
#endif

#ifdef _DEBUG
#define BHDebug( expression, message )\
	if( !(expression) )\
{\
	boost::mutex::scoped_lock lk(debug_mutex);\
	global_debug_file\
	<< L"----------------------------------" << std::endl\
	<< L"[FILE] " << __FILEW__ << std::endl\
	<< L"[FUNC] " << __FUNCTIONW__ << std::endl\
	<< L"[LINE] " << __LINE__ << std::endl\
	<< message << std::endl\
	<< L"----------------------------------" << std::endl;\
}

#define dout( os, mess ) os << mess << endl << endl;

#endif

#ifndef _DEBUG
#define BHDebug( expression, message )
#define dout( os, mess )
#endif

#ifdef _DEBUG
#define wconsole( mess )\
{\
	std::wstring __debugfile(__FILE__);\
	__debugfile = __debugfile.substr(\
		__debugfile.rfind(L"\\"), __debugfile.length()-__debugfile.rfind(L"\\"));\
	std::wcout << mess << L" [" << __debugfile << L":" << __LINE__ << L"]" << endl;\
}
#define aconsole( mess )\
{\
	std::string __debugfile(__FILE__);\
	__debugfile = __debugfile.substr(\
	__debugfile.rfind("\\"), __debugfile.length()-__debugfile.rfind("\\"));\
	std::cout << mess << " [" << __debugfile << ":" << __LINE__ << "]" << endl;\
}
#endif

#ifndef _DEBUG
#define wconsole( mess )
#define aconsole( mess )
#endif

#endif