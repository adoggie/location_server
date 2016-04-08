#-- coding: utf-8 --
# 生成gps位置点信息，从传送到mq

import time,os,os.path,sys,traceback,string


def sendLocation(fcd):
	rec_count = 0
	file_count = 0
	files = os.listdir(fcd)
	devs={}
	for file in files:
		if file.find('.txt') == -1:
			continue
		print 'ready for : %s'%file
		file_count+=1
		print file_count
		f = open(fcd+'/'+file)
		lines = f.readlines()
		for line in lines:
			pp = line.split(',')

			pp = map(string.strip,pp)
			if len(pp)!=9: continue
			time_,devid,lon,lat,angle,speed,p1,p2,time2 = pp
			if not devs.has_key(devid) :
				devs[devid[1:]] = 1
			
			if len(devs) >3000:
				f = open('devids.txt','w')
				for k,v in devs.items():
					f.write('%s\n'%k);
				f.close()
				return;
#			



def filter_data(fcd,ids='25079'):
	rec_count = 0
	file_count = 0
	files = os.listdir(fcd)
	devs=[]
	for file in files:
		if file.find('.txt') == -1:
			continue
		print 'ready for : %s'%file
		file_count+=1
		print file_count
		f = open(fcd+'/'+file)
		lines = f.readlines()
		for line in lines:
			pp = line.split(',')

			pp = map(string.strip,pp)
			if len(pp)!=9: continue
			time_,devid,lon,lat,angle,speed,p1,p2,time2 = pp
			
			if devid[1:] == ids:
				devs.append('%s,%.4f,%.4f,%s'%(devid[1:],float(lon),float(lat),time_))
			
			if len(devs) >50:
				f = open('dev_points.txt','w')
				for x in devs:
					f.write('%s\n'%x);
				f.close()
				return;


if __name__=='__main__':
	fcd = '/home/shared/fcd/4'
	#sendLocation(fcd)
	filter_data(fcd)
