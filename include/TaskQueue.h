#pragma once

#include"MutexLock.h"
#include"Condition.h"
#include<functional>
#include<queue>

using std::function;
using std::queue;

using Task = function<void()>;
// using Elem = function<void()>;

class TaskQueue
{
public:
    TaskQueue(size_t queSize);
    ~TaskQueue();
    bool empty() const;
    bool full() const;
    void push(Task &&value);
    Task pop();

    //将所有的等在在_notEmpty上的线程唤醒
    void wakeup();
private:
    size_t _queSize;
    queue<Task> _que;
    MutexLock _mutex;
    Condition _notFull;
    Condition _notEmpty;
    //_flag 是一个标志位，用于控制任务队列的操作。它的作用是在任务队列中的任务被消费完后，标记队列已经关闭，不再接受新的任务。
    bool _flag;
};

