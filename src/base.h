#ifndef _LOC_BASE_H
#define _LOC_BASE_H

#include <stdint.h>
#include <unistd.h>

#include <string>
#include <list>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include <boost/shared_ptr.hpp>

//typedef long uint64_t;
typedef unsigned int uint32_t ;

typedef uint64_t 	UniqueId_t;
typedef uint64_t 	DeviceId_t;
//typedef uint64_t 	GroupId_t;
typedef std::string 	GroupId_t;

typedef unsigned int 	TimeStamp_t;

typedef std::vector<UniqueId_t> UniqueIdList_t;
typedef std::map<std::string,std::string> Properties_t;

enum DataEncodeType{
	ENCODE_XML = 0,
	ENCODE_JSON,
};


#define INVALID_ID 0	//无效的编号
#define UNLIMITED 0		//无限制


typedef std::vector<char> DataChunk;
typedef boost::shared_ptr<DataChunk> DataChunkPtr;
typedef std::list< DataChunkPtr > DataChunkList;

typedef std::list<std::string> StringList_t;

typedef StringList_t LogMemo_t;

enum HTTP_ERRORS{
	HTTP_SUCC = 0,
	HTTP_ERROR = 1
};

enum HTTP_QUERY_ERROR{
	HQR_UNKNOWN = 0,
	HQR_PARAM_DIRTY = 1,
	HQR_SERVER_BUSY = 2,
	HQR_DEV_NOT_EXIST = 3,
	HQR_REGION_TOO_WIDE = 4,
};



#endif

