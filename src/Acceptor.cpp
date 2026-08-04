#include "Acceptor.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Server.h"
#include <arpa/inet.h>
#include <stdio.h>

Acceptor::Acceptor(EventLoop *_loop) : loop(_loop)
{
    sock = new Socket();
    addr = new InetAddress("127.0.0.1", 8888);
    sock->bind(addr);
    sock->listen(); 
    sock->setnonblocking();
    acceptChannel = new Channel(loop, sock->getFd());
    //设置回调函数，使用std::bind绑定成员函数和参数
    std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
    acceptChannel->setCallback(cb);
    acceptChannel->enableReading();
}

Acceptor::~Acceptor(){
    delete sock;
    delete addr;
    delete acceptChannel;
}

void Acceptor::acceptConnection(){
    //接受一个客户端连接，需要使用accept函数 对于每一个客户端，在接受连接时也需要保存客户端的socket地址信息
    InetAddress *clnt_addr = new InetAddress();
    Socket *clnt_sock = new Socket(sock->accept(clnt_addr));
    clnt_sock->setnonblocking();
    printf("new client fd %d! IP: %s Port: %d\n",
           clnt_sock->getFd(), inet_ntoa(clnt_addr->addr.sin_addr),
           ntohs(clnt_addr->addr.sin_port));
    delete clnt_addr;
    newConnectionCallback(clnt_sock);
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> _cb){
    newConnectionCallback = _cb;
}
