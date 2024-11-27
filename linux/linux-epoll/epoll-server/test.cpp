#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <dirent.h>  
#include <stdlib.h>  
#include <string.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
int isrfile(char *path)
{
	struct stat buf;
	int cc;
	int euid,egid;
	cc=stat(path,&buf);
	if(!cc) {
		//S_IFMT是一个掩码，它的值是017000（注意这里用的是八进制）用来过滤出前四位表示的文件类型
		if((buf.st_mode & S_IFMT) != S_IFREG) return 0;
		euid=geteuid(); //返回有效用户的ID
		egid=getegid();  //取得执行目前进程有效组识别码
		if(euid==0) {
			if(buf.st_mode & S_IRUSR || buf.st_mode & S_IRGRP ||
			   buf.st_mode & S_IROTH)
				 return 1;
			else return 0;
		}
		if((buf.st_mode & S_IROTH)!=0) return 1;   //S_IROTH group has read permission
		if((buf.st_gid == egid) && ((buf.st_mode & S_IRGRP)!=0))   //S_IRGRP group has read permission 
				return 1;
		if((buf.st_uid == euid) && ((buf.st_mode & S_IRUSR)!=0))   //S_IRUSR owner has read permission
				return 1;

	}
	return cc;
}  

int checkFileExist(char* filepath){
    if((access(filepath,F_OK))!=-1)  
    {  
        return -1;
    } 
	
    struct stat s_buf;  
  

    stat(filepath,&s_buf); 
	if(S_ISREG(s_buf.st_mode))  
	{  
		return 1;
	}  
	return 0;
}  
int main(int argc, char **argv)  
{  
	printf("sizeof(size_t):%d\n",sizeof(size_t));
	return 0;
}
