#pragma once

class InetAddress;
class Socket
{
private:
    int fd;
public:
    Socket();
    Socket(int);
    ~Socket();
    
    //建立
    void bind(InetAddress*);
    //监听
    void listen();
    //连接服务器
    void connect(InetAddress*);
    //设置非阻塞
    void setnonblocking();
    // 接受连接
    int accept(InetAddress*);

    int getFd();
};
