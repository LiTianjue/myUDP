/***********************
 *  Title  : UDP server
 *  Auther : andy
 *  Create : 2014-08-01
 *************************/

/*-----Change_Log-----------*/
/*		Date			Auther			Desc
 * 2014/08/01			Andy		这是一个测试版本	
 *---------------------------*/

#include <stdio.h>  
#include <stdlib.h>
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

/*消息队列相关*/
#include <sys/msg.h>
#include <sys/ipc.h>

#include "protocol.h"
//static recv_buff RECV_BUFF[BUFF_COUNT];
//static int SAVE = 0;

#include "MY_UDP.h"
#include "Common.h"
#include "config.h"

//#include "buff.c"
//#include "Per_Process.c"

#include "thread_process.c"


static long long save;
/********************************************/

void handler(int sig)
{
	printf("Andy -->signal\n");
	return;
}
 
static int my_count =0;
int main()  
{
	signal(SIGPIPE,handler);
	//signal(SIGINT,handler);
	signal(SIGHUP,handler);

	/**--创建UDP套接字用于接收数据--**/
	int sockfd;
	struct sockaddr_in servaddr,client;
	int sin_size = sizeof(struct sockaddr_in);

	Init_sockaddr_s(&servaddr,AF_INET,SERVER_IP,SERVER_PORT);
	sockfd = Socket(AF_INET,SOCK_DGRAM,0);

	int opt = SO_REUSEADDR;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));


	/**--设置扩大系统接收缓冲区的大小提高UDP接收的性能--**/
	int n = 512*1024;
	if(0 != setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&n,sizeof(n)))
		perror("setsockopt");

	if(bind(sockfd,(struct sockaddr *)&servaddr,sizeof(struct sockaddr)) == -1)  
	{
		perror("bind error");  
		exit(-1);
	}

	int num;
	char msg[MAXDATASIZE] = {0};

	printf("IP is %s,PORT is %d\n",SERVER_IP,SERVER_PORT);

	/************************************************/
	/*----消息队列和线程相关----*
	 */
	char main_log[512] ={0};
	
	pthread_t tid;
	struct msgbuf *msgbuf;
	MyPthread mytest;

	int c ,oflag,mqid;
	oflag = IPC_CREAT;
	oflag |= IPC_EXCL;
	key_t my_key;
	my_key = ftok(TMP_FILE,0);
	if(my_key == -1)
	{
		sprintf(main_log,"ftok error :%s",strerror(errno));
		WriteSysLog(LOG_PATH,main_log);
		exit(-1);
	}

	mqid = msgget(my_key, oflag);
	if(mqid == -1)
	{
		sprintf(main_log,"ftok error :%s",strerror(errno));
		WriteSysLog(LOG_PATH,main_log);
		exit(-1);
	}
	mytest.msqid = mqid;
	mytest.key = my_key;

	int only = 1;	//用作标志在循环里只创建一次线程
	

	/*测试代码时间*/
	struct timeval startTime,endTime;
	float Timeuse;

	if(only == 1)
	{
		int ret = 0;
		ret = pthread_create(&tid,NULL,&File_Process,(void *)mqid);
		if(ret < 0)
		{
			perror("create phtread fail");
			return -1;
		}
		only = 0;
	}
	//usleep(200);

	
	save = 0;
/*
	printf("daemon: start\n");
	if(daemon(0,0)< 0)
	{
		fprintf(stderr,"daemon faile:%s\n",strerror(errno));
		return -1;
	}
	printf("daemon: stop\n");
*/
	while(1)
	{
		//printf("Andy ===>> wait for recv\n");
		fflush(NULL);


		num = recvfrom(sockfd,msg,MAXDATASIZE,0,(struct sockaddr *)&client,&sin_size);
		if(num < 0)
		{
			//printf("recvfrom error.\n");
			sprintf(main_log,"Recvfrom error :%s",strerror(errno));
			WriteSysLog(LOG_PATH,main_log);
		}
		
		msgbuf = calloc(sizeof(long) + num,sizeof(char));
		if(msgbuf == NULL)
		{
			sprintf(main_log,"calloc error :%s",strerror(errno));
			WriteSysLog(LOG_PATH,main_log);
			exit(-1);
		}

		memcpy(msgbuf->mtext,msg,num);
		
		long long pack_nu;
		memcpy(&pack_nu,msg+2,8);
	//	printf("--------->Recv pack %lld\n",pack_nu);

		if(pack_nu == 1)
			save = 0;
		if((pack_nu != (save + 1)))
		{
			sprintf(main_log,"====>lost package,Recv pack error :save=%lld,pack_nu=%lld\n",save,pack_nu);
			WriteSysLog(LOG_PATH,main_log);
			save = pack_nu;
		}
		else 
		{
			save = pack_nu;
		}
		
		fflush(NULL);
		msgbuf->mtype = (long)pack_nu;


		gettimeofday(&startTime,NULL);

		//if((msgsnd(mqid,msgbuf,num,0) == -1))
		if((msgsnd(mqid,msgbuf,num,IPC_NOWAIT) == -1))
		{
			sprintf(main_log,"Send msg error :%s",strerror(errno));
			WriteSysLog(LOG_PATH,main_log);
			msgsnd(mqid,msgbuf,num,0);
			//free(msgbuf);
			//exit(-1);
		}

		gettimeofday(&endTime,NULL);
		Timeuse = 1000000*(endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec);
	/*	if(Timeuse > 700)
		{
			sprintf(main_log,"###########Sendmsg block =%f ####,pack_nu=%lld\n",Timeuse,pack_nu);
			WriteSysLog(LOG_PATH,main_log);
		}
	*/
		free(msgbuf);
		

	//	printf("Recv Timeus = %f\n",Timeuse);
		//puts("\n-----------------------------");
		//printf("Recv message %d from: %s \n",num,inet_ntoa(client.sin_addr));
		
		//Per_Process(msg,num);
		//Save_UDP(msg);
		//my_count++;
		//printf("Andy -->count = %d\n",my_count);
	}
}
