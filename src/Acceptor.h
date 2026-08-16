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
    //Channel对象用于封装监听socket和事件的相关信息
    Channel *acceptChannel;
    //回调函数用于处理新的连接请求，参数为新连接的Socket对象
    std::function<void(Socket*)> newConnectionCallback; 
public:
    Acceptor(EventLoop *_loop);
    ~Acceptor();
    void acceptConnection();
    void setNewConnectionCallback(std::function<void(Socket*)>);
};