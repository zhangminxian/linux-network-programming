#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include "util.h"
#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd){  //ET 模式，必须配合非阻塞 socket 使用。
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}
void handleReadEvent(int);

int main(){

    //1.创建一个socket   ip地址类型  数据传输方式   协议
    Socket *serv_sock = new Socket();
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8888);
    serv_sock->bind(serv_addr);
    serv_sock->listen();  

    //  epoll创建
    Epoll *ep = new Epoll();
    serv_sock->setnonblocking(); //设置为非阻塞socket
    
    // 创建一个Channel对象，将serv_sock的文件描述符和epoll对象传入
    Channel *servChannel = new Channel(ep, serv_sock->getFd());
    // 启用读事件
    servChannel->enableReading(); //将serv_sock的文件描述符加入epoll中，监听读事件

    while(true){
        // epoll_wait等待事件发生，返回就绪的文件描述符数量
        std::vector<Channel*> activeChannels = ep->poll();
        int nfds = activeChannels.size();
        for(int i = 0; i < nfds; ++i){
            if(activeChannels[i]->getFd() == serv_sock->getFd()){        //新客户端连接
                //要接受一个客户端连接，需要使用accept函数 对于每一个客户端，在接受连接时也需要保存客户端的socket地址信息
                InetAddress *clnt_addr = new InetAddress();      //会发生内存泄露！没有delete
                //接受一个客户端的连接，返回一个新的套接字 accept也是一个socket
                Socket *clnt_sock = new Socket(serv_sock->accept(clnt_addr));       //会发生内存泄露！没有delete
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
                clnt_sock->setnonblocking(); //设置为非阻塞socket

                //创建一个Channel对象，将clnt_sock的文件描述符和epoll对象传入
                Channel *clntChannel = new Channel(ep, clnt_sock->getFd());
                clntChannel->enableReading(); //将clnt_sock的文件描述符加入epoll中，监听读事件
                
            }
            else if(activeChannels[i]->getRevents() & EPOLLIN){       //否则就是已经连接的客户端发送数据给服务器端
                handleReadEvent(activeChannels[i]->getFd());  //读取数据
            } else{         //其他事件，之后的版本实现
                printf("something else happened\n");
            }
        }
    }
    delete serv_sock;
    delete serv_addr;
    return 0;
}
    
    
void handleReadEvent(int sockfd){
    char buf[READ_BUFFER];
    while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if(bytes_read > 0){
            printf("message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, sizeof(buf));
        } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
            printf("continue reading");
            continue;
        } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once, errno: %d\n", errno);
            break;
        } else if(bytes_read == 0){  //EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
            break;
        }
    }
}    

    
    