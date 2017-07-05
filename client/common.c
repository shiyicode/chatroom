/*************************************************************************
	> File Name: common.c
	> Author: 师毅
	> Mail:  458241107@qq.com
	> Created Time: 2015年08月12日 星期三 13时51分24秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<fcntl.h>
#include"client.h"

//获取用户操作
int get_cmd(char buf[])
{	
	int i;
	i = 0;
	while(1)
	{
		buf[i] = getchar();

		if(buf[i] == '\n')
		{
			buf[i]='\0';
			i++;
			break;
		}
		else
		{
			i++;
		}
	}
	return buf[0]=='\0'? 0 : 1;
}

//解析用户操作
int made_cmd(char cmd[][500], char buf[])
{
	int i,j,k,len = strlen(buf);
	j=k=0;
	for(i=0;i<len;i++)
	{
		if(buf[i]==' ')
		{
			if(k>0)
			{
				cmd[j][k]='\0';
				k=0;
				j++;
			}
		}
		else
		{
			cmd[j][k]=buf[i];
			k++;
		}
	}

	if(k!=0)
	{
		cmd[j][k]='\0';
		j++;
	}
	return j;	
}

void get_pass(char pass[])
{
	int i=0;
	while(1)
	{
		pass[i] = getchar();
		if(pass[i] == 127 && i>=0)
		{
			if(i==0)
			{
				continue;
			}
			putchar('\b');
			putchar(' ');
			putchar('\b');
			i--;
		}
		else if(pass[i] == '\n')
		{
			pass[i]='\0';
			puts("");
			break;
		}
		else
		{
			putchar('*');
			i++;
		}
	}
}

//文件传输
int send_file(new_t *new)
{
	int fd;

    fd = open(new->file_send, O_RDONLY | O_CREAT, 0666);
	new->grp.num=0;
    while(new->file_num = read(fd, new->buf, 100000))
    {

		if(send(new->conn_fd, new, sizeof(new_t), 0) < 0)
		{
			perror("send");
		}
		usleep(50000);
		memset(new->buf, 0, sizeof(new->buf));       
    }
	new->grp.num=-1;
    send(new->conn_fd, new, sizeof(new_t), 0);
    printf("[提醒]文件%s发送成功\n",new->file_send);
    close(fd);
	free(new);
	return 1;
}

//保存聊天记录
int save_chat(new_t new,char filename[])
{
	char file[20];
	char test[500];
	int fd;
	
	sprintf(file,"%s.txt",filename);

	fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0666);
	if(fd == -1)
	{
		perror("open");
	}
	
	sprintf(test, "%s[%s]%s\n\n",ctime(&new.now_time),new.from,new.buf);

	int i=0;
	
	while(test[i]!='\0')
	{
		write(fd, &test[i++], 1);
	}

    close(fd);
    return 1;
}

//输出提示信息
void print()
{
	printf("************************************************\n");
	printf("****    /@call XXX <---> 锁定好友XXX私聊      ****\n");
	printf("****    /@add  XXX <---> 添加好友XXX         ****\n");
	printf("****    /@del  XXX <---> 删除好友XXX         ****\n");
	printf("****    /@apt  XXX <---> 接受XXX的好友请求    ****\n");
	printf("****    /@look all <---> 查看好友及状态       ****\n");
	printf("****    /#call XXX <---> 锁定群组XXX群聊     ****\n");	
	printf("****    /#look all <---> 查看讨论组及状态       ****\n");
	printf("****    /#add  XXX <---> 创建讨论组XXX       ****\n");
	printf("****    /#del  XXX <---> 退出讨论组XXX       ****\n");
	printf("****    /$send X N <---> 向X发送文件NN       ****\n");
	printf("****    /$apt  X N <---> 接受X的文件请求      ****\n");
	printf("****    /quit      <---> 退出               ****\n");	

	printf("************************************************\n");	
}

