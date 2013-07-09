#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
	int fd;

	fd = open("test.txt",O_RDWR); //打开一个文件
	if(fd == -1)
	{
		
	}
}
