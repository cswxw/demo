/*
根据/proc/{pid}/status,遍历进程
*/
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
typedef struct{
    pid_t pid;
    char name[256];//进程名称
    int vmsize;//虚拟内存信息
}proc_info_st;//保存读取的进程信息

#define PROC_NAME_LINE 1//名称所在行
#define PROC_PID_LINE 6//pid所在行
#define PROC_VMSIZE_LINE 14//虚拟内存所在行
#define BUFF_LEN 1024 //行缓冲区的长度

#ifndef TRUE
# define TRUE 1
#endif

#ifndef FALSE
# define FALSE 0
#endif

void read_proc(proc_info_st* info,const char* c_pid);//读取进程信息
int read_line(FILE* fp,char* buff,int b_l,int l);//读取一行

int main()
{
    //打开目录
    DIR *dir;
    struct dirent *ptr;
    if (!(dir = opendir("/proc")))
        return 0;
    //读取目录
    while (ptr = readdir(dir))
    {//循环读取出所有的进程文件
        if (ptr->d_name[0] > '0' && ptr->d_name[0] <= '9')
        {
            //获取进程信息
            proc_info_st info;
            read_proc(&info,ptr->d_name);//读取信息
            printf("pid:%d\tpname:%s\tvmsize:%d\n",info.pid,info.name,info.vmsize);
            
           // printf("\n\n");//再空两行
        }
    }
}


/**************************************************
 **说明:根据进程pid获取进程信息,存放在proc_info_st结构体中
 **
 **输入:
 **        /proc_info_st* info  返回进程信息
 **        /char* c_pid  进程pid的字符串形式
 **
 **
 **
 *************************************************/
void read_proc(proc_info_st* info,const char* c_pid)
{
    FILE* fp = NULL;
    char file[512] = {0};
    char line_buff[BUFF_LEN] = {0};//读取行的缓冲区
    
    sprintf(file,"/proc/%s/status",c_pid);//读取status文件
    if (!(fp = fopen(file,"r")))
    {
        printf("read %s file fail!\n",file);
        return;
    }

    char name[32];
    //先读取进程名称
    if (read_line(fp,line_buff,BUFF_LEN,PROC_NAME_LINE))
    {
        sscanf(line_buff,"%s %s",name,(info->name));
    }
   
    fseek(fp,0,SEEK_SET);//回到文件头部
    //读取进程pid
    if (read_line(fp,line_buff,BUFF_LEN,PROC_PID_LINE))
    {
        sscanf(line_buff,"%s %d",name,&(info->pid));
    }

    fseek(fp,0,SEEK_SET);//回到文件头部
    //读取进程vmsize
    if (read_line(fp,line_buff,BUFF_LEN,PROC_VMSIZE_LINE))
    {
        sscanf(line_buff,"%s %d",name,&(info->vmsize));
    }


    fclose(fp);
    
}


/**************************************************
 **说明:读取文件的一行到buff
 **
 **输入:
 **     /FILE* fp  文件指针
 **        /char* buff  缓冲区
 **     /int b_l  缓冲区的长度
 **        /l  指定行
 **
 **输出:
 **     /true 读取成功
 **     /false 读取失败
 *************************************************/
int read_line(FILE* fp,char* buff,int b_l,int l)
{
    if (!fp)
        return FALSE;
    
    char line_buff[b_l];
    int i;
    //读取指定行的前l-1行,转到指定行
    for (i = 0; i < l-1; i++)
    {
        if (!fgets (line_buff, sizeof(line_buff), fp))
        {
            return FALSE;
        }
    }

    //读取指定行
    if (!fgets (line_buff, sizeof(line_buff), fp))
    {
        return FALSE;
    }

    memcpy(buff,line_buff,b_l);

    return TRUE;
}
