#pragma once
#include <functional>
#include <string>

class EventLoop;
class Socket;
class Channel;
class Buffer;
// Connection类的作用是封装一个客户端连接，提供对客户端连接的操作接口
class Connection
{
private:
    EventLoop *loop;
    Socket *sock;
    Channel *channel;
    //回调函数，用于处理客户端连接断开时的操作
    std::function<void(Socket*)> deleteConnectionCallback;
    std::string *inBuffer;
    Buffer *readBuffer;
public:
    Connection(EventLoop *_loop, Socket *_sock);
    ~Connection();
    //处理客户端连接的读写事件
    void echo(int sockfd);
    //设置回调函数，用于处理客户端连接断开时的操作
    void setDeleteConnectionCallback(std::function<void(Socket*)>);
};