int writen(int fd, const void *vptr, size_t n)
{
    size_t          nleft = n;  //writen函数还需要写的字节数
    ssize_t        nwrite = 0; //write函数本次向fd写的字节数
    const char*    ptr = vptr; //指向缓冲区的指针

    while (nleft > 0)
    {
        if ((nwrite = write(fd, ptr, nleft)) <= 0)
        {
            if (nwrite < 0 && EINTR == errno)
                nwrite = 0;
            else
                return -1;
        }
        nleft -= nwrite;
        ptr += nwrite;
    }
    return n;
}
