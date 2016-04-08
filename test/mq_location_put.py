#!/usr/bin/env python

import sys
from qpid.messaging import *
import time

xml='''<location user_id="1" dev_id="2000" lon="121.3455" lat="31.245" time="1349674863" speed="45.23"
	direction="245" roadlink="12345678" group_ids="1001,1002,1003"/>'''

xml='''
   <gps lon="121.3455" lat="31.245" time="1349674863" speed="45.23" direction="245"/>
   <group_ids>1001,1002,1003</group_ids>
   <user_id>1</user_id>
   <roadlink>123456</roadlink>
   <dev_id>2000</dev_id>
'''

sys.argv.append('')

broker =  "localhost:5672" 
address = "q1"

connection = Connection(broker)

try:
	connection.open()
	session = connection.session()
	sender = session.sender(address)
	while True:
		sender.send(Message(xml));
		if sys.argv[1] != 'keeping':
			break
		time.sleep(1)
	print 'send one'
except MessagingError,m:
	print m

connection.close()
