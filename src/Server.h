#pragma once
#include <map>

class EventLoop;
class Socket;
class Acceptor;
class Connection;
class Server
{
private:
    EventLoop *loop;
    Acceptor *acceptor;
    // 用于保存客户端连接的map，key为客户端socket的文件描述符，value为对应的Connection对象
    std::map<int, Connection*> connections;
public:
    Server(EventLoop*);
    ~Server();

    // 处理读事件的回调函数
    void handleReadEvent(int);
    // 处理新连接的回调函数
    void newConnection(Socket *sock);
    void deleteConnection(Socket *sock);
};
