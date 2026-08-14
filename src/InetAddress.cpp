#include "InetAddress.h"
#include <string.h>
InetAddress::InetAddress(){
    // 初始化sockaddr_in结构体
    bzero(&addr, sizeof(addr));
}
InetAddress::InetAddress(const char* _ip, uint16_t _port){
    //初始化结构体
    bzero(&addr, sizeof(addr));
    //设置地址族、ip地址和端口
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(_ip);
    addr.sin_port = htons(_port);
    // addr_len = sizeof(addr);
}

InetAddress::~InetAddress(){
}
void InetAddress::setInetAddr(sockaddr_in _addr){
    addr = _addr;
}

sockaddr_in InetAddress::getAddr(){
    return addr;
}

char* InetAddress::getIp(){
    return inet_ntoa(addr.sin_addr);
}

uint16_t InetAddress::getPort(){
    return ntohs(addr.sin_port);
}