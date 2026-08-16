#include <stdio.h>
#include "EventLoop.h"
#include "Server.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include <functional>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <assert.h>
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"

#define READ_BUFFER 1024

Server::Server(EventLoop *loop) : main_reactor_(loop), acceptor_(nullptr), thread_pool_(nullptr) {
    // 创建Acceptor对象，传入主线程的事件循环对象
    acceptor_ = new Acceptor(main_reactor_);
    // 设置Acceptor的回调函数，当有新的连接时，会调用Server的NewConnection函数处理连接
    std::function<void(Socket *)> cb = std::bind(&Server::NewConnection, this, std::placeholders::_1);
    acceptor_->setNewConnectionCallback(cb);

    int size = static_cast<int>(std::thread::hardware_concurrency());
    thread_pool_ = new ThreadPool(size);
    for (int i = 0; i < size; ++i) {
        sub_reactors_.push_back(new EventLoop());
    }

    for (int i = 0; i < size; ++i) {
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, sub_reactors_[i]);
        thread_pool_->add(std::move(sub_loop));
    }
}

Server::~Server() {
    delete acceptor_;
    delete thread_pool_;
}

void Server::NewConnection(Socket *sock) {
    assert(sock->getFd() != -1);
    uint64_t random = sock->getFd() % sub_reactors_.size();
    Connection *conn = new Connection(sub_reactors_[random], sock);
    std::function<void(Socket *)> cb = std::bind(&Server::DeleteConnection, this, std::placeholders::_1);
    conn->SetDeleteConnectionCallback(cb);
    conn->SetOnConnectCallback(on_connect_callback_);
    connections_[sock->getFd()] = conn;
}

void Server::DeleteConnection(Socket *sock) {
  int sockfd = sock->getFd();
  auto it = connections_.find(sockfd);
  if (it != connections_.end()) {
    Connection *conn = connections_[sockfd];
    connections_.erase(sockfd);
    delete conn;
    conn = nullptr;
  }
}

void Server::OnConnect(std::function<void(Connection *)> fn) { 
    on_connect_callback_ = std::move(fn); 
}
