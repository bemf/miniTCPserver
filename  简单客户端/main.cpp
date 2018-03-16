//
//  main.cpp
//  TCPClient
//
//  Created by wb on 2018/3/15.
//  Copyright © 2018年 wb. All rights reserved.
//

#include <iostream>
#include "common.hpp"

#define PORT 123456

char buf[MAXLINE];
int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "TCP Client\n";
    char *addr="127.0.0.1";
    int port=PORT;
    int clientfd=open_clientfd(addr,port);
    
    rio_t rio;
    rio_readinitb(&rio, clientfd);
    
    while(fgets(buf, MAXLINE, stdin)!=NULL)
    {
        rio_writen(clientfd, buf, strlen(buf));
        rio_readlineb(&rio, buf, MAXLINE);
        fputs(buf, stdout);
    }
    close(clientfd);
    
    return 0;
}
