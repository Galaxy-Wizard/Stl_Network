#include "stdafx.h"
#include "xerces-helper.h"


XercesString fromNative(const char* str)
{
	boost::scoped_array<XMLCh> ptr(xercesc::XMLString::transcode(str));
	return XercesString(ptr.get( ));
}

// Converts from a narrow-character string to a wide-charactr string.
XercesString fromNative(const std::string& str)
{
	return fromNative(str.c_str( ));
}

// Converts from a wide-character string to a narrow-character string.
std::string toNative(const XMLCh* str)
{
	boost::scoped_array<char> ptr(xercesc::XMLString::transcode(str));
	return std::string(ptr.get( ));
}

// Converts from a wide-character string to a narrow-character string.
std::string toNative(const XercesString& str)
{
	return toNative(str.c_str( ));
}

// Converts from a Xerces String to a std::wstring
std::wstring xercesToWstring(const XercesString& str)
{
	return std::wstring(str.begin( ), str.end( ));
}

// Converts from a std::wstring to a XercesString
XercesString wstringToXerces(const std::wstring& str)
{
	return XercesString(str.begin( ), str.end( ));
}

