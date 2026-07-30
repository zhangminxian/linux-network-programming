#include "Channel.h"
#include "Epoll.h"

Channel::Channel(Epoll *_ep, int _fd) : ep(_ep), fd(_fd), events(0), revents(0), inEpoll(false){

}

Channel::~Channel()
{
}

//  监听读事件
void Channel::enableReading(){
    events = EPOLLIN | EPOLLET;
    ep->updateChannel(this);
}

int Channel::getFd(){
    return fd;
}
//  获取事件
uint32_t Channel::getEvents(){
    return events;
}
//  获取就绪事件
uint32_t Channel::getRevents(){
    return revents;
}
//  获取是否在epoll中
bool Channel::getInEpoll(){
    return inEpoll;
}
//  设置是否在epoll中
void Channel::setInEpoll(){
    inEpoll = true;
}

// void Channel::setEvents(uint32_t _ev){
//     events = _ev;
// }
//  设置就绪事件
void Channel::setRevents(uint32_t _ev){
    revents = _ev;
}