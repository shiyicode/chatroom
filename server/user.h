/*************************************************************************
	> File Name: user.h
	> Author: 师毅
	> Mail:  shiyi19960604@gmail.com
	> Created Time: 2015年08月03日 星期一 15时49分15秒
 ************************************************************************/

#ifndef _USER_H
#define _USER_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<time.h>

typedef struct User
{
    int id;
    char name[20];
    char pass[20];
}user_t;

typedef struct Grp
{
	int num;//群人数
	char make[20];
	char name[20];
	char people[20][20];
}grp_t;

typedef struct News
{
        int type;
        int flag;
        int conn_fd;
        char buf[500];
        char from[20];
        char to[20]; 
        time_t now_time;
        user_t user;
		grp_t grp;
		char file_send[20];
		char file_recv[20];
		int file_num;
		struct stat file;
}new_t;

//保存系统日志
int save_log(char test[]);

//根据用户名查找用户
int find_user_id(char name[], user_t *buf);

//根据id查找用户
int find_user_name(int id, user_t *buf);

//增加用户
int add_user(user_t *buf);

//查看好友
int look_firend(char user[], char str[]);

//检测好友是否存在
int find_friend(char user[],char fri[]);

//添加好友
int add_friend(char user[],char fri[]);

//删除好友
int del_friend(char user[],char fri[]);

//创建id号
int make_id();

//查找讨论组
int find_group(char name[],grp_t *buf);

//创建讨论组
int add_group(grp_t *buf);

//查看讨论组
int look_group(new_t *new);

//删除讨论组
int del_group(new_t *new);

#endif
