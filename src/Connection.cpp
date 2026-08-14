#include "Connection.h"
#include "Socket.h"
#include "Channel.h"
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <functional>
#include "Buffer.h"
#include "util.h"
#define READ_BUFFER 1024
//每一个客户端连接都对应一个Connection对象，Connection对象封装了客户端连接的socket、Channel、Buffer等信息
Connection::Connection(EventLoop *_loop, Socket *_sock) : loop(_loop), sock(_sock), channel(nullptr), readBuffer(nullptr){
    
    channel = new Channel(loop, sock->getFd()); //connection对象创建时就创建一个channel对象，channel对象封装了客户端连接的socket文件描述符和事件信息
    channel->enableRead(); //启用读事件
    channel->useET(); //使用ET模式
    std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
    //设置回调函数，使用std::bind绑定成员函数和参数
    channel->setReadCallback(cb);
    readBuffer = new Buffer();
}

Connection::~Connection(){
    delete channel;
    delete sock;
    delete readBuffer;
}

//由于客户端 socket 是非阻塞的，全部数据读完后： read() == -1；errno == EAGAIN

void Connection::echo(int sockfd){
    char buf[1024];     //这个buf大小无所谓
    while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if(bytes_read > 0){
            readBuffer->append(buf, bytes_read);
        } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
            printf("continue reading");
            continue;
        } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once\n");
            printf("message from client fd %d: %s\n", sockfd, readBuffer->c_str());
            errif(write(sockfd, readBuffer->c_str(), readBuffer->size()) == -1, "socket write error");
            readBuffer->clear();
            break;
        } else if(bytes_read == 0){  //EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            // close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
            deleteConnectionCallback(sockfd);
            break;
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(int)> _cb){
    //设置回调函数，用于处理客户端连接断开时的操作
    deleteConnectionCallback = _cb;
}
// 发送数据给客户端
void Connection::send(int sockfd){
    char buf[readBuffer->size()];
    strcpy(buf, readBuffer->c_str());
    int  data_size = readBuffer->size(); 
    int data_left = data_size; 
    while (data_left > 0) 
    { 
        ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left); 
        if (bytes_write == -1 && errno == EAGAIN) { 
            break;
        }
        data_left -= bytes_write; 
    }
}