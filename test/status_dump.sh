rm -rf /var/location.status
kill -10 `ps -eaf | grep location | awk '{print $2}'`
sleep 2
cat /var/location.status

