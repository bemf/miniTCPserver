//
//  main.cpp
//  TcpServer_Multi_Phreads
//
//  Created by wb on 2018/3/16.
//  Copyright © 2018年 wb. All rights reserved.
//

#include <iostream>
#include "common.hpp"
#include <pthread.h>

void str_echo(int socket_fd)
{
    ssize_t n;
    char buf[256];
    rio_t my_rio;
    rio_readinitb(&my_rio, socket_fd);
    while((n=rio_readlineb(&my_rio, buf, 256))!=0)
    {
        //write(socket_fd, buf, n);
        rio_writen(socket_fd, buf, n);
    }
}


void *usethread(void *arg)
{
    int connfd=*(int *)arg;
    pthread_detach(pthread_self());
    free(arg);
    str_echo(connfd);
    close(connfd);
    return NULL;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "This is TCP Server use threads\n";
    
    int listenfd=open_listenfd(PORT);
    sockaddr_in client_addr;
    socklen_t client_len;
    pthread_t pid;
    while(1)
    {
        int *connfd=new int();
        *connfd=accept(listenfd, (sockaddr *)&client_addr, &client_len);
        pthread_create(&pid, NULL, usethread, connfd);
        
    }
    
    return 0;
}
