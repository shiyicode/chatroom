/*************************************************************************
	> File Name: common.h
	> Author: 师毅
	> Mail:  458241107@qq.com
	> Created Time: 2015年08月12日 星期三 13时51分30秒
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H

//密码接收
void get_pass(char pass[]);

//获取用户操作
int get_cmd(char buf[]);

//解析用户操作
int made_cmd(char cmd[][500], char buf[]);

//文件传输
int send_file(new_t *new);

//保存聊天记录
int save_chat(new_t new, char filename[]);

//输出提示信息
void print();

#endif
