#include "Epoll.h"
#include "util.h"
#include <unistd.h>
#include <string.h>
#include "Channel.h"

#define MAX_EVENTS 1000

Epoll::Epoll() : epfd(-1), events(nullptr){
    //1. 创建一个epoll实例
    epfd = epoll_create1(0);
    errif(epfd == -1, "epoll create error");
    // 2. 分配内存用于存储事件
    events = new epoll_event[MAX_EVENTS];
    // 初始化事件数组
    bzero(events, sizeof(*events) * MAX_EVENTS);
}

Epoll::~Epoll(){
    if(epfd != -1){
        //关闭epoll实例
        close(epfd);
        epfd = -1;
    }
    delete [] events;
}

void Epoll::addFd(int fd, uint32_t op){
    //添加文件描述符到epoll中
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.fd = fd;
    ev.events = op;
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add event error");
}

// std::vector<epoll_event> Epoll::poll(int timeout){
//     //等待事件发生
//     std::vector<epoll_event> activeEvents;
//     //调用epoll_wait等待事件发生，返回就绪的文件描述符数量
//     int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
//     errif(nfds == -1, "epoll wait error");
//     for(int i = 0; i < nfds; ++i){
//         //将就绪的事件添加到activeEvents中
//         activeEvents.push_back(events[i]);
//     }
//     return activeEvents;
// }

std::vector<Channel*> Epoll::poll(int timeout){
    //等待事件发生
    std::vector<Channel*> activeChannels;
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error");
    for(int i = 0; i < nfds; ++i){
        Channel *ch = (Channel*)events[i].data.ptr;
        ch->setRevents(events[i].events);
        activeChannels.push_back(ch);
    }
    return activeChannels;
}

void Epoll::updateChannel(Channel *channel){
    int fd = channel->getFd(); //拿到channel的文件描述符
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.ptr = channel;
    ev.events = channel->getEvents();  //拿到Channel希望监听的事件
    if(!channel->getInEpoll()){
        errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");//如果Channel不在epoll中，就添加到epoll中
        channel->setInEpoll();
        // debug("Epoll: add Channel to epoll tree success, the Channel's fd is: ", fd);
    } else{
        errif(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify error"); //如果Channel已经在epoll中，就修改它的监听事件
        // debug("Epoll: modify Channel in epoll tree success, the Channel's fd is: ", fd);
    }
}