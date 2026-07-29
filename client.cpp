#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "util.h"

int main() {
    //1. 创建一个socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");
    // 初始化结构体   绑定的是要连接的服务器的ip和端口号
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    //bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)); 客户端不进行bind操作
    //建立连接
    errif(connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket connect error");
    while(true){
        char buf[1024]; //定义缓冲区
        bzero(&buf, sizeof(buf));  //初始化缓冲区
        std::cout<<"请输入要发送的数据："<<std::flush;;
        scanf("%s", buf); //输入要发送的数据
        ssize_t write_bytes = write(sockfd, buf, sizeof(buf)); //发送缓冲区的数据到服务器socket，返回已发送数据大小
        if(write_bytes == -1){
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        bzero(&buf, sizeof(buf)); //清空缓冲区
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf)); //从服务器socket读到缓冲区，返回已读数据大小
        if(read_bytes > 0){
            printf("message from server: %s\n", buf);
        }else if(read_bytes == 0){
            printf("server socket disconnected!\n");
            break;
        }else if(read_bytes == -1){
            close(sockfd);
            errif(true, "socket read error");
        }
    }
    close(sockfd);
    return 0;
    
}