#!/bin/sh

DstIp=192.168.10.20

while true;do
	ping -w 1 -c 1 $DstIp >/dev/null 2>&1
	if [[ $? -eq 0 ]];then
		sleep 1
		cd /home/root
		./demo &	
		break
	fi
	sleep 1
done 
