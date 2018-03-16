//
//  common.hpp
//  TCPServer_Multi_Process_use_Select
//
//  Created by wb on 2018/3/15.
//  Copyright © 2018年 wb. All rights reserved.
//

#ifndef common_hpp
#define common_hpp

#include <sys/types.h>
#define RIO_BUFSIZE 1024

//io
typedef struct{
    int rio_fd;             /* descriptor for this internal buf */
    ssize_t rio_cnt;        /* unread bytes in internal buf */
    char *rio_bufptr;       /* next unread byte in internal buf */
    char rio_buf[RIO_BUFSIZE]; /* internal buffer */
}rio_t;

ssize_t rio_readn(int fd, void *usrbuf, size_t n);
ssize_t rio_writen(int fd, void *usrbuf, size_t n);
void rio_readinitb(rio_t *rp, int fd);
ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n);
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);

//server
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <string>
#include <arpa/inet.h>

#define MAX_CONN 256
#define PORT 123456
int open_listenfd(int port);
int open_clientfd(char *addr, int port);

#endif /* common_hpp */
