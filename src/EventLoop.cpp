#include"EventLoop.h"
#include<unistd.h>
#include <sys/eventfd.h>

EventLoop::EventLoop(Acceptor &acceptor)
:_epfd(createEpollFd()),
_evtfd(createEventFd()),
_acceptor(acceptor),
_isLooping(false),
_eventList(1024)
{
    //把listenfd放在红黑树上进行监听
    addEpollReadFd(acceptor.fd());
}

EventLoop::~EventLoop(){
    if(_epfd){
        close(_epfd);
    }
}

void EventLoop::loop(){
    _isLooping = true;
    while(_isLooping){
        waitEpollFd();
    }
}

void EventLoop::unloop(){
    _isLooping = false;
}

void EventLoop::runInLoop(Functor &&cb)
{
    //可以使用大括号将某些栈变量/栈对象提前结束
    {
        MutexLockGuard autoLock(_mutex);
        _pengingsCb.push_back(std::move(cb));
    }

    //....
    //...
    wakeup();
}

void EventLoop::doPengingFunctors()
{
    vector<Functor> tmp;
    {
        //粒度
        MutexLockGuard autoLock(_mutex);
        tmp.swap(_pengingsCb);
    }

    //vector<Functor> _pengingsCb;
    for(auto &cb : tmp)
    {
        cb();
    }
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    int ret = ::write(_evtfd, &one, sizeof(one));
    if(ret != sizeof(one))
    {
        perror("write");
        return;
    }
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    int ret = ::read(_evtfd, &one, sizeof(one));
    if(ret != sizeof(one))
    {
        perror("read");
        return;
    }
}

void EventLoop::waitEpollFd(){
    int readyNum = -1;
    do{ 
        readyNum = epoll_wait(_epfd,&*_eventList.begin(),_eventList.size(),5000);
    }while(readyNum == -1 && errno == EINTR);
    if(readyNum == -1){
        perror("readyNUm == -1");
        return;
    }
    else if(readyNum == 0){
        printf(">>epoll_wait timeout\n");
    }
    else{
        if(readyNum == (int)_eventList.capacity()){
            //考虑到监听超过上限的问题
            _eventList.resize(2*readyNum);
        }
        for(int i = 0; i < readyNum; ++i){
            int fd = _eventList[i].data.fd;
            if(_acceptor.fd() == fd){
                //过来的是服务器的fd
                //有新的连接请求进来
                //_eventList[i].events & EPOLLIN的含义是检查_eventList[i]事件的events字段中是否包含EPOLLIN标志，即判断该事件是否为读事件。
                if(_eventList[i].events & EPOLLIN){
                    handleNewConnection();
                }
            }
            else{
                if(_eventList[i].events & EPOLLIN){
                    handleMessage(fd);//有数据的传输
                }
            }
        }
    }
}



void EventLoop::handleNewConnection(){
    int peerfd = _acceptor.accept();
    addEpollReadFd(peerfd);//把peerfd放到红黑树上进行监听
    TcpConnectionPtr con(new TcpConnection(peerfd));
    //TCP创建之后进行三个事件的注册
    con->setConnectionCallback(_onConnectionCb);
    con->setMessageCallback(_onMessageCb);
    con->setCloseCallback(_onCloseCb);

    _conns.insert(std::make_pair(peerfd,con));
    con->handleConnectionCallback();
}

void EventLoop::handleMessage(int fd){
    auto iter = _conns.find(fd);
    if(iter != _conns.end()){
        //用Tcp连接里面定义一个函数
        //isClose,里面执行recv，recv的返回值等于0
        bool flag = iter->second->isClosed();
        if(flag){
            //连接是断开的
            iter->second->handleCloseCallback();
            delEpollReadFd(fd);//将文件描述符从红黑树上删除
            _conns.erase(iter);//将文件描述符从map中删除
        }
        else{
            //连接是正常
            iter->second->handleMessageCallback();
        }
    }
    else{
        printf("该连接不存在\n");
    }
}


int EventLoop::createEpollFd(){
    int fd = epoll_create(1);
    if(fd == -1){
        perror("epoll_create");
        return -1;
    } 
    return fd;
}

int EventLoop::createEventFd()
{
    int fd= eventfd(10, 0);
    if(-1 == fd)
    {
        perror("eventfd");
        return -1;
    }

    return fd;
}

void EventLoop::addEpollReadFd(int fd){
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;
    int ret = epoll_ctl(_epfd,EPOLL_CTL_ADD,fd,&event);
    if(ret == -1){
        perror("epoll_ctl add");
        return;
    }
}

void EventLoop::delEpollReadFd(int fd){
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;
    int ret = epoll_ctl(_epfd,EPOLL_CTL_DEL,fd,&event);
    if(ret == -1){
        perror("epoll_ctl del");
        return;
    }
}


void EventLoop::setConnectionCallback(TcpConnectionCallback &&cb){
    _onConnectionCb = std::move(cb);
}

void EventLoop::setMessageCallback(TcpConnectionCallback &&cb){
    _onMessageCb = std::move(cb);
}

void EventLoop::setCloseCallback(TcpConnectionCallback &&cb){
    _onCloseCb = std::move(cb);
}
