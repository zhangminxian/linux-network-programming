#pragma once

#include "Epoller.h"
#include "common.h"
#include <memory>

class Epoller;
class EventLoop
{
public:
    DISALLOW_COPY_AND_MOVE(EventLoop);
    EventLoop();
    ~EventLoop();

    void Loop() const;
    void UpdateChannel(Channel *ch) const;
    void DeleteChannel(Channel *ch) const;

private:
    // poller_是一个智能指针，指向Epoller对象，用于管理事件循环中的I/O事件
    std::unique_ptr<Epoller> poller_;
};