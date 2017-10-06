#ifndef __NETWORKHELPER_H__
#define __NETWORKHELPER_H__


struct IPRouteInfo
{
	IPRouteInfo()
		: index(0)
		, type(0)
		, metric(0)
		, is_default_gateway(false)
		, is_primary_gateway(false)
	{
	}
	~IPRouteInfo()
	{
	}

	std::string destination;
	std::string netmask;
	std::string gateway;
	unsigned long index;
	unsigned long type;
	unsigned long metric;
	bool is_default_gateway;
	bool is_primary_gateway;
};

typedef std::deque<IPRouteInfo> RoutingTable;

class IPRoutingTable
{
public:
	IPRoutingTable();
	~IPRoutingTable();

private:
	void RefleshTable( void );
public:
	RoutingTable GetRoutingTable( void );

private:
	struct Impl;
	boost::shared_ptr<Impl> pimpl;
};




#endif

