#-- coding: utf-8 --
# 生成gps位置点信息，从传送到mq

import time,os,os.path,sys,traceback,string
from qpid.messaging import *


#log>>gz
def extract_files(fcd):
	files = os.listdir(fcd)
	for file in files:
#		print os.path.dirname(fcd)
		log = fcd +'/' + file
		file = os.path.basename(file)
		gz = fcd+'/'+file.split('.')[0] + ".gz"
		print log,gz

		txt =  fcd+'/'+file.split('.')[0] + ".txt"
#		os.system('mv %s %s'%(log,txt))
#		os.system('gzip -d %s'%(gz))
#		os.system('mv %s %s'%(log,gz))
		print txt
#		break


	pass

xml='''
   <gps lon="%s" lat="%s" time="%s" speed="%s" direction="%s"/>
   <group_ids>1001</group_ids>
   <user_id>1</user_id>
   <roadlink>123456</roadlink>
   <dev_id>%s</dev_id>
'''

sys.argv.append('')

broker =  "localhost:5672"
address = "q1"

connection = Connection(broker,tcp_nodelay=True)


connection.open()


session = connection.session()
session.sync(False)

sender = session.sender(address)
#sender.setCapacity(1000);



#connection.close()




# untar gz
#读取指定目录下的轨迹文件记录，发送至MQ
def sendLocation(fcd):
	rec_count = 0
	file_count = 0
	files = os.listdir(fcd)
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
			rec_count+=1

			d = xml%(lon,lat,int(time.time()),speed,angle,devid[1:])
			#print d
#			if rec_count >10: return
			sender.send(Message(d));
			time.sleep(0.00001)

	print rec_count

#模拟生成指定数量的设备位置信息
def test_capacity(n,devnum):
	xml='''
   <gps lon="121.34" lat="31.22" time="%s" speed="10" direction="200.3"/>
   <group_ids>1001</group_ids>
   <user_id>1</user_id>
   <roadlink>123456</roadlink>
   <dev_id>%s</dev_id>
	'''
#	n = 0
	while n <= devnum:
		d = xml%( int(time.time()),n)
		sender.send(Message(d));
		n+=1
	

if __name__=='__main__':
	fcd = '/home/shared/fcd/4'
#	extract_files(fcd)
#	sendLocation(fcd)

	startime = int(time.time())
	print 'start time:', startime
	
	#test_capacity(10000*200,10000*500)
	
	sendLocation(fcd)
	endtime = int(time.time())
	print 'elapsed:',endtime-startime
	
	
	


