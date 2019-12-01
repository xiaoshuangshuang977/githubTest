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

#include "common.h"

/**************************************
 *函数名：do_query
 *参   数：消息结构体
 *功   能：查询
 ****************************************/
void do_admin_query(int sockfd,MSG *msg)//管理员查询
{
	while(1)
	{
		printf("**********************************************\n");
		printf("**1.按人名查询       2.查询全部       3.退出**\n");
		printf("**********************************************\n");
		printf("请输入您的选择（数字）>>");
		int i;
		scanf("%d",&i);
		getchar();
		switch (i)
		{
			case 1:
				do_admin_query1(sockfd, msg);
				break;
			case 2:
				do_admin_query2(sockfd, msg);
				break;
			case 3: //退出
				admin_menu_break(sockfd,msg);
			default:
				printf("输入错误,请重新输入！\n");
		}
	}
}

int do_admin_query1(int sockfd,MSG *msg)//管理员按人名查询
{
	int i;
	printf("请输入用户名：");
	scanf("%s",msg->info.name);
	getchar();
	msg->flags = 1;
	//发送请求
	send(sockfd, msg, sizeof(MSG), 0);
	char buf[DATALEN] = {0};
	//memset(buf,0,DATALEN);
	
	//接受服务器响应
	recv(sockfd, msg, sizeof(MSG), 0);
	printf("msg->recvmsg :%s\n",msg->recvmsg);
	//判断人名是否存在
	if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{//存在
		printf("用户名存在\n");
		printf("工号   用户类型	  姓名	  密码	 年龄   电话	\t地址\t	职位\t   入职年月   等级	 工资\n");
		recv(sockfd,buf,sizeof(buf),0);
		printf("==============================================================================================\n");
		printf("%s",buf);
		memset(buf,0,DATALEN);			
	}
	else
	{//不存在人名
		printf("用户不存在，请重新输入\n");
	}
	return 0;
}

int do_admin_query2(int sockfd,MSG *msg)//管理员查询全部
{//查询全部
	int i,nrow;
	msg->flags = 2;
	char buf[DATALEN] = {0};
	//发送请求
	send(sockfd, msg, sizeof(MSG), 0);
	//接受服务器响应
	recv(sockfd, msg, sizeof(MSG), 0);
	printf("msg->recvmsg :%s\n",msg->recvmsg);
	nrow = msg->nrows;
	if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{
		printf("工号   用户类型	  姓名	  密码	 年龄   电话	\t地址\t	职位\t   入职年月   等级	 工资\n");
		for(i=0; i<nrow; i++)
		{
			recv(sockfd,buf,sizeof(buf),0);
			printf("==============================================================================================\n");
			printf("%s",buf);
			memset(buf,0,DATALEN);		
		}
	}else
	{
		printf("查询失败！\n");
	}
	return 0;
}
/**************************************
 *函数名：admin_modification
 *参   数：消息结构体
 *功   能：管理员修改
 ****************************************/
 
void do_admin_modification(int sockfd,MSG *msg)//管理员修改
{
	printf("-------管理员 修改-----%s-----------%d.\n",__func__,__LINE__);
	printf("请输入您要修改的工号：");
	scanf("%d",&msg->info.staffno);
	getchar();
	printf("请输入您要修改的用户名:");
	scanf("%s",msg->info.name);
	getchar();
	
	printf("************************************************\n");
	printf("*****1.姓名    2.年龄  3.家庭地址    4.电话*****\n");
	printf("*****5.职位    6.工资  7.入职年月    8.评级*****\n");
	printf("**********   9.密码      0 退出      ***********\n");
	printf("************************************************\n");
	printf("请输入您的选择（数字）>>");
	int i;
	scanf("%d",&i);
	switch (i)
	{
		case 1: //修改姓名
			do_admin_modif1(sockfd,msg);
			break;
		case 2://修改年龄
			do_admin_modif2(sockfd,msg);
			break;
		case 3://修改家庭住址
			do_admin_modif3(sockfd,msg);
			break;
		case 4://修改电话
			do_admin_modif4(sockfd,msg);
			break;
		case 5://修改职位
			do_admin_modif5(sockfd,msg);
			break;
		case 6://修改工资
			do_admin_modif6(sockfd,msg);
			break;
		case 7://修改入职年月
			do_admin_modif7(sockfd,msg);
			break;
		case 8://修改评级
			do_admin_modif8(sockfd,msg);
			break;
		case 9://修改密码
			do_admin_modif9(sockfd,msg);
			break;
		case 0: 
			admin_menu_break(sockfd,msg);
			break;
		default:
			printf("输入错误,请重新输入！\n");
	}
}

