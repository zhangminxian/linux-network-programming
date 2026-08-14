#pragma once
#include <sys/epoll.h>
#include <functional>
#include <vector>
class socket;
class EventLoop;
class Epoll;
//  Channel类用于封装文件描述符和事件的相关信息
class Channel
{
private:
    EventLoop *loop; //指向所属的事件循环对象 channel属于哪个EventLoop对象
    int fd;  //文件描述符，表示该Channel所管理的文件描述符
    uint32_t events; //表示希望监听这个文件描述符的哪些事件，因为不同事件的处理方式不一样
    uint32_t ready; //表示当前这个文件描述符就绪的事件，ready是由epoll返回的
    bool inEpoll; //表示该Channel是否在epoll中
    std::function<void()> readCallback; //表示读事件的回调函数
    std::function<void()> writeCallback; //表示写事件的回调函数
public:
    
    Channel(EventLoop *_loop, int _fd);
    ~Channel();

    // 处理事件的回调函数
    void handleEvent();
    // 启用读事件
    void enableRead();

    int getFd();
    // 获取事件
    uint32_t getEvents();
    // 获取就绪事件
    uint32_t getReady();
    // 获取是否在epoll中
    bool getInEpoll();
    // 设置是否在epoll中
    void setInEpoll(bool _in = true);
    void useET();

    // void setEvents(uint32_t);
    //  设置就绪事件
    //void setRevents(uint32_t);
    // 设置回调函数
    void setReady(uint32_t);
    // 设置回调函数
    void setReadCallback(std::function<void()>);
};