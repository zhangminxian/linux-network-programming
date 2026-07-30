#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "Socket.h"
#include "InetAddress.h"
#
#include "util.h"

#define BUFFER_SIZE 1024 

int main() {
    // 使用封装后的 Socket 和 InetAddress 创建客户端并连接服务器
    Socket *sock = new Socket();
    InetAddress *server_addr = new InetAddress("127.0.0.1", 8888);
    sock->connect(server_addr);

    while(true){
        char buf[1024];
        bzero(&buf, sizeof(buf));
        std::cout << "请输入要发送的数据：" << std::flush;

        if(scanf("%1023s", buf) == EOF){
            break;
        }

        ssize_t write_bytes = write(sock->getFd(), buf, sizeof(buf));
        if(write_bytes == -1){
            printf("socket already disconnected, can't write any more!\n");
            break;
        }

        bzero(&buf, sizeof(buf));
        ssize_t read_bytes = read(sock->getFd(), buf, sizeof(buf));
        if(read_bytes > 0){
            printf("message from server: %s\n", buf);
        }else if(read_bytes == 0){
            printf("server socket disconnected!\n");
            break;
        }else{
            perror("socket read error");
            break;
        }
    }

    delete sock;
    delete server_addr;
    return 0;
}