
#pragma once
#include "common.h"

#include <functional>
#include <memory>

class EventLoop;
class Channel {
    public:
        DISALLOW_COPY_AND_MOVE(Channel);
        // Channel类是对socket的封装，封装了socket的fd，监听的事件，准备好的事件，以及回调函数
        Channel(int fd, EventLoop * loop);
        
        ~Channel();

        void HandleEvent() const; // 处理事件
        void EnableRead();  // 允许读
        void EnableWrite(); // 允许写
        void EnableET(); // 以ET形式触发
        void DisableWrite(); // 禁止写

        int fd() const;  // 获取fd
        short listen_events() const; // 监听的事件
        short ready_events() const; // 准备好的事件

        bool IsInEpoll() const; // 判断当前channel是否在poller中
        void SetInEpoll(bool in = true); // 设置当前状态为poller中
        

        void SetReadyEvents(int ev);
        void set_read_callback(std::function<void()> const &callback);// 设置回调函数
        void set_write_callback(std::function<void()> const &callback); // 设置回调函数

    private:
        int fd_;
        EventLoop *loop_;
        
        short listen_events_;
        short ready_events_;
        bool in_epoll_{false};
        std::function<void()> read_callback_;
        std::function<void()> write_callback_;

};