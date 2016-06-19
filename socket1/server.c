#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
///学习使用流套接字来传递数据

int main()
{
    int retVal = 0;
    typedef struct sockaddr_in SERVER_ADDR_T;
    typedef struct sockaddr_in CLIENT_ADDR_T;
    int listen_socket;
    int client_socket;
    int loop = 1;
    int  client_addr_len = 0;
    char buf='\0';
    pid_t pid;
    SERVER_ADDR_T server_addr;
    CLIENT_ADDR_T client_addr;///用于链接的客户端
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port        = htons(10000);
    ///创建监听套接字
    listen_socket =  socket(AF_INET, SOCK_STREAM, 0);
    if(listen_socket == -1)
    {
        fprintf(stderr, "[ERR] create the listen socket failed\n");
        goto finish;
    }
    ///给套接字命名
    retVal = bind(listen_socket,(struct sockaddr *)&server_addr, sizeof(SERVER_ADDR_T));
    if(retVal == -1)
    {
        fprintf(stderr, "[ERR] bind the listen socket failed\n");
        goto finish;
    }
    ///创建套接字队列
    retVal = listen(listen_socket, 65535);
    if(retVal == -1)
    {
        fprintf(stderr, "[ERR] listen socket failed\n");
        goto finish;
    }
    //signal(SIGCHLD, SIG_IGN);!!!!!!!!!!!!!!1
    while(loop)
    {
        printf("[DEBUG] 11111111111111111\n");
        client_addr_len = sizeof(CLIENT_ADDR_T);
        ///接受链接
        fprintf(stdout, "[INF] accept the client now...\n");
        fflush(stdout);
        client_socket = accept(listen_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if(client_socket == -1)
        {
            fprintf(stderr, "[ERR] accept a client socket  error\n");
            break;
        }
        ///创建子进程来新建的链接服务
        pid = fork();
        if(pid == 0)
        {
            //child
            printf("[DEBUG] into the service process\n");
read_leb:    if(read(client_socket, &buf, 1) != 1)
                goto read_leb;
             buf++;
write_leb:   if(write(client_socket, &buf,1) != 1)
                goto write_leb;

             close(client_socket);
            printf("[DEBUG] out the service process\n");
             exit(1);

        }
        else if(pid > 0)
        {
            //parent
            wait(NULL);
            printf("[DEBUG] parent close the client socket now\n");
            close(client_socket);
        }
        else
        {
            //fork error
        }
    }
finish:
    if(listen_socket != -1)
        close(listen_socket);
    return retVal;

}
