#pragma once

#include "TcpServer.h"
#include "ThreadPool.h"
#include <iostream>
#include <unistd.h>

using std::cout;
using std::endl;

class MyTask
{
public:
    MyTask(const string &msg, const TcpConnectionPtr &con)
    : _msg(msg)
    , _con(con)
    {

    }

    //该函数在线程池中执行的
    void process()
    {
        //在process函数中去进行真正的业务逻辑的处理
        //....
        //...
        /* string newMsg = msg + 1; */
        //现在计算线程（ThreadPool）处理完业务逻辑之后，需要
        //将处理完后数据发送给IO线程（EventLoop/Reactor），
        //IO线程如何与计算线程进行通信？(eventfd)
        _con->sendInLoop(_msg);//IO操作，与计算操作分离
        //数据的发送需要在EventLoop里面进行发送
        //TcpConnection需要将数据发送给EventLoop，让EventLoop去
        //进行发送数据IO操作
        //此时TcpConnection需要知道EventLoop存在
    }
private:
    string _msg;
    TcpConnectionPtr _con;
};

class EchoServer
{
public:
    EchoServer(size_t threadNum, size_t queSize
               , const string &ip, unsigned short port)
    : _pool(threadNum, queSize)
    , _server(ip, port)
    {

    }

    void start()
    {
        _pool.start();//计算线程全部启动
        using namespace std::placeholders;
        _server.setAllCallback(bind(&EchoServer::onConnection, this, _1)
                               , bind(&EchoServer::onMessage, this, _1)
                               , bind(&EchoServer::onClose, this, _1));
        _server.start();//IO线程启动
    }

    void stop()
    {
        _pool.stop();
        _server.stop();
    }

    void onConnection(const TcpConnectionPtr &con)
    {
        cout << con->toString() << " has connected!" << endl;
    }
    
    void onMessage(const TcpConnectionPtr &con)
    {
        //回显
        string msg = con->receive();
        cout << "recv msg  " << msg << endl;
        //接收与发送之间，消息msg是没有做任何处理的
        //...
        //
        //处理msg的业务逻辑全部在此处实现的话
        //将msg这些信息打个包交给MyTask，然后将MyTask注册给线程池
        //让线程池去处理具体的业务逻辑，此时业务逻辑的处理就不在
        //EventLoop线程中
        
        MyTask task(msg, con);
        /* gThreadPool->addTask(std::bind(&MyTask::process, &task)); */
        _pool.addTask(std::bind(&MyTask::process, task));
    
    }
    
    void onClose(const TcpConnectionPtr &con)
    {
        cout << con->toString() << " has closed!" << endl;
    }
private:
    ThreadPool _pool;
    TcpServer _server;
};
