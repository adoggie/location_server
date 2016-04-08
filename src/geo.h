#ifndef _LOC_GEO_H
#define _LOC_GEO_H

#include "base.h"

struct GeoPoint{
	float lat;
	float lon;
	GeoPoint();
	GeoPoint(float lon_,float lat_);
	float distance(const GeoPoint& pt) const;
	static bool fromText(const std::string& text,GeoPoint& pt);
};

struct GeoSize{
	float width;
	float height;
	static bool fromText(const std::string& text,GeoSize& pt);
//	bool operator >(const GeoSize& ohs){
//		return width>ohs.width && height >ohs.height;
//	}
	float min(){
		return std::min(width,height);
	}
	float max(){
		return std::max(width,height);
	}
	GeoSize(){
		width = height = 0;
	}

	GeoSize(float w,float h){
		width = w;
		height = h;
	}

	std::string str();

};

struct GeoRect{
	float x;
	float y;
	float width;
	float height;
	GeoPoint _1() const ; //LB
	GeoPoint _2() const ; //RB
	GeoPoint _3() const; //RT
	GeoPoint _4() const ;//LT
	bool within(const GeoPoint& pt) const ;
	static bool fromText( const std::string& text,GeoRect& rc);
	GeoSize size(){
		return GeoSize(width,height);
	}
	GeoRect(){
		x = y = width = height = 0;
	}
};

struct GeoCircle{
	GeoPoint center;
	float radius;
	static bool fromText(const std::string& text,GeoCircle& circle);
	GeoCircle(){
		radius = 0;
	}
};



struct GpsData{
	GeoPoint 	pt;
	uint32_t	time;
	float speed;
	float direction;
};


#endif
