#include "InetAddress.h"
#include <string.h>
InetAddress::InetAddress() : addr_len(sizeof(addr)){
    // 初始化sockaddr_in结构体
    bzero(&addr, sizeof(addr));
}
InetAddress::InetAddress(const char* ip, uint16_t port) : addr_len(sizeof(addr)){
    //初始化结构体
    bzero(&addr, sizeof(addr));
    //设置地址族、ip地址和端口
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    addr_len = sizeof(addr);
}

InetAddress::~InetAddress(){
}
