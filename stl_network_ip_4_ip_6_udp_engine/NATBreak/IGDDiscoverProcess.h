#ifndef __IGDDISCOVERPROCESS_H__
#define __IGDDISCOVERPROCESS_H__

//////////////////////////////////////////////////////////////////////////
//
// NOTE:This class DOES NOT corresponding to the IPv6 yet.
//
//////////////////////////////////////////////////////////////////////////

class IGDDiscoverProcess
{
public:
	typedef std::deque<sockaddr_in> Interfaces;
	typedef std::deque<in_addr> Addresses;
	typedef std::deque<std::string> AddressStrings;

public:
	// adhoc. These values should be sophisticated.
	enum IGDDiscoverResult
	{
		Succeeded,
		Failed,
		SocketError,
		IncompleteTransmission,
		ReceiveError,
		NoInterface,
		Timeout,
		TooLongData,
		HTTPOK,
		HTTPNG,
		BadIGDResponce,
		SameControlInfoFound,
		InvalidSocket
	};

public:
	static const std::string UPNP_MULTICAST_ADDRESS;
	static const u_short UPNP_PORT;
	static const u_long BUFFER_SIZE;
	
	static const std::string MSEARCH_REQUEST;

public:
	struct ControlInfo
	{
		ControlInfo()
			: port(0)
			, has_problem(false)
		{
		}
		~ControlInfo(){}

		std::string service_type;
		std::string service_desc_url;
		std::string ip;
		unsigned short port;

		std::string service_description; // xml
		std::string control_url;

		bool has_problem;

		bool operator==( const ControlInfo &rhs ){
			if( 
				service_type == rhs.service_type
				&& service_desc_url == rhs.service_desc_url
				&& service_type == rhs.service_type
				&& ip == rhs.ip
				&& port == rhs.port
				&& service_description == rhs.service_description
				&& control_url == rhs.control_url )
			{
				return true;
			}
			return false;
		}
		bool operator!=( const ControlInfo &rhs ){
			if( 
				service_type == rhs.service_type
				&& service_desc_url == rhs.service_desc_url
				&& service_type == rhs.service_type
				&& ip == rhs.ip
				&& port == rhs.port
				&& service_description == rhs.service_description
				&& control_url == rhs.control_url )
			{
				return false;
			}
			return true;
		}
	};

	typedef std::deque<ControlInfo> IGDControlInformations;
	typedef std::map<std::string,IGDControlInformations> ControlInfosPerServiceType;

	// Functions
public:
	IGDDiscoverProcess();
	IGDDiscoverProcess( 
		long msearch_recv_timeout
		);

	~IGDDiscoverProcess();

public:
	IGDDiscoverResult InitializeUDPConnection( void );
	IGDDiscoverResult UninitializeUDPConnection( void );

private: // l0
	bool EnumelateIP( void );
	bool EnumerateIGD( void );

	IGDDiscoverResult Send( 
		const std::string &data //in
		, const in_addr &nic
		);
	IGDDiscoverResult Receive( 
		std::string &igdip //out
		, std::string &data //out
		);

	IGDDiscoverResult ExtractControlInfo( 
		const std::string &igd_responce,
		ControlInfo &cinfo,
		int &httpres
		);

	void GetServiceDescriptions( void );
	void ExtractControlURL( void );

	bool ExistedControlInfomation( const ControlInfo &target );

public:
	IGDDiscoverResult MSearch( const std::string &service_type );

	//////////////////////////////////////////////////////////////////////////
	// This class working in a thread routine and will finish when IGDs found.
	// Use the boost::thread to run this class.
	//////////////////////////////////////////////////////////////////////////
	void operator()( void );

public:
	IGDControlInformations GetAllIGDControlInformations( void ) const;

private:
	struct Impl;
	boost::shared_ptr<Impl> pimpl;


	//exception
public:
	struct exception
	{
		exception( const char *w ){ _what.assign(w); }
		~exception(){}

		const char* what( void ){ return _what.c_str(); }

		std::string _what;
	};
};


#endif
