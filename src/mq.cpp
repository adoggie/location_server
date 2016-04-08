#include "mq.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "query.h"
#include <iostream>
#include "server.h"
#include "tinyxml/tinyxml.h"
#include "tinyxml/tinystr.h"
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>


MessageQueue::MessageQueue(){
	_msgcnt = 0;
}

bool MessageQueue::open(){
	try{
		log_debug(_LOG,"MQ try open ...");

		std::string connectionOptions = "";
		_conn = boost::shared_ptr<messaging::Connection>(new messaging::Connection(_broker));
		_conn->setOption("tcp_nodelay",true);
		_conn->open();
		_session = _conn->createSession();
		_session.sync(false);
		_recver = _session.createReceiver( _addrecv);
		_recver.setCapacity(1000);
		_thread_read = boost::shared_ptr<boost::thread>( new boost::thread( boost::bind(&MessageQueue::readThread,this) ) );
		_thread_decode = boost::shared_ptr<boost::thread>( new boost::thread( boost::bind(&MessageQueue::decodeThread,this) ) );
	}catch( std::exception& e){ //打开异常
		if(_conn) _conn->close();
		log_debug(_LOG,"MQ open exception!");
		return false;
	}
	log_debug(_LOG,"MQ open succed!");
	return true;
}

void MessageQueue::close(){
	if(!_conn.get())	return;

	_conn->close();
	_thread_read->interrupt();
	_thread_decode->interrupt();
	_thread_read->join();
	_thread_decode->join();
	_dcs.clear();
	_conn =  boost::shared_ptr<messaging::Connection>();
}

void MessageQueue::readThread(){
	char * data;
	size_t size;
	try{
		while(true){
			messaging::Message m = _recver.fetch(messaging::Duration::FOREVER );
			data = (char*) m.getContentPtr();
			size = m.getContentSize();
//			continue;
			DataChunkPtr dc(new DataChunk(data,data+size));
			_session.acknowledge();
			{
				boost::mutex::scoped_lock lock(_mtx);
				_dcs.push_back(dc);
			}
			_cond.notify_one();
			//log_debug(_LOG,"pick one message.., wake buddy up !");
		}
	}catch(std::exception& e){
		log_info(_LOG," MessageQueue::readThread()  exiting..., (%s)",%e.what());
	}
	_conn->close();

}



void MessageQueue::decodeThread(){
	try{
		while(1){
			DataChunkPtr dc;
			{
				boost::mutex::scoped_lock lock(_mtx);
				if( _dcs.size()){
					dc = _dcs.front();
					_dcs.pop_front();
				}else{
					//_cond.wait(lock);
					_cond.timed_wait(lock,boost::get_system_time() + boost::posix_time::milliseconds(20));
					if( _dcs.size()){
						dc = _dcs.front();
						_dcs.pop_front();
					}
				}
			}
			if(!dc.get()){
				continue;
			}
			LocationDetailPtr loc;
			dc->push_back('\0');
			loc = parseMsg(dc);
			if( loc ){
				LocServer::instance().update(loc);
			}else{
				;//log_info(_LOG,"MQ decode error, MSG:%s",%&dc->at(0));
			}
		}
	}catch(std::exception& e){
		log_info(_LOG," MessageQueue::decodeThread()  exiting...reason: %s",%e.what());
	}
}

bool MessageQueue::init(const std::string & broker,const std::string& addr,const Properties_t& props){
	_broker = broker;
	_addrecv = addr;
	_props = props;
	return true;
}

void MessageQueue::check(){
	if(_conn.get() == NULL || _conn->isOpen() == false){
		open();
	}
}


/*

<location user_id="1" dev_id="2000" lon="121.3455" lat="31.245" time="1349674863" speed="45.23"
	direction="245" link_id="12345678" group_ids="1001,1002,1003"/>
 */
