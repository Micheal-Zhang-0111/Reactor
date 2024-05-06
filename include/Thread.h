#pragma once

#include<pthread.h>
#include<functional>

using std::function;
using std::bind;

using ThreadCallback = function<void()>;

class Thread
{
public:
    Thread(ThreadCallback &&cb);
    ~Thread();

    void start();
    void join();
private:
    //线程入口函数
    static void *threadFunc(void *arg);
private:
    pthread_t _pthid;
    bool _isRunning;
    ThreadCallback _cb;
};