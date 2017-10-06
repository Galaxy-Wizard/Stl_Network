#include "stdafx.h"

#include "UPNPNATServiceTypes.h"
#include "IGDDiscoverProcess.h"
#include "UPNPNATHTTPClient.h"

#include "../XMLParser/DOMParser.h"

#include "../util/boost-parser.h"
#include "../util/boost-string-conversion.h"
#include "../util/www-tools.h"

#include "libs/thread/src/pthread/timeconv.inl"

using namespace std;

namespace 
{
	// adhoc
	bool GetOwnIPAddress(
		deque<in_addr> &result
		)
	{
		char ac[512];
		if(
			gethostname( ac, sizeof(ac) ) == SOCKET_ERROR 
			)
		{
			BHDebug( 
				false,
				boost::str(boost::format( 
					"WSAError : %d when getting local host name." )%WSAGetLastError() ).c_str() );

			cerr 
				<< "Error " << WSAGetLastError() 
				<< "when getting local host name." << endl;
			return false;
		}

		hostent *he = 
			gethostbyname( ac );

		if( 0 == he ) 
		{
			cerr << "Yow! Bad host lookup." << endl;
			return false;
		}

		for( int i = 0; he->h_addr_list[i] != 0; ++i ) 
		{
			struct in_addr addr;
			memcpy(
				&addr, he->h_addr_list[i], sizeof(struct in_addr)
				);
			result.push_back( addr );
		}
		return true;
	}

	//ofstream service_desc( "service_desc.xml" );
}
// endof null-namespace


const std::string IGDDiscoverProcess::UPNP_MULTICAST_ADDRESS( "239.255.255.250" );
const u_short IGDDiscoverProcess::UPNP_PORT = 1900;
const u_long IGDDiscoverProcess::BUFFER_SIZE = 2048;

const std::string IGDDiscoverProcess::MSEARCH_REQUEST( 
"M-SEARCH * HTTP/1.1\r\nMX: 3\r\nHOST: 239.255.255.250:1900\r\nMAN: \"ssdp:discover\"\r\nST: " );

struct IGDDiscoverProcess::Impl
{

	Impl()
		: sock( INVALID_SOCKET )

		, find_layer3forwarding_service(true)
		, find_wanpppconnection_service(true)
		, find_wanipconnection_service(true)
		, find_wancommoninterface_config_service(true)
	{
		msearch_timeout.tv_sec = 0;
		msearch_timeout.tv_usec = 50000; // 50[microsec]
	}


	Impl( long mseach_recv_timeout )
		: sock( INVALID_SOCKET )

		, find_layer3forwarding_service(true)
		, find_wanpppconnection_service(true)
		, find_wanipconnection_service(true)
		, find_wancommoninterface_config_service(true)
	{
		long sec = mseach_recv_timeout / 1000000;
		long usec = 
			sec != 0 ? usec = mseach_recv_timeout % (sec*1000000) : mseach_recv_timeout;
		msearch_timeout.tv_sec = sec;
		msearch_timeout.tv_usec = usec; // 50[microsec]
	}


	~Impl()
	{
	}

	struct Manager
	{
		Manager( IGDDiscoverProcess *c ) : client(c)
		{
			if( Succeeded == client->InitializeUDPConnection() )
			{
				aconsole( "UDP initalized." );
				return;
			}
			aconsole( "UDP initalization error." );
		}
		~Manager()
		{
			if( Succeeded == client->UninitializeUDPConnection() )
			{
				aconsole( "UDP uninitialized." );
				return;
			}
			aconsole( "UDP uninitialization error." );
		}

		IGDDiscoverProcess *client;
	};


	// not be used in this sample
	typedef boost::mutex::scoped_lock _lock;
	boost::mutex _mutex;

	// network stuffs
	int sock; //udp
	Addresses ips;
	AddressStrings raw_nic_ips;;

	IGDControlInformations igd_infos;
	ControlInfosPerServiceType cinfos_per_type;

	bool find_layer3forwarding_service;
	bool find_wanpppconnection_service;
	bool find_wanipconnection_service;
	bool find_wancommoninterface_config_service;

	timeval msearch_timeout;

	// statistics
	deque<string> msearch_failed_igds;
};


IGDDiscoverProcess::IGDDiscoverProcess()
	: pimpl( new Impl() )
{
	if( !EnumelateIP() )
	{
		return;
	}
}


