#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include "basement.h"

int main()
{
    const char * datafile_name = "data.txt";
    int pipe_fd = -1;
    int data_fd = -1;
    int num_send = 0;
    char buf[BUF_SIZE+1]; //为了防止流缓冲加入一个字节好计算
    int num_read = 0;
    int num_write = 0;
    int loop = 1;
    //判断命名管道是否存在
    if(access(FIFO_NAME, F_OK) == RET_FAILURE)
    {
        //不存在则创建一个命名管道
        if(mkfifo(FIFO_NAME, 0777) == RET_FAILURE)
        {
            fprintf(stderr, "[ERR] mkfifo failed\n");
            goto finish;
        }

    }
    //以只写方式和阻塞方式打开命名管道
    pipe_fd =  open(FIFO_NAME, O_WRONLY); //此方式为阻塞方式打开管道文件
    //打开数据文件
    data_fd =  open(datafile_name, O_RDWR);
    assert(pipe_fd != -1 && data_fd != -1);
    //向命名管道中写入数据
    while(loop)
    {
        //读取数据文件并且向管道写入数据
        num_read = read(data_fd, buf, BUF_SIZE);
        if(num_read == -1)
        {
            fprintf(stderr, "[ERR] read data_fd failed\n");
            goto finish;
        }
        if(num_read == 0)
        {
            fprintf(stdout, "[INF] read data_fd end\n");
            goto finish;
        }
        //buf[num_read+1] = '\0';///!!!!!航缓冲导致的
        while(num_send < num_read)
        {
                num_write = write(pipe_fd, buf, num_read);
                num_send =+ num_write;
        }
        num_send = 0;

    }
    //关闭数据文件和命名管道
    close(pipe_fd);
    close(data_fd);
finish:
    return 0;
}
