/*************************************************************************
	> File Name: ds18b20.c
	> Author: izobs
	> Mail: ivincentlin@gmail.com 
	> Created Time: 2013年07月13日 星期六 14时01分52秒
 ************************************************************************/

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>

int main(int arc,char **argv)
{
    int fd;
    unsigned char result[2] = {0};
    float temp;

    fd = open("/dev/DS18B20",0);
    if(fd < 0)
    {
        perror("open device failed\n");
        exit(EXIT_FAILURE);
    }    

    while(1)
    {
        read(fd,&result,sizeof(result));
        temp = ((result[1]<<8)|result[0])*0.0625;
        printf("app:the temp is %f \n",temp);
        
    }

}