IGDDiscoverProcess::IGDDiscoverProcess( 
	long msearch_recv_timeout // default 50[microsec]	
	)
	: pimpl( new Impl(msearch_recv_timeout) )
{
	if(  msearch_recv_timeout < 0 ) 
		throw exception("invalid arg");

	if( !EnumelateIP() )
	{
		return;
	}
}


IGDDiscoverProcess::~IGDDiscoverProcess()
{

}


IGDDiscoverProcess::IGDDiscoverResult IGDDiscoverProcess::InitializeUDPConnection( void )
{
	int error(0);

	pimpl->sock = socket( 
		AF_INET, SOCK_DGRAM, 0
		);
	if( INVALID_SOCKET == pimpl->sock )
	{
		BHDebug( 
			false
			, L"socket had returned a invalid socket." );
		return InvalidSocket;
	}

	sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	u_short port = 20000;
	while( true )
	{
		addr.sin_port = htons( port );
		if( 
			0 == ::bind( 
			pimpl->sock, (sockaddr*)&addr, sizeof(addr) ) 
			)
		{
			break;
		}

		error = WSAGetLastError();
		if( WSAEADDRINUSE == error )
		{
			port++;
		}
		else if( 0 != error )
		{
			closesocket( pimpl->sock );
			pimpl->sock = INVALID_SOCKET;
			return Failed;
		}
	}

	// ttl/non-blocking setting for m-search
	unsigned int ttl = 4;
	unsigned long flags = 1;
	error = setsockopt(
		pimpl->sock
		, IPPROTO_IP, IP_MULTICAST_TTL
		, (char*)(&ttl), sizeof(ttl)
		);
	BHDebug( 0 == error, L"setsockopt error." );
	wchar_t local_buffer[20];
	_itow_s(WSAGetLastError(),local_buffer,20,10);
	BHDebug( 0 == error, local_buffer);
	//if( 0 != error ) return Failed;

	error = ioctlsocket(
		pimpl->sock, FIONBIO, &flags
		);
	BHDebug( 0 == error, L"ioctlsocket error.\n" );
	//if( 0 != error ) return Failed;

	/*pimpl->event_handle = WSACreateEvent();
	BHDebug( 
		WSA_INVALID_EVENT != pimpl->event_handle, L"WSA_INVALID_EVENT" );*/

	return Succeeded;
}


IGDDiscoverProcess::IGDDiscoverResult IGDDiscoverProcess::UninitializeUDPConnection( void )
{
	if( INVALID_SOCKET != pimpl->sock )
	{
		int res = 
			shutdown( pimpl->sock, SD_BOTH );
		if( 0 != res ) return Failed;
		res = closesocket( pimpl->sock );
		if( 0 != res ) return Failed;
	}
	return Succeeded;
}


bool IGDDiscoverProcess::EnumelateIP( void )
{
	pimpl->ips.clear();
	GetOwnIPAddress( pimpl->ips );

	if( pimpl->ips.empty() )
	{
		return false;
	}

	boost_foreach( const in_addr &a, pimpl->ips )
	{
		string tmp( inet_ntoa( a ) );
		if( CheckIPFormat( tmp ) )
		{
			pimpl->raw_nic_ips.push_back(tmp);
		}
	}

	return true;
}


bool IGDDiscoverProcess::EnumerateIGD( void )
{
	return true;
}


IGDDiscoverProcess::IGDDiscoverResult IGDDiscoverProcess::Send( 
	const string &data
	, const in_addr &nic
	)
{
	int error(0);
	int data_size = static_cast<int>( data.length() );

	sockaddr_in upnp_group;
	memset( &upnp_group, 0, sizeof(upnp_group) );
	upnp_group.sin_family = AF_INET;
	upnp_group.sin_addr.s_addr = inet_addr( UPNP_MULTICAST_ADDRESS.c_str() );
	upnp_group.sin_port = htons( UPNP_PORT );

	{
		in_addr from = nic;
		if( 
			setsockopt( 
				pimpl->sock, IPPROTO_IP, IP_MULTICAST_IF, 
				(char*)&from, sizeof(from)
				) == 0
				)
		{
			timeval timeout;
			timeout.tv_sec = 1;
			int total(0);
			while( true )
			{
				fd_set writable;
				FD_ZERO( &writable );
				FD_SET( pimpl->sock, &writable );
				error = select( 0, NULL, &writable, NULL, &timeout );
				if( SOCKET_ERROR != error && 0 < error )
				{
					if( FD_ISSET( pimpl->sock, &writable ) )
					{
						int sended = sendto(
							pimpl->sock, data.data(), static_cast<int>(data.length())
							, 0
							, (sockaddr*)&upnp_group, static_cast<int>(sizeof(upnp_group)) 
							);
						if( SOCKET_ERROR == sended )
						{
							BHDebug( false, L"socket error" );
							//i++;
							return SocketError;
						}
						total += sended;
						if( total >= data_size )
						{
							aconsole(boost::str(
								boost::format("MSEARCH has sended from %s.")%inet_ntoa(from)));
							break;
						}
					}
				}
				else if( 0 == error )
				{
					return Timeout;
				}
			}//while
		}
		else
		{
			BHDebug( false, L"winsock error" );
			return SocketError;
		}
	}

	return Succeeded;
}


