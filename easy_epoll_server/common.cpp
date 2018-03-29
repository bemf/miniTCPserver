//
// Created by parallels on 3/29/18.
//

#include "common.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

ssize_t rio_readn(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = (char *)usrbuf;
    while(nleft > 0){
        if((nread = read(fd, bufp, nleft)) < 0){
            if(errno == EINTR)
                nread = 0;
            else
                return -1;
        }
        else if(nread == 0)
            break;
        nleft -= nread;
        bufp += nread;
    }
    return (n - nleft);
}

ssize_t rio_writen(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nwritten;
    char *bufp = (char *)usrbuf;

    while(nleft > 0){
        if((nwritten = write(fd, bufp, nleft)) <= 0){
            if (errno == EINTR)
                nwritten = 0;
            else{
                return -1;
            }
        }
        nleft -= nwritten;
        bufp += nwritten;
    }
    return n;
}

static ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n)
{
    size_t cnt;
    while(rp->rio_cnt <= 0){
        rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
        if (rp->rio_cnt < 0){
            if(errno == EAGAIN){
                return -EAGAIN;
            }
            if(errno != EINTR){
                return -1;
            }
        }
        else if(rp->rio_cnt == 0)
            return 0;
        else
            rp->rio_bufptr = rp->rio_buf;
    }
    cnt = n;
    if(rp->rio_cnt < (ssize_t)n)
        cnt = rp->rio_cnt;
    memcpy(usrbuf, rp->rio_bufptr, cnt);
    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;
    return cnt;
}

void rio_readinitb(rio_t *rp, int fd)
{
    rp->rio_fd = fd;
    rp->rio_cnt = 0;
    rp->rio_bufptr = rp->rio_buf;
}

ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = (char *)usrbuf;
    while (nleft > 0){
        if((nread = rio_read(rp, bufp, nleft)) < 0){
            if(errno == EINTR)
                nread = 0;
            else
                return -1;
        }
        else if(nread == 0)
            break;
        nleft -= nread;
        bufp += nread;
    }
    return (n - nleft);
}

ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen)
{
    size_t n;
    ssize_t rc;
    char c, *bufp = (char *)usrbuf;
    for(n = 1; n < maxlen; n++){
        if((rc = rio_read(rp, &c, 1)) == 1){
            *bufp++ = c;
            if(c == '\n')
                break;
        }
        else if(rc == 0){
            if (n == 1){
                return 0;
            }
            else
                break;
        }
        else if(rc == -EAGAIN){
            return rc;
        }
        else{
            return -1;
        }
    }
    *bufp = 0;
    return n;
}

//打开一个listenfd, 监听是否有客户连接。
int open_listenfd(int port)
{
    sockaddr_in serv_addr;
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(port);
    serv_addr.sin_family=AF_INET;

    int listenfd=socket(AF_INET, SOCK_STREAM, 0);

    if(bind(listenfd, (sockaddr *) &serv_addr, sizeof(serv_addr))<0)
    {
        printf("bind() error\n");
        return -1;
    }
    if(listen(listenfd,MAX_CONN)<0)
    {
        printf("listen() error\n");
        return -1;
    }
    return listenfd;

}

//打开一个clientfd，使其连接至服务器
int open_clientfd(char *addr, int port)
{
    sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_addr.s_addr=inet_addr(addr);
    client_addr.sin_port=htons(port);
    client_addr.sin_family=AF_INET;

    int clientfd=socket(AF_INET, SOCK_STREAM, 0);

    if(connect(clientfd, (sockaddr *)&client_addr, sizeof(client_addr))<0)
    {
        printf("connect() error\n");
        return -1;
    }
    return clientfd;
}

