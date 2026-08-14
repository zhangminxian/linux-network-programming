#pragma once
#include <arpa/inet.h>

class InetAddress
{
private:
    //首先创建一个结构体sockaddr_in并初始化
    struct sockaddr_in addr;
public:

    InetAddress();
    
    //然后将IP地址和端口号填入结构体中
    InetAddress(const char* ip, uint16_t port);

    ~InetAddress();
    void setInetAddr(sockaddr_in _addr);
    sockaddr_in getAddr();
    char* getIp();
    uint16_t getPort();


};