IGDDiscoverProcess::IGDDiscoverResult IGDDiscoverProcess::Receive(
	string &igdip, string &data
	)
{
	int res(0);

	fd_set read_fds;
	FD_ZERO( &read_fds );

	FD_SET( pimpl->sock, &read_fds );

	res = select( pimpl->sock+1, &read_fds, NULL, NULL, &pimpl->msearch_timeout );
	if( 0 == res )
	{
		// timeouted
		return Timeout;
	}

	if( FD_ISSET( pimpl->sock, &read_fds ) )
	{
		char buff[BUFFER_SIZE] = {0};
		sockaddr_in from = {0};
		int from_len = static_cast<int>(sizeof(from));
		int received = 
			recvfrom( pimpl->sock, buff, sizeof(buff), 0, (sockaddr*)&from, &from_len );
		if( SOCKET_ERROR == received )
		{
			BHDebug( false, L"recv error." );
			return SocketError;
		}
		if( received < BUFFER_SIZE-2 )
		{
			igdip.assign( inet_ntoa( from.sin_addr ) );
			buff[received] = '\0';
			data.assign( buff );
		}
		else
		{
			// too long data
			return TooLongData;
		}

		return Succeeded;
	}

	return Failed;
}


IGDDiscoverProcess::IGDDiscoverResult IGDDiscoverProcess::ExtractControlInfo( 
	const string &igd_responce, ControlInfo &cinfo, int &httpres
	)
{
	/* sample responce
	HTTP/1.1 200 OK
	ST:urn:schemas-upnp-org:service:WANIPConnection:1
	USN:uuid:00A0B0D7-98AD-0000-0000-0002C0A80001::urn:schemas-upnp-org:service:WANIPConnection:1
	Location:http://192.168.0.1:80/desc.xml
	Cache-Control:max-age=1800
	Server:IGD-HTTP/1.1 UPnP/1.0 UPnP-Device-Host/1.0
	Ext:
	*/

	deque<string> lines;
	boost::atokenizer parser( igd_responce, boost::separator_linefeed_a );
	for( boost::atokenizer::iterator it = parser.begin(); it != parser.end(); it++ )
	{
		lines.push_back( *it );
	}

	for( deque<string>::iterator it = lines.begin(); it != lines.end(); it++ )
	{
		char head = (*it)[0];
		if( 'H' == head ) // http responce line
		{
			boost::atokenizer responce_parser( *it, boost::separator_space_a );
			boost::atokenizer::iterator it1 = responce_parser.begin();
			it1++;
			if( *it1 != string("200") )
			{
				// HTTP error
				int res = boost::lexical_cast<int,string>(*it1);
				httpres = res;

				aconsole( "HTTPError" );
				return HTTPNG;
			}
		}
		if( 'S' == head && (*it).size() >= 2 ) // ST(service type) line
		{
			if( (*it)[1] == 'T' )
			{
				size_t found = (*it).find( ":" );
				int minus_2 = 0;
				{
					size_t found_1 = (*it).find( ": " );
					if(found_1 != string::npos)
					{
						found = found_1+1;
					}
					size_t found_2 = (*it).find( "\r" );
					if(found_2 != string::npos)
					{
						minus_2 = -1;
						
						if(found_1 == string::npos)
						{
							minus_2 = 0;
						}
					}
				}
				cinfo.service_type = (*it).substr( found+1, (*it).length()-4 + minus_2); // "-4" to remove linefeeds
			}
		}
		if( 'L' == head ) // Location info line
		{
			size_t found = (*it).find( ":" );
			int minus_2 = 0;
			{
				size_t found_1 = (*it).find( ": " );
				if(found_1 != string::npos)
				{
					found = found_1+1;
				}
				size_t found_2 = (*it).find( "\r" );
				if(found_2 != string::npos)
				{
					minus_2 = -1;

					if(found_1 == string::npos)
					{
						minus_2 = 0;
					}
				}
			}
			cinfo.service_desc_url = (*it).substr( found+1, (*it).length() + minus_2);
			cinfo.service_desc_url = cinfo.service_desc_url.substr( 0, cinfo.service_desc_url.length()-1 );
		}
	}

	// extracting the ip and the port
	string ds( "//" );
	size_t double_slash = cinfo.service_desc_url.find( "//" );
	double_slash += ds.length();
	size_t base_end = cinfo.service_desc_url.find( "/", double_slash );
	string ip_port = cinfo.service_desc_url.substr( double_slash, base_end-double_slash );
	
	boost::atokenizer ipparser( ip_port, boost::separator_colon_a );
	boost::atokenizer::iterator it = ipparser.begin();
	size_t sz(0);
	for( it = ipparser.begin(); it != ipparser.end(); it++ ) sz++;
	it = ipparser.begin();
	if( sz == 2 )
	{
		cinfo.ip = *it++;
		cinfo.port = boost::lexical_cast<unsigned short,string>(*it);
	}
	else
	{
		aconsole( "BadIGDResponce" );
		return BadIGDResponce;
	}

	return Succeeded;
}


