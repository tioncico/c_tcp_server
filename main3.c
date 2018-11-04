//server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>

#define BUF_SIZE 100

void error_handing(char *buf);

int main(int arg_length, char *argv[]) {

    /*********socket代码**********/
    char listen_ip_str[] = "0.0.0.0";
    int port = 8080;
    size_t listen_ip;
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    /*if (arg_length != 3) {
        printf("Usage: %s <listen addr> <port> \n", argv[0]);
    }*/
    listen_ip=inet_addr(listen_ip_str);


    server_sock = socket(PF_INET, SOCK_STREAM, 0);//创建socket
    bzero(&server_addr, sizeof(server_addr));//清空server_addr
//    memset(&server_addr, 0, sizeof(server_addr));//清空
    server_addr.sin_family = INADDR_ANY;
    server_addr.sin_addr.s_addr = htons(listen_ip);
    server_addr.sin_port = htons(port);
    if (bind(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        error_handing("bind() error");
    }
    if (listen(server_sock, 5) == -1) {
        error_handing("listen() error");
    }
    /**********select代码************/

//    int fd_max;
//    fd_set reads,copy_reads;
//    FD_ZERO(&reads);//将描述符清空
//    FD_SET(server_sock,&reads);
//    fd_max = server_sock;
//


    printf("监听成功\n");
    while (1) {
        addr_size = sizeof(client_addr);
        printf("正在监听连接...\n");
        client_sock = accept(server_sock, (struct sockaddr *) &client_addr, &addr_size);
        printf("%d 连接成功\n", client_sock);
        char buffer[] = "恭喜你连接成功";
        write(client_sock,buffer,sizeof(buffer));
        if (client_sock == -1) {
            break;
        }

        sleep(1);
    }
}

void error_handing(char *buf) {
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}