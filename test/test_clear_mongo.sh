service mongod stop
rm -rf /var/lib/mongo/*
service mongod start
ps -eaf | grep mongod
