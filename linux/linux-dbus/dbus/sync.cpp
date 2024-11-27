#include <stdio.h>
#include <stdlib.h>
#include <dbus/dbus-glib.h>

int main( int argc , char ** argv)
{
    GError * error;
    DBusGConnection * connection;
    DBusGProxy    * proxy;
    char    * string;

   /* GType初始化 */
   g_type_init();
    error = NULL;

    /* dbus_g_bus_get用来建立连接，这里和session bus连接，也可以通过DBUS_BUS_SYSTEM与系统总线连接*/
    connection = dbus_g_bus_get (DBUS_BUS_SESSION, & error);
    if(connection == NULL){
        g_printerr ("Failed to open connection to bus : %s/n",error->message);
        g_error_free( error);
        exit( 1 );
    }
    
    
		//./dbus-1.0/dbus/dbus-shared.h:#define DBUS_INTERFACE_INTROSPECTABLE "org.freedesktop.DBus.Introspectable"
    /* Create a proxy object用来代表远端org.freedesktop.Notifications是系统带有的，可以使用DBUS_INTERFACE_INTROSPECTABLE等定义来标识它*/
    proxy = dbus_g_proxy_new_for_name (connection,
                                 "org.freedesktop.Notifications" /* service */ , 
                                 "/" /* path */ ,
                                 "org.freedesktop.DBus.Introspectable" /* interface,可以使用宏定义DBUS_INTERFACE_INTROSPECTABLE */ 
                                 );

    error = NULL;

    /* 采用同步方式，第二个参数是 method，第三个参数是 error，
    	后面是不定长的参数，
    		输入参数(类型,变量) + G_TYPE_INVALID + 输出参数(类型,变量) + G_TYPE_INVALID 
    	以G_TYPE_INVALID标识接收，每个参数由GType类型和存贮参数值的地址构成。
    	在这个例子中，输入没有参数，输入有一个参数，是string
    	
    	用于同步调用方法和接收回复值的命令。
    */
    if( !dbus_g_proxy_call (proxy,"Introspect",&error, 
    								G_TYPE_INVALID,G_TYPE_STRING, &string, G_TYPE_INVALID) ){
        if(error->domain == DBUS_GERROR && error->code == DBUS_GERROR_REMOTE_EXCEPTION)
            g_printerr("Caught remote method exception %s:%s",dbus_g_error_get_name(error),error->message);
        else
            g_printerr("Error : %s/n", error->message);
        g_error_free(error);
        exit (1);
    }

    g_print("Message Method return from bus:\n%s\n",string);

    g_free(string);
    g_object_unref(proxy);
    return 0;
}