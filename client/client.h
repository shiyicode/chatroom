/*************************************************************************
	> File Name: client.h
	> Author: 师毅
	> Mail:  458241107@qq.com
	> Created Time: 2015年08月12日 星期三 01时34分24秒
 ************************************************************************/

#ifndef _CLIENT_H
#define _CLIENT_H

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

#endif
