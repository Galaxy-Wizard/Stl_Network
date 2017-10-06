#ifndef __XERCES_HELPER_H__
#define __XERCES_HELPER_H__

#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/framework/Wrapper4InputSource.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>

using namespace xercesc;

typedef std::basic_string<XMLCh> XercesString;

// Converts from a narrow-character string to a wide-character string.
XercesString fromNative(const char* str);

// Converts from a narrow-character string to a wide-charactr string.
XercesString fromNative(const std::string& str);

// Converts from a wide-character string to a narrow-character string.
std::string toNative(const XMLCh* str);

// Converts from a wide-character string to a narrow-character string.
std::string toNative(const XercesString& str);

// Converts from a Xerces String to a std::wstring
std::wstring xercesToWstring(const XercesString& str);

// Converts from a std::wstring to a XercesString
XercesString wstringToXerces(const std::wstring& str);

class XercesInitializer 
{
public:
	XercesInitializer( ) { XMLPlatformUtils::Initialize( ); }
	~XercesInitializer( ) { XMLPlatformUtils::Terminate( ); }
private:
	XercesInitializer(const XercesInitializer&);
	XercesInitializer& operator=(const XercesInitializer&);
};

template<typename T>
class DOMPtr 
{
public:
	DOMPtr(T* t) : t_(t) { }
	~DOMPtr( ) { t_->release( ); }
	T* operator->( ) const { return t_; }
private:
	DOMPtr(const DOMPtr&);
	DOMPtr& operator=(const DOMPtr&);
	T* t_;
};

class CustomErrorHandler : public DOMErrorHandler 
{
public:
	bool handleError(const DOMError& e)
	{
		return false;
	}
};


#ifndef MEMPARSE_ENCODING
#if defined(OS390)
#define MEMPARSE_ENCODING "ibm-1047-s390"
#elif defined(OS400)
#define MEMPARSE_ENCODING "ibm037"
#else
#define MEMPARSE_ENCODING "ascii"
#endif
#endif /* ifndef MEMPARSE_ENCODING */

#endif