int do_admin_modif1(int sockfd,MSG *msg)//修改用户名
{
	//发送请求
	msg->flags = 1;  
	send(sockfd, msg, sizeof(MSG), 0);
	
	printf("请输入新的用户名：");
	scanf("%s",msg->info.name);
	getchar();
	
	send(sockfd, msg, sizeof(MSG), 0);
	//接受服务器响应
	recv(sockfd, msg, sizeof(MSG), 0);
	
	if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{
		printf("%s 修改用户名成功！\n",msg->recvmsg);
		admin_menu_break(sockfd,msg);
		
	}else
	{
		printf("修改失败！\n");
		admin_menu_break(sockfd,msg);
	}
	return 0;
}

int do_admin_modif2(int sockfd,MSG *msg)//修改年龄
{
	msg->flags = 2;
	printf("请输入新的年龄：");
	scanf("%d",&msg->info.age);
	getchar();
	//发送请求
	send(sockfd, msg, sizeof(MSG), 0);
	
	//接受服务器响应
	recv(sockfd, msg, sizeof(MSG), 0);
	
	if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{
		printf("%s 修改年龄成功！\n",msg->recvmsg);
		admin_menu_break(sockfd,msg);
		
	}else
	{
		printf("修改失败！\n");
		admin_menu_break(sockfd,msg);
	}
	return 0;
	
}

int do_admin_modif3(int sockfd,MSG *msg)//修改家庭住址
{
	msg->flags = 3;
	memset(msg->info.addr, 0, DATALEN);
	printf("请输入新的家庭住址:");
	scanf("%s",msg->info.addr);
	getchar();
	
	//发送请求
	send(sockfd, msg, sizeof(MSG), 0);
	
	//接收服务器
	recv(sockfd,msg,sizeof(MSG),0);
	
	if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{
		printf("%s 修改家庭住址成功！\n",msg->recvmsg);
		admin_menu_break(sockfd,msg);
		
	}else
	{
		printf("修改失败！\n");
		admin_menu_break(sockfd,msg);
	}
	return 0;
	
}

int do_admin_modif4(int sockfd,MSG *msg)//修改电话
{
	msg->flags = 4;
	memset(msg->info.phone, 0, NAMELEN);
	printf("请输入新的电话：");
	scanf("%s",msg->info.phone);
	getchar();
	
	//发送请求
	send(sockfd, msg, sizeof(MSG), 0);
	//接受服务器响应
	recv(sockfd,msg,sizeof(MSG),0);
		if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{
		printf("%s 修改电话成功！\n",msg->recvmsg);
		admin_menu_break(sockfd,msg);
		
	}else
	{
		printf("修改失败！\n");
		admin_menu_break(sockfd,msg);
	}
	return 0;
	
}

int do_admin_modif5(int sockfd,MSG *msg)//修改职位
{
	msg->flags = 5;
	memset(msg->info.work, 0, DATALEN);
	printf("请输入新的职位:");
	scanf("%s",msg->info.work);
	getchar();
	
	//发送请求
	send(sockfd, msg, sizeof(MSG), 0);
	
	//接收服务器
	recv(sockfd,msg,sizeof(MSG),0);
	
	if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{
		printf("%s 修改职位成功！\n",msg->recvmsg);
		admin_menu_break(sockfd,msg);
		
	}else
	{
		printf("修改失败！\n");
		admin_menu_break(sockfd,msg);
	}
	return 0;
	
	
}