void IGDDiscoverProcess::GetServiceDescriptions( void )
{
	vector<ControlInfo> done;
	for( 
		IGDControlInformations::iterator i = pimpl->igd_infos.begin(); 
		i != pimpl->igd_infos.end(); i++ )
	{
		ControlInfo &cinfo = (*i);

		///checking same igd///////////////////////////////////////////////////////
		bool dont_request(false);
		boost_foreach( const ControlInfo &igd, done )
		{
			if( igd.ip == cinfo.ip )
			{
				// same igd has already found
				cinfo.service_description = igd.service_description;
				dont_request = true;
			}
		}
		if( dont_request ) continue;
		//////////////////////////////////////////////////////////////////////////
		

		UPNPNATHTTPClient request( cinfo.ip, cinfo.port );

		aconsole( boost::str(
			boost::format("grabbing %s HOST: %s:%u\n")%cinfo.service_desc_url%cinfo.ip%cinfo.port ));

		string desc_url = (*i).service_desc_url;
		desc_url = desc_url.substr( desc_url.rfind( "/" ), desc_url.length() );
		string desc;
		UPNPNATHTTPClient::SoapResult res = 
			request.GetUPNPServiceDescription( desc, desc_url );
		if( res == UPNPNATHTTPClient::SoapSucceeded )
		{
			cinfo.service_description = desc;
		}
		else
		{
			switch( res ){
				case UPNPNATHTTPClient::SoapFailed:
					aconsole("SoapFailed");
					break;
				case UPNPNATHTTPClient::SoapSucceeded:
					aconsole("SoapSucceeded");
					break;
				case UPNPNATHTTPClient::SoapNoConnection:
					aconsole("SoapNoConnection");
					break;
				case UPNPNATHTTPClient::SoapParseError:
					aconsole("SoapParseError");
					break;
				case UPNPNATHTTPClient::SoapIllegalResponce:
					aconsole("SoapIllegalResponce");
					break;
			}
			cinfo.has_problem = true;
		}

		if( !cinfo.has_problem )
		{
			done.push_back( cinfo );
#if 1
//			service_desc << cinfo.service_description;
#endif
		}

	}

}


