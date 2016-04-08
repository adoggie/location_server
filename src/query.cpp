
#include "query.h"
#include <algorithm>
#include <boost/format.hpp>


/*
 <location user_id="1" dev_id="2000" lon="121.3455"
 lat="31.245" time="1349674863"
 speed="45.23" direction="245"
 roadlink="12345678"/>
 */
std::string LocationDetail::to_xml(){
	//LocationDetail
	boost::format fmt(
			 "<location user_id=\"%d\" dev_id=\"%d\" lon=\"%f\" "
			 "lat=\"%f\" time=\"%d\" "
			 "speed=\"%f\" direction=\"%f\" "
			 "roadlink=\"%d\"/>"
			);
	fmt%this->user_id%this->dev_id%this->gps.pt.lon%
		this->gps.pt.lat%this->gps.time%
		this->gps.speed%this->gps.direction%
		this->link_id;
	return fmt.str();
}

std::string LocationDetail::to_json(){
	return "";
}

float LocationDetail::distance(const GeoPoint& pt){
	return gps.pt.distance(pt);
}

bool LocationDetail::groupIn(GroupId_t gid){
	if( std::find( group_ids.begin(),group_ids.end(),gid) == group_ids.end() ){
		return false;
	}
	return true;
}

std::string to_xml(  LocationDetailPtrList& list){
	std::string xml;
	xml = "<locations>";
	LocationDetailPtrList::iterator itr;
	for(itr = list.begin() ;itr!= list.end();itr++){
		LocationDetailPtr& loc = *itr;
		xml+=loc->to_xml() +"\n";
	}
	xml+="</locations>";
	return xml;
}

std::string to_json( LocationDetailPtrList& list){
	return "";
}
