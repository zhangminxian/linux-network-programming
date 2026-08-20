/******************************
线程池的实现
******************************/

#pragma once
#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include "common.h"

class ThreadPool
{
private:
    std::vector<std::thread> threads; // 线程池中的线程
    std::queue<std::function<void()>> tasks; // 任务队列，存放待执行的任务
    std::mutex tasks_mtx; // 互斥锁，用于保护任务队列的访问
    std::condition_variable cv; // 条件变量，用于线程间的同步
    std::atomic<bool> stop_{false}; // 标志位，表示线程池是否停止运行 

public:
    ThreadPool(int size = std::thread::hardware_concurrency());// 构造函数，初始化线程池，默认线程数为硬件并发数
    ~ThreadPool();

    // void add(std::function<void()>);
    template<class F, class... Args>
    auto Add(F&& f, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type>;

};


//不能放在cpp文件，C++编译器不支持模版的分离编译
template<class F, class... Args>
auto ThreadPool::Add(F&& f, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared< std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);

        // don't allow enqueueing after stopping the pool
        if(stop_)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task](){ (*task)(); });
    }
    cv.notify_one();
    return res;
}