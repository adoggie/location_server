
#include "server.h"
//#include "utils/log4cpp.h"
#include "utils/log4cxx.h"
#include "utils/plainconfig.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <sys/file.h>

Log4cxxPtr glog4cxx;


char *get_exe_path(char *buf, int count)
{
    int i;
    int rslt = readlink("/proc/self/exe", buf, count - 1);
    if (rslt < 0 || (rslt >= count - 1))
    {
        return NULL;
    }
    buf[rslt] = '\0';
    for (i = rslt; i >= 0; i--)
    {
        if (buf[i] == '/')
        {
            buf[i + 1] = '\0';
            break;
        }
    }
    return buf;
}


//准备退出
void sighandle_stop(int sig){
	std::cout<<"sighandle_stop"<<std::endl;
	LocServer::instance().shutdown();
}

//输出运行参数信息到文件 /var/location.txt
void sighandle_dump(int sig){
	std::cout<<"sighandle_dump"<<std::endl;
	LocServer::instance().statistic();
}


LocServer::LocServer() { //:_signals(_ios,SIGINT,SIGTERM,SIGHUP){
	_logger= tce::Logger::create();
	char path[128];
	char * p;
	p = get_exe_path(path,sizeof(path));
	if(p){
		_exepath = p;
	}

}

bool init_env(){
	pid_t pid = getpid();
	std::string s;
	s = boost::lexical_cast<std::string>( pid);
	const char *lockfile = "/var/lock/subsys/locationd";
	int fd= open(lockfile,O_WRONLY|O_CREAT);
	if( flock(fd,LOCK_EX)){
		std::cout<<"file :" << lockfile << " try lock failed!";
		return false;
	}
	write(fd,s.c_str(),s.size());

	return true;
}

#ifdef _ADDIN
bool LocServer::init(const std::string& confile,const Properties_t& props){
	std::string sval;
	if(!init_env()){
		return false;
	}
	//bool okay = false;
	//0.1 初始化配置
	//--------signal init ---------------------
	signal(SIGINT,sighandle_stop);
	signal(SIGHUP,sighandle_stop);
	signal(SIGTERM,sighandle_stop);
	if( signal(SIGUSR1,sighandle_dump) == SIG_ERR){
		std::cout<< " signal:[SIGUSR1] set error!"<<std::endl;
	}
	//-----------------------------
	PlainConfig conf;
	sval = confile;
	boost::trim(sval);
	if( sval.at(0)!='/'){
		sval = _exepath+"/" + sval;
	}
	if(!conf.load( sval)){
		std::cout<<"read config error!"<<std::endl;
		log_error(_logger,"load configure failed! [file:%s]",%sval);
		return false;
	}
	_conf = conf;
	//0.2 log4cpp配置

	sval = conf.getProperty<std::string>("log4cpp","/srv/bin/server.conf");
	boost::trim(sval);
	if( sval.at(0) != '/'){
		sval = _exepath+"/"+sval;
	}
	//Log4cppPtr log4cpp =  Log4cpp::create(sval,"location");
	glog4cxx =  Log4cxx::create(sval,"location");
	if(!glog4cxx.get()){
		std::cout<<"log4cpp init failed!"<<" file:"<<sval<< std::endl;
		//log_error(_LOG,"load log4cpp failed! [file:%s]",sval);
		return false;
	}
	return true;

}

