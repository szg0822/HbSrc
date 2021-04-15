#!/bin/sh

szg_path="/home/root/szg/"

echo "Wait WIFI Connect..." >$szg_path/demo_wifi.log

DstIp=192.168.10.20

while true;do
	ping -w 1 -c 1 $DstIp >/dev/null 2>&1
	if [[ $? -eq 0 ]];then
		sleep 1
		echo "WIFI Connect Successed!" >>$szg_path/demo_wifi.log
		cd $szg_path
		./demo &	
		break
	fi
	sleep 1
done 

