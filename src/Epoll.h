#pragma once
#include <sys/epoll.h>
#include <vector>

class Channel;
class Epoll
{
private:

    int epfd;
    struct epoll_event *events;
public:
    Epoll();
    ~Epoll();
    //添加文件描述符到epoll中
    void addFd(int fd, uint32_t op);

    // 更新文件描述符在epoll中的监听事件
    void updateChannel(Channel*);

    // //修改文件描述符在epoll中的监听事件
    // void modFd(int fd, uint32_t op);
    // //从epoll中删除文件描述符
    // void delFd(int fd); 
    
    //std::vector<epoll_event> poll(int timeout = -1);
    std::vector<Channel*> poll(int timeout = -1);

};