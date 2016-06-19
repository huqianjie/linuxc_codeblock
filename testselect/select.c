#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

#include <libgen.h>

#include <errno.h>
///使用select io 复用来实现一个简单的服务起
#define ARGUMENT_NUM 3
#define RECV_NUM 1024
int main(int argc, char * argv[])
{
    int retVal = 0;
    int loop  = 1;
    int listen_fd,service_fd;
    char recv_buf[RECV_NUM];
    if(argc < ARGUMENT_NUM)
    {
        printf("useage:%s ip port\n",argv[0]);
        goto finish;
    }

    const char *ip = argv[1]; //监听的ip地址
    short port = (short)atoi(argv[2]); //监听的端口
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    bzero(&client_addr, sizeof(client_addr));
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    //创建监socket
    if( (listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "[ERR] create listen socket error\n");
        goto finish;
    }

    //bind Ip 地址
    retVal = bind(listen_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr));
    if(retVal == -1)
    {

        fprintf(stderr, "[ERR] errno:[%s] bind listen socket error\n", strerror(errno));
        goto finish;
    }
    //监听 socket
    retVal = listen(listen_fd, 65535);
    if(retVal == -1)
    {

        fprintf(stderr, "[ERR] errno:[%s]  listen socket error\n", strerror(errno));
        goto finish;
    }

    service_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_addr_len );
    if(service_fd == -1)
    {

        fprintf(stderr, "[ERR] errno:[%s]  accept connection error\n", strerror(errno));
        goto finish;
    }

    fd_set read_fds;
    FD_ZERO(&read_fds);
    while(loop)
    {
        bzero(recv_buf, RECV_NUM);
        //使用select

       FD_SET(service_fd, &read_fds);
       retVal = select(service_fd+1, &read_fds, NULL, NULL,NULL);//监听读事件,一直阻塞知道读事件发生
       if(retVal == -1)
       {
            fprintf(stderr, "[ERR] errno:[%s]  select function error\n", strerror(errno));
            goto finish;
       }

       if(FD_ISSET(service_fd, &read_fds))
       {
           ///读事件就绪，可以来读
            retVal = recv(service_fd, recv_buf, RECV_NUM-1, 0);
            if(retVal == -1)
            {
                fprintf(stderr, "[ERR] errno:[%s]  recv function error\n", strerror(errno));
                goto finish;
            }
            if(retVal == 0)
            {
                fprintf(stderr, "[INF] connect was shutdown by peer\n");
                goto finish;
            }

            printf("[INF] get %d bytes of normal data: %s", retVal, recv_buf );
            printf("[INF] get %d bytes of normal data: %2d %2d %2d\n", retVal,recv_buf[retVal-2], recv_buf[retVal-1], recv_buf[retVal] );
            fflush(stdout);
       }

    }

finish:
    if(listen_fd > 0)
        close(listen_fd);
     if(service_fd > 0)
        close(service_fd);
    return retVal;
}
