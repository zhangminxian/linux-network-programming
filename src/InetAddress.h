#pragma once
#include <arpa/inet.h>

class InetAddress
{
public:
    //首先创建一个结构体sockaddr_in并初始化
    struct sockaddr_in addr;
    socklen_t addr_len;

    InetAddress();
    
    //然后将IP地址和端口号填入结构体中
    InetAddress(const char* ip, uint16_t port);

    ~InetAddress();
};