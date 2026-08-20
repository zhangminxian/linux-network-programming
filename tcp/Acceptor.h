#pragma once
#include "common.h"

#include <memory>
#include <functional>

class EventLoop;
class Channel;

class Acceptor{
    public:
        DISALLOW_COPY_AND_MOVE(Acceptor);
        // Acceptor类是对socket的封装，封装了socket的fd，监听的事件，以及回调函数
        Acceptor(EventLoop *loop, const char * ip, const int port);
        ~Acceptor();
        // 设置新连接的回调函数
        void set_newconnection_callback(std::function<void(int)> const &callback);
        
        // 创建socket
        void Create();

        // 与ip地址绑定
        void Bind(const char *ip, const int port);
        
        // 监听Socket
        void Listen();

        // 接收连接
        void AcceptConnection();

    private:
        EventLoop *loop_;
        int listenfd_;
        std::unique_ptr<Channel> channel_;
        std::function<void(int)> new_connection_callback_;
};