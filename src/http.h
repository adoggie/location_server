
#ifndef _LOC_HTTP_H
#define _LOC_HTTP_H

#include "base.h"
#include "mongoose.h"

class HttpService{
public:
	HttpService();
	//bool 	init(const Properties_t& props=Properties_t());
	bool 	start(int listen=8080,int maxthread=20);
	void	stop();
	LogMemo_t statistic();
private:
	static void *  request_handle(enum mg_event event, struct mg_connection *conn);
	void* handle(enum mg_event event, struct mg_connection *conn);
	void handle_spatialQuery(struct mg_connection *conn);

	void error_return(struct mg_connection *conn,int reason);
	void spatialQuery_return(struct mg_connection *conn,const std::string & xml);
private:
	int 	_listen;
	int 	_threadnum;
	uint64_t _reqcnt1,_reqcnt2; //位置查询和空间查询

	struct mg_context *_ctx;
};


#endif
