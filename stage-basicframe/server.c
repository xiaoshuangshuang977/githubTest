/*************************************************************************
#	 FileName	: server.c
#	 Author		: fengjunhui 
#	 Email		: 18883765905@163.com 
#	 Created	: 2018年12月29日 星期六 13时44分59秒
 ************************************************************************/

#include<stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <pthread.h>
#include <time.h>

#include "common.h"

sqlite3 *db;  //仅服务器使用

int process_user_or_admin_login_request(int acceptfd,MSG *msg)//普通用户或管理员登录
{
	printf("------------%s-----------%d.\n",__func__,__LINE__);
	//封装sql命令，表中查询用户名和密码－存在－登录成功－发送响应－失败－发送失败响应	
	char sql[DATALEN] = {0};
	char *errmsg;
	char **result;
	int nrow,ncolumn;

	msg->info.usertype =  msg->usertype;
	strcpy(msg->info.name,msg->username);
	strcpy(msg->info.passwd,msg->passwd);
	
	printf("usrtype: %#x-----usrname: %s---passwd: %s.\n",msg->info.usertype,msg->info.name,msg->info.passwd);
	sprintf(sql,"select * from usrinfo where usertype=%d and name='%s' and passwd='%s';"
	,msg->info.usertype,msg->info.name,msg->info.passwd);
	if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
		printf("---****----%s.\n",errmsg);		
	}else{
		//printf("----nrow-----%d,ncolumn-----%d.\n",nrow,ncolumn);		
		if(nrow == 0){
			strcpy(msg->recvmsg,"name or passwd failed.\n");
			send(acceptfd,msg,sizeof(MSG),0);
		}else{
			strcpy(msg->recvmsg,"OK");
			send(acceptfd,msg,sizeof(MSG),0);
		}
	}
	return 0;	
}

