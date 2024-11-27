1.写一个简单小程序

	#include<stdio.h>
	#include<stdlib.h>
	 
	int main(int argc,char **argv)
	{
	　　while(1)
	　　{
	　　　　printf("hello world\n");
	　　　　sleep(2);//2s
	　　}
	}

2.gcc编译

    gcc -o hello hello.c
	./hello

3.在/etc/init.d/目录下生成hello.sh脚本

	#!/bin/bash
	 
	SERVERNAME="hello"
	 
	start()
	{
	    echo "start $SERVERNAME"
	    /home/yao/projects/$SERVERNAME
	    echo "start $SERVERNAME ok!"
	    exit 0;
	}
	 
	stop()
	{
	    echo "stop $SERVERNAME"
	    killall $SERVERNAME
	    echo "stop $SERVERNAME ok!"
	}
	 
	case "$1" in
	start)
	    start
	    ;;
	stop)
	    stop
	    ;;
	restart)
	    stop
	    start
	    ;;
	*)
	    echo "usage: $0 start|stop|restart"
	    exit 0;
	esac
	exit

4.更改脚本文件属性
	
	chmod +x hello.sh

5.运行

(1) 启动

	root@localhost:/home/yao/projects# service hello.sh start
	start hello
	hello world

(2) 停止：

	root@localhost:/home/yao/projects# service hello.sh stop
	stop hello
	stop hello ok!

(3) 重启：

	root@localhost:/etc/init.d# service hello.sh restart
	stop hello
	stop hello ok!
	start hello
	hello world
	hello world