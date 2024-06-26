#pragma once

#include"./NonCopyable.h"
#include<pthread.h>


class MutexLock;//前向声明

class Condition
: NonCopyable
{
public:
    Condition(MutexLock &mutex);
    ~Condition();
    void wait();
    void notify();
    void notifyAll();

private:
    pthread_cond_t _cond;
    MutexLock &_mutex;

};