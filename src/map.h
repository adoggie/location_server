
#ifndef _LOC_MAP_H
#define _LOC_MAP_H

#include "base.h"
#include "query.h"


#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>


typedef uint32_t MapCellId;

class GlobalMap;
class MapCell{
public:
	MapCell(GlobalMap* map);
	void check();
	void update(const LocationDetailPtr& loc);
	void doQuery(LocationDetailPtrList & list,const QueryCase_SpatialRelation& qc);
	typedef std::map< DeviceId_t, LocationDetailPtr > DeviceSet_t;
private:
	DeviceSet_t _devs;
	MapCellId	_id;
	boost::shared_mutex _mtxdevs;
	GlobalMap *_map;
};
typedef boost::shared_ptr<MapCell> MapCellPtr;
typedef std::list< MapCellPtr> MapCellPtrList;


#define METERS_PER_DEGREE  (1860*60.0)
#define DEGREE_PER_KM  (1000.0/METERS_PER_DEGREE)

#define MAPCELL_IDX_X(id)  ((id>>16)&0xffff)
#define MAPCELL_IDX_Y(id)	(id&0xffff)
#define MAPCELL_MAKE_ID(x,y)	((x<<16)|y)

class GlobalMap{
public:
	struct Properties{
		GeoPoint origin;
		GeoSize cell;
		Properties(){
			origin.lon = 70;
			origin.lat=10;
			cell.width = 5*DEGREE_PER_KM;
			cell.height = 5*DEGREE_PER_KM;
		}
	};

	void spatialQuery(LocationDetailPtrList& list,const QueryCase_SpatialRelation& qc);

	void update(const LocationDetailPtr& loc);

	bool init(const Properties& props);
//	Properties& properties() ;

	void check();	//

	MapCellId makeCellId(const GeoPoint& pt);

	LogMemo_t statistic();
private:
	MapCellPtrList getCells(const GeoRect& rc);
	void  doQueryByDistance(LocationDetailPtrList& list,const QueryCase_SpatialRelation& qc);
	void doQueryByRectangle(LocationDetailPtrList& list,const QueryCase_SpatialRelation& qc);
	void doQuery(LocationDetailPtrList& list, MapCellPtrList& cells,const QueryCase_SpatialRelation& qc);
private:
	std::map<MapCellId, MapCellPtr> _cells;
	boost::shared_mutex _mtxcells;
	Properties	_props;
};



#endif