int process_user_modify_request(int acceptfd,MSG *msg)//用户修改
{
	printf("----普通用户修改--------%s-----------%d.\n",__func__,__LINE__);
	printf("------------%s-----------%d.\n",__func__,__LINE__);
	char sql[DATALEN] = {0};
	char *errmsg;
	char **result;
	int nrow,ncolumn;
	char text[32] = {0};
	time_t t;
	struct tm *tm;
	time(&t);
	tm = localtime(&t);
	sprintf(text,"%4d-%02d-%02d %02d:%02d:%02d",tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
	sprintf(sql,"insert into historyinfo(time, name, words) values ('--%s--','---对%s操作----','**普通用户修改**');",text,msg->info.name);
	memset(text,0,32);
	if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
	{
		printf("---****----%s.\n",errmsg);
	}else
	{
		printf("记录成功!\n");
	}
	if(msg->flags == 1)//用户修改家庭住址
	{
		sprintf(sql,"update usrinfo set addr = '%s' where name = '%s';",msg->info.addr,msg->info.name);
		printf("-----***--进入家庭住址----\n");
		if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
		{
			printf("---**%s**-修改家庭住址错误---",errmsg);
		}else
		{
			printf("-------正在修改家庭住址-----%s-----------%d.\n",__func__,__LINE__);
			strcpy(msg->recvmsg,"OK");
			printf("修改家庭住址\n");
			send(acceptfd,msg,sizeof(MSG),0);
		}
	}
	if(msg->flags == 2)//用户修改手机号
	{
		sprintf(sql,"update usrinfo set phone = '%s' where name = '%s';",msg->info.phone,msg->info.name);
		printf("-----***--进入手机号----\n");
		if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
		{
			printf("---**%s**-修改手机号错误---",errmsg);
		}else
		{
			printf("------正在修改手机号------%s-----------%d.\n",__func__,__LINE__);
			strcpy(msg->recvmsg,"OK");
			printf("修改手机号\n");
			send(acceptfd,msg,sizeof(MSG),0);
		}
		
	}
	if(msg->flags == 3)//用户修改密码
	{
		sprintf(sql,"update usrinfo set passwd = '%s' where name = '%s' ;",msg->info.passwd,msg->info.name);
		printf("-----***--进入密码----\n");
		if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
		{
			printf("---**%s**-修改密码错误---",errmsg);
		}else
		{
			printf("-------正在修改密码-----%s-----------%d.\n",__func__,__LINE__);
			strcpy(msg->recvmsg,"OK");
			printf("修改用户密码\n");
			send(acceptfd,msg,sizeof(MSG),0);	
		}
	}
	memset(sql,0,sizeof(sql));
	return 0;	
}

int process_user_query_request(int acceptfd,MSG *msg)//用户查询
{

	printf("--------普通用户查询----%s-----------%d.\n",__func__,__LINE__);
	char sql[DATALEN] = {0};
	char buf[DATALEN] = {0};
	char *errmsg; //错误信息
	char **result; //
	int nrow,ncolumn; //行 列
	int i = 0;
	char text[32] = {0};
	time_t t;
	
	struct tm *tm;
	time(&t);
	tm = localtime(&t);
	sprintf(text,"%4d-%02d-%02d %02d:%02d:%02d",tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,\
				tm->tm_hour,tm->tm_min,tm->tm_sec);
	sprintf(sql,"insert into historyinfo(time, name, words) values ('--%s--','---对%s操作----','**普通用户查询**');",text,msg->info.name);
	memset(text,0,32);
	printf("查询时间:%s\n",text);
	if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
	{
		printf("---****----%s.\n",errmsg);
	}else
	{
		printf("记录成功!\n");
	}
	memset(sql,0,DATALEN);
	sprintf(sql,"select * from usrinfo where name = '%s'",msg->username);
	if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
	{
		printf("---****----%s.\n",errmsg);		
	}else
	{
		printf("人名存在\n");
		strcpy(msg->recvmsg,"OK");
		for(i = 0; i<ncolumn;i++)
		{
			sprintf(buf,"%-8s",result[i]);
		}
		printf("\n");
		int index = ncolumn;
		sprintf(buf,"%s\t%s\t%s     %s       %s    %s\t%s\t%s\t%s   %s  %s \n",result[ncolumn+index-11],result[index+ncolumn-10],
		result[index+ncolumn-9],result[index+ncolumn-8],result[index+ncolumn-7],result[index+ncolumn-6],
		result[index+ncolumn-5],result[index+ncolumn-4],
		result[index+ncolumn-3],result[index+ncolumn-2],result[index+ncolumn-1]);
		send(acceptfd,msg,sizeof(MSG),0);
		send(acceptfd,buf,sizeof(buf),0);
		memset(buf,0,sizeof(buf));
	}
	memset(sql,0,sizeof(sql));
	return 0;
}

int process_admin_modify_request(int acceptfd,MSG *msg)//管理员修改
{
	printf("-------管理员修改-----%s-----------%d.\n",__func__,__LINE__);
	
	char sql[DATALEN] = {0};
	char *errmsg;
	char **result;
	int nrow,ncolumn;
	
	char text[32] = {0};
	time_t t;
	struct tm *tm;
	time(&t);
	tm = localtime(&t);
	sprintf(text,"%4d-%02d-%02d %02d:%02d:%02d",tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
	sprintf(sql,"insert into historyinfo(time, name, words) values ('--%s--','---对%s操作----','**管理员修改**');",text,msg->info.name);
	printf("管理员修改历史记录：%s\n",text);
	memset(text,0,32);
	if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
	{
		printf("---****----%s.\n",errmsg);
	}else
	{
		printf("记录成功!\n");
	}
	sprintf(sql,"select * from usrinfo where name = '%s' and staffno = '%d';",msg->info.name,msg->info.staffno);
	printf("正在查看数据库中的信息、\n");
	if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
	{
		printf("---****----%s.\n",errmsg);
	}else
	{
		
		printf("看一下是否有此人的存在!\n");
		if(0 == nrow)
		{
			printf("人名不存在!\n");
			printf("---**%s**-----\n",msg->info.name);
			strcpy(msg->recvmsg,"NO");
			send(acceptfd,msg,sizeof(MSG),0);
		}else
		{
			printf("此人存在 正在查询要修改的内容\n");
			sprintf(sql,"select * from usrinfo where staffno='%d' and name = '%s';",msg->info.staffno,msg->info.name);
			printf("---**%s**-----\n",msg->info.name);
			if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
				printf("---****----%s.\n",errmsg);		
			}else{
				if(msg->flags == 1)
				{
					recv(acceptfd,msg,sizeof(MSG),0);
					printf("修改用户名!\n");
					sprintf(sql,"update usrinfo set name = '%s' where staffno='%d';",msg->info.name,msg->info.staffno);
					if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
					{
						printf("修改错误!----%s----\n",errmsg);
					}else
					{
						printf("修改成功!\n");
						printf("修改的用户名为:%s\n",msg->info.name);
						strcpy(msg->recvmsg,"OK");
						send(acceptfd,msg,sizeof(MSG),0);
					}
				}
				if(msg->flags == 2)
				{
					printf("修改年龄!\n");
					sprintf(sql,"update usrinfo set age = '%d' where staffno = '%d';",msg->info.age,msg->info.staffno);
					if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
					{
						printf("修改错误!----%s----\n",errmsg);
					}else
					{
						printf("修改成功!\n");
						printf("修改的年龄为:%s",msg->info.name);
						strcpy(msg->recvmsg,"OK");
						send(acceptfd,msg,sizeof(MSG),0);
					}
				}
				if(msg->flags == 3)
				{
					printf("修改家庭住址!\n");
					sprintf(sql,"update usrinfo set addr = '%s' where staffno = '%d';",msg->info.addr,msg->info.staffno);
					if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
					{
						printf("修改错误!----%s----\n",errmsg);
					}else
					{
						printf("修改成功!\n");
						printf("修改的家庭住址为%s:",msg->info.addr);
						strcpy(msg->recvmsg,"OK");
						send(acceptfd,msg,sizeof(MSG),0);
					}
				}
				if(msg->flags == 4)
				{
					printf("修改电话!\n");
					sprintf(sql,"update usrinfo set phone = '%s' where staffno = '%d';",msg->info.phone,msg->info.staffno);
					if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
					{
						printf("修改错误!----%s----\n",errmsg);
					}else
					{
						printf("修改成功!\n");
						printf("修改的电话为:%s",msg->info.phone);
						strcpy(msg->recvmsg,"OK");
						send(acceptfd,msg,sizeof(MSG),0);
						
					}
				}
				if(msg->flags == 5)
				{
					printf("修改职位!\n");
					sprintf(sql,"update usrinfo set work = '%s' where staffno = '%d';",msg->info.work,msg->info.staffno);
					if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
					{
						printf("修改错误!----%s----\n",errmsg);
					}else
					{
						printf("修改成功!\n");
						printf("修改的职位为:%s",msg->info.work);
						strcpy(msg->recvmsg,"OK");
						send(acceptfd,msg,sizeof(MSG),0);						
					}
				}
				if(msg->flags == 6)
				{
					printf("修改工资!\n");
					sprintf(sql,"update usrinfo set salary = '%lf' where staffno = '%d';",msg->info.salary,msg->info.staffno);
					if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
					{
						printf("修改错误!----%s----\n",errmsg);
					}else
					{
						printf("修改成功!\n");
						printf("修改的工资为:%lf",msg->info.salary);
						strcpy(msg->recvmsg,"OK");
						send(acceptfd,msg,sizeof(MSG),0);
						
					}
				}
				if(msg->flags == 7)
				{
					printf("修改入职年月!\n");
					sprintf(sql,"update usrinfo set date = '%s' where staffno = '%d';",msg->info.date,msg->info.staffno);
					if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
					{
						printf("修改错误!----%s----\n",errmsg);
					}else
					{
						printf("修改成功!\n");
						printf("修改的入职年月为%s:",msg->info.date);
						strcpy(msg->recvmsg,"OK");
						send(acceptfd,msg,sizeof(MSG),0);
					}
				}
				if(msg->flags == 8)
				{
					printf("修改评级!\n");
					sprintf(sql,"update usrinfo set level = '%d' where staffno = '%d';",msg->info.level,msg->info.staffno);
					if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
					{
						printf("修改错误!----%s----\n",errmsg);
					}else
					{
						printf("修改成功!\n");
						printf("修改的评级为:%d",msg->info.level);
						strcpy(msg->recvmsg,"OK");
						send(acceptfd,msg,sizeof(MSG),0);
						
					}
				}
				if(msg->flags == 9)
				{
					printf("修改密码!\n");
					sprintf(sql,"update usrinfo set passwd = '%s' where staffno = '%d';",msg->info.passwd,msg->info.staffno);
					if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
					{
						printf("修改错误!----%s----\n",errmsg);
					}else
					{
						printf("修改成功!\n");
						printf("修改的密码为%s:",msg->info.passwd);
						strcpy(msg->recvmsg,"OK");
						send(acceptfd,msg,sizeof(MSG),0);
					}
				}	
			}
		}
	}
	memset(sql,0,sizeof(sql));
	return 0;	
}

int process_admin_adduser_request(int acceptfd,MSG *msg)//管理员添加用户
{
	printf("--------管理员添加用户----%s-----------%d.\n",__func__,__LINE__);
	char sql[DATALEN] = {0};
	char *errmsg;
	char **result;
	int nrow,ncolumn;
	
	char text[32] = {0};
	time_t t;
	struct tm *tm;
	time(&t);
	tm = localtime(&t);
	sprintf(text,"%4d-%02d-%02d %02d:%02d:%02d",tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
	sprintf(sql,"insert into historyinfo(time, name, words) values ('--%s--','---对%s操作----','**管理员添加用户**');",text,msg->info.name);
	printf("管理员添加用户记录%s\n",text);
	memset(text,0,32);
	
	sprintf(sql,"select * from usrinfo where staffno = '%d';",msg->info.staffno);
	if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
	{
		printf("---****----%s.\n",errmsg);
	}else
	{
		if(0 == nrow)
		{
			if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
			{
				printf("---****----%s.\n",errmsg);
			}else
			{
				printf("记录成功!\n");
			}
			sprintf(sql,"insert into usrinfo(staffno,usertype,name,passwd,age,phone,addr,work,date,level,\
			salary)	values (%d,%d,'%s','%s',%d,'%s','%s','%s','%s',%d,%lf);",\
			msg->info.staffno,msg->info.usertype,msg->info.name,msg->info.passwd,msg->info.age,msg->info.phone,\
			msg->info.addr,msg->info.work,msg->info.date,msg->info.level,msg->info.salary);
			if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
			{
				printf("---****----%s.\n",errmsg);
			}else
			{
				printf("成功添加成员!\n");
				strcpy(msg->recvmsg,"OK");
				send(acceptfd,msg,sizeof(MSG),0);
			}			
		}else
		{
			printf("此工号已占用！\n");
			strcpy(msg->recvmsg,"NO");
			send(acceptfd,msg,sizeof(MSG),0);
			}
	}

	memset(sql,0,sizeof(sql));
}

int process_admin_deluser_request(int acceptfd,MSG *msg)//管理员删除用户
{
	char sql[DATALEN] = {0};
	char *errmsg;
	char **result;
	int nrow,ncolumn;
	printf("---------管理员删除用户---%s-----------%d.\n",__func__,__LINE__);
	char text[32] = {0};
	time_t t;
	struct tm *tm;
	time(&t);
	tm = localtime(&t);
	sprintf(text,"%4d-%02d-%02d %02d:%02d:%02d",tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
	sprintf(sql,"insert into historyinfo(time, name, words) values ('--%s--','---对%s操作----','**管理员删除用户**');",text,msg->info.name);
	if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
	{
		printf("---****----%s.\n",errmsg);
	}else
	{
		printf("记录成功!\n");
	}
	//recv(acceptfd,msg,sizeof(MSG),0);
	memset(text,0,32);
	sprintf(sql,"select * from usrinfo where staffno = '%d' and name = '%s';",msg->info.staffno,msg->info.name);
	if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
	{
		printf("---****----%s.\n",errmsg);
	}else
	{
		if(nrow == 0)
		{
			printf("删除失败,没有工号为：%d,用户名为：%s的成员!\n",msg->info.staffno,msg->info.name);
			strcpy(msg->recvmsg,"NO");
			send(acceptfd,msg,sizeof(MSG),0);
		}else
		{
			sprintf(sql,"delete from usrinfo where staffno = %d and name = '%s';",msg->info.staffno,msg->info.name);
			if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
			{
				printf("---****----%s.\n",errmsg);
			}else
			{
				printf("成功删除成员!\n");
				strcpy(msg->recvmsg,"OK");
				send(acceptfd,msg,sizeof(MSG),0);
			}				
		}
	}
	memset(sql,0,sizeof(sql));
	return 0;
}

int process_admin_query_request(int acceptfd,MSG *msg)//管理员查询用户
{
	printf("-------管理员查询-----%s-----------%d.\n",__func__,__LINE__);
	char sql[DATALEN] = {0};
	char buf[DATALEN] = {0};
	char *errmsg; //错误信息
	char **result; //
	int nrow,ncolumn; //行 列
	int i , j;
	char text[32] = {0};
	time_t t;
	struct tm *tm;
	time(&t);
	tm = localtime(&t);
	sprintf(text,"%4d-%02d-%02d %02d:%02d:%02d",tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
	sprintf(sql,"insert into historyinfo(time, name, words) values ('--%s--','---对%s操作----','**管理员查询用户**');",text,msg->info.name);
	memset(text,0,32);
	if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
	{
		printf("---****----%s.\n",errmsg);
	}else
	{
		printf("记录成功!\n");
	}
		if(msg->flags == 1)
		{
			sprintf(sql,"select * from usrinfo where name = '%s';",msg->info.name);
			printf("msg->info.name = %s",msg->info.name);
			if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
			{
				printf("---**打印一个列表失败**----%s.\n",errmsg);	
			}else
			{
				if(nrow == 0)
				{
					printf("人名不存在!\n");
					strcpy(msg->recvmsg,"NO");
					send(acceptfd,msg,sizeof(MSG),0);
				}else
				{
					printf("人名存在\n");
					strcpy(msg->recvmsg,"OK");
					send(acceptfd,msg,sizeof(MSG),0);
					for(i = 0; i<ncolumn;i++)
					{
						sprintf(buf,"%-8s",result[i]);
					}
					printf("\n");
					int index = ncolumn;
					sprintf(buf,"%s\t%s\t%s     %s       %s    %s\t%s\t%s\t%s   %s  %s \n",result[ncolumn+index-11],result[index+ncolumn-10],
					result[index+ncolumn-9],result[index+ncolumn-8],result[index+ncolumn-7],result[index+ncolumn-6],
					result[index+ncolumn-5],result[index+ncolumn-4],
					result[index+ncolumn-3],result[index+ncolumn-2],result[index+ncolumn-1]);
					send(acceptfd,buf,sizeof(buf),0);
					memset(buf,0,DATALEN);			
				}
			}
		}
		if(msg->flags == 2)
		{
			sprintf(sql,"select * from usrinfo;");
			if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
			{
				printf("---**打印全部列表失败**----%s.\n",errmsg);	
			}else
			{
				msg->nrows = nrow;
				strcpy(msg->recvmsg,"OK");
				send(acceptfd,msg,sizeof(MSG),0);	
				int index = ncolumn;
				printf("msg->nrows=%d",msg->nrows);
				
				for(i=0;i<nrow;i++)
				{
					sprintf(buf,"%s\t%s\t%s     %s       %s    %s\t%s\t%s\t%s   %s  %s \n",result[ncolumn+index-11],result[index+ncolumn-10],
					result[index+ncolumn-9],result[index+ncolumn-8],result[index+ncolumn-7],result[index+ncolumn-6],
					result[index+ncolumn-5],result[index+ncolumn-4],
					result[index+ncolumn-3],result[index+ncolumn-2],result[index+ncolumn-1]);				
					send(acceptfd,buf,sizeof(buf),0);	
					memset(buf,0,sizeof(buf));	
					index+=ncolumn;	
				}
			}
		}
		memset(sql,0,sizeof(sql));
		return 0;
}
int process_admin_history_request(int acceptfd,MSG *msg)//管理员查看历史消息记录
{
	printf("--------历史记录----%s-----------%d.\n",__func__,__LINE__);
	char sql[DATALEN] = {0};
	char buf[DATALEN] = {0};
	char *errmsg;
	char **result;
	int nrow,ncolumn,i,j;
	sprintf(sql,"select * from historyinfo");
	if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
	{
		printf("---**%s**---\n",errmsg);
	}else
	{//打印 historyinfo表 里面的信息
		msg->nrows = nrow;
		strcpy(msg->recvmsg,"OK");
		send(acceptfd,msg,sizeof(MSG),0);	
		int index = ncolumn;
		printf("msg->nrows=%d",msg->nrows);
		
		for(i=0;i<nrow;i++)
		{
			sprintf(buf,"%s \t %s \t %s \n",result[ncolumn+index-3],result[index+ncolumn-2],
			result[index+ncolumn-1]);				
			send(acceptfd,buf,sizeof(buf),0);	
			memset(buf,0,sizeof(buf));	
			index+=ncolumn;	
		}
	}
	memset(sql,0,sizeof(sql));
	return 0;
}

int process_client_quit_request(int acceptfd,MSG *msg)//客户端退出
{
	printf("------客户端退出------%s-----------%d.\n",__func__,__LINE__);
	char sql[DATALEN] = {0};
	char *errmsg;
	char **result;
	int nrow,ncolumn;
	sprintf(sql,"drop table historyinfo");
	if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK)
	{
		printf("---**打印一个列表失败**----%s.\n",errmsg);	
	}else
	{
		printf("删除历史记录成功!\n");
	}
}

int process_client_request(int acceptfd,MSG *msg)
{
	printf("------------%s-----------%d.\n",__func__,__LINE__);
	switch (msg->msgtype)
	{
		case USER_LOGIN: //普通用户登录
		case ADMIN_LOGIN://管理员登录
			process_user_or_admin_login_request(acceptfd,msg);
			break;
		case USER_MODIFY://用户修改
			process_user_modify_request(acceptfd,msg);
			break;
		case USER_QUERY://用户修改
			process_user_query_request(acceptfd,msg);
			break;
		case ADMIN_MODIFY://管理员修改
			process_admin_modify_request(acceptfd,msg);
			break;

		case ADMIN_ADDUSER://管理员添加用户
			process_admin_adduser_request(acceptfd,msg);
			break;

		case ADMIN_DELUSER://管理员删除用户
			process_admin_deluser_request(acceptfd,msg);
			break;
		case ADMIN_QUERY://管理员查询
			process_admin_query_request(acceptfd,msg);
			break;
		case ADMIN_HISTORY://管理员查看历史记录
			process_admin_history_request(acceptfd,msg);
			break;
		case QUIT://退出
			process_client_quit_request(acceptfd,msg);
			break;
		default:
			break;
	}	
	return 0;
}

int main(int argc, const char *argv[])
{
	//socket->填充->绑定->监听->等待连接->数据交互->关闭 
	int sockfd;
	int acceptfd;
	ssize_t recvbytes;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	socklen_t addrlen = sizeof(serveraddr);
	socklen_t cli_len = sizeof(clientaddr);

	MSG msg;
	//thread_data_t tid_data;
	char *errmsg;

	if(sqlite3_open(STAFF_DATABASE,&db) != SQLITE_OK){
		printf("%s.\n",sqlite3_errmsg(db));
	}else{
		printf("the database open success.\n");
	}

	if(sqlite3_exec(db,"create table usrinfo(staffno integer primary key ,usertype integer,name text,passwd text,age integer,phone text,addr text,work text,date text,level integer,salary REAL);",NULL,NULL,&errmsg)!= SQLITE_OK){
		printf("%s.\n",errmsg);
	}else{
		printf("create usrinfo table success.\n");
	}

	if(sqlite3_exec(db,"create table historyinfo(time text,name text,words text);",NULL,NULL,&errmsg)!= SQLITE_OK){
		printf("%s.\n",errmsg);
	}else{ //华清远见创客学院         嵌入式物联网方向讲师
		printf("create historyinfo table success.\n");
	}

	//创建网络通信的套接字
	sockfd = socket(AF_INET,SOCK_STREAM, 0);
	if(sockfd == -1){
		perror("socket failed.\n");
		exit(-1);
	}
	printf("sockfd :%d.\n",sockfd); 

	
	/*优化4： 允许绑定地址快速重用 */
	int b_reuse = 1;
	setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &b_reuse, sizeof (int));
	
	//填充网络结构体
	memset(&serveraddr,0,sizeof(serveraddr));
	memset(&clientaddr,0,sizeof(clientaddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port   = htons(atoi(argv[2]));
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
//	serveraddr.sin_port   = htons(5001);
//	serveraddr.sin_addr.s_addr = inet_addr("192.168.1.200");


	//绑定网络套接字和网络结构体
	if(bind(sockfd, (const struct sockaddr *)&serveraddr,addrlen) == -1){
		printf("bind failed.\n");
		exit(-1);
	}

	//监听套接字，将主动套接字转化为被动套接字
	if(listen(sockfd,10) == -1){
		printf("listen failed.\n");
		exit(-1);
	}

	//定义一张表
	fd_set readfds,tempfds;
	//清空表
	FD_ZERO(&readfds);
	FD_ZERO(&tempfds);
	//添加要监听的事件
	FD_SET(sockfd,&readfds);
	int nfds = sockfd;
	int retval;
	int i = 0;

#if 0 //添加线程控制部分
	pthread_t thread[N];
	int tid = 0;
#endif

	while(1){
		tempfds = readfds;
		//记得重新添加
		retval =select(nfds + 1, &tempfds, NULL,NULL,NULL);
		//判断是否是集合里关注的事件
		for(i = 0;i < nfds + 1; i ++){
			if(FD_ISSET(i,&tempfds)){
				if(i == sockfd){
					//数据交互 
					acceptfd = accept(sockfd,(struct sockaddr *)&clientaddr,&cli_len);
					if(acceptfd == -1){
						printf("acceptfd failed.\n");
						exit(-1);
					}
					printf("ip : %s.\n",inet_ntoa(clientaddr.sin_addr));
					FD_SET(acceptfd,&readfds);
					nfds = nfds > acceptfd ? nfds : acceptfd;
				}else{
					recvbytes = recv(i,&msg,sizeof(msg),0);
					printf("msg.type :%#x.\n",msg.msgtype);
					if(recvbytes == -1){
						printf("recv failed.\n");
						continue;
					}else if(recvbytes == 0){
						printf("peer shutdown.\n");
						close(i);
						FD_CLR(i, &readfds);  //删除集合中的i
					}else{
						process_client_request(i,&msg);
					}
				}
			}
		}
	}
	close(sockfd);

	return 0;
}












