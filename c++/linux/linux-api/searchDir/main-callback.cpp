#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/file.h> 
#include <sys/stat.h>
#include <unistd.h>  

#define RESULT_FILE_NAME "result.txt"

#define MAX_PATH 1000

char filename[256][MAX_PATH];
int len = 0;

//回调函数声明
typedef void (*callback)(const char *, char *);

/*
*   filename	:	文件的路径
*   buf 		:	需要写入的数据
*/
void writeFileByAppend(const char * filename, char * buf) {
	//追加方式
	FILE* fp = fopen(filename, "a+");
	if(fp == NULL) {
		printf("fopen error \n");
		return;
	}
	//if (flock(fp->_fileno, LOCK_EX) != 0) printf("file lock by others\n");  
	fwrite(buf, sizeof(char),strlen(buf), fp);
	fwrite("\n", sizeof(char),strlen("\n"), fp);
	//flock(fp->_fileno, LOCK_UN); //文件解锁  
	fclose(fp);
	
	
}

/*
*   basePath	:	基路径
*   name 		:	文件名称
*/
void filecallback(const char *basePath, char * name){
	sprintf(filename[len++], "d_name:%s/%s", basePath, name);
	writeFileByAppend(RESULT_FILE_NAME, name);
}

/*
*   basePath	:	基路径
*   call 		:	回调函数
*	isRecursion	:	是否对子目录进行递归遍历
*/
int readFileList(const char *basePath, callback call, int isRecursion)
{
    DIR *dir;
    struct dirent *ptr;
    char base[MAX_PATH];

    if ((dir=opendir(basePath)) == NULL)
    {
        perror("Open dir error...");
        return -1;
    }

    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
            continue;
        /*else if(ptr->d_type == DT_REG){    ///file DT_REG = 8 DT_REG 普通文件
			call(basePath, ptr->d_name);
		}
        else if(ptr->d_type == DT_LNK){    ///link file   DT_LNK = 10  link文件
			call(basePath, ptr->d_name);
		}
        else if(ptr->d_type == DT_DIR && isRecursion == 1){    ///DT_DIR 普通目录 DT_DIR = 4
            memset(base,'\0',sizeof(base));
            strcpy(base,basePath);
            strcat(base,"/");
            strcat(base,ptr->d_name);
            readFileList(base, call, isRecursion);
        }*/
		else{
				//DT_UNKNOWN，未知的类型
				//DT_REG，普通文件
				//DT_DIR，普通目录
				//DT_FIFO，命名管道或FIFO
				//DT_SOCK，本地套接口
				//DT_CHR，字符设备文件
				//DT_BLK，块设备文件
				//...
				//
				//S_ISLNK (st_mode)     判断是否为符号连接  
				//S_ISREG (st_mode)     是否为一般文件  
				//S_ISDIR (st_mode)      是否为目录  
				//S_ISCHR (st_mode)     是否为字符三设备文件  
				//S_ISBLK (s3e)           是否为先进先出  
				//S_ISSOCK (st_mode)   是否为socket  
			char fullPath[MAX_PATH];
			struct stat st;
			sprintf(fullPath, "%s/%s", basePath, ptr->d_name);
			stat(fullPath, &st);
			if(!S_ISDIR(st.st_mode)){  // 排除目录
				call(basePath, ptr->d_name);
			}
		}
    }
    closedir(dir);
    return 1;
}


int main()
{
    int isRecursion = 1;
    int i=0;

	readFileList("/home/adcsw/code",filecallback, isRecursion);
    for(i = 0; i < len; i++)
    {
        printf("%s \n", filename[i]);
    }
	printf("total : %d \n", i);
    printf("\n");
    return 0;
}