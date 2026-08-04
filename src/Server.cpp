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
#include <arpa/inet.h>
#include <sys/socket.h>
#include "Acceptor.h"
#include "Connection.h"

#define READ_BUFFER 1024

Server::Server(EventLoop *_loop) : loop(_loop), acceptor(nullptr){ 
    acceptor = new Acceptor(loop);
    std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);
}

Server::~Server()
{
    for(auto &connection : connections){
        delete connection.second;
    }
    delete acceptor;
    
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

void Server::newConnection(Socket *sock){
    //当有新的客户端连接时，创建一个Connection对象，并将其保存到connections map中
    Connection *conn = new Connection(loop, sock);
    //设置回调函数，当客户端连接断开时，调用Server的deleteConnection方法删除对应的Connection对象
    std::function<void(Socket*)> cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
    //设置回调函数，用于处理客户端连接断开时的操作
    conn->setDeleteConnectionCallback(cb);
    connections[sock->getFd()] = conn;
}

void Server::deleteConnection(Socket * sock){
    //当客户端连接断开时，从connections map中删除对应的Connection对象，并释放内存
    auto it = connections.find(sock->getFd());
    if(it == connections.end()){
        return;
    }
    Connection *conn = it->second;
    connections.erase(it);
    delete conn;
}
