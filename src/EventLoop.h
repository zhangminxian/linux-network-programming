#pragma once
class Epoll;
class Channel;
// 处理事件循环的类
class EventLoop
{
private:
    // 事件循环的状态
    Epoll *ep;
    bool quit;
public:
    EventLoop();
    ~EventLoop();

    // 事件循环的主函数
    void loop();
    // 更新Channel的事件
    void updateChannel(Channel*);
};