bool LocServer::prepare(){
	std::string sval;
	bool okay = false;

	PlainConfig &conf = _conf;
	_logger->addHandler(glog4cxx);

	//////////////////////////////////////
	sval = conf.getProperty<std::string>("spatialquery.restrict.maxregion","0.1,0.1");
	okay = GeoSize::fromText(sval,_props.spatialquery_restrict_maxregion);
	if( !okay){
		log_error(_LOG,"configure item: [spatialquery_restrict_maxregion] invalid value!");
		return false;
	}

	log_info(_LOG,"Location service start...")
	//1.数据库初始化
	DBSyncer::Properties& dbprops = _syncer.getProperties();
	dbprops.host = conf.getProperty<std::string>("mongodb.host","localhost");
	dbprops.port = conf.getProperty<int>("mongodb.port",27017);
	dbprops.user = conf.getProperty<std::string>("mongodb.user","");
	dbprops.passwd = conf.getProperty<std::string>("mongodb.passwd","");
	dbprops.syncwater = conf.getProperty<uint32_t>("syncpoint",20);
	_props.dbsync = conf.getProperty<int>("dbsync",1); //默认写入数据库
	if( !_syncer.open() ){
		return false;
	}
	//==================
	//2. 地图和缓冲池初始化
	//==================
	GlobalMap::Properties maprops;
	sval = conf.getProperty<std::string>("map.origin","10,70");
	if( !GeoPoint::fromText( sval,maprops.origin)){
		log_error(_LOG,"dirty configure item [ map.origin:%s ]",%sval);
		return false;
	}
	sval = conf.getProperty<std::string>("map.cell","0.0448,0.0448");
	if( !GeoSize::fromText( sval,maprops.cell)){
		log_error(_LOG,"dirty configure item [ map.cell:%s ]",%sval);
		return false;
	}
	_map.init(maprops);

	//==================
	//3.htpp初始化
	//==================
	log_info(_LOG,"init http service...")
	if ( !_http.start( conf.getProperty<int>("http.port",8080),
				conf.getProperty<int>("http.threadnum",20)
			)){
		log_error(_LOG,"http start failed!");
		return false;
	}

	//==================
	//4. qpid 消息队列初始化
	//==================
	std::string broker,addr;
	broker = conf.getProperty<std::string>("mq.broker","localhost:5672");
	addr = conf.getProperty<std::string>("mq.name","mq_location");

	log_info(_LOG,"MQ Initializing ...")
	_mq.init(broker,addr);
	_mq.open();

	//==================
	// 工作线程
	//==================
	_props.checkwait = conf.getProperty<int>("checkwait",5);
	_props.checkwait = std::max( _props.checkwait,5);

	_chktimer = boost::shared_ptr<boost::asio::deadline_timer>(new boost::asio::deadline_timer(_ios,boost::posix_time::seconds(_props.checkwait))) ;
	_chktimer->async_wait( boost::bind(&LocServer::salfCheck,this));
	_thread_ios = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&boost::asio::io_service::run,&_ios )));
	log_info(_LOG,"LocServer has  been launched!");
	return true;
}

#else

bool LocServer::prepare(){

	return true;
}