int do_admin_modif6(int sockfd,MSG *msg)//修改工资
{
	msg->flags = 6;
	printf("请输入新的工资:");
	scanf("%lf",&msg->info.salary);
	getchar();
	
	//发送请求
	send(sockfd, msg, sizeof(MSG), 0);
	
	//接收服务器
	recv(sockfd,msg,sizeof(MSG),0);
	
	if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{
		printf("%s 修改工资成功！\n",msg->recvmsg);
		admin_menu_break(sockfd,msg);
		
	}else
	{
		printf("修改失败！\n");
		admin_menu_break(sockfd,msg);
	}
	return 0;
	
}

int do_admin_modif7(int sockfd,MSG *msg)//修改入职年月
{
	msg->flags = 7;
	memset(msg->info.work, 0, DATALEN);
	printf("请输入新的入职年月:");
	scanf("%s",msg->info.date);
	getchar();
	
	//发送请求
	send(sockfd, msg, sizeof(MSG), 0);
	
	//接收服务器
	recv(sockfd,msg,sizeof(MSG),0);
	
	if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{
		printf("%s 修改入职年月成功！\n",msg->recvmsg);
		admin_menu_break(sockfd,msg);
		
	}else
	{
		printf("修改失败！\n");
		admin_menu_break(sockfd,msg);
	}
	return 0;
	
}

int do_admin_modif8(int sockfd,MSG *msg)//修改评级
{
	msg->flags = 8;
	printf("请输入新的评级:");
	scanf("%d",&msg->info.level);
	getchar();
	
	//发送请求
	send(sockfd, msg, sizeof(MSG), 0);
	
	//接收服务器
	recv(sockfd,msg,sizeof(MSG),0);
	
	if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{
		printf("%s 修改评级成功！\n",msg->recvmsg);
		admin_menu_break(sockfd,msg);
		
	}else
	{
		printf("修改失败！\n");
		admin_menu_break(sockfd,msg);
	}
	return 0;
	
	
}

int do_admin_modif9(int sockfd,MSG *msg)//修改密码
{
	msg->flags = 9;
	printf("请输入新的密码:");
	scanf("%s",msg->info.passwd);
	getchar();
	
	//发送请求
	send(sockfd, msg, sizeof(MSG), 0);
	
	//接收服务器
	recv(sockfd,msg,sizeof(MSG),0);
	
	if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{
		printf("%s 修改密码成功\n",msg->recvmsg);
		admin_menu_break(sockfd,msg);
	}else
	{
		printf("修改密码失败！\n");
		admin_menu_break(sockfd,msg);
	}
	return 0;
}

/**************************************
 *函数名：admin_adduser
 *参   数：消息结构体
 *功   能：管理员创建用户
 ****************************************/
