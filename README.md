tables
=====================
"location.gpsloc_shortcut";
"location.gpsloc_log";



indexes of table
===================
1. dev_id on location.gpsloc_shortcut
2. dev_id,time on location.gpsloc_log



message queues
===================
1.location_put

	<location user_id="1" dev_id="2000" lon="121.3455" lat="31.245" time="1349674863" speed="45.23"
	direction="245" roadlink="12345678" group_ids="1001,1002,1003"/>


http request
===================
1. location query:  
    http://127.0.0.1/location_get?dev_id=2001
    
2. spatial query:    
	http://127.0.0.1/spatial_query?point=121.34,31.44&elasped=300&radius=0.01&limit=100&order=1&qtype=1
   

libraries linked
=====================
 dl boost_system-mt boost_thread-mt pthread qpidmessaging log4cpp mongoclient
 

obtain dumpping log
=====================
kill -10 $location_id
cat /etc/var/location.status 


comments
====================


						"dev_id"<< (long long)loc->dev_id <<
											"user_id"<< (long long)loc->user_id <<
											"lon" << loc->gps.pt.lon <<
											"lat" << loc->gps.pt.lat <<
											"time" << (long long)loc->gps.time <<
											"speed" << loc->gps.speed <<
											"direction" << loc->gps.direction <<
											"link_id" << (long long)loc->link_id <<
											"group_ids" << groupids.c_str()
