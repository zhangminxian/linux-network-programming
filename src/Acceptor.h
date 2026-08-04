#pragma once
#include <functional>

class EventLoop;
class Socket;
class InetAddress;
class Channel;
// Acceptor类的作用是监听客户端的连接请求，并在有新的连接时调用回调函数处理连接
class Acceptor
{
private:
    //Acceptor类的成员变量包括EventLoop对象、监听socket、监听地址和Channel对象
    EventLoop *loop;
    Socket *sock;
    InetAddress *addr;
    Channel *acceptChannel;
public:
    Acceptor(EventLoop *_loop);
    ~Acceptor();
    void acceptConnection();
    std::function<void(Socket*)> newConnectionCallback;
    void setNewConnectionCallback(std::function<void(Socket*)>);
};