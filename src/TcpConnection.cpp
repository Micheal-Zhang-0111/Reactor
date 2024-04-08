#include"TcpConnection.h"
#include<iostream>
#include<sstream>

using std::cout;
using std::endl;
using std::ostringstream;

TcpConnection::TcpConnection(int fd)
:_sock(fd),
_sockIO(fd),
_localAddr(getLocalAddr()),
_peerAddr(getPeerAddr())
{
}

TcpConnection::~TcpConnection(){

}

void TcpConnection::send(const string &msg){
    _sockIO.writen(msg.c_str(),msg.size());
}

string TcpConnection::receive(){
    char buf[65535] = {0};
    _sockIO.readLine(buf,sizeof(buf));
    return string(buf);
}

string TcpConnection::toString(){
    ostringstream oss;
    oss << _localAddr.ip() << ":"
        << _localAddr.port() << "---->"
        << _peerAddr.ip() << ":"
        << _peerAddr.port();
    return oss.str();
}

bool TcpConnection::isClosed() const
{
    char buf[10] = {};
    int ret = ::recv(_sock.fd(), buf, sizeof(buf), MSG_PEEK);
    return (ret == 0);
}

//获取本端的网络地址信息
InetAddress TcpConnection::getLocalAddr(){
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int ret = getsockname(_sock.fd(),(struct sockaddr*)&addr,&len);
    if(ret == -1){
        perror("getsockname");
    }
    return InetAddress(addr);
}

//获取对端的网络地址信息
InetAddress TcpConnection::getPeerAddr(){
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int ret = getpeername(_sock.fd(),(struct sockaddr*)&addr,&len);
    if(ret == -1){
        perror("getpeername");
    }
    return InetAddress(addr);
}


//注册三个事件的回调函数
void TcpConnection::setConnectionCallback(const TcpConnectionCallback &cb){
    _onConnectionCb = std::move(cb);
}

void TcpConnection::setMessageCallback(const TcpConnectionCallback &cb){
    _onMessageCb = std::move(cb);
}

void TcpConnection::setCloseCallback(const TcpConnectionCallback &cb){
    _onCloseCb = std::move(cb);
}

//三个事件回调函数的执行
void TcpConnection::handleConnectionCallback(){
    if(_onConnectionCb){
        _onConnectionCb(shared_from_this());
    }
}

void TcpConnection::handleMessageCallback(){
    if(_onMessageCb){
        _onMessageCb(shared_from_this());
    }
}

void TcpConnection::handleCloseCallback(){
    if(_onCloseCb){
        _onCloseCb(shared_from_this());
    }
}

