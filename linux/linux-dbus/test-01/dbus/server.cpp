#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>
#include <unistd.h>
 
void listen_signal()
{
    DBusMessage * msg;
    DBusMessageIter arg;
    DBusConnection * connection;
    DBusError err;
    int ret;
    char * sigvalue;
 
     //步骤1:建立与D-Bus后台的连接
    dbus_error_init(&err); //Initializes a DBusError structure. 
    
    
    //DBUS_BUS_SESSION 	The login session bus. 
    //DBUS_BUS_SYSTEM 	The systemwide bus. 
    //DBUS_BUS_STARTER 	The bus that started us, if any. 
    connection =dbus_bus_get(DBUS_BUS_SESSION, &err);
    if(dbus_error_is_set(&err)){
        fprintf(stderr,"ConnectionError %s\n",err.message);
        dbus_error_free(&err);
    }
    if(connection == NULL)
        return;
 
   //步骤2:给连接名分配一个可记忆名字test.singal.dest作为Bus name，这个步骤不是必须的,但推荐这样处理
   /*
   	要求总线通过在总线上调用RequestName方法将给定名称分配给此连接。
			connection	the connection
			name	the name to request
			flags	flags
			error	location to store the error 
   */
    ret =dbus_bus_request_name(connection,"test.singal.dest",DBUS_NAME_FLAG_REPLACE_EXISTING,&err);
    if(dbus_error_is_set(&err)){
        fprintf(stderr,"Name Error%s\n",err.message);
        dbus_error_free(&err);
    }
    if(ret !=DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
        return;
 
    //步骤3:通知D-Bus daemon，希望监听来行接口test.signal.Type的信号
    /*
    	添加匹配规则以匹配通过消息总线的消息。
    	
    */
    dbus_bus_add_match(connection,"type='signal',interface='test.signal.Type'",&err);
    //实际需要发送东西给daemon来通知希望监听的内容，所以需要flush
    /*
    Blocks until the outgoing message queue is empty. 
    */
    dbus_connection_flush(connection);
    if(dbus_error_is_set(&err)){
        fprintf(stderr,"Match Error%s\n",err.message);
        dbus_error_free(&err);
    }
   
    //步骤4:在循环中监听，每隔开1秒，就去试图自己的连接中获取这个信号。这里给出的是中连接中获取任何消息的方式，所以获取后去检查一下这个消息是否我们期望的信号，并获取内容。我们也可以通过这个方式来获取method call消息。
    
		/*
			只要连接打开，此函数将阻塞，直到它可以读取或写入，然后读取或写入，然后返回TRUE。
			connection	the connection 
			timeout_milliseconds	max time to block or -1 for infinite 
		*/
    while(dbus_connection_read_write(connection,-1)){
    	  
        /*
        	返回来自传入消息队列的第一个接收消息，将其从队列中删除。
        */
        while((msg =dbus_connection_pop_message (connection))){
        	int current_type;
						//if(msg == NULL){
						//    sleep(1);
						//    continue;
					  //	}

						if(dbus_message_is_signal(msg,"test.signal.Type","Test")){
								/*
									初始化DBusMessageIter以读取传入的消息的参数。
									如果可能，dbus_message_get_args（）更方便。 但是，某些类型的参数只能用DBusMessageIter读取。
									DBusMessageIter不包含已分配的内存; 它不需要被释放，可以通过赋值或memcpy（）复制。
									return value:
										FALSE if the message has no arguments 
								*/
						    if(!dbus_message_iter_init(msg,&arg)){
						    		fprintf(stderr,"MessageHas no Param");
						    }
						    /*
						    	返回消息迭代器指向的参数的参数类型。
						    	如果迭代器位于消息的末尾，则返回DBUS_TYPE_INVALID.
						    */
								while ((current_type = dbus_message_iter_get_arg_type (&arg)) != DBUS_TYPE_INVALID){
							    if(current_type == DBUS_TYPE_STRING){
											dbus_message_iter_get_basic(&arg,&sigvalue);
											printf("Got Singal withvalue : %s\n",sigvalue);							    	
							    }
							        
							    else
							    	g_printerr("Param isnot string");

									
									dbus_message_iter_next (&arg);
									
								}
  

						}
						
						dbus_message_unref(msg);
        }
        g_printerr("start next read\n");
        
        
      
    }//End of while
       
}
 
int main( int argc , char ** argv){
    listen_signal();
    return 0;
}

