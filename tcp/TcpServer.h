#pragma once
#include "common.h"
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>
class EventLoop;
class TcpConnection;
class Acceptor;
class ThreadPool;
class TcpServer
{
    public:
    DISALLOW_COPY_AND_MOVE(TcpServer);
    TcpServer(const char *ip, const int port);
    ~TcpServer();

    void Start();

    void set_connection_callback(std::function < void(TcpConnection *)> const &fn);
    void set_message_callback(std::function < void(TcpConnection *)> const &fn);

    void HandleClose(int fd);
    void HandleNewConnection(int fd);

    private:
        std::unique_ptr<EventLoop> main_reactor_; // 主事件循环
        int next_conn_id_; // 下一个连接的ID
        
        std::unique_ptr<Acceptor> acceptor_; // 监听器，用于接收新连接
        std::vector<std::unique_ptr<EventLoop>> sub_reactors_; // 子事件循环，用于处理已连接的客户端请求
        //std::unordered_map<int, std::unique_ptr<TcpConnection>> connectionsMap_; // 连接映射表，存储所有已连接的客户端连接
	std::unordered_map<int, TcpConnection *> connectionsMap_;// 连接映射表，存储所有已连接的客户端连接
        
        std::unique_ptr<ThreadPool> thread_pool_; // 线程池，用于处理客户端请求的任务

        std::function<void(TcpConnection *)> on_connect_; // 新连接的回调函数
        std::function<void(TcpConnection *)> on_message_; // 接收到消息的回调函数

};