//
//  threadpool.cpp
//  TcpServer_Multi_Phreads
//
//  Created by wb on 2018/3/24.
//  Copyright © 2018年 wb. All rights reserved.
//

#include "threadpool.hpp"

//初始化线程池
thread_pool * init_thread_pool(int thread_num)
{
    thread_pool *pool;
    if((pool=(thread_pool *)malloc(sizeof(thread_pool)))==NULL)
    {
        printf("thread_pool_init()\n");
        return nullptr;
    }
    //thread数组存放对应的线程
    pool->thread_num=0;
    pool->task_size=0;
    pool->started=0;
    pool->threads=(pthread_t *)malloc(sizeof(pthread_t)*thread_num);
    
    //初始化任务节点
    pool->head=(task *)malloc(sizeof(task));
    
    pool->head->fun=NULL;
    pool->head->args=NULL;
    pool->head->next=NULL;
    
    if(pthread_mutex_init(&(pool->lock), NULL)!=0)
    {
        printf("lock init error\n");
    }
    if(pthread_cond_init(&(pool->cond), NULL)!=0)
    {
        printf("cond init error\n");
    }
    
    //创建线程
    for(int i=0;i<thread_num;i++)
    {
        if(pthread_create(&(pool->threads[i]), NULL, thread_pool_work, (void *)pool)!=0)
        {
            printf("creat thread error\n");
            return NULL;
        }
        pool->thread_num+=1;
        pool->started+=1;
    }
    
    return pool;
}

int thread_pool_add(thread_pool *pool, void (*fun)(void *args),void *args)
{
    // 对线程池加锁
    if(pthread_mutex_lock(&(pool->lock))!=0)
    {
        return -1;
    }
    //建立任务列表
    task *the_task=(task *)malloc(sizeof(task));
    the_task->fun=fun;
    the_task->args=args;
    
    //在pool的head后插入
    
    the_task->next=pool->head->next;
    pool->head->next=the_task;
    pool->task_size++;
    
    pthread_cond_signal(&(pool->cond));
    if(pthread_mutex_unlock(&(pool->lock))!=0)
    {
        return -1;
    }
    return 0;
}

void *thread_pool_work(void *arg)
{
    if(arg==NULL)
    {
        return NULL;
    }
    
    thread_pool *pool=(thread_pool *)arg;
    task *the_task;
    while(1)
    {
        //对线程上锁
        pthread_mutex_lock(&(pool->lock));
        while(pool->task_size==0)
        {
            pthread_cond_wait(&(pool->cond), &(pool->lock));
        }
        
        //获取任务
        the_task=pool->head->next;
        if(the_task==NULL)
        {
            //释放锁
            pthread_mutex_unlock(&(pool->lock));
            continue;
        }
        //将task执行，并取走
        pool->head->next=the_task->next;
        pool->task_size--;
        pthread_mutex_unlock(&(pool->lock));
        //执行任务
        (*(the_task->fun))(the_task->args);
        free(the_task);
    }
    //退出线程
    pool->started--;
    pthread_mutex_unlock(&(pool->lock));
    pthread_exit(NULL);
    return NULL;
}







