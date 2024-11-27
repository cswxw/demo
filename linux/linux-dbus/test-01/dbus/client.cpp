#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>
#include <unistd.h>
 
int send_a_signal( char * sigvalue)
{
    DBusError err;
    DBusConnection * connection;
    DBusMessage * msg;
    DBusMessageIter arg;
    dbus_uint32_t  serial =0;
    int ret;
 
    //步骤1:建立与D-Bus后台的连接
    
    dbus_error_init(&err);
     
    connection =dbus_bus_get(DBUS_BUS_SESSION ,&err );
    if(dbus_error_is_set(&err)){
        fprintf(stderr,"ConnectionErr : %s\n",err.message);
        dbus_error_free(&err);
    }
    if(connection == NULL)
        return -1;
 
    //步骤2:给连接名分配一个well-known的名字作为Bus name，这个步骤不是必须的，可以用if 0来注释着一段代码，我们可以用这个名字来检查，是否已经开启了这个应用的另外的进程。
#if 1
    ret =dbus_bus_request_name(connection,"test.singal.source",DBUS_NAME_FLAG_REPLACE_EXISTING,&err);
    if(dbus_error_is_set(&err)){
        fprintf(stderr,"Name Err :%s\n",err.message);
        dbus_error_free(&err);
    }
    if(ret !=DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
        return -1;
#endif
 
    //步骤3:发送一个信号
    //根据图，我们给出这个信号的路径（即可以指向对象），接口，以及信号名，创建一个Message
    /*
			path	the path to the object emitting the signal
			iface	the interface the signal is emitted from
			name	name of the signal 
    */
    if((msg =dbus_message_new_signal("/test/signal/Object","test.signal.Type","Test"))== NULL){
        fprintf(stderr,"MessageNULL\n");
        return -1;
    }
    //给这个信号（messge）具体的内容
    //初始化DBusMessageIter以将参数附加到消息的末尾。
    dbus_message_iter_init_append(msg,&arg);
    
    /*
    	在消息中附加基本类型值。
				iter	the append iterator
				type	the type of the value
				value	the address of the value 
    */
   if(!dbus_message_iter_append_basic(&arg,DBUS_TYPE_STRING,&sigvalue)){
        fprintf(stderr,"Out OfMemory!\n");
        return -1;
    }
    
#if 1
    char *str="this is two";
   if(!dbus_message_iter_append_basic(&arg,DBUS_TYPE_STRING,&str)){
        fprintf(stderr,"Out OfMemory!\n");
        return -1;
    }
    
#endif
 
    //步骤4: 将信号从连接中发送
    /*
    	将消息添加到传出消息队列。
    	不阻止将消息写入网络; 这是异步发生的。 要强制写入消息，请调用dbus_connection_flush（），但不必手动调用dbus_connection_flush（）; 消息将在下次运行主循环时发送。 仅应使用dbus_connection_flush（），例如，如果应用程序在运行主循环之前需要退出。
    	一旦此方法返回，就可以调用dbus_message_unref（），因为消息队列将保留自己的ref，直到发送消息。
    */
    if( !dbus_connection_send(connection,msg,&serial)){
        fprintf(stderr,"Out of Memory!\n");
        return -1;
    }
    dbus_connection_flush(connection);
    printf("Signal Send\n");
 
   //步骤5: 释放相关的分配的内存。
    dbus_message_unref(msg );
    return 0;
}
int main( int argc , char ** argv){
   send_a_signal("Hello,world!");
    return 0;
}

