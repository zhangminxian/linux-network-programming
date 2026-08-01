#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include <vector>

EventLoop::EventLoop() : ep(nullptr), quit(false){
    ep = new Epoll();
}

EventLoop::~EventLoop()
{
    delete ep;
}


void EventLoop::loop(){
    while(!quit){
    std::vector<Channel*> chs;
        chs = ep->poll();
        //处理就绪的事件
        for(auto it = chs.begin(); it != chs.end(); ++it){
            //调用Channel的回调函数处理事件
            (*it)->handleEvent();
        }
    }
}
// 更新Channel的事件                                                                                                            

void EventLoop::updateChannel(Channel *ch){
    ep->updateChannel(ch);
}