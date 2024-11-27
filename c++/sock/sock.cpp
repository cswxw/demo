int readline(char* buf, int size){
	int ret = 0;
	int i = 0;
	while(1){
		ret = SSL_read(m_ssl, buf + i,1);
		if(ret == 0) break;
		if(ret > 0){
			i += ret;
			if(i >= 2){
				if(buf[i - 1] == '\n' && buf[i - 2] == '\r'){
					break;
				}
			}
		}
	}
	return 1;
	
}
int RecvAll(void *data, unsigned int  size)
{
	unsigned int  recv_size, sz, ret;
	
	if (size == 0)
	{
		return 1;
	}


	recv_size = 0;

	while (1)
	{
		sz = size - recv_size;
		ret = SSL_read(m_ssl, (unsigned char *)data + recv_size, sz);
		if (ret == 0)
		{
			return -1;
		}
		recv_size += ret;
		if (recv_size >= size)
		{
			return 1;
		}
	}
}
