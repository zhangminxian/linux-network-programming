#pragma once

class EventLoop;
class Socket;
class Server
{
private:
    EventLoop *loop;
public:
    Server(EventLoop*);
    ~Server();

    // 处理读事件的回调函数
    void handleReadEvent(int);
    // 处理新连接的回调函数
    void newConnection(Socket *serv_sock);
};