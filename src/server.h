
#ifndef _LOC_SERVER_H
#define _LOC_SERVER_H

#include "base.h"
#include "ao.h"
#include "pool.h"
#include "sync.h"
#include "map.h"
#include "mq.h"
#include "http.h"
#include "sync.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/barrier.hpp>

#include <boost/asio.hpp>
#include <boost/asio/error.hpp>

#include "utils/plainconfig.h"
#include "utils/logger.h"
using namespace tce ;



class LocServer{
public:
	LocServer();

	struct Properties{
		uint32_t threadnum;			//工作线程数量
		uint32_t http_thread;
		uint32_t http_port;
		std::string http_addr;
		std::string log2cpp;		// log2cpp 配置文件
		std::string mgr__addr;		//管理端口地址
		int checkwait	;	//checkwait
		GeoSize spatialquery_restrict_maxregion; //=0.1,0.1
		int dbsync; //是否写入位置记录到数据库 gpsloc_log , 0  - 不写入 1 - 写入
	};

	LocServer::Properties& properties(){	return _props;}


	bool init(const std::string& confile,const Properties_t& props = Properties_t());
	int exec();

	void update(const LocationDetailPtr& loc,bool sync = true);

	static LocServer& instance(){
		static boost::shared_ptr<LocServer> _handle;
		if( !_handle.get()){
			_handle = boost::shared_ptr<LocServer>(new LocServer);
		}
		return *_handle;
	}

	void setLogger( const tce::LoggerPtr& log);
	void salfCheck();
	void shutdown();

	AO_Pool& getPool(){ return _pool;}
	GlobalMap& getMap(){	return _map;}
	LoggerPtr getLogger(){ return _logger;}

	LogMemo_t statistic();
private:
	bool prepare();
private:
	AO_Pool 	_pool;
	GlobalMap	_map;
	HttpService _http;
	MessageQueue _mq;
	DBSyncer	_syncer;

	//boost::asio::signal_set  _signals;
	boost::asio::io_service  _ios;
	boost::shared_ptr<boost::asio::deadline_timer> _chktimer;
	boost::mutex _mtxlog;

	PlainConfig _conf;
	LoggerPtr _logger;
	boost::shared_ptr< boost::thread> _thread_ios;

	Properties _props;
	std::string _exepath;
};

#define _LOG LocServer::instance().getLogger()
#endif
