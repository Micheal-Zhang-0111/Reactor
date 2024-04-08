#include"InetAddress.h"
#include<string.h>


InetAddress::InetAddress(const string &ip,unsigned short port)
{
    memset(&_addr,0,sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = inet_addr(ip.c_str());
    _addr.sin_port = htons(port);
}

InetAddress::InetAddress(const struct sockaddr_in &addr)
:_addr(addr)
{
}

InetAddress::~InetAddress(){

}

string InetAddress::ip() const{
    //转化为点分十进制
    return string(inet_ntoa(_addr.sin_addr));
}

unsigned short InetAddress::port() const{
    return ntohs(_addr.sin_port);
}

const sockaddr_in *InetAddress::getInetAddrPtr() const{
    return &_addr;
}
