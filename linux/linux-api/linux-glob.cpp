#include <stdio.h>
#include <glob.h>
 
 int main(int argc, const char *argv[])
 {
     glob_t buf;
     int i;
     glob("/etc/rc.d/*",GLOB_NOSORT, NULL, &buf);                                                                                  
 
     for(i=0; i < buf.gl_pathc; i++)
     {
         printf("buf.gl_pathv[%d]= %s \n", i, (buf.gl_pathv[i]));
     }
 
    globfree(&buf);
     return 0;
 }

 /*
 输出:
	# ./main
	buf.gl_pathv[0]= /etc/rc.d/init.d 
	buf.gl_pathv[1]= /etc/rc.d/rc0.d 
	buf.gl_pathv[2]= /etc/rc.d/rc1.d 
	buf.gl_pathv[3]= /etc/rc.d/rc2.d 
	buf.gl_pathv[4]= /etc/rc.d/rc3.d 
	buf.gl_pathv[5]= /etc/rc.d/rc4.d 
	buf.gl_pathv[6]= /etc/rc.d/rc5.d 
	buf.gl_pathv[7]= /etc/rc.d/rc6.d 
	buf.gl_pathv[8]= /etc/rc.d/rc.local 
	#
 */


