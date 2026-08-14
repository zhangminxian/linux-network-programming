#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <strings.h>
#include <errno.h>
#include <stdio.h>

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

void Socket::bind(InetAddress *_addr){
    
    struct sockaddr_in addr = _addr->getAddr();
    //绑定 socket和文件描述符   绑定的时候要转化为通用socket地址（sockaddr）
    errif(::bind(fd, (sockaddr*)&addr, sizeof(addr)) == -1, "socket bind error");
}

void Socket::listen(){
    //监听这个socket端口   第二个参数是监听函数的最大监听队列长度 SOMAXCONN=128
    errif(::listen(fd, SOMAXCONN) == -1, "socket listen error");
}

void Socket::setnonblocking(){

    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress *_addr){
    //要接受一个客户端连接，需要使用accept函数 对于每一个客户端，在接受连接时也需要保存客户端的socket地址信息
    //接受一个客户端的连接，返回一个新的套接字 accept也是一个socket
    int clnt_sockfd=-1;
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    socklen_t addr_len = sizeof(addr);
    if(fcntl(fd, F_GETFL) & O_NONBLOCK){
        while(true){
            clnt_sockfd = ::accept(fd, (sockaddr*)&addr, &addr_len);
            if(clnt_sockfd == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
                // printf("no connection yet\n");
                continue;
            } else if(clnt_sockfd == -1){
                errif(true, "socket accept error");
            } else{
                break;
            }
        }
    }
    else{
        clnt_sockfd = ::accept(fd, (sockaddr*)&addr, &addr_len);
        errif(clnt_sockfd == -1, "socket accept error");
    }
    _addr->setInetAddr(addr);
    return clnt_sockfd;
}

void Socket::connect(InetAddress *_addr){
    // for client socket
    //连接服务器端的socket   连接的时候要转化为通用socket地址（sockaddr）
    struct sockaddr_in addr = _addr->getAddr();
    if(fcntl(fd, F_GETFL) & O_NONBLOCK){
        while(true){
            int ret = ::connect(fd, (sockaddr*)&addr, sizeof(addr));
            if(ret == 0){
                break;
            } else if(ret == -1 && (errno == EINPROGRESS)){
                continue;
            /* 连接非阻塞式sockfd建议的做法：
                The socket is nonblocking and the connection cannot be
              completed immediately.  (UNIX domain sockets failed with
              EAGAIN instead.)  It is possible to select(2) or poll(2)
              for completion by selecting the socket for writing.  After
              select(2) indicates writability, use getsockopt(2) to read
              the SO_ERROR option at level SOL_SOCKET to determine
              whether connect() completed successfully (SO_ERROR is
              zero) or unsuccessfully (SO_ERROR is one of the usual
              error codes listed here, explaining the reason for the
              failure).
              这里为了简单、不断连接直到连接完成，相当于阻塞式
            */
            } else if(ret == -1){
                errif(true, "socket connect error");
            }
        }
    } else{
        errif(::connect(fd, (sockaddr*)&addr, sizeof(addr)) == -1, "socket connect error");
    }
}

int Socket::getFd(){
    return fd;
}
