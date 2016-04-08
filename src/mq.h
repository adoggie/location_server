
#ifndef _LOC_MQ_H
#define _LOC_MQ_H

#include "base.h"
#include "query.h"


#include <boost/thread.hpp>
#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Session.h>

#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/mutex.hpp>

#include "utils/mutexobj.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

using namespace qpid ; //::messaging;

class MessageQueue{
public:
	MessageQueue();
	bool open();
	void close();
	bool init(const std::string & broker,const std::string& addr,const Properties_t& props = Properties_t());
	void check();
	LogMemo_t statistic();
private:
	void readThread();
	LocationDetailPtr parseMsg(const DataChunkPtr& dc);
	void decodeThread();

private:

	std::string _broker;
	std::string _addrecv;
	Properties_t _props;

	boost::shared_ptr<messaging::Connection> _conn;
	messaging::Session  _session;
	messaging::Receiver _recver;
	boost::shared_ptr<boost::thread> _thread_read;
	boost::shared_ptr<boost::thread> _thread_decode;
	DataChunkList	_dcs;
	boost::mutex _mtx;
	//tce::MutexObject<DataChunk>	_wait;

	boost::condition_variable _cond;

	uint64_t _msgcnt; //获取消息总数
};
//
//namespace StringUtil{
//	inline
//	const char* getDefaultValue(const char * d,const char* default_){
//		if( !d  || strcmp(d,"")==0 ){
//			return default_;
//		}
//		return d;
//	}
//
//}

#endif
