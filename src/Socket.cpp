#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

Socket::Socket() : fd(-1){
    ////1.创建一个socket   ip地址类型  数据传输方式   协议
    fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(fd == -1, "socket create error");
}
Socket::Socket(int _fd) : fd(_fd){
    errif(fd == -1, "socket create error");
}

Socket::~Socket(){
    if(fd != -1){
        close(fd);
        fd = -1;
    }
}

void Socket::bind(InetAddress *addr){
    //绑定 socket和文件描述符   绑定的时候要转化为通用socket地址（sockaddr）
    errif(::bind(fd, (sockaddr*)&addr->addr, addr->addr_len) == -1, "socket bind error");
}

void Socket::listen(){
    //监听这个socket端口   第二个参数是监听函数的最大监听队列长度 SOMAXCONN=128
    errif(::listen(fd, SOMAXCONN) == -1, "socket listen error");
}

void Socket::connect(InetAddress *addr){
    //连接服务器   连接的时候要转化为通用socket地址（sockaddr）
    errif(::connect(fd, (sockaddr*)&addr->addr, addr->addr_len) == -1,
          "socket connect error");
}

void Socket::setnonblocking(){

    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress *addr){
    //要接受一个客户端连接，需要使用accept函数 对于每一个客户端，在接受连接时也需要保存客户端的socket地址信息
    //接受一个客户端的连接，返回一个新的套接字 accept也是一个socket
    int clnt_sockfd = ::accept(fd, (sockaddr*)&addr->addr, &addr->addr_len);
    errif(clnt_sockfd == -1, "socket accept error");
    return clnt_sockfd;
}

int Socket::getFd(){
    return fd;
}
