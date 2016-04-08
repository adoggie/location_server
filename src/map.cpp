
#include "map.h"
#include <algorithm>
#include <iterator>
#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <iostream>

MapCell::MapCell(GlobalMap* map){
	_map = map;
	_id =0;
}


void MapCell::check(){

}

void MapCell::update(const LocationDetailPtr& loc){
	std::map< DeviceId_t, LocationDetailPtr >::iterator itr;
	{
		boost::unique_lock< boost::shared_mutex > lock(_mtxdevs);
		itr = _devs.find(loc->dev_id);
		if(itr ==  _devs.end()){
			_devs[loc->dev_id] = loc;
			return;
		}
		itr->second = loc;
	}
}


void MapCell::doQuery(LocationDetailPtrList & list,const QueryCase_SpatialRelation& qc){
	boost::shared_lock<boost::shared_mutex> lock(_mtxdevs);
	DeviceSet_t::iterator itr;

	for(itr = _devs.begin();itr!=_devs.end();itr++){
		LocationDetailPtr loc = itr->second;
		LocationDetailPtr *ptr = NULL;

		if( qc.sqt == SQT_BY_DISTANCE){
			if( loc->distance(qc.circle.center)<= qc.circle.radius){
				ptr = &loc;
			}
		}else if( qc.sqt ==SQT_BY_RECTANGLE ){
			if( qc.rect.within( loc->gps.pt )){
				ptr = &loc;
			}
		}
		if( !ptr){
			continue;
		}
		//组编号过滤
		if( qc.group_id != ""){ //考虑分组
			if( loc->groupIn(qc.group_id)){
				ptr = & loc;
			}else{
				ptr = NULL;
				continue;
			}
		}
		//最近时间过滤
		if( qc.elapsed != UNLIMITED){ //限制时间
//			std::cout<< (uint64_t)time(NULL) - loc->gps.time << std::endl;
			if( (uint64_t)time(NULL) - loc->gps.time <= qc.elapsed){
				ptr = &loc;
			}else{
				ptr = NULL;
				continue;
			}
		}
		if(ptr){
			list.push_back(*ptr);
		}
	}
}


MapCellPtrList GlobalMap::getCells(const GeoRect& rc){
	MapCellPtrList list;
	MapCellId _1,_3;
	_1 = makeCellId( rc._1());
	_3 = makeCellId( rc._3());
	size_t x1,x2,y1,y2;
	size_t row,col;
	x1 = MAPCELL_IDX_X(_1);
	y1 = MAPCELL_IDX_Y(_1);
	x2 = MAPCELL_IDX_X(_3);
	y2 = MAPCELL_IDX_Y(_3);

	boost::shared_lock<boost::shared_mutex> lock(_mtxcells);
	std::map<MapCellId, MapCellPtr>::iterator itr;
	for(row = y1;row<=y2;row++){
		for(col = x1;col<=x2; col++){
			MapCellId id = MAPCELL_MAKE_ID(col,row);
			itr = _cells.find(id);
			if(itr != _cells.end()){
				list.push_back(itr->second);
			}
		}
	}

	return list;
}

//距离查询
void  GlobalMap::doQueryByDistance(LocationDetailPtrList& list,const QueryCase_SpatialRelation& qc){
	MapCellPtrList cells;
	GeoRect rc;
	rc.x = qc.circle.center.lon - qc.circle.radius;
	rc.y = qc.circle.center.lat - qc.circle.radius;
	rc.width = qc.circle.radius * 2;
	rc.height = rc.width;
	cells = getCells(rc);
	doQuery(list,cells,qc);
}

void GlobalMap::doQuery(LocationDetailPtrList& list, MapCellPtrList& cells,const QueryCase_SpatialRelation& qc){
	MapCellPtrList::iterator itr;
	for(itr=cells.begin();itr!=cells.end();itr++){
		MapCellPtr& ptr = *itr;
		ptr->doQuery(list,qc);
	}
}

//地理区域查询
void GlobalMap::doQueryByRectangle(LocationDetailPtrList  &list,const QueryCase_SpatialRelation& qc){
	MapCellPtrList cells;
	cells = getCells(qc.rect);
	doQuery(list,cells,qc);
}

bool sortLocation( const LocationDetailPtr& a,const LocationDetailPtr&b){
	return true;
}

bool sort_1(const int& a,const int & b){
	return true;
}

struct CompareOrderQuery{
	const QueryCase_SpatialRelation* qc;
	CompareOrderQuery(const QueryCase_SpatialRelation* qc_){
		qc = qc_;
	}
	bool operator ()( const LocationDetailPtr& a,const LocationDetailPtr&b){
		if( qc->qot == QOT_BY_TIME){
			return a->gps.time < b->gps.time;
		}else if( qc->qot == QOT_BY_DISTANCE){
			return a->distance(qc->circle.center) < b->distance(qc->circle.center);
		}
		return true;
	}
};



void GlobalMap::spatialQuery(LocationDetailPtrList& list,const QueryCase_SpatialRelation& qc){
	if( qc.sqt == SQT_BY_DISTANCE){
		doQueryByDistance(list,qc);
	}else if( qc.sqt ==SQT_BY_RECTANGLE )	{
		doQueryByRectangle(list,qc);
	}
	//sort
	if( qc.qot != QOT_BY_UNSORTED){
		list.sort(CompareOrderQuery(&qc));
	}
	//limit
	if(list.size() > qc.maxnum){
		LocationDetailPtrList::iterator itr = list.begin();
		std::advance(itr,qc.maxnum);
		list.erase(itr,list.end());
	}
}

void GlobalMap::update(const LocationDetailPtr& loc){
	std::map<MapCellId, MapCellPtr>::iterator itr;
	{
		MapCellPtr mc;
		{
			boost::shared_lock< boost::shared_mutex > lock(_mtxcells);
			MapCellId cid = makeCellId(loc->gps.pt);
			itr = _cells.find(cid);
			if(itr ==_cells.end()){
				mc = MapCellPtr( new MapCell(this));
				_cells[cid] = mc;
			}else{
				mc = itr->second;
			}
		}
		if(mc.get()){
			mc->update(loc);
			return;
		}
	}
}

bool GlobalMap::init(const Properties& props){
	_props = props;
	return true;
}

/*
Properties& GlobalMap::properties(){
	return _props;
}
*/

void GlobalMap::check(){

}


MapCellId GlobalMap::makeCellId(const GeoPoint& pt){
	uint16_t x,y;
	float div;
	div = (pt.lon - _props.origin.lon)/ _props.cell.width;
	x = uint16_t( div);
	if( div ) x++;
	div =(pt.lat - _props.origin.lat) / _props.cell.height;
	y = uint16_t( div);
	if (div) y++;

	return MAPCELL_MAKE_ID(x,y);

}


LogMemo_t GlobalMap::statistic(){
	std::string pattern;
	pattern =
			"<map>"
				"<cell_num>%d</cell_num>"
			"</map>";
	boost::format fmt(pattern);
	LogMemo_t logs;

	boost::shared_lock< boost::shared_mutex > lock(_mtxcells);
	fmt% _cells.size();
	logs.push_back(fmt.str());
	return logs;
}




