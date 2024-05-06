#pragma once

#include"MutexLock.h"
#include"Acceptor.h"
#include"TcpConnection.h"
#include<sys/epoll.h>
#include<vector>
#include<map>
#include<memory>
#include<functional>

using std::vector;
using std::map;
using std::shared_ptr;
using std::function;

using TcpConnectionPtr = shared_ptr<TcpConnection>;
using TcpConnectionCallback = function<void(const TcpConnectionPtr &)>;
using Functor = function<void()>;

class EventLoop 
{
public:
    EventLoop(Acceptor &acceptor);
    ~EventLoop();
    void loop();
    void unloop();

    void runInLoop(Functor &&cb);

    void wakeup();
    void handleRead();

    void doPengingFunctors();

    void setConnectionCallback(TcpConnectionCallback &&cb);
    void setMessageCallback(TcpConnectionCallback &&cb);
    void setCloseCallback(TcpConnectionCallback &&cb);

private:
    void waitEpollFd();
    void handleNewConnection();
    void handleMessage(int fd);
    int createEpollFd();
    int createEventFd();
    void addEpollReadFd(int fd);
    void delEpollReadFd(int fd);

private:
    int _epfd;//红黑树根节点
    int _evtfd;//eventfd的句柄
    Acceptor &_acceptor;
    bool _isLooping;
    vector<struct epoll_event> _eventList;
    map<int,TcpConnectionPtr> _conns;

    TcpConnectionCallback _onConnectionCb;
    TcpConnectionCallback _onMessageCb;
    TcpConnectionCallback _onCloseCb;

    vector<Functor> _pengingsCb;//待处理的IO
    MutexLock _mutex;
};