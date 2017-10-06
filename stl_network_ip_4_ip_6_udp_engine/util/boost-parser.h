#pragma once

#include <boost/tokenizer.hpp>

namespace boost
{

	typedef boost::tokenizer<
		boost::char_separator<wchar_t>,
		std::wstring::const_iterator,
		std::wstring
	> wtokenizer;
	static boost::char_separator<wchar_t> separator_equal( L"=" );
	static boost::char_separator<wchar_t> separator_space( L" " );
	static boost::char_separator<wchar_t> separator_wspace( L"Å@" );
	static boost::char_separator<wchar_t> separator_comma( L"," );
	static boost::char_separator<wchar_t> separator_underscore( L"_" );
	static boost::char_separator<wchar_t> separator_period( L"." );
	static boost::char_separator<wchar_t> separator_tilde( L"~" );

	typedef boost::tokenizer<
		boost::char_separator<char>,
		std::string::const_iterator,
		std::string
	> atokenizer;
	static boost::char_separator<char> separator_equal_a( "=" );
	static boost::char_separator<char> separator_space_a( " " );
	static boost::char_separator<char> separator_comma_a( "," );
	static boost::char_separator<char> separator_underscore_a( "_" );
	static boost::char_separator<char> separator_period_a( "." );
	static boost::char_separator<char> separator_tilde_a( "~" );
	static boost::char_separator<char> separator_colon_a( ":" );
	static boost::char_separator<char> separator_linefeed_a( "\n" );
}
