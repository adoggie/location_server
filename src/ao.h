

#ifndef _LOC_ACTIVEOBJECT_H
#define _LOC_ACTIVEOBJECT_H


#include "base.h"
#include <boost/thread.hpp>
#include <boost/thread/shared_mutex.hpp>

#include "query.h"

struct ActiveObject{
	LocationDetail 	loc;
};
typedef boost::shared_ptr<ActiveObject> ActiveObjectPtr;
typedef std::list< ActiveObjectPtr> ActiveObjectPtrList;

// sizeof(shared_ptr<>) = 16
struct AOGroup{
	UniqueId_t id;
	std::map< UniqueId_t,ActiveObjectPtrList>	aos; //所有的设备组
	boost::shared_mutex	mtx;
};

typedef boost::shared_ptr<AOGroup> AOGroupPtr;






#endif

