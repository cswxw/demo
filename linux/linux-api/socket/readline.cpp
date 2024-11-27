readline函数功能：读到'\n'或者读满缓冲区才返回

readline函数实现：

static ssize_t readch(int fd, char *ptr)
{
    static int          count = 0;
    static char*        read_ptr = 0;
    static char        read_buf[1024*4] = {0};

    if (count <= 0)
    {
    again:
        count = read(fd, read_buf, sizeof(read_buf));
        if (-1 == count)
            if (EINTR == errno)
                goto again;
            else
                return -1;
        else if (0 == count)
            return 0;
        read_ptr = read_buf;
    }
    count--;
    *ptr = *read_ptr++;
    return 1;
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t        i = 0;
    ssize_t        ret = 0;
    char            ch = '\0';
    char*          ptr = NULL;

    ptr = (char *)vptr;

    for (i = 1; i < maxlen; ++i)
    {
        ret = readch(fd, &ch);
        if (1 == ret)
        {
            *ptr++ = ch;
            if ('\n' == ch)
                break;
        }
        else if (0 == ret)
        {
            *ptr = 0;
            return i-1;
        }
        else
            return -1;
    }
    *ptr = 0;
    return i;
}
