import os,time,sys,time,traceback,httplib,socket

hold=[]
for n in range(100):
	h = httplib.HTTPConnection('localhost',8081)
	h.request("GET","/location_get?dev_id=25399")
	hold.append(h)
	print n
	#resp = h.getresponse()
	#print resp.read()
	
time.sleep(1000)


