#include <stdio.h>
#include <arpa/inet.h>//inet_addr() sockaddr_in
#include <string.h>//bzero()
#include <sys/socket.h>//socket
#include <unistd.h>
#include <stdlib.h>//exit()

#define BUFFER_SIZE 1024

int main() {
    char listen_addr_str[] = "0.0.0.0";
    size_t listen_addr = inet_addr(listen_addr_str);
    int port = 8080;
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[BUFFER_SIZE];//缓冲区大小

    size_t client_arr[100];//存储客户端数组
    int client_length=0;//记录客户端数量

    int str_length;

    server_socket = socket(PF_INET, SOCK_STREAM, 0);//创建套接字

    bzero(&server_addr, sizeof(server_addr));//初始化
    server_addr.sin_family = INADDR_ANY;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = listen_addr;

    if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        printf("绑定失败\n");
        exit(1);
    }
    if (listen(server_socket, 5) == -1) {
        printf("监听失败\n");
        exit(1);
    }

    printf("创建tcp服务器成功\n");

    while (1) {
        addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &addr_size);
        client_arr[client_length] = client_socket;
        client_length++;
        printf("%d 连接成功\n", client_socket);
        char msg[] = "恭喜你连接成功";
        write(client_socket, msg, sizeof(msg));

        for (int i = 0; i < client_length; ++i) {
            if (client_arr[i]==0){
                continue;
            }
            str_length = read(client_arr[i], buffer, BUFFER_SIZE);
            if (str_length == 0)    //读取数据完毕关闭套接字
            {
                close(client_arr[i]);
                client_arr[i]=0;
                printf("连接已经关闭: %d \n", client_arr[i]);
                break;
            } else {
                printf("客户端发送数据:%s",buffer);
                write(client_arr[i], buffer, str_length);//发送数据
            }
        }
    }
}