void do_admin_adduser(int sockfd,MSG *msg)//管理员添加用户
{
	char m,n,o;
	printf("*************欢迎新员工*************\n");
	printf("请输入工号：");
	scanf("%d",&msg->info.staffno);
	getchar();
	printf("工号信息一旦录入无法更改，请确认您所输入的是否正确！(Y/N)");
	scanf("%c",&m);
	getchar();
	if(m == 'Y')
	{
		printf("请输入用户名:");
		scanf("%s",msg->info.name);
		printf("请输入密码:");
		scanf("%s",msg->info.passwd);
		printf("请输入年龄:");
		scanf("%d",&msg->info.age);
		printf("请输入电话:");
		scanf("%s",msg->info.phone);
		printf("请输入家庭住址:");
		scanf("%s",msg->info.addr);
		printf("请输入职位:");
		scanf("%s",msg->info.work);
		printf("请输入入职日期:");
		scanf("%s",msg->info.date);
		printf("请输入评级:");
		scanf("%d",&msg->info.level);
		printf("请输入工资:");
		scanf("%lf",&msg->info.salary);
		printf("是否为管理员:(Y/N):");
		scanf("%s",&n);
		getchar();
		if(n == 'Y')//是管理员
		{
			msg->usertype = ADMIN;
			send(sockfd,msg,sizeof(MSG),0);
			recv(sockfd,msg,sizeof(MSG),0);
			if(strncmp(msg->recvmsg,"OK",2) == 0)
			{//添加成功
				printf("添加成功!\n");
				memset(msg,0,sizeof(MSG));
				admin_menu_break(sockfd,msg);
			}else
			{//添加失败
				printf("添加失败 此工号已占用!\n");
				memset(msg,0,sizeof(MSG));
				admin_menu_break(sockfd,msg);
			}
			
		}else if(n == 'N')//不是管理员
		{
			msg->usertype = USER;
			send(sockfd,msg,sizeof(MSG),0);
			recv(sockfd,msg,sizeof(MSG),0);	
			if(strncmp(msg->recvmsg,"OK",2) == 0)
			{//添加成功
				printf("添加成功!\n");
				memset(msg,0,sizeof(MSG));	
				admin_menu_break(sockfd,msg);
			}else
			{//添加失败
				printf("添加失败 此工号已占用!\n");
				memset(msg,0,sizeof(MSG));	
				admin_menu_break(sockfd,msg);
			}			
		}else
		{
			printf("输入错误\n");
			memset(msg,0,sizeof(MSG));	
			admin_menu_break(sockfd,msg);
		}
	}else if(m == 'N')
	{//转到重新输入工号
		memset(msg,0,sizeof(MSG));
		admin_menu_break(sockfd,msg);
	}else
	{
		printf("输入错误!\n");
		memset(msg,0,sizeof(MSG));
		admin_menu_break(sockfd,msg);
	}
}

/**************************************
 *函数名：admin_deluser
 *参   数：消息结构体
 *功   能：管理员删除用户
 ****************************************/
void do_admin_deluser(int sockfd,MSG *msg)//管理员删除用户
{
	printf("-------管理员删除用户-----%s-----------%d.\n",__func__,__LINE__);
	printf("请输入要删除的工号:");
	scanf("%d",&msg->info.staffno);
	printf("请输入要删除的用户名:");
	scanf("%s",msg->info.name);
	
	//发送请求
	send(sockfd,msg,sizeof(MSG),0);
	//接受服务器响应
	recv(sockfd,msg,sizeof(MSG),0);
	if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{
		printf("msg->recvmsg=%s",msg->recvmsg);
		printf("删除成功!\n");	
		admin_menu_break(sockfd,msg);
	}else
	{
		printf("删除失败!\n");	
		admin_menu_break(sockfd,msg);
	}
}

/**************************************
 *函数名：do_history
 *参   数：消息结构体
 *功   能：查看历史记录
 ****************************************/
void do_admin_history (int sockfd,MSG *msg)//历史记录
{
	printf("-------历史记录-----%s-----------%d.\n",__func__,__LINE__);
	char buf[DATALEN] = {0};
	int i,nrow;
	//发送请求
	send(sockfd, msg, sizeof(MSG), 0);
	//接受服务器响应
	recv(sockfd, msg, sizeof(MSG), 0);
	printf("msg->recvmsg :%s\n",msg->recvmsg);
	nrow = msg->nrows;
	if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{
		for(i=0;i<nrow;i++)
		{
			recv(sockfd,buf,sizeof(buf),0);
			//printf("==============================================================================================\n");
			printf("%s",buf);
			memset(buf,0,DATALEN);	
		}
		printf("查看历史记录成功\n");
		memset(buf,0,DATALEN);	
		admin_menu_break(sockfd,msg);
		
	}else
	{
		printf("查看历史记录失败\n");
		memset(buf,0,DATALEN);	
		admin_menu_break(sockfd,msg);
	}

}

/**************************************
 *函数名：admin_menu
 *参   数：套接字、消息结构体
 *功   能：管理员菜单
 ****************************************/