void IGDDiscoverProcess::ExtractControlURL( void )
{
	for( 
		IGDControlInformations::iterator i = pimpl->igd_infos.begin(); 
		i != pimpl->igd_infos.end(); i++ )
	{
		ControlInfo &cinfo = (*i);

		if( cinfo.service_description.find("<?xml") == string::npos )
		{
			continue;
		}

		DOMParser parser( cinfo.service_description, cinfo.service_desc_url );

		wstring _control_url = parser.UPNPGetControlURL(cinfo.service_type);
		if(_control_url.empty())
		{
			continue;
		}
		assert( !_control_url.empty() );

		wstring tmp;
		try
		{
			if( _control_url.find( L"http://" ) == wstring::npos )
			{
				cinfo.control_url = boost::lexical_cast<string,wstring>(_control_url);
			}
			else
			{
				cinfo.control_url = boost::lexical_cast<string,wstring>(_control_url);
				
				cinfo.port = ExtractPort( cinfo.control_url );

				size_t pos = cinfo.control_url.find("http://");
				pos += string("http://").length();
				size_t sub = cinfo.control_url.find( "/", pos );
				if( sub != string::npos )
				{
					cinfo.control_url = cinfo.control_url.substr( sub, cinfo.control_url.length()-sub );
				}
				else
				{
					BHDebug( false, L"maybe there is an invalid controlURL on the service description." );
					continue;
				}
				cinfo.control_url = cinfo.control_url;
			}
			
		}
		catch( boost::bad_lexical_cast &e )
		{
			e.what();
			BHDebug( false, e.what() );
			continue;
		}
	}

#ifdef _DEBUG
	boost_foreach( ControlInfo &info, pimpl->igd_infos )
	{
		aconsole(boost::str(boost::format(
			"\tIGD:%s\n\tServiceDescURL:%s\n\tServiceType:%s"
			) % info.ip.c_str()%info.service_desc_url.c_str()%info.service_type.c_str() 
			) 
			);
	}
#endif
}


bool IGDDiscoverProcess::ExistedControlInfomation( const ControlInfo &target )
{
	for( 
		IGDControlInformations::iterator it = pimpl->igd_infos.begin(); 
		it != pimpl->igd_infos.end(); it++ )
	{
		ControlInfo &info = (*it);
		if( info == target )
			return true; // exists
	}
	return false;
}


IGDDiscoverProcess::IGDDiscoverResult IGDDiscoverProcess::MSearch( 
	const std::string &service_type 
	)
{
	Impl::Manager connection( this ); // RAII object

	string data( MSEARCH_REQUEST );
	data += service_type;
	data += "\r\n\r\n";

	IGDDiscoverResult res;

	// sending m-search to all nics that have found.
	boost_foreach( const in_addr &to, pimpl->ips )
	{
		res = Send( data, to );
		if( Succeeded  != res )
		{
			res;
			aconsole("Succeeded  != res");
		}
	}

	// receiving and extracting upnp-igd control informations 
	// from responces
	string igdip;
	string received;
	while( Succeeded == (res = Receive( igdip, received)) )
	{
		ControlInfo cinfo;
		int httpres(0);
		res = ExtractControlInfo( received, cinfo, httpres );
		
		if( Succeeded  != res )
		{
			aconsole(boost::str(boost::format(
				"failed to ExtractControlInfo on [NIC:...]" )));
			continue;
		}
		else
		{
			if( !ExistedControlInfomation( cinfo ) )
			{
				pimpl->igd_infos.push_back( cinfo );
			}
		}
	}

	return Failed;
}


//////////////////////////////////////////////////////////////////////////
//MAIN ROUTINE for DISCOVERING PROCESS
// This function assuming will be used by the boost::thread
//////////////////////////////////////////////////////////////////////////
void IGDDiscoverProcess::operator()(void)
{
	static int count(0);
	boost::xtime xt;
	while( true )
	{
		if( pimpl->find_layer3forwarding_service )
		{
			MSearch( UPNPSERVICE_LAYER3FORWARDING1 );
			pimpl->find_layer3forwarding_service = false;
		}

		if( pimpl->find_wanpppconnection_service )
		{
			MSearch( UPNPSERVICE_WANPPPCONNECTION1 );
			pimpl->find_wanpppconnection_service = false;
		}

		if( pimpl->find_wanipconnection_service )
		{
			MSearch( UPNPSERVICE_WANIPCONNECTION1 );
			pimpl->find_wanipconnection_service = false;
		}

		if( pimpl->find_wancommoninterface_config_service )
		{
			MSearch( UPNPSERVICE_WANCOMMONINTERFACECONFIG1 );
			pimpl->find_wancommoninterface_config_service = false;
		}

		GetServiceDescriptions();
		ExtractControlURL();

		vector<ControlInfo> infos;
		for( IGDControlInformations::iterator it = pimpl->igd_infos.begin();
			it != pimpl->igd_infos.end(); it++ )
		{
			infos.push_back( *it );
		}

		break; // out from this thread

		to_time( 10, xt );

		boost::thread::sleep( xt );
	}

	aconsole( "FINISHED DISCOVERING" );
}


IGDDiscoverProcess::IGDControlInformations IGDDiscoverProcess::GetAllIGDControlInformations( void ) const
{
	return pimpl->igd_infos;
}


