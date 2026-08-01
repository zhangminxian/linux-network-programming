#pragma once
#include <sys/epoll.h>
#include <functional>
#include <vector>
class EventLoop;
class Epoll;
//  Channel类用于封装文件描述符和事件的相关信息
class Channel
{
private:
    EventLoop *loop;
    int fd;
    uint32_t events; //表示希望监听这个文件描述符的哪些事件，因为不同事件的处理方式不一样
    uint32_t revents; //表示实际发生的事件，revents是由内核返回的，events是用户设置的
    bool inEpoll; //表示该Channel是否在epoll中
    std::function<void()> callback; //回调函数，用于处理事件
public:
    
    Channel(EventLoop *_loop, int _fd);
    ~Channel();

    // 处理事件的回调函数
    void handleEvent();
    // 启用读事件
    void enableReading();

    int getFd();
    // 获取事件
    uint32_t getEvents();
    // 获取就绪事件
    uint32_t getRevents();
    // 获取是否在epoll中
    bool getInEpoll();
    // 设置是否在epoll中
    void setInEpoll();

    // void setEvents(uint32_t);
    //  设置就绪事件
    void setRevents(uint32_t);
    // 设置回调函数
    void setCallback(std::function<void()>);
};