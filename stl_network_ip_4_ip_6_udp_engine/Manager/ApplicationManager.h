#ifndef __APPLICATIONMANAGER_H__
#define __APPLICATIONMANAGER_H__

class ApplicationManager
{
public:
	ApplicationManager();
	~ApplicationManager();

private:
	struct Impl;
	boost::shared_ptr<Impl> pimpl;

};

#endif