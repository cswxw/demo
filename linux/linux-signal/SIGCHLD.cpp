

//方法1
/**************************************************************************************************
**  函数名称:  start_proc
**  功能描述:  启动进程开始运行
**  输入参数:  无
**  输出参数:  无
**  返回参数:  启动成功返回1，启动失败则返回0
**  注意事项:  这里要fork两次，利用系统孤儿进程的回收机制来处理。否则，会出现僵尸进程。具体做法是
**  注意事项:  利用一代子进程再产生二代子进程，同时将一代子进程结束掉，并在父进程中进行收尸处理
**************************************************************************************************/
int start_proc_by_name(const char *procname)
{
    pid_t first_pid, second_pid;
    char filename[100];

    sprintf(filename, "%s%s", PROC_FILE_PATH, procname);

    printf("going to start proc by filename \"%s\"...\n", filename);

    if (access(filename, X_OK | F_OK) != 0)
    {
        /* 确认文件属性是否正确 */
        printf("access filename \"%s\" fail!!!\n", filename);

        return 0;

    } first_pid = fork();

    /* 创建一代子进程 */
    if (first_pid < 0)   /* 一代子进程创建失败 */
    {
        printf("fork 1st child_proc fail!!!\n");
        return 0;

    }
    else if (first_pid == 0)   /* 创建成功，此处是一代子进程的代码 */
    {
        printf("fork 1st child_proc success. this is 1st child_proc going to fork 2nd child_proc...\n");
        second_pid = fork();/* 创建二代孙进程 */ 
        if (second_pid < 0)/* 二代孙进程创建失败 */ 
        {
            printf("fork 2nd child_proc fail!!!\n");
            return 0;
        }
        else if (second_pid == 0)
        {
            /* 创建成功，此处是二代孙进程的代码 */
            printf("fork 2nd child_proc success. this is 2nd child_proc running...\n");
            if (execl(filename, procname, (char *)NULL) != -1)
            {
                /* 在子进程中执行该程序 */ printf("2nd child_proc: execl proc \"%s\" success!\n",
                                                            procname);
                return 1;
                /* 执行完毕直接退出 */
            }
            else
            {
                printf("2nd child_proc: execl proc \"%s\" fail!\n", procname);
                return 0;
            }
        }
        else
        {
            /* 创建成功，此处是一代子进程的代码 */
            printf("fork 2nd child_proc success. 1st child_proc exiting...\n");
            sleep(1);///是否是必须的？
            exit(1);
        }
    }
    else
    {
        /* 创建成功，此处是父进程的代码 */
        printf("fork 1st child_proc success. father_proc continue to run...\n");
        if (waitpid(first_pid, NULL, 0) != first_pid)
        {
            /* 父进程必须为一代子进程收尸 */
            printf("fork 1st child_proc success. 1st child_proc killed...\n");
        }
    } 
    return 1;

}

//方法2
//创建捕捉子进程退出信号
signal(SIGCHLD,sig_child);

static void sig_child(int signo)
{
     pid_t        pid;
     int        stat;
     //处理僵尸进程
     while ((pid = waitpid(-1, &stat, WNOHANG)) >0)
            printf("child %d terminated.\n", pid);
}


//方法3：
signal(SIGCHLD,SIG_IGN);





