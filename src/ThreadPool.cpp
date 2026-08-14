#include "ThreadPool.h"

//线程池构造函数
ThreadPool::ThreadPool(int size) : stop(false){
    //创建线程池中的线程
    for(int i = 0; i < size; ++i){
        threads.emplace_back(std::thread([this](){
            while(true){
                // 从任务队列中取出任务并执行
                std::function<void()> task;
                {
                    //加锁，防止多个线程同时访问任务队列
                    std::unique_lock<std::mutex> lock(tasks_mtx);
                    //等待条件变量，直到有任务到来或者线程池停止
                    cv.wait(lock, [this](){
                        return stop || !tasks.empty();
                    });
                    //如果线程池已经停止并且任务队列为空，则退出循环
                    if(stop && tasks.empty()) return;
                    //从任务队列中取出任务
                    task = tasks.front();
                    //从任务队列中移除任务
                    tasks.pop();
                }
                //执行任务
                task();
            }
        }));
    }
}

ThreadPool::~ThreadPool(){
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        stop = true;
    }
    //通知所有线程退出
    cv.notify_all();
    //等待所有线程退出
    for(std::thread &th : threads){
        if(th.joinable())
            th.join();
    }
}
// //添加任务到线程池
// void ThreadPool::add(std::function<void()> func){
//     {
//         //加锁，防止多个线程同时访问任务队列
//         std::unique_lock<std::mutex> lock(tasks_mtx);
//         //如果线程池已经停止，则抛出异常
//         if(stop)
//             throw std::runtime_error("ThreadPool already stop, can't add task any more");
//         //将任务添加到任务队列中
//         tasks.emplace(func);
//     }
//     //通知一个线程有新任务到来
//     cv.notify_one();
// }