LocationDetailPtr MessageQueue::parseMsg(const DataChunkPtr& dc){
	TiXmlDocument doc;
	std::string xml;
//	xml = "<location>";
	xml=(char*)&dc->at(0);
//	xml+="</location>";
	doc.Parse( xml.c_str() ) ;
	if(doc.Error()){
		return LocationDetailPtr();
	}
	TiXmlElement* e = doc.RootElement();
	if(!e ||  !e->Value() || e->Value() != std::string("NaviMSG")){
		return LocationDetailPtr();
	}
	std::string attr;
	LocationDetailPtr loc(new LocationDetail);
	try{
		e = e->FirstChildElement();
		while(e){
			std::string name,value;
			//text = e->GetText();
			name = e->Value()?e->Value():"";
			value = e->GetText()?e->GetText():"";

			if( name =="user_id"){
				loc->user_id = boost::lexical_cast<UniqueId_t>( value );
			}else if(name == "dev_id"){
				loc->dev_id = boost::lexical_cast<UniqueId_t>( value );
			}else if( name == "gps" || name == "GPS"){
				loc->gps.pt.lon = boost::lexical_cast<float>(e->Attribute("lon")?e->Attribute("lon"):"0");
				loc->gps.pt.lat = boost::lexical_cast<float>(e->Attribute("lat")?e->Attribute("lat"):"0");
				loc->gps.time = boost::lexical_cast<uint32_t>(e->Attribute("time")?e->Attribute("time"):"0");
				loc->gps.speed= boost::lexical_cast<float>(e->Attribute("speed")?e->Attribute("speed"):"0");
				loc->gps.direction = boost::lexical_cast<float>(e->Attribute("direction")?e->Attribute("direction"):"0");
			}else if(name == "roadlink"){
				//value = StringUtil::getDefaultValue(value.c_str(),"0");
				if( value =="") value="0";
				loc->link_id = boost::lexical_cast<UniqueId_t>( value );
			}else if( name == "group_ids"){
				std::vector<std::string> ids;
				boost::split( ids,value,boost::is_any_of(","),boost::token_compress_on);

				BOOST_FOREACH(std::string& id,ids){
					boost::trim(id);
					if(id == "") continue;
					loc->addGroup( boost::lexical_cast<GroupId_t>(id));
				}
			}else{
				log_error(_LOG,"Unknown Message \n %s \n ",%(char*)&dc->at(0));
				return LocationDetailPtr();
			}
			e = e->NextSiblingElement();
		}
		/*
		loc->user_id = boost::lexical_cast<UniqueId_t>( (std::string)e->Attribute("user_id"));
		loc->dev_id = boost::lexical_cast<UniqueId_t>((std::string)e->Attribute("dev_id"));
		loc->gps.pt.lon = boost::lexical_cast<float>((std::string)e->Attribute("lon"));
		loc->gps.pt.lat = boost::lexical_cast<float>((std::string)e->Attribute("lat"));
		loc->gps.time = boost::lexical_cast<uint32_t>((std::string)e->Attribute("time"));
		loc->gps.speed= boost::lexical_cast<float>((std::string)e->Attribute("speed"));
		loc->gps.direction = boost::lexical_cast<float>((std::string)e->Attribute("direction"));
		loc->link_id = boost::lexical_cast<uint64_t>((std::string)e->Attribute("link_id"));
		attr = e->Attribute("group_ids");
		*/
		//log_debug(_LOG,"One LocRec committed.");
		log_debug(_LOG,"XML-data: %s",%(char*)&dc->at(0));
		return loc;
	}catch(std::exception& e){
		log_error(_LOG,"MQ: message decode error! | %s  \n%s",%e.what()%(char*)&dc->at(0));
	}

	return LocationDetailPtr();
}


LogMemo_t MessageQueue::statistic(){
	std::string pattern;
	pattern =
			"<mq>"
				"<msgcnt>%d</msgcnt>"
				"<broker>%s</broker>"
				"<address>%s</address>"
			"</mq>";
	boost::format fmt(pattern);
	LogMemo_t logs;
	fmt% _msgcnt % _broker %_addrecv;
	logs.push_back(fmt.str());
	return logs;
}



