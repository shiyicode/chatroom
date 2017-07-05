/*************************************************************************
	> File Name: 1.c
	> Author: 师毅
	> Mail:  shiyi19960604@gmail.com
	> Created Time: 2015年08月07日 星期五 08时41分20秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<fcntl.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<time.h>
#include"user.h"



#define LISTENMAX 200
#define PORT 8001
#define IP_ADDR "127.0.0.1"
#define EPOLL_MAX 2000

time_t nowtime;
int f[2001] = {0};


int send_to_one(new_t *new)
{
	int temp_id;
	user_t user;
	
	temp_id = find_user_id(new->to, &user);
	if(temp_id == 0)//好友名不存在，通知用户
	{//printf("ssssss%s %s %s\n",new->to,new->from,new->buf);
		return 0;
	}
	else
	{
		while(1)
		{//printf("==%s %s %s\n",new->to,new->from,new->buf);
			if(f[temp_id])
			{
				if(send(f[temp_id], new, sizeof(new_t), 0) < 0)
				{
					perror("send");
				}
				break;
			}
			sleep(1);
		}	
		return 1;//发送成功
	}
	free(new);

}

int send_to_more(new_t *new)
{
	pthread_t thread;
	new_t *data;
	int i=0;
	
	data = (struct News *)malloc(sizeof(new_t));
	memcpy(data,new,sizeof(new_t));
	strcpy(data->to,new->grp.make);
	
	//printf("==%s %s %s\n",data->to,data->from,data->buf);
	thread = pthread_create(&thread, NULL, (void*)send_to_one, data);
	

	for(i=0;i<new->grp.num;i++)
	{
		data = (struct News *)malloc(sizeof(new_t));
		memcpy(data,new,sizeof(new_t));
		strcpy(data->to,new->grp.people[i]);
			
		thread = pthread_create(&thread, NULL, (void*)send_to_one, data);
	}	
	return 1;
}

void def(struct News *new)
{
	user_t user;
	int conn_fd;
	int ret;
	char test[500];
	
	conn_fd = new->conn_fd;
	switch (new->type)
	{	
		case 1://注册
			
			if(find_user_id(new->user.name, &user) != 0)
			{

				strcpy(new->buf, "用户名已存在");
			}
			else
			{
				if(add_user(&new->user) == 1)
				{
					strcpy(new->buf, "注册成功");
					sprintf(test, "%s%s注册成功\n\n",ctime(&new->now_time), new->user.name);
					save_log(test);
				}
			}

			send(conn_fd, new, sizeof(new_t), 0);
			break;
		
		case 2://登陆
		
			if(find_user_id(new->user.name, &user) == 0)
			{
				strcpy(new->buf, "用户名不存在");
			}
			else
			{

				if(strcmp(new->user.pass, user.pass) == 0)
				{
					if(f[user.id]==0)
					{
						strcpy(new->buf, "登陆成功");
						printf("%s 上线了！\n",user.name);
						sprintf(test, "%s\n%s上线了\n\n",ctime(&new->now_time), user.name);
						save_log(test);
						f[user.id] = conn_fd;
					}
					else
					{
						strcpy(new->buf, "账号已在线");
					}
				}
				else
				{
					strcpy(new->buf, "密码错误");
				}
			}
			send(conn_fd, new, sizeof(new_t), 0);
			break;
		
		case 3://私聊
			if(find_friend(new->from,new->to) == 1)//并非好友关系
			{
				send_to_one(new);
			}
			else
			{
				strcpy(new->buf, "不是您的好友，请先建立好友关系吧");
				send(new->conn_fd, new, sizeof(new_t), 0);
			}
			break;
		case 4://群聊
			if(find_group(new->grp.name, &new->grp) == 0)
			{				
				strcpy(new->buf, "不存在");
				new->from[0]='\0';
				send(new->conn_fd, new, sizeof(new_t), 0);
			}
			else
			{
			
				send_to_more(new);
			}
			break;
		case 5:
			if(new->flag == 0)//请求
			{
				if(find_user_id(new->to, &user) == 0)
				{
					strcpy(new->buf, "不存在");
					send(new->conn_fd, new, sizeof(new_t), 0);
				}
				else
				{
					if(find_friend(new->from, new->to) == 1)
					{
						strcpy(new->buf, "已经是您的好友");
						send(new->conn_fd, new, sizeof(new_t), 0);
					}
					else
					{
						send_to_one(new);
					}
				}
			}
			else if(new->flag == 1)//添加
			{
				strcpy(new->buf, "成为您的好友");
				add_friend(new->from, new->to);
				send(new->conn_fd, new, sizeof(new_t), 0);
				add_friend(new->to, new->from);
				send_to_one(new);
			}
			else if(new->flag == 2)//删除
			{
				strcpy(new->buf, "与您解除了好友关系");
				del_friend(new->from, new->to);
				send(new->conn_fd, new, sizeof(new_t), 0);
				del_friend(new->to, new->from);
				send_to_one(new);
			}
			else if(new->flag == 3)//查看
			{
				look_firend(new->from, new->buf);
				send(new->conn_fd, new, sizeof(new_t), 0);
			}
			break;
		
		case 6:
		
			if(new->flag == 1)//创建
			{
				if(add_group(&new->grp) == 1)
				{
					strcpy(new->buf, "创建成功，您已是其中一员");
					sprintf(test, "%s%s创建了讨论组%s\n\n",ctime(&new->now_time), new->from, new->grp.name);
					save_log(test);
					send_to_more(new);
				}
				else
				{
					strcpy(new->buf, "创建失败，讨论组名已存在");
					send(new->conn_fd, new, sizeof(new_t), 0);
				}
			}
			else if(new->flag == 2)//退出
			{
				ret = del_group(new);
				if(ret == 0)//群不存在
				{
					sprintf(new->buf, "[提醒]退出失败，讨论组%s不存在",new->grp.name);
					send(new->conn_fd, new, sizeof(new_t), 0);
				}
				else if(ret == 1)//解散群
				{
					sprintf(new->buf, "[提醒]%s解散了讨论组%s",new->from, new->grp.name);
					sprintf(test, "%s%s解散了讨论组%s\n\n",ctime(&new->now_time), new->from, new->grp.name);
					save_log(test);
					send_to_more(new);
				}
				else if(ret == 2)//并未加入该群
				{
					sprintf(new->buf, "[提醒]退出失败，您并未加入讨论组%s",new->grp.name);
					send(new->conn_fd, new, sizeof(new_t), 0);
				}
				else//退出
				{
					sprintf(new->buf, "[提醒]%s离开了讨论组%s",new->from, new->grp.name);
					sprintf(test, "%s%s离开了讨论组%s\n\n",ctime(&new->now_time), new->from, new->grp.name);
					save_log(test);
					send_to_more(new);
				}
				
			}
			else if(new->flag == 3)//查看
			{
				look_group(new);
				send(new->conn_fd, new, sizeof(new_t), 0);
			}			
			break;
			
		case 7:
			
			if(new->flag == 0)
			{
				send_to_one(new);
			}
			else if(new->flag == 1)
			{
				send_to_one(new);
			}
			else if(new->flag == 2)
			{
				send_to_one(new);
			}
		
		default:
			break;
	}

	free(new);
	pthread_exit(0);
	return;
}

int main()
{
	struct epoll_event ev, events[LISTENMAX];
	int epfd, listen_fd, nfds, sock_fd;
	struct sockaddr_in addr;
	socklen_t addr_len;
	int n, i, j;
	pthread_t thread;
	new_t *new;
	user_t user;
	char test[500];
	
	epfd = epoll_create(EPOLL_MAX+1);
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	i=1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(int));
	ev.data.fd = listen_fd;
	ev.events = EPOLLIN;
	epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);
		
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);	
	addr.sin_port = htons(PORT);

	if(bind(listen_fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0)
	{
		perror("bind");
		exit(1);
	}
	
	listen(listen_fd, LISTENMAX);
	
	addr_len = sizeof(struct sockaddr_in);
	
	while(1)
	{
		nfds = epoll_wait(epfd, events, EPOLL_MAX, 1000);
		
		for(i=0; i<nfds; i++)
		{
			if(events[i].data.fd == listen_fd)
			{
				sock_fd = accept(listen_fd, (struct sockaddr*)&addr,&addr_len);
				printf("accept a new client: %s\n",inet_ntoa(addr.sin_addr));
				ev.data.fd = sock_fd;
				ev.events = EPOLLIN;
				epoll_ctl(epfd, EPOLL_CTL_ADD, sock_fd, &ev);
			}
			
			else if(events[i].events & EPOLLIN)
			{
				new = (struct News*)malloc(sizeof(new_t));
				n = recv(events[i].data.fd, new, sizeof(struct News), 0);
				
				if(n < 0)
				{
					close(events[i].data.fd);
					perror("recv");
					continue;
				}
				else if(n == 0)
				{
					ev.data.fd = events[i].data.fd;
					epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);			
					for(j=1;j<=2000;j++)
					{
						if(f[j] == events[i].data.fd)
						{
							break;
						}
					}
					if(j <= 2000)
					{
						f[j] = 0;
						find_user_name(j, &user);
						printf("%s 下线了！\n",user.name);
						time(&new->now_time);
						sprintf(test, "%s%s\n%s下线了\n",ctime(&new->now_time),inet_ntoa(addr.sin_addr),user.name);
						save_log(test);
					}

					close(events[i].data.fd);
					
					continue;
				}
				
				new->conn_fd = events[i].data.fd;
				pthread_create(&thread, NULL, (void *)def, new);
			}
		}
	}
}
