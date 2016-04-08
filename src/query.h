
#ifndef _LOC_QUERY_H
#define _LOC_QUERY_H

#include "base.h"
#include "geo.h"


enum SpatialQueryType{
	SQT_BY_DISTANCE =1,	//半径查询
	SQT_BY_RECTANGLE	//矩形查询
};

//记录及排序方式
enum QueryOrderType{
	QOT_BY_TIME = 1,
	QOT_BY_DISTANCE,
	QOT_BY_UNSORTED
};



struct QueryCase_SpatialRelation{
//	GeoPoint pt;
//	float radius;	//半径
	GeoCircle circle;
	GeoRect rect;
	SpatialQueryType sqt;
	QueryOrderType	qot;
	uint32_t	elapsed;	//最近时间  （UNLIMITED as default）
	GroupId_t group_id;	// UNLIMITED as default
	uint32_t maxnum;		//最大记录数

};

struct LocationDetail{
	UniqueId_t 		dev_id;
	UniqueId_t 		user_id;
	GpsData			gps;
	UniqueId_t 		link_id;
	std::vector<GroupId_t>  group_ids; //要求TC每次传送 组id列表 开销是否值得?
	void addGroup(GroupId_t gid){
		group_ids.push_back(gid);
	} //需要考虑考虑

	std::string to_xml();
	std::string to_json();
	float distance(const GeoPoint& pt);
	bool groupIn(GroupId_t gid);  //是否存在相同组id
};
typedef boost::shared_ptr<LocationDetail> LocationDetailPtr;
typedef std::list< LocationDetailPtr> LocationDetailPtrList;

struct LocQueryResult{
	LocationDetail loc;
};

struct LocSpatialQueryResult{
	LocationDetailPtrList loclist;
	std::string to_xml();
	std::string to_json();
};

std::string to_xml(  LocationDetailPtrList& list);
std::string to_json( LocationDetailPtrList& list);


#endif

