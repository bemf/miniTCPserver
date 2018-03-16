# miniTCPserver

这个是自己学习socket服务器编程 的一些实现
封装了一个common头文件，方便调用
1. select +多进程版本
基于TCP的简单服务器，使用多进程对每个客户进行服务，同时使用select来进行IO复用
2. select 
基于TCP的简单服务器，使用select 的io复用对每个客户进行服务
3. 简单的TCPClient 用来测试
