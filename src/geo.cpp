
#include "geo.h"
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <cmath>
#include <boost/format.hpp>
//#include <boost/geometry/geometry.hpp>


GeoPoint::GeoPoint(){
	lon = lat = 0;
}

GeoPoint::GeoPoint(float lon_,float lat_){
	lon =lon_;
	lat =lat_;
}

float GeoPoint::distance(const GeoPoint& pt) const{
	float dist;

	dist = (float)std::sqrt( std::pow((pt.lon - this->lon) ,2) + std::pow((pt.lat - this->lat),2));
	return dist;
}

 bool
GeoPoint::fromText(const  std::string& text_,GeoPoint& pt){
	std::vector<std::string> values;
	std::string text;
	text = text_;
	boost::trim(text);
	boost::split( values,text,boost::is_any_of(","),boost::token_compress_on);
	if(values.size()!=2){
		return false;
	}
	try{
		pt.lon = boost::lexical_cast<float>(values[0]);
		pt.lat = boost::lexical_cast<float>(values[1]);
	}catch(...){
		return false;
	}
	return true;
}


 ////////////////////////////////////////////////////////////////////////////////////
bool GeoSize::fromText(const std::string& text_,GeoSize& pt){
	std::vector<std::string> values;
	std::string text;
	text = text_;
	boost::trim(text);
	boost::split( values,text,boost::is_any_of(","),boost::token_compress_on);
	if(values.size()!=2){
		return false;
	}
	try{
		pt.width = boost::lexical_cast<float>(values[0]);
		pt.height = boost::lexical_cast<float>(values[1]);
	}catch(...){
		return false;
	}
	return true;
}

std::string GeoSize::str(){
	boost::format fmt("%f,%f");
	fmt%width%height;
	return fmt.str();
}

bool GeoCircle::fromText(const std::string& text_,GeoCircle& circle){
	std::vector<std::string> values;
	std::string text;
	text = text_;
	boost::trim(text);
	boost::split( values,text,boost::is_any_of(","),boost::token_compress_on);
	if(values.size()!=3){
		return false;
	}
	try{
		circle.center.lon = boost::lexical_cast<float>(values[0]);
		circle.center.lat = boost::lexical_cast<float>(values[1]);
		circle.radius = boost::lexical_cast<float>(values[2]);
	}catch(...){
		return false;
	}
	return true;
}

 	////////////////////////////////////////////////////////////////////////////////////


GeoPoint GeoRect::_1() const {
	return GeoPoint(x,y);
}

GeoPoint GeoRect::_2() const {
	return GeoPoint(x+width,y);
}

GeoPoint GeoRect::_3() const {
	return GeoPoint(x+width,y+height);
}

GeoPoint GeoRect::_4() const {
	return GeoPoint(x,y+height);
}

bool GeoRect::within(const GeoPoint& pt) const {
	return (pt.lon>=x && pt.lon<=(x+width) && pt.lat>=y&& pt.lat<=(y+height) );
}

bool GeoRect::fromText(const std::string& text_,GeoRect& rc){
	std::vector<std::string> values;
	std::string text;
	text = text_;
	boost::trim(text);
	boost::split( values,text,boost::is_any_of(","),boost::token_compress_on);
	if(values.size()!=4){
		return false;
	}
	try{
		rc.x = boost::lexical_cast<float>(values[0]);
		rc.y = boost::lexical_cast<float>(values[1]);
		rc.width = boost::lexical_cast<float>(values[2]);
		rc.height = boost::lexical_cast<float>(values[3]);
	}catch(...){
		return false;
	}
	return true;
}




