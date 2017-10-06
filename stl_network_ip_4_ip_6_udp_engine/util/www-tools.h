#pragma once

std::wstring FormURL( const std::wstring &base, const std::wstring &rel );

unsigned short ExtractPort( const std::wstring &url );
unsigned short ExtractPort( const std::string &url );

bool CheckIPFormat( const std::wstring &ip, bool v6 = false );
bool CheckIPFormat( const std::string &ip, bool v6 = false );