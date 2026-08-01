#include <stdio.h>
#include "EventLoop.h"
#include "Server.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include <functional>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define READ_BUFFER 1024

Server::Server(EventLoop *_loop) : loop(_loop){    
    //1.创建一个socket   ip地址类型  数据传输方式   协议
    Socket *serv_sock = new Socket();
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8888);
    serv_sock->bind(serv_addr);
    serv_sock->listen();
    serv_sock->setnonblocking(); //设置为非阻塞socket

    //创建一个Channel对象，将serv_sock的文件描述符和epoll对象传入 
    Channel *servChannel = new Channel(loop, serv_sock->getFd());
    //设置回调函数，使用std::bind绑定成员函数和参数
    std::function<void()> cb = std::bind(&Server::newConnection, this, serv_sock);
    servChannel->setCallback(cb);
    servChannel->enableReading();

}

Server::~Server()
{
    
}
// 处理读事件的回调函数
void Server::handleReadEvent(int sockfd){
    //读取客户端发送的数据
    char buf[READ_BUFFER];
    while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if(bytes_read > 0){
            printf("message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, sizeof(buf));
        } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
            printf("continue reading");
            continue;
        } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once, errno: %d\n", errno);
            break;
        } else if(bytes_read == 0){  //EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
            break;
        }
    }
}

void Server::newConnection(Socket *serv_sock){
    //要接受一个客户端连接，需要使用accept函数 对于每一个客户端，在接受连接时也需要保存客户端的socket地址信息
    InetAddress *clnt_addr = new InetAddress();      //会发生内存泄露！没有delete
    Socket *clnt_sock = new Socket(serv_sock->accept(clnt_addr));       //会发生内存泄露！没有delete
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
    clnt_sock->setnonblocking();
    Channel *clntChannel = new Channel(loop, clnt_sock->getFd());
    std::function<void()> cb = std::bind(&Server::handleReadEvent, this, clnt_sock->getFd());
    clntChannel->setCallback(cb);
    clntChannel->enableReading();
}