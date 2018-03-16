//
//  main.cpp
//  TCPServer_pthreads_use_Select
//
//  Created by wb on 2018/3/15.
//  Copyright © 2018年 wb. All rights reserved.
//

#include <iostream>
#include "common.hpp"

#define FDSIZE 256
#define MAXLINE 256

typedef struct {
    int maxfd;
    fd_set ready_set;
    fd_set read_set;
    int nready;
    int maxi;
    int clientfd[FDSIZE];
    rio_t clientrio[FDSIZE];
} pool_select;

void init_pool(int listenfd, pool_select *p)
{
    p->maxi=0;//标记客户端最大的那个下标
    for(int i=0;i<FDSIZE;i++)
    {
        p->clientfd[i]=-1;
    }
    
    p->maxfd=listenfd;
    __DARWIN_FD_ZERO(&p->read_set);
    __DARWIN_FD_SET(listenfd, &p->read_set);//将listenfd注册到read_set
    
}

void check_client(pool_select *p)
{
    char buf[MAXLINE];
    int n;
    if(p->nready>0)
    {
        for(int i=0;i<p->maxi;i++)
        {
            if(p->clientfd[i]>0&&
               __DARWIN_FD_ISSET(p->clientfd[i], &p->ready_set))
                //可以读
            {
                if((n=rio_readlineb(&p->clientrio[i], buf, MAXLINE)!=0))
                {
                    rio_writen(p->clientfd[i], buf, n);
                }
                else{
                    close(p->clientfd[i]);
                    __DARWIN_FD_CLR(p->clientfd[i], &p->read_set);
                    p->clientfd[i]=-1;
                }
            }
        }
    }
}

void add_client(pool_select *p, int connfd)
{
    int i=0;
    //记录描述符号
    for(i=0;i<FDSIZE;i++)
    {
      if(p->clientfd[i]==-1)
      {
          p->clientfd[i]=connfd;//记录客户的描述符
          rio_readinitb(&p->clientrio[i], connfd);//将描述符号记录到rio
          if(i>p->maxi)
          {
              p->maxi=i;
          }
          if(connfd>p->maxfd)
          {
              p->maxfd=connfd;
          }
          break;
      }
    }
    if(i==FDSIZE)
    {
        printf("addclient() error\n");
    }
    p->nready--;
    __DARWIN_FD_SET(connfd, &p->read_set);
}


int main(int argc, const char * argv[]) {
    // insert code here...
    printf("This is miniTCPServer use select\n");
    
    pool_select *p=new pool_select();
    int listenfd=open_listenfd(PORT);
    
    init_pool(listenfd, p);
    
    sockaddr_in client_addr;
    socklen_t client_len;
    
    int connfd;
    while(1)
    {
        p->ready_set=p->read_set;
        p->nready=select(p->maxfd+1, &p->ready_set, NULL, NULL, NULL);
        if(__DARWIN_FD_ISSET(listenfd, &p->ready_set))
        {
            connfd=accept(listenfd, (sockaddr *)&client_addr, &client_len);
            add_client(p, connfd);//添加客户端
        }
        
        check_client(p);//判断客户端是否可读
    }
    
}
















