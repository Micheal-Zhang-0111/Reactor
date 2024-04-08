#include"SocketIO.h"
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<errno.h>
#include<stdio.h>


SocketIO::SocketIO(int fd)
:_fd(fd)
{
}

SocketIO::~SocketIO(){
}

int SocketIO::readn(char *buf,int len){
    int leftLength = len;
    char *pstr = buf;
    int ret = 0;
    while(leftLength > 0){
        ret = read(_fd,pstr,leftLength);
        if(ret == -1 && errno == EINTR){//中断
            continue;
        }
        else if(ret == -1){
            perror("read error -1");
            return leftLength;
        }
        else if(ret == 0){
            break;
        }
        else{
            pstr += ret;
            leftLength -= ret;
        }
    }
    return len - leftLength;
}

int SocketIO::readLine(char *buf,int len){
    int leftLength = len - 1;
    char *pstr = buf;
    int ret = 0,total = 0;
    while(leftLength > 0){
        ret =  recv(_fd,pstr,leftLength,MSG_PEEK);
        if(ret == -1 && errno == EINTR){
            continue;
        }
        else if(ret == -1){
            perror("readLine error -1");
            return leftLength;
        }
        else if(ret == 0){
            break;
        }
        else{
            for(int idx = 0;idx < ret; ++idx){
                if(pstr[idx] == '\n'){
                    int size = idx + 1;
                    readn(pstr,size);
                    pstr += size;
                    *pstr = '\0';
                    return total + size;
                }
            }
            readn(pstr,ret);
            total += ret;
            pstr += ret;
            leftLength -= ret;
        }
    }
    *pstr = '\0';
    return total - leftLength;
}

int SocketIO::writen(const char *buf,int len){
    int leftLength = len;
    const char *pstr = buf;
    int ret = 0;
    while(leftLength > 0){
        ret = write(_fd,pstr,leftLength);
        if(ret == -1 && errno == EINTR){//中断
            continue;
        }
        else if(ret == -1){
            perror("write error -1");
            return leftLength;
        }
        else if(ret == 0){
            break;
        }
        else{
            pstr += ret;
            leftLength -= ret;
        }
    }
    return len - leftLength;
}