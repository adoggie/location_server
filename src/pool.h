

#ifndef _LOC_AOPOOL_H
#define _LOC_AOPOOL_H

#include "base.h"
#include "ao.h"
#include <boost/thread/shared_mutex.hpp>

class AO_Pool{
public:
	AO_Pool();
	void check();
	void update(const LocationDetailPtr& loc);
	LocationDetailPtr whereis(UniqueId_t dev_id,uint32_t elapsed = 0);
	LogMemo_t statistic();
private:
	std::map< UniqueId_t,LocationDetailPtr>	_aos; 		//所有的设备组
	//std::map< UniqueId_t, AOGroupPtr>	_groups;		//ao对象组
	boost::shared_mutex _mtxaos;
	//boost::shared_mutex _mtxgroup;
};


#endif


