//
//  threadpool.hpp
//  TcpServer_Multi_Phreads
//
//  Created by wb on 2018/3/24.
//  Copyright © 2018年 wb. All rights reserved.
//

#ifndef threadpool_hpp
#define threadpool_hpp
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>


struct task{
    void (*fun)(void *args);//r任务列表
    void *args;//任务函数的参数
    struct task *next; //任务下一节点
};

struct thread_pool{
    pthread_mutex_t lock; //互斥
    pthread_cond_t cond; //条件
    pthread_t *threads;
    int thread_num; //线程数量
    task *head; //任务头
    int *task_size; //任务长度
    int started;
};


thread_pool * init_thread_pool(int thread_num);
int thread_pool_add(thread_pool *pool, void (*fun)(void *args),void *args);
void *thread_pool_work(void *arg);

#endif /* threadpool_hpp */
