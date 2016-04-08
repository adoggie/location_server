
#ifndef _LOC_SYNC_H
#define _LOC_SYNC_H
#include "base.h"
#include "query.h"

#include <boost/thread/mutex.hpp>

#include <mongo/client/dbclient.h>
using namespace mongo;


class DBSyncer{
public:
	DBSyncer();
	~DBSyncer();

	struct Properties{
		std::string host;
		int 	port;
		std::string user;
		std::string passwd;
		Properties_t props;
		uint32_t 	syncwater;	//缓存记录水位
		Properties(){
			syncwater =100*5;
			port = 27017;
			host ="localhost";
		}
	};

	enum Error{

	};

	bool open();
	void close();
	bool prepare(); // load locations into memory
	void check();
	void update(const LocationDetailPtr& loc);
	Properties& getProperties(){ return _props;}
	LogMemo_t statistic();
private:
	int sync(bool force=false);
	int sync2db(LocationDetailPtrList& list);
private:
	Properties _props;
	boost::mutex _mtx;
	LocationDetailPtrList _locs;
	DBClientConnection _db;
	uint64_t _sync_succ_num; //同步到db数量
	uint64_t _sync_error_num;
};


#endif
