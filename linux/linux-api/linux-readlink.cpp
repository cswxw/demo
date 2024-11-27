#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(){
	char current_absolute_path[256];
	//获取当前程序绝对路径
	int cnt = readlink("/proc/self/exe", current_absolute_path, sizeof(current_absolute_path));
	if (cnt < 0 || cnt >= sizeof(current_absolute_path))
	{
		printf("***Error***\n");
		exit(-1);
	}
	//获取当前目录绝对路径，即去掉程序名
	int i;
	for (i = cnt; i >=0; --i)
	{
		if (current_absolute_path[i] == '/')
		{
		    current_absolute_path[i+1] = '\0';
		    break;
		}
	}
	printf("current absolute path:%s\n", current_absolute_path); 
	
	
}

