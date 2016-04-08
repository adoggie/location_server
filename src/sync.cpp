
#include "sync.h"
#include "server.h"
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <iterator>

const char * TABLE1 = "location.gpsloc_shortcut";
const char * TABLE2 = "location.gpsloc_log";

DBSyncer::DBSyncer(){
	_sync_succ_num = 0;
	_sync_error_num = 0;
}

DBSyncer::~DBSyncer(){

}


bool DBSyncer::open(){
	try{
		boost::format fmt("%s:%d");
		fmt%_props.host%_props.port;
		_db.connect(fmt.str());
		prepare();
	}catch(ConnectException & e){
		log_error(_LOG,"mongo db open failed! host=%s:%d | %s",%_props.host%_props.port%e.what());
		return false;
	}
	return true;
}

void DBSyncer::close(){
	sync(true);
}

//检测数据库断开需要重新发起连接
int DBSyncer::sync(bool force){
	LocationDetailPtrList locs;
	{
		boost::mutex::scoped_lock lock(_mtx);
		if( force || _locs.size() >= _props.syncwater){
			locs = _locs;
			_locs.clear();
		}
	}
	sync2db(locs);
	return 0;
}

//写入 gpsloc_shortcut ,gpsloc_log 两张表
int DBSyncer::sync2db(LocationDetailPtrList& list){
//	log_info(_LOG,"begin sync2db()..");

	LocationDetailPtrList::iterator itr;
	for(itr = list.begin();itr!=list.end();itr++){
		LocationDetailPtr& loc = *itr;
		try{
			//uint64_t size;
			std::string groupids;
			std::vector<GroupId_t>::iterator itr;
			for(itr=loc->group_ids.begin();itr!= loc->group_ids.end(); itr++){
				groupids += boost::lexical_cast<std::string>(*itr);
				if( std::distance(itr,loc->group_ids.end()) ){
					groupids+=",";
				}
			}

			//更新快照表
			//size = _db.count("location.gpsloc_shortcut",BSON("dev_id"<< (long long)loc->dev_id));
			BSONObj res = _db.findOne( TABLE1 , BSONObjBuilder().append( "dev_id" , (long long)loc->dev_id ).obj() );
			if( res.isEmpty() ){ // do insert
				_db.insert(TABLE1,
						BSON(GENOID<<
								"dev_id"<< (long long)loc->dev_id <<
								"user_id"<< (long long)loc->user_id <<
								"lon" << loc->gps.pt.lon <<
								"lat" << loc->gps.pt.lat <<
								"time" << (long long)loc->gps.time <<
								"speed" << loc->gps.speed <<
								"direction" << loc->gps.direction <<
								"link_id" << (long long)loc->link_id <<
								"group_ids" << groupids.c_str()
								));
			}else{ // do update
				BSONObj after =	BSON(
						"dev_id"<< (long long)loc->dev_id <<
						"user_id"<< (long long)loc->user_id <<
						"lon" << loc->gps.pt.lon <<
						"lat" << loc->gps.pt.lat <<
						"time" << (long long)loc->gps.time <<
						"speed" << loc->gps.speed <<
						"direction" << loc->gps.direction <<
						"link_id" << (long long)loc->link_id <<
						"group_ids" << groupids.c_str()
						);
				_db.update( TABLE1 , BSONObjBuilder().append( "dev_id" , (long long)loc->dev_id ).obj() , after );
			}

			//插入历史表
			_db.insert(TABLE2,
									BSON(GENOID<<
											"dev_id"<< (long long)loc->dev_id <<
											"user_id"<< (long long)loc->user_id <<
											"lon" << loc->gps.pt.lon <<
											"lat" << loc->gps.pt.lat <<
											"time" << (long long)loc->gps.time <<
											"speed" << loc->gps.speed <<
											"direction" << loc->gps.direction <<
											"link_id" << (long long)loc->link_id <<
											"group_ids" << groupids.c_str()
											));
			_sync_succ_num++;
		}catch(DBException& e){
			_sync_error_num++;
			log_error(_LOG,"[mongo db] load locations exception");
			return -1;

		}
	}
	return 0;
}

void DBSyncer::update(const LocationDetailPtr& loc){
	boost::mutex::scoped_lock lock(_mtx);
	_locs.push_back(loc);
}

void DBSyncer::check(){
	sync(true);
}

/*
<location user_id="1" dev_id="2000" lon="121.3455" lat="31.245" time="1349674863" speed="45.23"
	direction="245" roadlink="12345678" group_ids="1001,1002,1003"/>
 */

//加载快照表
bool DBSyncer::prepare(){
	try{
		_db.ensureIndex( TABLE1, BSON( "dev_id" << 1 ) );
		_db.ensureIndex( TABLE2, BSON( "dev_id" << 1 <<"time"<< 1 ) );

		auto_ptr<DBClientCursor> cursor = _db.query(TABLE1, BSONObj());
		BSONElement  e;
		uint64_t num = 0;
	    while( cursor->more() ) {
	     	BSONObj obj = cursor->next();
	     	LocationDetailPtr  loc( new LocationDetail);
	     	loc->user_id = (UniqueId_t)obj["user_id"].Long();
	     	loc->dev_id = (UniqueId_t)obj["dev_id"].Long();
	     	loc->gps.pt.lon = (float)obj["lon"].Number();
	     	loc->gps.pt.lat = (float)obj["lat"].Number();
	     	loc->gps.time = (uint32_t)obj["time"].Long();
	     	loc->gps.speed= (float)obj["speed"].Number();
	     	loc->gps.direction= (float)obj["direction"].Number();
	     	loc->link_id = (UniqueId_t)obj["link_id"].Long();
	     	std::string ids = obj.getStringField("group_ids");
	     	std::vector<std::string> items;
	     	boost::split( items,ids,boost::is_any_of(","),boost::token_compress_on);
	     	for(size_t n=0;n<items.size();n++){
			//BOOST_FOREACH(std::string& id,items){
				try{
					std::string & id = items[n];
					if(id == ""){
						continue;
					}
					loc->addGroup( boost::lexical_cast<GroupId_t>(id));
				}catch(...){}
			}
			num++;
	     	LocServer::instance().update( loc,false);
//	     	if(num >10000) break;
	    }
	    log_info(_LOG,"total %d location records be loaded!",%num);
	}catch(DBException& e){
		log_error(_LOG,"[mongo db] load locations error! |\n %s",%e.what());
		return false;
	}
	return true;
}

LogMemo_t DBSyncer::statistic(){
	std::string pattern;
	pattern =
			"<dbsync>"
				"<succ_num>%d</succ_num>"
				"<error_num>%d</error_num>"
			"</dbsync>";
	boost::format fmt(pattern);
	LogMemo_t logs;
	fmt% _sync_succ_num % _sync_error_num;
	logs.push_back(fmt.str());
	return logs;
}