void admin_menu(int sockfd,MSG *msg)//管理员菜单
{
	int n;
	while(1)
	{
		printf("*************************************************************\n");
		printf("* 1：查询  2：修改 3：添加用户  4：删除用户  5：查询历史记录*\n");
		printf("* 6：退出													*\n");
		printf("*************************************************************\n");
		printf("请输入您的选择（数字）>>");
		scanf("%d",&n);
		getchar();

		switch(n)
		{
			case 1:
				msg->msgtype = ADMIN_QUERY;
				do_admin_query(sockfd,msg); //管理员 查询
				break;
			case 2:
				msg->msgtype = ADMIN_MODIFY;
				do_admin_modification(sockfd,msg); //管理员 修改
				break;
			case 3:
				msg->msgtype = ADMIN_ADDUSER;
				do_admin_adduser(sockfd,msg);//管理员添加
				break;
			case 4:
				msg->msgtype = ADMIN_DELUSER;
				do_admin_deluser(sockfd,msg);//管理员删除
				break;
			case 5:
				msg->msgtype = ADMIN_HISTORY;
				do_admin_history(sockfd,msg);//历史记录
				break;
			case 6:
				admin_or_user_login_break(sockfd);//跳转到登录界面
			default:
				printf("请重新输入！\n");
		}
	}
}

int admin_menu_break(int sockfd,MSG *msg)//跳转到管理员菜单
{
	int n;
	printf("*************************************************************\n");
	printf("* 1：查询  2：修改 3：添加用户  4：删除用户  5：查询历史记录*\n");
	printf("* 6：退出													*\n");
	printf("*************************************************************\n");
	printf("请输入您的选择（数字）>>");
	scanf("%d",&n);
	getchar();

	switch(n)
	{
		case 1:
			msg->msgtype = ADMIN_QUERY;
			do_admin_query(sockfd,msg); //管理员 查询
			break;
		case 2:
			msg->msgtype = ADMIN_MODIFY;
			do_admin_modification(sockfd,msg); //管理员 修改
			break;
		case 3:
			msg->msgtype = ADMIN_ADDUSER;
			do_admin_adduser(sockfd,msg);//管理员添加
			break;
		case 4:
			msg->msgtype = ADMIN_DELUSER;
			do_admin_deluser(sockfd,msg);//管理员删除
			break;
		case 5:
			msg->msgtype = ADMIN_HISTORY;
			do_admin_history(sockfd,msg);//历史记录
			break;
		case 6:
			admin_or_user_login_break(sockfd);
			break;
		default:
			printf("输入有误，请重新输入！\n");
	}	
}
/**************************************
 *函数名：do_query
 *参   数：消息结构体
 *功   能：查询
 ****************************************/
void do_user_query(int sockfd,MSG *msg)//用户查询
{
	printf("--------普通用户查询----%s-----------%d.\n",__func__,__LINE__);
	char sql[DATALEN] = {0};
	char buf[DATALEN] = {0};
	int i ,j;
	//发送请求
	send(sockfd, msg, sizeof(MSG), 0);
	//接受服务器响应
	recv(sockfd, msg, sizeof(MSG), 0);
	printf("msg->recvmsg :%s\n",msg->recvmsg);
	//判断人名是否存在
	if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{//存在
		printf("工号   用户类型	  姓名	  密码	 年龄   电话	\t地址\t	职位\t   入职年月   等级	 工资\n");
		printf("==============================================================================================\n");
		recv(sockfd,buf,sizeof(buf),0);
		printf("%s",buf);
		memset(buf,0,DATALEN);
	}else
	{//不存在人名
	recv(sockfd, msg->username, sizeof(MSG), 0);
		printf("人名不存在，请重新输入\n");

	}
}

/**************************************
 *函数名：do_modification
 *参   数：消息结构体
 *功   能：修改
 ****************************************/
