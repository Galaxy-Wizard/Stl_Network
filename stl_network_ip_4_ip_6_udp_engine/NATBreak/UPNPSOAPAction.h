#ifndef __UPNPSOAPACTION_H__
#define __UPNPSOAPACTION_H__


class UPNPSOAPAction
{
public:
	UPNPSOAPAction();
	~UPNPSOAPAction();

public:
	bool Initialize( void );

private:
	struct Impl;
	boost::shared_ptr<Impl> pimpl;
};


#endif

