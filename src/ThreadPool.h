#pragma once
#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
//线程池类
class ThreadPool
{
private:
    //线程池大小
    std::vector<std::thread> threads;
    //任务队列
    std::queue<std::function<void()>> tasks;
    //任务队列互斥锁
    std::mutex tasks_mtx;
    //条件变量
    std::condition_variable cv;
    //停止标志
    bool stop;
public:

    ThreadPool(int size = 10);
    ~ThreadPool();

    //添加任务到线程池
    void add(std::function<void()>);

};