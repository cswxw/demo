int readn(int fd, void *vptr, size_t n)
{
    size_t          nleft = n;          //readn函数还需要读的字节数
    ssize_t        nread = 0;          //read函数读到的字节数
    unsigned char  *ptr = (char *)vptr; //指向缓冲区的指针

    while (nleft > 0)
    {
        nread = read(fd, ptr, nleft);
        if (-1 == nread)
        {
            if (EINTR == errno)
                nread = 0;
            else
                return -1;
        }
        else if (0 == nread)
        {
            break;
        }
        nleft -= nread;
        ptr += nread;
    }
    return n - nleft;
}
