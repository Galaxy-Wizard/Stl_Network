#ifndef __NETWORKMANAGER_H__
#define __NETWORKMANAGER_H__

class NetworkManager
{
public:
	NetworkManager();
	~NetworkManager();
private:
	struct Impl;
	boost::shared_ptr<Impl> pimpl;
};

#endif

