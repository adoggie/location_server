
#include "pool.h"
#include <boost/format.hpp>

AO_Pool::AO_Pool(){

}

void AO_Pool::check(){

}

void AO_Pool::update(const LocationDetailPtr& loc){
	{
		boost::unique_lock< boost::shared_mutex>  wlock(_mtxaos);
		_aos[loc->dev_id] = loc;
	}
	//刷入组内，便于根据组快速检索
	{
		//boost::unique_lock< boost::shared_mutex>  wlock(_mtxgroup);
		//UniqueIdList_t::iterator itr;
		//_groups.find()
		//for(itr = loc->gr )
	}
}

LocationDetailPtr AO_Pool::whereis(UniqueId_t dev_id,uint32_t elapsed){
	boost::shared_lock< boost::shared_mutex>  rlock(_mtxaos);
	std::map< UniqueId_t,LocationDetailPtr>::iterator itr;
	LocationDetailPtr loc;

	itr = _aos.find(dev_id);
	if( itr!=_aos.end()){
		loc = itr->second;
	}
	return loc;
}

LogMemo_t  AO_Pool::statistic(){
	std::string pattern;
	pattern =
			"<pool>"
				"<ao_num>%d</ao_num>"
			"</pool>";
	boost::format fmt(pattern);
	LogMemo_t logs;

	boost::shared_lock< boost::shared_mutex>  rlock(_mtxaos);

	fmt% _aos.size();
	logs.push_back(fmt.str());
	return logs;
}