void do_user_modification(int sockfd,MSG *msg)//用户修改
{
	printf("-------用户修改-----%s-----------%d.\n",__func__,__LINE__);

	//发送请求
	send(sockfd, msg, sizeof(MSG), 0);
	
	while(1)
	{
		printf("*************************************************\n");
		printf("1：家庭住址   2：电话  3：密码  4：退出\n");
		printf("*************************************************\n");
		printf("请输入您的选择（数字）>>");
		int i;
		scanf("%d",&i);
		getchar();
		switch (i)
		{
			case 1:
				do_user_modification1(sockfd,msg);
				break;
			case 2:
				do_user_modification2(sockfd,msg);
				break;
			case 3:
				do_user_modification3(sockfd,msg);
				break;
			case 4:
				user_menu_break(sockfd,msg);
			default :
				printf("输入错误， 请重新输入！");
				
		}
	}
}
int user_menu_break(int sockfd,MSG *msg)//跳转到用户菜单
{
	int n;
	printf("*************************************************************\n");
	printf("*******    1：查询    	2：修改		  3：退出    *******\n");
	printf("*************************************************************\n");
	printf("请输入您的选择（数字）>>");
	scanf("%d",&n);
	getchar();

	switch(n)
	{
		case 1:
			msg->msgtype = USER_QUERY;
			do_user_query(sockfd,msg);
			break;
		case 2:
			msg->msgtype = USER_MODIFY;
			do_user_modification(sockfd,msg);
			break;
		case 3:
			admin_or_user_login_break(sockfd);
		default:
			printf("您输入有误，请输入数字\n");
			break;
	}	
	return 0;
}
int do_user_modification1(int sockfd,MSG *msg)//用户修改家庭地址
{
	msg->flags = 1;
	memset(msg->info.addr, 0, NAMELEN);
	printf("请输入新的家庭住址：");
	scanf("%s",msg->info.addr);
	getchar();

	//发送请求
	send(sockfd, msg, sizeof(MSG), 0);
	
	//接受服务器响应
	recv(sockfd, msg, sizeof(MSG), 0);
	if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{
		printf("%s 修改家庭住址成功！\n",msg->recvmsg);
	}
	return 0;
}

int do_user_modification2(int sockfd,MSG *msg)//用户修改手机号
{
	msg->flags = 2;
	memset(msg->info.phone, 0, NAMELEN);
	printf("请输入新的手机号：");
	scanf("%s",msg->info.phone);
	getchar();
	
	//发送请求
	send(sockfd, msg, sizeof(MSG), 0);
	
	//接受服务器响应
	recv(sockfd, msg, sizeof(MSG), 0);

	if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{
		printf("%s 修改手机号成功！\n",msg->recvmsg);
	}
	return 0;
}

int do_user_modification3(int sockfd,MSG *msg)//用户修改密码
{
	msg->flags = 3;
	memset(msg->info.passwd, 0, NAMELEN);
	printf("请输入新的密码：");
	scanf("%s",msg->info.passwd);
	getchar();

	//发送请求
	send(sockfd, msg, sizeof(MSG), 0);
	
	//接受服务器响应
	recv(sockfd, msg, sizeof(MSG), 0);
	if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{
		printf("%s 修改密码成功！\n",msg->recvmsg);
	}
	return 0;
}

/**************************************
 *函数名：user_menu
 *参   数：消息结构体
 *功   能：用户菜单
 ****************************************/
void user_menu(int sockfd,MSG *msg)//用户菜单
{
	printf("------------%s-----------%d.\n",__func__,__LINE__);
	int n;
	while(1)
	{
		printf("*************************************************************\n");
		printf("*******    1：查询    	2：修改		  3：退出    *******\n");
		printf("*************************************************************\n");
		printf("请输入您的选择（数字）>>");
		scanf("%d",&n);
		getchar();

		switch(n)
		{
			case 1:
				msg->msgtype = USER_QUERY;
				do_user_query(sockfd,msg);
				break;
			case 2:
				msg->msgtype = USER_MODIFY;
				do_user_modification(sockfd,msg);
				break;
			case 3:
				admin_or_user_login_break(sockfd);//跳转到用户登录界面
			default:
				printf("您输入有误，请输入数字\n");
				break;
		}
	}
}

