/*
flock
参数  operation有下列四种情况:

　　LOCK_SH 建立共享锁定。多个进程可同时对同一个文件作共享锁定。

　　LOCK_EX 建立互斥锁定。一个文件同时只有一个互斥锁定。

　　LOCK_UN 解除文件锁定状态。

　　LOCK_NB 无法建立锁定时，此操作可不被阻断，马上返回进程。通常与LOCK_SH或LOCK_EX 做OR(|)组合。

　　单一文件无法同时建立共享锁定和互斥锁定，而当使用dup()或fork()时文件描述词不会继承此种锁定。

　　返回值  返回0表示成功，若有错误则返回-1，错误代码存于errno。 



*/

    #include <stdio.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <errno.h>
    #include <sys/file.h>

    int main()
    {
        int fd,i;
        char path[]="test.txt";
        char s[]="writing.../nwriting......\n";
        fd=open(path,O_RDWR|O_CREAT|O_TRUNC);
        if(fd!=-1)
        {

            if(flock(fd,LOCK_EX|LOCK_NB)==0)
            {
                printf("the file was locked by the process.\n");
                if(-1!=write(fd,s,128))
                {
                    printf("write %s to the file %s\n",s,path);
                }
                else
                {
                    printf("cannot write the file %s\n",path);
                }
                   
            }
            else
            {
                printf("the file was locked by other process.Can't write...\n");
            }
            flock(fd,LOCK_UN);
            close(fd);
        }
        else
        {
            printf("cannot open file %s\n",path);
        }
    }







    #include <stdio.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <errno.h>

    int main()
    {
        int fd,i;
        char path[]="test.txt";
        fd=open(path,O_RDWR|O_CREAT);
        if(fd!=-1)
        {
            if(flock(fd,LOCK_EX)==0)
            {
                printf("the file was locked.\n");
            }
            else
            {
                printf("the file was not locked.\n");
            }
            sleep(10);
            if(flock(fd,LOCK_UN)==0)
            {
                printf("the file was unlocked.\n");
            }
            else
            {
                printf("the file was not unlocked.\n");
            }
            close(fd);
        }
        else
        {
            printf("cannot open file %s\n",path);
        }
    }



