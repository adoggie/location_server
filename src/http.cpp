
#include "http.h"
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include "map.h"
#include "pool.h"
#include "server.h"
#include "query.h"

/*
static const char *HTTP_RESPONSE_HEAD =
  "HTTP/1.1 200 OK\r\n"
  "Cache: no-cache\r\n"
  "Content-Type: application/x-javascript\r\n"
  "\r\n";
*/


void prepare_http_xml(struct mg_connection *conn){
	const char *HTTP_RESPONSE =
	  "HTTP/1.1 200 OK\r\n"
	  "Cache: no-cache\r\n"
	  "Content-Type: application/xml\r\n"
	  "\r\n";
	mg_printf(conn, "%s", HTTP_RESPONSE);
}


HttpService::HttpService(){
	_listen = 8080;
	_threadnum = 20;
	_reqcnt1 =  _reqcnt2 = 0;
	_ctx = NULL;
}


std::string
get_qsvar(const struct mg_request_info *request_info,const std::string& name,const std::string & _default="") {
	const char *qs = request_info->query_string;
	char buf[128];
	memset(buf,0,sizeof buf);
	mg_get_var(qs, strlen(qs == NULL ? "" : qs), name.c_str(), buf, sizeof(buf) -1 ) ;
	if( strlen(buf) == 0){
		return _default;
	}
	return std::string(buf);
}

void HttpService::error_return(struct mg_connection *conn,int reason){

	std::string xml;
	boost::format fmt("<result code =\"%d\"><error code=\"%d\"/></result>");
	fmt%HTTP_ERROR%reason;
	prepare_http_xml(conn);
	xml = fmt.str();
	mg_printf(conn,"%s",xml.c_str());
}

void HttpService::spatialQuery_return(struct mg_connection *conn,const std::string & xml){
	boost::format fmt("<result code =\"%d\">"
			"%s"
			"</result>");
	fmt%HTTP_SUCC%xml;
	prepare_http_xml(conn);
	mg_printf(conn,"%s",fmt.str().c_str());
}

void HttpService::handle_spatialQuery(struct mg_connection *conn){
	Properties_t props;
	const struct mg_request_info *req = mg_get_request_info(conn);
	//props["point"] =get_qsvar( req,"point","0,0");
	props["radius"] = get_qsvar(req,"radius","0.0");
	props["elapsed"] = get_qsvar( req,"elapsed","0");

	props["rect"] = get_qsvar(req,"rect","");
	props["limit"] = get_qsvar(req,"limit","1"); // UNLIMITED
	props["order"] = get_qsvar(req,"order","1"); // by time
	props["group_id"] = get_qsvar(req,"group_id","");
	//props["qtype"] = get_qsvar(req,"qtype","1"); // by distance
	props["circle"] = get_qsvar(req,"circle","");

	QueryCase_SpatialRelation qc;
	bool ok;
	std::string xml;

	ok = GeoCircle::fromText(std::string(props["circle"].c_str()),qc.circle);
	if(ok){
		qc.sqt =SQT_BY_DISTANCE;
	}else if(GeoRect::fromText(props["rect"] , qc.rect)){
		qc.sqt = SQT_BY_RECTANGLE;
	}else{
		return error_return(conn,HQR_PARAM_DIRTY);
	}
	try{
		qc.elapsed = boost::lexical_cast<uint32_t>(props["elapsed"]);
		qc.maxnum  =boost::lexical_cast<uint32_t>(props["limit"]);
		qc.qot = (QueryOrderType) boost::lexical_cast<int>(props["order"]);
		qc.group_id = boost::lexical_cast<GroupId_t>(props["group_id"]);
	}catch(std::exception& e ){
		return error_return(conn,HQR_PARAM_DIRTY);
	}
	//-- check request params
	LocServer::Properties& sps = LocServer::instance().properties();
	if( qc.circle.radius*2 > sps.spatialquery_restrict_maxregion.min() || qc.rect.size().max() > sps.spatialquery_restrict_maxregion.min() ){
//		send_http_error(conn,500,"system resticted:  ","spatialquery_restrict_maxregion = %s",
//				 sps.spatialquery_restrict_maxregion.str().c_str());
		return error_return(conn,HQR_REGION_TOO_WIDE);
	}

	//-- end check --
	LocationDetailPtrList result;
	LocServer::instance().getMap().spatialQuery(result,qc);
	xml = to_xml( result);
	spatialQuery_return(conn,xml);
}

void * HttpService::handle(enum mg_event event, struct mg_connection *conn){
	const struct mg_request_info *req = mg_get_request_info(conn);
	std::string xml;
	 if (event != MG_NEW_REQUEST) {
		 return NULL;
	 }

	int code = HTTP_SUCC;
	int reason = HQR_UNKNOWN;
	//位置查询
	if (strcmp(req->uri, "/location_get") == 0){
		_reqcnt1++;

		boost::format fmt("<result code=\"%d\">");
		//xml =

		try{
			uint64_t id = boost::lexical_cast<uint64_t>( get_qsvar( req,"dev_id") );
			LocationDetailPtr loc = LocServer::instance().getPool().whereis(id);
			prepare_http_xml(conn);

			if( loc){
				xml+= loc->to_xml();
			}else{
				code = HTTP_ERROR;
				reason = HQR_DEV_NOT_EXIST;
			}

		}catch(std::exception& e){
			//send_http_error(conn,500,"Internal Server Error","%s",e.what());
			code = HTTP_ERROR;
			reason =HQR_PARAM_DIRTY;
		}
		fmt%code;
		xml = fmt.str() + xml;
		if( code != HTTP_SUCC){
			fmt = boost::format("<error code=\"%d\"/>");
			fmt%reason;
			xml+= fmt.str();
		}
		xml+="</result>";
		mg_printf(conn,"%s",xml.c_str());

	}else if( strcmp(req->uri,"/spatial_query") == 0){
		_reqcnt2++;
//		try{
			handle_spatialQuery(conn);
//		}catch(std::exception& e){
//			send_http_error(conn,500,"Internal Server Error","%s",e.what());
//		}
	}else{
		send_http_error(conn,500,"Internal Server Error","%s","closed");
	}
	//mg_close_connection(conn);
	///conn->must_close = 1;
	return (void*)1;
}

void *  HttpService::request_handle(enum mg_event event, struct mg_connection *conn){
	HttpService* http = (HttpService*)mg_get_user_data(conn);
	try{
		return http->handle(event,conn);
	}catch(...){
		;
	}
	return NULL;
}

bool 	HttpService::start(int listen,int maxthread){
	_listen = listen;
	_threadnum = maxthread;

	const char *options[] = {"listening_ports", "8080", "num_threads","10",NULL};
	std::string port;
	std::string threads;
	port = boost::lexical_cast<std::string>(listen);
	threads = boost::lexical_cast<std::string>(maxthread);
	options[1] = port.c_str();
	options[3] = threads.c_str();
	_ctx = mg_start(HttpService::request_handle,this, options);

	return true;
}

void	HttpService::stop(){
	mg_stop(_ctx);
}

LogMemo_t HttpService::statistic(){
	std::string pattern;
	pattern =
			"<http>"
				"<thread_num>%d</thread_num>"
				"<listen>%s %s</listen>"
				"<request_count>%d %d</request_count>"
			"</http>";
	boost::format fmt(pattern);
	LogMemo_t logs;
	fmt% _threadnum %"all"%_listen%_reqcnt1%_reqcnt2;
	logs.push_back(fmt.str());
	return logs;
}