int admin_or_user_login(int sockfd,MSG *msg)//普通用户或管理员登录
{
	printf("------------%s-----------%d.\n",__func__,__LINE__);
	//输入用户名和密码
	memset(msg->username, 0, NAMELEN);
	printf("请输入用户名：");
	scanf("%s",msg->username);
	getchar();

	memset(msg->passwd, 0, DATALEN);
	printf("请输入密码（6位）");
	scanf("%s",msg->passwd);
	getchar();

	//发送登陆请求
	send(sockfd, msg, sizeof(MSG), 0);
	//接受服务器响应
	recv(sockfd, msg, sizeof(MSG), 0);
	printf("msg->recvmsg :%s\n",msg->recvmsg);

	//判断是否登陆成功
	if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{
		if(msg->usertype == ADMIN)
		{
			printf("亲爱的管理员，欢迎您登陆员工管理系统！\n");
			admin_menu(sockfd,msg);
		}
		else if(msg->usertype == USER)
		{
			printf("亲爱的用户，欢迎您登陆员工管理系统！\n");
			user_menu(sockfd,msg);
		}
	}
	else
	{
		printf("登陆失败！%s\n", msg->recvmsg);
		return -1;
	}

	return 0;
}
int admin_or_user_login_break(int sockfd)//跳转到登录界面
{
int n;
	MSG msg;

	while(1){
		printf("*************************************************************\n");
		printf("********  1：管理员模式    2：普通用户模式    3：退出********\n");
		printf("*************************************************************\n");
		printf("请输入您的选择（数字）>>");
		scanf("%d",&n);
		getchar();

		switch(n)
		{
			case 1:
				msg.msgtype  = ADMIN_LOGIN;
				msg.usertype = ADMIN;
				break;
			case 2:
				msg.msgtype =  USER_LOGIN;
				msg.usertype = USER;
				break;
			case 3:
				msg.msgtype = QUIT;
				if(send(sockfd, &msg, sizeof(MSG), 0)<0)
				{
					perror("do_login send");
					return -1;
				}
				close(sockfd);
				exit(0);
			default:
				printf("您的输入有误，请重新输入\n"); 
		}

		admin_or_user_login(sockfd,&msg);
	}
	return 0;
}
/************************************************
 *函数名：do_login
 *参   数：套接字、消息结构体
 *返回值：是否登陆成功
 *功   能：登陆
 *************************************************/
int do_login(int sockfd)//登录菜单
{	
	int n;
	MSG msg;

	while(1){
		printf("*************************************************************\n");
		printf("********  1：管理员模式    2：普通用户模式    3：退出********\n");
		printf("*************************************************************\n");
		printf("请输入您的选择（数字）>>");
		scanf("%d",&n);
		getchar();

		switch(n)
		{
			case 1:
				msg.msgtype  = ADMIN_LOGIN;
				msg.usertype = ADMIN;
				break;
			case 2:
				msg.msgtype =  USER_LOGIN;
				msg.usertype = USER;
				break;
			case 3:
				msg.msgtype = QUIT;
				if(send(sockfd, &msg, sizeof(MSG), 0)<0)
				{
					perror("do_login send");
					return -1;
				}
				close(sockfd);
				exit(0);
			default:
				printf("您的输入有误，请重新输入\n"); 
		}

		admin_or_user_login(sockfd,&msg);
	}
	return 0;
}

int main(int argc, const char *argv[])
{
	//socket->填充->绑定->监听->等待连接->数据交互->关闭 
	int sockfd;
	int acceptfd;
	ssize_t recvbytes,sendbytes;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	socklen_t addrlen = sizeof(serveraddr);
	socklen_t cli_len = sizeof(clientaddr);

	//创建网络通信的套接字
	sockfd = socket(AF_INET,SOCK_STREAM, 0);
	if(sockfd == -1){
		perror("socket failed.\n");
		exit(-1);
	}
	printf("sockfd :%d.\n",sockfd); 

	//填充网络结构体
	memset(&serveraddr,0,sizeof(serveraddr));
	memset(&clientaddr,0,sizeof(clientaddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port   = htons(atoi(argv[2]));
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
//	serveraddr.sin_port   = htons(5001);
//	serveraddr.sin_addr.s_addr = inet_addr("192.168.1.200");

	if(connect(sockfd,(const struct sockaddr *)&serveraddr,addrlen) == -1){
		perror("connect failed.\n");
		exit(-1);
	}

	do_login(sockfd);

	close(sockfd);

	return 0;
}



