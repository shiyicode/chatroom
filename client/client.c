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
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<time.h>
#include"client.h"
#include"common.h"

#define IPADDR "192.168.20.145"
#define PORT_NUM 8001

char user_name[20];

void def(int *conn)
{
	new_t new;
	int conn_fd;
	conn_fd = *conn;
	int ret;
	int file_fd;
	pthread_t thread;
	
	while(1)
	{
		ret = recv(conn_fd, &new, sizeof(new_t), 0);
		if(ret < 0)
		{
			perror("recv");
		}
		else if(ret == 0)
		{
			printf("[错误]服务端意外关闭\n");
			//pthread_exit(0);
			exit(1);
		}

		switch(new.type)
		{
			case 3://私聊
				if(strcmp(new.from, user_name) == 0)
				{
					printf("%s[提醒]%s%s\n\n" ,ctime(&new.now_time),new.to,new.buf);
				}
				else
				{
					printf("%s[%s]%s\n\n",ctime(&new.now_time),new.from, new.buf);
					save_chat(new,new.from);
				}
				break;
			case 4://群聊
				if(new.from[0] == '\0')
				{
					printf("%s[提醒]%s%s\n\n" ,ctime(&new.now_time),new.grp.name,new.buf);
				}
				else
				{
					printf("%s[%s : %s]%s\n\n",ctime(&new.now_time), new.grp.name, new.from, new.buf);
				}
				break;
			case 5://好友管理
				if(new.flag == 0)
				{
					if(strcmp(new.from, user_name) == 0)
					{
						printf("%s[提醒]%s%s\n\n",ctime(&new.now_time), new.to, new.buf);
					}
					else
					{
						printf("%s[%s]请求添加您为好友,若同意请输入/@apt %s\n\n",ctime(&new.now_time),new.from, new.from);
					}
				}
				else if(new.flag == 1)
				{
					if(strcmp(new.from, user_name) == 0)
					{
						printf("%s[提醒]%s%s\n\n",ctime(&new.now_time), new.to, new.buf);
					}
					else
					{
						printf("%s[提醒]%s%s\n\n",ctime(&new.now_time),new.from, new.buf);
					}
				}
				else if(new.flag == 2)
				{
					if(strcmp(new.from, user_name) == 0)
					{
						printf("%s[提醒]%s%s\n\n",ctime(&new.now_time), new.to, new.buf);
					}
					else
					{
						printf("%s[提醒]%s%s\n\n",ctime(&new.now_time), new.from, new.buf);
					}
				}
				else if(new.flag == 3)
				{
					if(new.buf[0] == '\0')
					{
						printf("%s[提醒]您暂无好友，快去添加吧\n\n",ctime(&new.now_time));	
					}
					else
					{
						printf("%s\t\033[1;33m【在线】\033[0m\n",ctime(&new.now_time));
						printf("%s\n\n",new.buf);
					}
				}
				break;
			case 6://群组管理
			
				if(new.flag == 1)//创建
				{
					printf("%s[提醒]讨论组%s%s\n\n",ctime(&new.now_time),new.grp.name, new.buf);						
				}
				else if(new.flag == 2)//退出
				{
					printf("%s%s\n\n",ctime(&new.now_time), new.buf);
				}
				else if(new.flag == 3)//查看
				{
					printf("%s%s\n\n",ctime(&new.now_time), new.buf);
				}
				break;
			
			case 7://文件传输
				if(new.flag == 0)
				{
					printf("%s[%s]请求向您发送文件[%s],若同意请输入 /$apt %s %s\n\n",ctime(&new.now_time),new.from, new.file_send, new.from, new.file_send);
				}
				else if(new.flag == 1)
				{				

					char t[20];
					strcpy(t, new.from);
					strcpy(new.from, new.to);
					strcpy(new.to, t);
					
					new_t *temp = (new_t *)malloc(sizeof(new_t));
					memcpy(temp, &new, sizeof(new_t));
					temp->flag = 2;
					temp->conn_fd = conn_fd;
					pthread_create(&thread, NULL, (void*)send_file, temp);
				}
				else if(new.flag == 2)
				{
					file_fd = open(new.file_recv, O_WRONLY | O_CREAT | O_APPEND, 0666);
	
					if(new.grp.num == -1)
					{
						printf("[提醒]文件%s接收成功\n",new.file_recv);
					}
					else
					{
						write(file_fd, new.buf, new.file_num);
					}
										
					close(file_fd);
				}

				break;
	
			default :
				break;
		}
	}

	return;
}

