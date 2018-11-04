#include <stdio.h>
#include <arpa/inet.h>//inet_addr() sockaddr_in
#include <string.h>//bzero()
#include <sys/socket.h>//socket
#include <unistd.h>
#include <stdlib.h>//exit()
#include <sys/epoll.h> //epoll

#define BUFFER_SIZE 1024
#define CLIENT_MAX_SIZE 1024

int main() {
    char listen_addr_str[] = "0.0.0.0";
    size_t listen_addr = inet_addr(listen_addr_str);
    int port = 8080;
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[BUFFER_SIZE];//缓冲区大小
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


    struct epoll_event event;//监听事件
    struct epoll_event wait_event_list[CLIENT_MAX_SIZE];//监听结果
    int fd[CLIENT_MAX_SIZE];
    int j = 0;
    int epoll_fd = epoll_fd = epoll_create(10);//创建epoll句柄,里面的参数10没有意义
    if (epoll_fd == -1) {
        printf("创建epoll句柄失败\n");
        exit(1);
    }
    event.events = EPOLLIN;//可读事件
    event.data.fd = server_socket;//server_socket

    int result = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket, &event);

    if (result == -1) {
        printf("注册epoll 事件失败\n");
        exit(1);
    }

    while (1) {
        result = epoll_wait(epoll_fd, wait_event_list, CLIENT_MAX_SIZE, -1);//阻塞
        if (result <= 0) {
            continue;
        }

        for (j = 0; j < result; j++) {
            printf("%d 触发事件 %d \n", wait_event_list[j].data.fd, wait_event_list[j].events);
            //server_socket触发事件
            if (server_socket == wait_event_list[j].data.fd && EPOLLIN == wait_event_list[j].events & EPOLLIN) {
                addr_size = sizeof(client_addr);
                client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &addr_size);
                printf("%d 连接成功\n", client_socket);
                char msg[] = "恭喜你连接成功";
                write(client_socket, msg, sizeof(msg));


                //遍历fd数组,将client_socket存储到空键名中
                event.data.fd = client_socket;
                event.events = EPOLLIN;//可读或错误
                result = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &event);
                if (result == -1) {
                    printf("注册客户端 epoll 事件失败\n");
                    exit(1);
                }
                continue;
            }

            //客户端触发事件
            if ((wait_event_list[j].events & EPOLLIN)
                ||(wait_event_list[j].events & EPOLLERR))//可读或发生错误
            {
                memset(&buffer, 0, sizeof(buffer));
                str_length = read(wait_event_list[j].data.fd, buffer, BUFFER_SIZE);
                if (str_length == 0)    //读取数据完毕关闭套接字
                {
                    close(wait_event_list[j].data.fd);
                    event.data.fd = wait_event_list[j].data.fd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, wait_event_list[j].data.fd, &event);
                    printf("连接已经关闭: %d \n", wait_event_list[j].data.fd);
                } else {
                    printf("客户端发送数据:%s \n", buffer);
                    write(wait_event_list[j].data.fd, buffer, str_length);//执行回声服务  即echo
                }

            }
        }
    }

//    return 0;
}