#pragma once
#include <map>
#include <vector>

class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;
class Server
{
private:
    EventLoop *mainReactor; //主事件循环，只负责监听新连接
    Acceptor *acceptor; //管理监听 socket，执行 accept()
    // 用于保存客户端连接的map，key为客户端socket的文件描述符，value为对应的Connection对象
    std::map<int, Connection*> connections; //保存当前所有客户端连接
    // 用于保存子反应堆的vector，每个子反应堆对应一个EventLoop对象
    std::vector<EventLoop*> subReactors; //保存多子事件循环
    ThreadPool *thpool; //给每个subReactor分配一个线程，线程池管理这些线程
public:
    Server(EventLoop*);
    ~Server();

    // 处理读事件的回调函数
    void handleReadEvent(int);
    // 处理新连接的回调函数
    void newConnection(Socket *sock);
    void deleteConnection(int sockfd);
};
