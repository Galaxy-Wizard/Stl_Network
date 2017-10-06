#include "stdafx.h"
#include "NetworkHelper.h"

#pragma comment(lib, "iphlpapi.lib")
#define DIRECT_ROUTE        MIB_IPROUTE_TYPE_DIRECT
#define INDIRECT_ROUTE      MIB_IPROUTE_TYPE_INDIRECT


struct IPRoutingTable::Impl
{
	Impl()
	{
	}

	~Impl()
	{
	}

	RoutingTable table;
};


IPRoutingTable::IPRoutingTable()
	: pimpl( new Impl )
{

}


IPRoutingTable::~IPRoutingTable()
{

}


void IPRoutingTable::RefleshTable( void )
{
	DWORD lowest_metric = ~0;
	int pgw(-1);
	
	DWORD dwSize(0);
	GetIpForwardTable(NULL, &dwSize, false);
	PMIB_IPFORWARDTABLE pForwardInfo = 
		(MIB_IPFORWARDTABLE *)calloc(dwSize, 1);
	if( GetIpForwardTable(pForwardInfo, &dwSize, false) == NO_ERROR )
	{
		for( unsigned long i(0); i < pForwardInfo->dwNumEntries; i++ )
		{
			MIB_IPFORWARDROW *row = &pForwardInfo->table[i];
			assert( row );

			IPRouteInfo route_entry;
			route_entry.destination.assign( inet_ntoa(*(struct in_addr *)&row->dwForwardDest) );
			route_entry.netmask.assign( inet_ntoa(*(struct in_addr *)&row->dwForwardMask) ) ;
			route_entry.gateway.assign( inet_ntoa(*(struct in_addr *)&row->dwForwardNextHop) );
			route_entry.index = row->dwForwardIfIndex;
			route_entry.type = row->dwForwardType;
			route_entry.metric = row->dwForwardMetric1;
			if( 
				!row->dwForwardDest 
				&& !row->dwForwardMask )
			{
				route_entry.is_default_gateway = true;
			}
			pimpl->table.push_back( route_entry );
			if(
				route_entry.is_default_gateway
				&& (lowest_metric > row->dwForwardMetric1) )
			{
				pgw = i;
				lowest_metric = row->dwForwardMetric1;
			}
		}
		if( pgw >= 0 )
		{
			pimpl->table[pgw].is_primary_gateway = true;
		}
		free(pForwardInfo);
	}

}


RoutingTable IPRoutingTable::GetRoutingTable( void )
{
	RefleshTable();
	return pimpl->table;
}