bool LocServer::init(const std::string& confile,const Properties_t& props){
	std::string sval;
	bool okay = false;

	if(!init_env()){
		return false;
	}
	//0.1 初始化配置
	//--------signal init ---------------------
	signal(SIGINT,sighandle_stop);
	signal(SIGHUP,sighandle_stop);
	signal(SIGTERM,sighandle_stop);
	if( signal(SIGUSR1,sighandle_dump) == SIG_ERR){
		std::cout<< " signal:[SIGUSR1] set error!"<<std::endl;
	}
	//-----------------------------
	PlainConfig conf;
	if(!conf.load( confile)){
		std::cout<<"read config error!"<<std::endl;
		log_error(_logger,"load configure failed! [file:%s]",%confile);
		return false;
	}
	_conf = conf;

	//0.2 log4cpp配置

	sval = conf.getProperty<std::string>("log4cpp","/etc/sns/location.conf");
	//Log4cppPtr log4cpp =  Log4cpp::create(sval,"location");
	Log4cxxPtr log4cxx =  Log4cxx::create(sval,"location");
	if(!log4cxx.get()){
		std::cout<<"log4cpp init failed!"<<" file:"<<sval<< std::endl;
		//log_error(_LOG,"load log4cpp failed! [file:%s]",sval);
		return false;
	}
	_logger->addHandler(log4cxx);

	//////////////////////////////////////
	sval = conf.getProperty<std::string>("spatialquery.restrict.maxregion","0.1,0.1");
	okay = GeoSize::fromText(sval,_props.spatialquery_restrict_maxregion);
	if( !okay){
		log_error(_LOG,"configure item: [spatialquery_restrict_maxregion] invalid value!");
		return false;
	}

	log_info(_LOG,"Location service start...")
	//1.数据库初始化
	DBSyncer::Properties& dbprops = _syncer.getProperties();
	dbprops.host = conf.getProperty<std::string>("mongodb.host","localhost");
	dbprops.port = conf.getProperty<int>("mongodb.port",27017);
	dbprops.user = conf.getProperty<std::string>("mongodb.user","");
	dbprops.passwd = conf.getProperty<std::string>("mongodb.passwd","");
	dbprops.syncwater = conf.getProperty<uint32_t>("syncpoint",20);
	_props.dbsync = conf.getProperty<int>("dbsync",1); //默认写入数据库
	if( !_syncer.open() ){
		return false;
	}
	//==================
	//2. 地图和缓冲池初始化
	//==================
	GlobalMap::Properties maprops;
	sval = conf.getProperty<std::string>("map.origin","10,70");
	if( !GeoPoint::fromText( sval,maprops.origin)){
		log_error(_LOG,"dirty configure item [ map.origin:%s ]",%sval);
		return false;
	}
	sval = conf.getProperty<std::string>("map.cell","0.0448,0.0448");
	if( !GeoSize::fromText( sval,maprops.cell)){
		log_error(_LOG,"dirty configure item [ map.cell:%s ]",%sval);
		return false;
	}
	_map.init(maprops);

	//==================
	//3.htpp初始化
	//==================
	log_info(_LOG,"init http service...")
	if ( !_http.start( conf.getProperty<int>("http.port",8080),
				conf.getProperty<int>("http.threadnum",20)
			)){
		log_error(_LOG,"http start failed!");
		return false;
	}

	//==================
	//4. qpid 消息队列初始化
	//==================
	std::string broker,addr;
	broker = conf.getProperty<std::string>("mq.broker","localhost:5672");
	addr = conf.getProperty<std::string>("mq.name","mq_location");

	log_info(_LOG,"MQ Initializing ...")
	_mq.init(broker,addr);
	_mq.open();

	//==================
	// 工作线程
	//==================
	_props.checkwait = conf.getProperty<int>("checkwait",5);
	_props.checkwait = std::max( _props.checkwait,5);

	_chktimer = boost::shared_ptr<boost::asio::deadline_timer>(new boost::asio::deadline_timer(_ios,boost::posix_time::seconds(_props.checkwait))) ;
	_chktimer->async_wait( boost::bind(&LocServer::salfCheck,this));
	_thread_ios = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&boost::asio::io_service::run,&_ios )));
	log_info(_LOG,"LocServer has  been launched!");
	return true;
}
#endif // _ADDIN




int LocServer::exec(){
	if(!prepare()){
		return 2;
	}
	_thread_ios->join();
	_http.stop();
	_mq.close();
	log_info(_LOG,"LocServer ending..");
	return 1;
}



void LocServer::update(const LocationDetailPtr& loc,bool sync){
	_map.update(loc);
	_pool.update(loc);
	if(sync){
		//是否开启本地写入位置记录到数据库，同时只能有一个server开启写入功能
		if(_props.dbsync){
			_syncer.update(loc);
		}
	}
}

void LocServer::salfCheck(){
	_map.check();
	_pool.check();
	_mq.check();
	_syncer.check();
	_chktimer->expires_from_now(boost::posix_time::seconds(5));
	_chktimer->async_wait( boost::bind(&LocServer::salfCheck,this));
}

void LocServer::shutdown(){
	_ios.stop();
	_thread_ios->interrupt();
}

LogMemo_t  LocServer::statistic(){
	LogMemo_t logs;
	LogMemo_t x;
	logs.push_back("<location>");
	x = _syncer.statistic();
	std::copy(x.begin(),x.end(),std::back_inserter(logs));
	x = _map.statistic();
	std::copy(x.begin(),x.end(),std::back_inserter(logs));
	x = _http.statistic();
	std::copy(x.begin(),x.end(),std::back_inserter(logs));
	x = _mq.statistic();
	std::copy(x.begin(),x.end(),std::back_inserter(logs));

	x = _pool.statistic();
	std::copy(x.begin(),x.end(),std::back_inserter(logs));

	logs.push_back("</location>");

	std::string text;
	text = boost::join(logs,"\n");
	std::string statusdump = _conf.getProperty<std::string>("statusdump","/var/location.status");
	FILE * fp = fopen(statusdump.c_str(),"w");
	if( fp){
		fwrite(text.c_str(),text.size(),1,fp);
		fclose(fp);
	}else{
		log_error(_LOG,"dumpping status failed! | file:%s open denied!",%statusdump);
	}
	return LogMemo_t ();
}
