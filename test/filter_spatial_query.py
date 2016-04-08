#-- coding: utf-8 --
# 

import time,os,os.path,sys,traceback,string



def query_data():
	f1 = open('spatial_circle.txt','w')
	f2 = open('spatial_rect.txt','w')
	
	
	org=[120,30]
	end =map(lambda xy:xy+2,org)
	unit =(0.0448,0.0448)
	x,y = org
	while y < end[1]:
		x = org[0]
		while x < end[0]:
			x+=unit[0]
			# lon,lat,radius,limit,order
			f1.write('%.4f,%.4f,0.02,100,1\n'%(x,y))
			f2.write('%.4f,%.4f,0.04,0.04,100,1\n'%(x,y))
		y+=unit[1]
		
		
	f1.close()
	f2.close()
	






if __name__=='__main__':
	query_data()
