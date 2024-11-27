#!/bin/bash

if [ $1 ];then
	exefile=$1
	while [ 1 ]
	do
		num=`ps -e|grep xxx|wc -l`
		if [ $num -lt 1 ];then
			if [ $2 ];then
				$exefile $2 &
			else
				$exefile &
			fi
		fi
		sleep 60
	done
fi
