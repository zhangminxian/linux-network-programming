#include <iostream>
#include <string>
#include "src/ThreadPool.h"

void print(int a, double b, const char *c, std::string d){
    std::cout << a << b << c << d << std::endl;
}

void test(){
    std::cout << "hellp" << std::endl;
}

int main(int argc, char const *argv[])
{
    //创建线程池对象，默认线程池大小为10
    ThreadPool *poll = new ThreadPool();
    //创建任务对象，使用std::bind绑定函数和参数
    std::function<void()> func = std::bind(print, 1, 3.14, "hello", std::string("world"));
    //将任务添加到线程池中
    poll->add(func);
    //创建任务对象，使用lambda表达式
    func = test;
    //将任务添加到线程池中
    poll->add(func);
    
    delete poll;
    return 0;
}