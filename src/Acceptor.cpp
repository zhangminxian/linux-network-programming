#include "Acceptor.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Server.h"
#include <arpa/inet.h>
#include <stdio.h>

Acceptor::Acceptor(EventLoop *_loop) : loop(_loop), sock(nullptr), acceptChannel(nullptr)
{
    sock = new Socket();
    InetAddress *addr = new InetAddress("127.0.0.1", 8888);
    sock->bind(addr);
    sock->listen(); 
    //sock->setnonblocking();
    acceptChannel = new Channel(loop, sock->getFd());//创建监听channel
    //设置回调函数，使用std::bind绑定成员函数和参数
    //把accpetConnection函数绑定到Channel的回调函数上，当有新的连接时，Channel会调用acceptConnection函数处理连接
    std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
    acceptChannel->setReadCallback(cb);
    acceptChannel->enableRead();
    delete addr;
    
}

Acceptor::~Acceptor(){
    delete sock;
    delete acceptChannel;
}

void Acceptor::acceptConnection(){
    //接受一个客户端连接，需要使用accept函数 对于每一个客户端，在接受连接时也需要保存客户端的socket地址信息
    InetAddress *clnt_addr = new InetAddress();
    Socket *clnt_sock = new Socket(sock->accept(clnt_addr));
    clnt_sock->setnonblocking();
    printf("new client fd %d! IP: %s Port: %d\n",
           clnt_sock->getFd(), clnt_addr->getIp(), clnt_addr->getPort());
    newConnectionCallback(clnt_sock);
    delete clnt_addr;
    //delete clnt_sock;
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> _cb){
    newConnectionCallback = _cb;
}
