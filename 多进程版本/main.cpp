//
//  main.cpp
//  TCPServer_Multi_Process_use_Select
//
//  Created by wb on 2018/3/14.
//  Copyright © 2018年 wb. All rights reserved.
//

//
//  main.cpp
//  TcpServer_Multi_Process
//
//  Created by wb on 2018/3/14.
//  Copyright © 2018年 wb. All rights reserved.
//

#include <iostream>
#include "common.hpp"

#define MYPORT 123456

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


void sig_child(int child)
{
    pid_t p;
    int stat; //保留子进程的状态信息
    if(p=waitpid(-1, &stat, WNOHANG))//-1表示等待第一个已终止的子进程，WNOHANG 表示告诉内核不要等待
    {
        printf("child %d is terminated", p);
    }
    return ;
}

int main(int argc, const char * argv[]) {
    printf("多进程简单TCP服务器 Select\n");
    //initialization
    int listenfd,connfd,sockfd;
    listenfd=socket(AF_INET,SOCK_STREAM,0);
    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(MYPORT);
    sockaddr_in clien_addr;
    socklen_t len_client;
    
    //使用Select框架变量
    int nready=0; //记录多少个描述符就绪
    int maxfd=-1;
    int client[256];//用来存储可以读写就绪好的描述符
    for(int i=0;i<256;i++)
    {
        client[i]=-1;
    }
    fd_set read_set,all_set;//read_set:已就绪 all_set:监听的描述符
    __DARWIN_FD_ZERO(&all_set);
    
    char buf[64];
    ssize_t n;
    if(bind(listenfd,(sockaddr *)&serv_addr,sizeof(serv_addr))<0)
    {
        //printf(strerror(errno));
        printf("bind error\n");
        return 0;
    }
    __DARWIN_FD_SET(listenfd, &all_set);
    
    if(listenfd>maxfd)
    {
        maxfd=listenfd;
    }
    
    listen(listenfd, 256);
    //等待是否有子进程退出，内核会直接捕获信号，
    signal(SIGCHLD, sig_child);
    
    pid_t child;
    while(1)
    {//
        read_set=all_set;
        nready=select(maxfd+1, &read_set, NULL, NULL, NULL);
            //处理accept添加客户
            if(__DARWIN_FD_ISSET(listenfd, &read_set))
            {
                len_client=sizeof(clien_addr);
                connfd=accept(listenfd, (sockaddr *) &clien_addr, &len_client);
                for(int i=0;i<256;i++)
                {
                    if(client[i]<0)
                    {
                        client[i]=connfd;
                        break;
                    }

                }
                __DARWIN_FD_SET(connfd, &all_set);
                if(connfd>maxfd)
                {
                    maxfd=connfd;
                }
                if(--nready<=0)
                {
                    continue;
                }
            }
            //处理已连接的客户
            //if(nready>0)
            for(int i=0;i<256;i++)
            {
                sockfd=client[i];
                if(sockfd>=0)//表示已连接的客户描述符
                {
                    if(__DARWIN_FD_ISSET(sockfd, &read_set))
                    {
//                        if((n=read(sockfd,buf,64))==0)
//                        {
//                            close(sockfd);
//                            __DARWIN_FD_CLR(sockfd, &all_set);
//                            client[i]=-1;
//                        }
//                        else{
//                            //printf("%s\n",buf);
//                            write(sockfd, buf, n);
//                        }
                        if((child=fork())==0)
                        {
                            //子进程
                            close(listenfd);
                            str_echo(sockfd);
                            close(sockfd);
                            exit(0);
                        }
                        client[i]=-1;
                        nready--;
                        if(nready<=0)
                        {
                            break;
                        }
                    }
                }
                
            }
        
        
        
        /*
        len_client=sizeof(clien_addr);
        connfd=accept(listenfd, (sockaddr *) &clien_addr, &len_client);
        if(connfd<0)
        {
            continue;
        }
        if((child=fork())==0)//子进程；
        {
            close(listenfd);
             //do something;
            str_echo(connfd);
            exit(0);
        }
        close(connfd);*/
    }
    
}

