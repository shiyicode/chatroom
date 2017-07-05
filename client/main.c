/*************************************************************************
	> File Name: cli.c
	> Author: 师毅
	> Mail:  shiyi19960604@gmail.com
	> Created Time: 2015年08月06日 星期四 01时10分43秒
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

#define IPADDR "192.168.20.145"
#define PORT_NUM 8001

char user_name[20];

typedef struct User
{
        int id;
        char name[20];
        char pass[20];
}user_t;

typedef struct Group
{
	int num;
	char grp_name[20];
	char make[20];
	char name[20][20];
}group_t;

typedef struct News
{
    int type;
    int flag;
    char buf[500];
	char from_name[20];
	char to_name[20]; 
	user_t user;
	group_t grp;
}new_t;

void* def(void* conn)
{
	new_t new;
	int conn_fd = *(int*)conn;
	int ret;

	while(1)
	{	
		ret = recv(conn_fd, &new, sizeof(new_t), 0);
		if( ret < 0)
		{
			perror("recv");
			exit(1);
		}
		else if(ret == 0)
		{
			close(conn_fd);
			printf("服务器关闭\n");
			exit(1);
		}
		
		switch(new.type)
		{
			case 3:
				if(new.flag == 0)
				{
					printf("！！！ %s 不存在，消息发送失败\n",new.to_name);
				}
				else
				{
					printf("%s : %s\n",new.from_name, new.buf);
				}
				break;
			case 4:
				if(new.flag==1)
				{
					printf("[%s] %s : %s\n",new.grp.grp_name,new.user.name, new.buf);
				}
				else
				{
					printf("讨论组 %s 不存在\n",new.grp.grp_name);
				}
				break;
			case 5:
				
				if(new.buf[0]!='*' && new.buf[0]!='#')
				{
					printf("暂无好友，快去添加吧\n");
				}
				else
				{
					printf("\033[1;33m在线\t\033[0m离线\n");
					ret = 0;
					while(new.buf[ret]!='\0')
					{
						if(new.buf[ret] == '*')
						{
							ret++;
							while(new.buf[ret]!='*' && new.buf[ret]!='#' && new.buf[ret]!='\0')
							{
								printf("\033[1;33m%c\033[0m",new.buf[ret++]);
							}
							printf("\n");
						}
						else
						{
							ret++;
							while(new.buf[ret]!='*' && new.buf[ret]!='#' && new.buf[ret]!='\0')
							{
								printf("%c",new.buf[ret++]);
							}
							printf("\n");
						}
					}
				}
				break;
			case 6:
				printf("%s\n",new.buf);
				break;
			case 7:
				printf("%s\n",new.buf);
				break;
			default:
				break;
		}
	}
	
	pthread_exit(0);
	return NULL;
}

//创建套接字并连接服务端
int main()
{
	int conn_fd, i;
	new_t new;
    struct sockaddr_in serv_addr;
    pthread_t thread;
    int flag;
    
    memset(&serv_addr,0,sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_NUM);
    inet_aton(IPADDR,&serv_addr.sin_addr);

    conn_fd = socket(AF_INET,SOCK_STREAM,0);
    if(conn_fd < 0)
    {
		perror("socket");
        return 0;
    }

    if(connect(conn_fd,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr_in)) < 0)
    {
        perror("connect");
        return 0;
    }

	while(1)
	{
		printf("<1>注册 <2>登陆 <0>退出\n");
		scanf("%d",&new.type);
		switch(new.type)
		{
			case 1:
				printf("输入用户名：");
				scanf("%s",new.user.name);
				printf("输入密码：");
				scanf("%s",new.user.pass);
				send(conn_fd, &new, sizeof(new_t), 0);
				recv(conn_fd, &new, sizeof(new_t), 0);
				printf("%s\n",new.buf);
				break;
			case 2:
				printf("输入用户名：");
				scanf("%s",new.user.name);
				strcpy(user_name,new.user.name);
				printf("输入密码：");
				scanf("%s",new.user.pass);
				send(conn_fd, &new, sizeof(new_t), 0);				
				recv(conn_fd, &new, sizeof(new_t), 0);
				printf("%s\n",new.buf);
				break;
			case 0:
				return 0;
			default:
				break;
		}

		if(strcmp(new.buf, "登陆成功") == 0)
		{
			break;
		}
	}	

	pthread_create(&thread, NULL, def, (void*)&conn_fd);

	while(1)
	{
		memset(&new, 0, sizeof(new_t));
		printf("<3>私聊 <4>群聊 <5>查看好友 <6>好友管理 <7>讨论组管理 <8>文件传输 <0>退出\n");
		scanf("%d",&new.type);
		switch(new.type)
		{
			case 0:
				return 0;
			case 3:
				strcpy(new.from_name,user_name);
				printf("输入好友名：");
				scanf("%s",new.to_name);
				printf("输入消息内容：");
				scanf("%s",new.buf);
				send(conn_fd, &new, sizeof(new_t), 0);
				break;
			case 4:
				strcpy(new.user.name,user_name);
				printf("输入群组名：");
				scanf("%s",new.grp.grp_name);
				printf("输入消息内容：");
				scanf("%s",new.buf);
				send(conn_fd, &new, sizeof(new_t), 0);				
				break;
				
			case 5:
				strcpy(new.user.name, user_name);
				send(conn_fd, &new, sizeof(new_t), 0);
				break;
					
			case 6:
				do
				{
					printf("<1> 添加好友 <2> 删除好友\n");
					scanf("%d",&flag);
				}
				while(flag!=1 && flag!=2);
				printf("输入好友名：");
				strcpy(new.user.name,user_name);
				if(flag == 1)
				{
					scanf("%s",new.to_name);
				}
				else
				{
					scanf("%s",new.from_name);
				}
				send(conn_fd, &new, sizeof(new_t), 0);
				break;
			
			case 7:
				strcpy(new.user.name,user_name);
				do
				{
					printf("<1>查看讨论组 <2>创建讨论组 <3>退出讨论组\n");
					scanf("%d",&new.flag);
				}
				while(new.flag!=1 && new.flag!=2 && new.flag!=3);
				if(new.flag!=1)
				{
					printf("输入讨论组名称：");
					scanf("%s", new.grp.grp_name);
				}
				strcpy(new.user.name,user_name);
				if(new.flag == 2)
				{
					strcpy(new.grp.make, user_name);
					printf("输入初始人数：");
					
					scanf("%d",&new.grp.num);
					printf("依次输入群成员名称\n");
					for(i=0;i<new.grp.num;i++)
					{
						printf("%d:",i+1);
						scanf("%s", new.grp.name[i]);
					}
				}
				send(conn_fd, &new, sizeof(new_t), 0);
				
				break;
				
			case 8:
				
				break;
				
			default:
				break;
		}
	}
	
	return 0;	
}
