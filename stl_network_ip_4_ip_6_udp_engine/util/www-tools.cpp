
#include "stdafx.h"

#include "www-tools.h"

#include "boost-parser.h"

using namespace std;
using namespace boost;

namespace {

	std::wstring _httpw(L"http://");
	std::string _http("http://");
	std::wstring _80port(L":80");
	std::wstring _8080port( L":8080" );
}

// may be...
bool IsFirstLayer( const std::wstring &url )
{
	size_t pos = url.find(_httpw);
	if( pos == std::wstring::npos )
		return false;
	pos += _httpw.length()-1;
	
	int slanum(0);
	while( true )
	{
		pos = url.find( L"/", pos+1 );
		if( pos == std::wstring::npos )
			break;
		else
			slanum++;
	}
	if( slanum >= 2 ) return false;

	return true;
}

std::wstring FormURL( const std::wstring &base, const std::wstring &rel )
{
	std::wstring res = base;

	size_t first = res.find( L"//" );
	if( first == std::wstring::npos )
	{
		return std::wstring(L"invalid url");
	}
	first += std::wstring(L"//").length();

	size_t second = res.find( L"/", first );
	if( second - first < 5 )
	{
		return std::wstring(L"too short");
	}

	if( second != std::wstring::npos )
	{
		res = res.substr( 0, second );
		res += rel;
	}

	return res;
}


unsigned short ExtractPort( const std::wstring &url )
{
	if( url.find( _httpw ) == wstring::npos ) return 0;

	size_t sla = _httpw.length();
	size_t colon = url.find( L":", sla );
	sla = url.find( L"/", sla );

	wstring port(L"0");
	if( sla != wstring::npos && colon != wstring::npos )
	{
		port = url.substr( colon+1, sla-colon-1 );
	}
	else if( sla == wstring::npos && colon != wstring::npos )
	{
		port = url.substr( colon+1, url.length()-colon-1 );
	}

	unsigned short p(0);
	try{
		p = lexical_cast<unsigned short,wstring>(port);
	}catch( bad_lexical_cast &e )
	{
		e;
		return 0;
	}

	return p;
}


unsigned short ExtractPort( const std::string &url )
{
	if( url.find( _http ) == string::npos ) return 0;

	size_t sla = _http.length();
	size_t colon = url.find( ":", sla );
	sla = url.find( "/", sla );

	string port("0");
	if( sla != string::npos && colon != string::npos )
	{
		port = url.substr( colon+1, sla-colon-1 );
	}
	else if( sla == string::npos && colon != string::npos )
	{
		port = url.substr( colon+1, url.length()-colon-1 );
	}

	unsigned short p(0);
	try{
		p = lexical_cast<unsigned short,string>(port);
	}catch( bad_lexical_cast &e )
	{
		e;
		return 0;
	}

	return p;
}


bool CheckIPFormat( const std::wstring &ip, bool v6 )
{
	wtokenizer parser( ip, separator_comma );

	wtokenizer::iterator it = parser.begin();
	
	char count(0);
	for( ; it != parser.end(); it++ ) count++;
	
	if( v6 )
	{
		if( count != 6 ){
			return false;
		}
	}
	else
	{
		if( count != 4 ){
			return false;
		}
	}
	
	return true;
}


bool CheckIPFormat( const std::string &ip, bool v6 )
{
	atokenizer parser( ip, separator_comma_a );

	atokenizer::iterator it = parser.begin();

	char count(0);
	for( ; it != parser.end(); it++ ) count++;

	if( v6 )
	{
		if( count != 6 ){
			return false;
		}
	}
	else
	{
		if( count != 4 ){
			return false;
		}
	}

	return true;
}

