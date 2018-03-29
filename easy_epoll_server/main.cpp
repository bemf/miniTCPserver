#include <iostream>


#include "common.h"
#include <sys/epoll.h>
#include <fcntl.h>
#include <wait.h>
#include <stdio.h>
#include <string.h>

#define MAX_EVENT 1024

void handle_signal(int sig)
{
    pid_t pid;
    int stat;
    if(pid=waitpid(-1,&stat,WNOHANG))
    {
        printf("child %d is terminated\n",pid);
    }
    return ;

}

void str_echo(int socket_fd)
{
    size_t n;
    char buf[256];
    rio_t r;
    rio_readinitb(&r,socket_fd);
    while((n=rio_readlineb(&r,buf,256))>0)
    {
        rio_writen(socket_fd,buf,n);
    }

}


int main() {
    int listenfd;
    fcntl(listenfd,F_SETFL,O_NONBLOCK);
    listenfd=open_listenfd(PORT);
    //handle process exit
    signal(SIGCHLD,handle_signal);
    struct epoll_event ev, events[MAX_EVENT];

    int epoll_fd=epoll_create(MAX_EVENT);
    ev.data.fd=listenfd;
    ev.events=EPOLLIN;

    pid_t pid;
    if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,listenfd, &ev)==-1)
    {
        printf("epoll_ctl() error\n");
        return -1;
    }
    int ready=-1;
    sockaddr_in client_addr;
    socklen_t client_len;
    for(;;)
    {
        ready=epoll_wait(epoll_fd,events,MAX_EVENT,-1);
        for(int i=0;i<ready;i++)
        {
            //new connection
            if(events[i].data.fd==listenfd)
            {
                memset(&client_addr,0,sizeof(client_addr));
                int connfd=accept(listenfd,(sockaddr *)&client_addr,&client_len);
                if(connfd==-1)
                {
                    printf("accept() error\n");
                }
                if(fcntl(connfd,F_SETFL,O_NONBLOCK)==-1)
                {
                    continue;
                }
                ev.data.fd=connfd;
                ev.events=EPOLLIN;
                if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,connfd,&ev)==-1) {
                    printf("epoll_ctl() error\n");
                    close(connfd);
                }
            }
            else if(events[i].events&EPOLLIN)
            {
                int cli_fd=events[i].data.fd;
                //doit(clifd);
                //child process
                if((pid=fork())==0)
                {
                    str_echo(cli_fd);
                    close(cli_fd);
                    exit(0);
                }

                close(cli_fd);
            }
            else if(events[i].events&EPOLLERR)
            {
                printf("epoll error\n");
                continue;
            }



        }

    }


}