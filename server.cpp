#include<iostream>
#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include"util.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd){  //ET 模式，必须配合非阻塞 socket 使用。
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main(){

    //1.创建一个socket   ip地址类型  数据传输方式   协议
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd==-1,"socket creat error");

    //2.将socket绑定到一个ip和端口上

    //首先创建一个结构体
    struct sockaddr_in serv_addr;
    //初始化结构体
    bzero(&serv_addr, sizeof(serv_addr));
    //设置地址族、ip地址和端口
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    
    //3. 绑定 socket和文件描述符   绑定的时候要转化为通用socket地址（sockaddr）
    errif(bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr))==-1,"socket bind error");
     //4. 监听这个socket端口   第二个参数是监听函数的最大监听队列长度 SOMAXCONN=128
    errif(listen(sockfd, SOMAXCONN)==-1,"socket listen error");
    
    // ！！epoll创建
    int epfd = epoll_create1(0);
    errif(epfd == -1, "epoll create error");

    struct epoll_event events[MAX_EVENTS], ev;
    bzero(&events, sizeof(events));  //初始化
    bzero(&ev, sizeof(ev));
    ev.data.fd = sockfd;  
    ev.events = EPOLLIN | EPOLLET; //采用边缘触发ET
    setnonblocking(sockfd);  //设置为非阻塞socket
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev); //监听socket事件放入epoll中

    
    while (true) {
        //等待事件发生
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        errif(nfds == -1, "epoll wait error");
        for(int i=0;i<nfds;i++){
            if(events[i].data.fd==sockfd){ //新客户连接
                //4. 要接受一个客户端连接，需要使用accept函数 对于每一个客户端，在接受连接时也需要保存客户端的socket地址信息
                struct sockaddr_in clnt_addr;
                socklen_t clnt_addr_len = sizeof(clnt_addr);
                bzero(&clnt_addr, sizeof(clnt_addr));
                //接受一个客户端的连接，返回一个新的套接字 accept也是一个socket
                int clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len);
                errif(clnt_sockfd==-1,"socket accpet error!");
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
                //连接成功之后  将这个socket加入epoll中
                bzero(&ev, sizeof(ev));
                ev.data.fd = clnt_sockfd;
                ev.events = EPOLLIN | EPOLLET;
                setnonblocking(clnt_sockfd);
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sockfd, &ev); //添加到epoll中
            }
            else if (events[i].events & EPOLLIN)
            {
                char buf[READ_BUFFER]; //定义缓冲区
                while(true){
                    bzero(&buf, sizeof(buf)); //清空缓冲区
                    ssize_t bytes_read = read(events[i].data.fd, buf, sizeof(buf));
                    if(bytes_read > 0){
                        printf("message from client fd %d: %s\n", events[i].data.fd, buf);
                        write(events[i].data.fd, buf, sizeof(buf));
                    } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
                        printf("continue reading");
                        continue;
                    } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
                        printf("finish reading once, errno: %d\n", errno);
                        break;
                    } else if(bytes_read == 0){  //EOF，客户端断开连接
                        printf("EOF, client fd %d disconnected\n", events[i].data.fd);
                        close(events[i].data.fd);   //关闭socket会自动将文件描述符从epoll树上移除
                        break;
                    }
                }
            }
            else{         //其他事件，之后的版本实现
                printf("something else happened\n");
            }
            
        }
    }
    close(sockfd);
    return 0;
}