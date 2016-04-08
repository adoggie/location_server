//============================================================================
// Name        : location.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#ifndef _ADDIN

#include <iostream>
#include "base.h"
#include "server.h"
#include <iterator>
#include <vector>

#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "tinyxml/tinyxml.h"
#include "tinyxml/tinystr.h"


void usage(){
	std::cout<<"usage:"<<std::endl;
	std::cout<<"       location    server.conf" << std::endl;
}

int main(int argc,char ** argv) {

	const char * conf = NULL;

		if( argc < 2){
			conf = getenv("LOCATION_CONFIG");
			if( !conf){
				std::cout<<"[ERROR]  configure file not given! " << std::endl;
				usage();
				return -1;
			}
		}else{
			conf = argv[1];
		}

	int okay = LocServer::instance().init( conf);
	if(!okay){
		return -2;
	}
	return LocServer::instance().exec();
}

#endif
