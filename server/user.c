/*************************************************************************
	> File Name: user.c
	> Author: 师毅
	> Mail:  shiyi19960604@gmail.com
	> Created Time: 2015年08月03日 星期一 15时49分01秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include"user.h"

#define USER_FILE "user.txt"
#define GRP_FILE "group.txt"
#define LOG_FILE "log.txt"

extern int f[2001];

//保存系统日志
int save_log(char test[])
{
	int fd;
	
	fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0666);
	if(fd == -1)
	{
		perror("open");
	}

	int i=0;
	
	while(test[i]!='\0')
	{
		write(fd, &test[i++], 1);
	}

    close(fd);
    return 1;
}



//根据用户名查找id，若存在返回id，否则，返回0
int find_user_id(char name[], user_t *buf)
{
    int fd;
    fd = open(USER_FILE, O_RDONLY | O_CREAT, 0666);


    if(fd == -1)
    {
        perror("open");
        return 0;
    }

    while(read(fd, buf, sizeof(user_t)))
    {
        if(strcmp(buf->name, name) == 0)
        {
            close(fd);
            return buf->id;
        }
    }

    close(fd);

    return 0;
}

int find_user_name(int id, user_t *buf)
{
	int fd;

    fd = open(USER_FILE, O_RDONLY);

    if(fd == -1)
    {
        perror("open");
        return 0;
    }

    while(read(fd, buf, sizeof(user_t)))
    {
        if(buf->id == id)
        {
            close(fd);
            return 1;
        }
    }

    close(fd);

    return 0;
}

int add_user(user_t *buf)
{
    int fd;

    buf->id = make_id();

    fd = open(USER_FILE, O_WRONLY | O_CREAT | O_APPEND, 0666);

    if(fd == -1)
    {
        perror("open");
        return 0;
    }
    
    if(write(fd,buf,sizeof(user_t)) == -1)
    {
        perror("write");
        return 0;
    }

    close(fd);
    return 1;
}

int ID=1;

int make_id()
{
    return ID++;
}

//查看好友是否存在
int find_friend(char user[],char fri[])
{
	char file[20];
	int fd;
	user_t buf;
	
	sprintf(file,"%s_friend.txt",user);
	
    fd = open(file, O_RDONLY);

    if(fd == -1)
    {
        return 0;
    }

    while(read(fd, &buf, sizeof(user_t)))
    {
        if(strcmp(buf.name, fri) == 0)
        {
            close(fd);
            return 1;
        }
    }

    close(fd);

    return 0;
}

//添加好友
int add_friend(char user[],char fri[])
{
	char file[20];
	int fd;
	user_t buf;
	
	sprintf(file,"%s_friend.txt",user);

	if(find_user_id(fri,&buf) == 0)
	{
		return 0;
	}

    fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0666);

    if(fd == -1)
    {
        perror("open");
        exit(1);
    }
    
    if(write(fd,&buf,sizeof(user_t)) == -1)
    {
        perror("write");
        exit(1);
    }

    close(fd);
    return 1;
}

int del_friend(char user[],char fri[])
{
	char file[20];
	char temp_file[20] = "temp.txt";
	int fd;
	int temp_fd;
	user_t buf;
	
	sprintf(file,"%s_friend.txt",user);
	if(rename(file, temp_file) < 0)
	{
		exit(1);
	}
	
	fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0666);
	if(fd == -1)
	{
		perror("open");
	}
	
	temp_fd = open(temp_file, O_RDONLY);

    if(temp_fd == -1)
    {
        perror("open");
    }
	
	while(read(temp_fd,&buf,sizeof(user_t)))
	{
		if(strcmp(buf.name,fri) == 0)
		{
			continue;
		}
		write(fd, &buf, sizeof(user_t));
	}

	close(fd);
	close(temp_fd);
	
	remove(temp_file);
	
    return 1;
}

int look_firend(char user[], char str[])
{
	char file[20];
	int fd;
	user_t buf;
	
	sprintf(file,"%s_friend.txt",user);	

    fd = open(file, O_RDONLY);

    if(fd == -1)
    {
        return 0;
    }
    
    str[0]='\0';
    int i=0;
    while(read(fd, &buf, sizeof(user_t)))
    {
    	i++;
    	if(f[buf.id])
    	{
			strcat(str,"\033[1;33m");
			strcat(str,buf.name);
			strcat(str,"\033[0m\t");
    	}
    	else
    	{
			strcat(str,buf.name);
			strcat(str,"\t");
    	}
    	if(i%4==0)
    	{
    		strcat(str,"\n");
    	}
    }
	
	close(fd);

	
//	printf("=== %s\n",str);
	return 1;
}

int find_group(char name[],grp_t *buf)
{
    int fd;

    fd = open(GRP_FILE, O_RDONLY);

    if(fd == -1)
    {
        perror("open");
        return 0;
    }
    while(read(fd, buf, sizeof(grp_t)))
    {
        if(strcmp(buf->name, name) == 0)
        {
            close(fd);
            return 1;
        }
    }

    close(fd);

    return 0;
		
}

int add_group(grp_t *buf)
{
    int fd;
	grp_t grp;

	if(find_group(buf->name, &grp)==1)
	{
		return 0;
	}

    fd = open(GRP_FILE, O_WRONLY | O_CREAT | O_APPEND, 0666);

    if(fd == -1)
    {
        perror("open");
        return 0;
    }
    
    if(write(fd,buf,sizeof(grp_t)) == -1)
    {
        perror("write");
        return 0;
    }

    close(fd);
    return 1;
	
}

int del_group(new_t *new)
{
	grp_t grp;
	char temp_file[20] = "temp.txt";
	int fd;
	int temp_fd;
	int flag=0, i;
	
	if(find_group(new->grp.name, &new->grp) == 0)
	{
		return 0;
	}
	
	if(rename(GRP_FILE, temp_file) < 0)
	{
		exit(1);
	}
	
	fd = open(GRP_FILE, O_WRONLY | O_CREAT | O_APPEND, 0666);
	if(fd == -1)
	{
		perror("open");
	}
	
	temp_fd = open(temp_file, O_RDONLY);

    if(temp_fd == -1)
    {
        perror("open");
    }
	while(read(temp_fd,&grp,sizeof(grp_t)))
	{
		if(strcmp(new->grp.name, grp.name) == 0)
		{
			if(strcmp(grp.make, new->from) == 0)
			{
				flag = 1;//解散群
			}
			else
			{
				for(i=0;i<grp.num;i++)
				{
					if(strcmp(grp.people[i],new->from) == 0)
					{
						break;
					}	
				}
				if(i!=grp.num)
				{
					for(;i<grp.num-1;i++)
					{
						strcpy(grp.people[i],grp.people[i+1]);
					}
					grp.num--;
					flag = 3;//退出
				}
				else
				{
					flag = 2;//并未加入该群
					
				}
				write(fd, &grp, sizeof(grp_t));
			}
		}
		else
		{
			write(fd, &grp, sizeof(grp_t));	
		}
	}
	remove(temp_file);
	
    return flag;
}

int look_group(new_t *new)
{
	int fd, i, flag=0;
	grp_t grp;
	
    fd = open(GRP_FILE, O_RDONLY);

	if(fd == -1)
    {
        return 0;
    }

    while(read(fd, &grp, sizeof(grp_t)))
    {
    	flag = 0;
    	for(i=0;i<grp.num;i++)
    	{
    		if(strcmp(new->from, grp.people[i]) == 0)
    		{
    			flag = 1;
    			break;
    		}
    	}
    	if(strcmp(new->from, grp.make) == 0)
    	{
    		flag = 1;
    	}
    	
    	if(strcmp(new->grp.name, "all") == 0)
    	{
    		flag = 1;
    	}
    	
    	if(flag == 1)
    	{
    		strcat(new->buf, grp.name);
    		strcat(new->buf, " 创建者：");
    		strcat(new->buf, grp.make);
    		strcat(new->buf,"\n");
			strcat(new->buf, "成员：");
			if(grp.num == 0)
			{
				strcat(new->buf, "孤家寡人");
			}
    		for(i=0;i<grp.num;i++)
    		{
	    		strcat(new->buf, grp.people[i]);
	    		strcat(new->buf, " ");
    		}
    		strcat(new->buf, "\n\n");
    	}
    }
	
	close(fd);
	if(flag==0)
	{
		strcpy(new->buf, "暂无讨论组，快去创建吧\n");
	}

	return flag;
}