//创建套接字并连接服务端
int main()
{
	int conn_fd;
	new_t new;
    struct sockaddr_in serv_addr;
    pthread_t thread;


    
    memset(&serv_addr,0,sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_NUM);
    inet_aton(IPADDR,&serv_addr.sin_addr);

	//创建套接字
    conn_fd = socket(AF_INET,SOCK_STREAM,0);
    if(conn_fd < 0)
    {
		perror("socket");
        return 0;
    }
    //连接服务器
    if(connect(conn_fd,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr_in)) < 0)
    {
        perror("connect");
        return 0;
    }
	
	while(1)//注册登陆流程
	{		
		printf("<1>注册 <2>登陆 <0>退出\n");
		scanf("%d",&new.type);
		while(getchar()!='\n');
		switch(new.type)
		{
			case 1:
				printf("输入用户名：");
				scanf("%s",new.user.name);
				printf("输入密码：");
				getchar();
				system("stty -echo");
				system("stty -icanon");
				get_pass(new.user.pass);
				//scanf("%s",new.user.pass);
				system("stty echo");
				system("stty icanon");
				new.conn_fd = conn_fd;
				time(&new.now_time);
				send(conn_fd, &new, sizeof(new_t), 0);
				recv(conn_fd, &new, sizeof(new_t), 0);
				printf("[提醒]%s\n",new.buf);
				break;
			case 2:
				printf("输入用户名：");
				scanf("%s",new.user.name);
				time(&new.now_time);
				printf("输入密码：");
				getchar();
				system("stty -echo");
				system("stty -icanon");
				get_pass(new.user.pass);
				//scanf("%s",new.user.pass);
				system("stty echo");
				system("stty icanon");
				send(conn_fd, &new, sizeof(new_t), 0);				
				recv(conn_fd, &new, sizeof(new_t), 0);
				printf("[提醒]%s\n",new.buf);
				break;
			case 0:
				return 0;
			default:
				break;
		}
		//登陆成功后跳出循环
		if(strcmp(new.buf, "登陆成功") == 0)
		{				
			strcpy(user_name,new.user.name);
			break;
		}
	}	

	//创建线程用于接受服务端消息
	pthread_create(&thread, NULL, (void*)def, &conn_fd);

	char buf[500],cmd[10][500];
	int num;
	
	print();//显示提示界面

	while(1)
	{ //输入解析


		memset(&new, 0 ,sizeof(new_t));

		if(get_cmd(buf) == 0)
		{
			printf("[提醒]输入为空，请重新输入\n");
			continue;
		}
		
		if(buf[0]=='/')
		{
			memset(cmd, 0 ,sizeof(cmd));
			num = made_cmd(cmd, buf);
		}
		else
		{
			if(cmd[0][2]!='c')
			{
				memset(cmd, 0 ,sizeof(cmd));
			}
		}
		
		new.conn_fd = conn_fd;
		time(&new.now_time);
		if(strcmp(cmd[0], "/@call") == 0 && num == 2)//私聊
		{
			new.type = 3;
			strcpy(new.from, user_name);
			strcpy(new.to, cmd[1]);
			strcpy(new.buf, buf);
			save_chat(new,new.to);
			send(conn_fd, &new, sizeof(new_t), 0);
		}
		else if(strcmp(cmd[0], "/#call") == 0 && num == 2)//群聊
		{
			new.type = 4;
			strcpy(new.from, user_name);
			strcpy(new.grp.name, cmd[1]);
			strcpy(new.buf, buf);
			//save_chat(new,new.to);
			send(conn_fd, &new, sizeof(new_t), 0);
				
		}
		else if(strcmp(cmd[0], "/@add") == 0 && num == 2)
		{
			new.type = 5;
			new.flag = 0;
			strcpy(new.from, user_name);
			strcpy(new.to, cmd[1]);
			send(conn_fd, &new, sizeof(new_t), 0);
		}	
		else if(strcmp(cmd[0], "/#add") == 0 && num == 2)//创建群组
		{
			new.type = 6;
			new.flag = 1;
			strcpy(new.from, user_name);
			strcpy(new.grp.make, user_name);
			strcpy(new.grp.name, cmd[1]);
			printf("请输入讨论组成员人数：");
			do
			{
				scanf("%d",&new.grp.num);
				while(getchar()!='\n');
			}while(new.grp.num<0);
			int i;
			for(i=0;i<new.grp.num;i++)
			{
				printf("输入成员%d：",i+1);
				scanf("%s",new.grp.people[i]);
				while(getchar()!='\n');
			}						
			puts(new.grp.people[0]);
			send(conn_fd, &new, sizeof(new_t), 0);		
		}
		else if(strcmp(cmd[0], "/@del") == 0 && num == 2)//删除好友
		{
			new.type = 5;
			new.flag = 2;
			strcpy(new.from, user_name);
			strcpy(new.to, cmd[1]);
			send(conn_fd, &new, sizeof(new_t), 0);
		}
		else if(strcmp(cmd[0], "/#del") == 0 && num == 2)//删除群组
		{
			new.type = 6;
			new.flag = 2;
			strcpy(new.from, user_name);
			strcpy(new.grp.name, cmd[1]);
			send(conn_fd, &new, sizeof(new_t), 0);
		}
		else if(strcmp(cmd[0], "/@look") == 0 && num == 2)
		{
				if(strcmp(cmd[1], "all") != 0)
				{
					printf("[提醒]命令不合法，请重新输入\n");
					continue;
				}
				new.type = 5;
				new.flag = 3;
				strcpy(new.from, user_name);
				strcpy(new.to, buf);
				send(conn_fd, &new, sizeof(new_t), 0);
		}
		else if(strcmp(cmd[0], "/#look") == 0 && num == 2)//查看群组
		{
			if(strcmp(cmd[1], "me") != 0 && strcmp(cmd[1], "all") != 0)
			{
				printf("[提醒]命令不合法，请重新输入\n");
				continue;
			}
			new.type = 6;
			new.flag = 3;
			strcpy(new.from, user_name);
			strcpy(new.grp.name, cmd[1]);
			send(conn_fd, &new, sizeof(new_t), 0);			
		}
		else if(strcmp(cmd[0], "/@apt") == 0 && num == 2)//接受好友
		{			
			new.type = 5;
			new.flag = 1;
			strcpy(new.from, user_name);
			strcpy(new.to, cmd[1]);
			send(conn_fd, &new, sizeof(new_t), 0);
		}
		else if(strcmp(cmd[0], "/$apt") == 0 && num == 3)//接受文件
		{
			struct stat temp;
			printf("请输入接收后的文件名：");
			while(1)
			{
				scanf("%s",new.file_recv);
				while(getchar()!='\n');			
				if(stat(new.file_recv, &temp) == 0)
				{
					printf("[提醒]%s已存在，请重新指定文件名\n",new.file_recv);
					continue;
				}
				else
				{
					break;
				}
			}
			new.type = 7;
			new.flag = 1;
			strcpy(new.from, user_name);
			strcpy(new.to, cmd[1]);
			strcpy(new.file_send, cmd[2]);
	
			send(conn_fd, &new, sizeof(new_t), 0);
		}
		else if(strcmp(cmd[0], "/$send") == 0 && num == 3)//发送文件
		{
			if(stat(cmd[2], &new.file) == 0)
			{
				new.type = 7;
				new.flag = 0;
				strcpy(new.file_send, cmd[2]);
				strcpy(new.from, user_name);
				strcpy(new.to, cmd[1]);
				//sprintf("%s");
				send(conn_fd, &new, sizeof(new_t), 0);
			}
			else
			{
				printf("[提醒]该文件不存在\n");
			}
		}
		else if(strcmp(cmd[0], "/quit") == 0 && num == 1)//退出
		{
			printf("再见～\n");
			exit(1);
		}
		else
		{
			if(buf[0]!='/')
			{
				printf("[提醒]请先输入命令，再做操作\n");	
			}
			else
			{
				printf("[提醒]命令不合法,请重新输入\n");
			}
		}
	}
	
	return 0;	
}
