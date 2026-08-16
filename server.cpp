#include "src/EventLoop.h"
#include "src/Server.h"
#include "src/Socket.h"
#include "src/Connection.h"
#include <stdio.h>
#include <iostream>

int main() {
    // 创建事件循环对象和服务器对象
    EventLoop *loop = new EventLoop();
    // 创建服务器对象，传入事件循环对象
    Server *server = new Server(loop);
    // 设置连接回调函数，当有新的连接时，会调用这个回调函数
    server->OnConnect([](Connection *conn) {
        conn->Read();
        if (conn->GetState() == Connection::State::Closed) {
            conn->Close();
            return;
        }
        std::cout << "Message from client " << conn->GetSocket()->getFd() << ": " << conn->ReadBuffer() << std::endl;
        conn->SetSendBuffer(conn->ReadBuffer());
        conn->Write();
    });
    loop->loop();
    delete server;
    delete loop;
    return 0;
    
}