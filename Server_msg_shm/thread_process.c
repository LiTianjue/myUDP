/**线程处理函数的实现**/
/* Auther : Andy 
 * Date   : 2014-08-09
 ***********************/
int select_timer(int sec,int usec)
{
	int rc = 0;
	struct timeval tv; 
	tv.tv_sec = sec;
	tv.tv_usec = usec;

	rc = select(0,NULL,NULL,NULL,&tv);
	if(rc == 0)
		return 1;
	else
		return 0;
}


void *close_file(void *arg)
{
	;
}

void *File_Process(void *arg)
{
	pthread_detach(pthread_self());
	int mqid = (int)arg;
	//key_t key;

	//mqid = mine->msqid;
	//printf("mqid = %d\n",mqid);
	//key = mine->key;

	int fd;
	FILE *fp;
	struct msgbuf *buff = malloc(MAXMSG);
	int n , ret;

	mode_t fdmode = (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	int wlen;

	long long pack_total;
	long long pack_saved; 
	long long pack_move; 

	int wait_count = WAIT_MSG;

	char *msg = NULL;


	char log[512];

	char save_path[512] ={0};

	/**UDP 数据包结构**/
	char protocol;

	long long file_size;
	int file_name_length;
	char *file_name;
	int pack_length;
	char *pack;

	/**--设计select定时器--**/
	int recvflag = 0;


	/**--时间统计--**/
	struct timeval startTime,endTime,time1,time2,time3,time4,time5,time6;
	float Timeuse,Timeuse1,Timeuse2,Timeusec;
	float Time0,Time1,Time2,Time3,Time4,Time5;
//	select_timer(0,1000);

again:	
	pack_saved = 0;
	pack_move = 1;
	pack_total = 10;
	//wait_count = WAIT_MSG;
	//recvflag = IPC_NOWAIT;


	while(1)
	{	
//		wait_count = WAIT_MSG;
//		printf("pack_total = %lld\n",pack_total);
//		printf("pack_saved = %lld\n",pack_saved);
//		printf("Andy --> normal:pack_move = %lld\n",pack_move);
//		fflush(NULL);
//		if(pack_saved == 0)
//			recvflag = 0;
//		else
//			recvflag = IPC_NOWAIT;
		//睡眠１００s等待消息队列满
		sleep(100);
		if((n = msgrcv(mqid,buff,MAXMSG,pack_move,IPC_NOWAIT)) < 0)
		//if((n = msgrcv(mqid,buff,MAXMSG,pack_move,0)) < 0)
		{
			n = msgrcv(mqid,buff,MAXMSG,0,0);
			pack_move = buff->mtype;
		}

		gettimeofday(&startTime,NULL);

		if(n >= 0)
		{
			pack_move ++;
		}

		msg = buff->mtext;

		if(pack_saved == 0)
		{
			memcpy(&file_size,msg+10,8);
			pack_total = file_size/1024;
			if(file_size%1024 != 0)
				pack_total+=1;

			memcpy(&file_name_length,msg+18,4);
			file_name = (char *)malloc(file_name_length+1);
			memcpy(file_name,msg+22,file_name_length);
			*(file_name+file_name_length) = '\0';
			sprintf(save_path,RUN_PATH);
			strcat(save_path,file_name);

			memcpy(&pack_length,msg+22+file_name_length,4);
			pack = msg +22 +file_name_length +4;

			//ret = CreateDir(file_name);
			ret = CreateDir(save_path);
			if(ret < 0)
			{
				;
			}
			/*
			fd = open(file_name,O_RDWR|O_CREAT|O_APPEND,fdmode);
			if(fd == -1)
			{
				sprintf(log,"Open %s error:%s",file_name,strerror(errno));
				WriteSysLog(LOG_PATH,log);
				goto err_end;
			}
			*/
			//fp = fopen(file_name,"w+");
			fp = fopen(save_path,"w+");
			if(fp == NULL)
			{
				sprintf(log,"Open %s error:%s",file_name,strerror(errno));
				WriteSysLog(LOG_PATH,log);
 				goto err_end;
			}
		}

//		Time0 = 1000000*(time1.tv_sec - startTime.tv_sec) + (time1.tv_usec - startTime.tv_usec);
//		printf("Createdir && open file Timeus = %f\n",Time0);
		//fflush(NULL);


		/*
		wlen = write(fd,pack,pack_length);		
		if(wlen != pack_length)
		{
			sprintf(log,"Write %s error:%s",file_name,strerror(errno));
			WriteSysLog(LOG_PATH,log);
			goto err_end;
		}
		*/

		gettimeofday(&time1,NULL);
		fwrite(pack,sizeof(char),pack_length,fp);
		/*
		if((pack_move%10) == 0)
			fflush(fp);
		*/
		pack_saved++;				/**写文件成功，记录**/

		gettimeofday(&time2,NULL);
//		Time1 = 1000000*(time2.tv_sec - time1.tv_sec) + (time2.tv_usec - time1.tv_usec);
//		printf("Write file Timeus = %f\n",Time1);
		//fflush(NULL);

		//检查文件大小
		if(pack_total == pack_saved )
		{
			gettimeofday(&time5,NULL);
			sync();
			//fflush(NULL);
			//remove(file_name);
			fclose(fp);
			remove(save_path);
			//printf("--andy===>>file complety\n");
			//close(fd);
			free(file_name);

			gettimeofday(&time6,NULL);
			Time4 = 1000000*(time6.tv_sec - time5.tv_sec) + (time6.tv_usec - time5.tv_usec);
		/*	if(Time4 > 2000)
			{
				sprintf(log,"---------==Check time too long %f\n",Time4); 
				WriteSysLog(LOG_PATH,log);
			}
		*/
			goto again;
		}
		else if(pack_total == (pack_move -1))
		{
			sprintf(log,"====lost package====File %s is incomplete",file_name);
			WriteSysLog(LOG_PATH,log);
			//close(fd);
			fclose(fp);
			remove(save_path);
			free(file_name);
			goto again;
		}

		gettimeofday(&endTime,NULL);
		Timeuse = 1000000*(endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec);
		Time1 = 1000000*(time1.tv_sec - startTime.tv_sec) + (time1.tv_usec - startTime.tv_usec);
		Time2 = 1000000*(time2.tv_sec - startTime.tv_sec) + (time2.tv_usec - startTime.tv_usec);
		//printf("Check total Timeus = %f\n",Timeuse);
		//fflush(NULL);

		/*
		if(Timeuse > 700)
		{
			//sprintf(log,"Thread use too long time :open&create time:%f write time:%f  ,check_time:%f total time:%f",Time0,Time1,Time4,Timeuse);
			if(Timeuse > 10000)
			{
		 		sprintf(log,"#######---Too long Time-- : time1 :%f | time2:%f  -->total :%f\n",Time1,Time2,Timeuse);
				WriteSysLog(LOG_PATH,log);
				fflush(NULL);
			}
			else
			{
				sprintf(log,"Thread : time1 :%f | time2:%f  -->total :%f\n",Time1,Time2,Timeuse);
				WriteSysLog(LOG_PATH,log);
		 		fflush(NULL);
			}
		}
		printf("Thread : time1 :%f | time2:%f  -->total :%f\n",Time1,Time2,Timeuse);
		*/
		fflush(NULL);
	}

err_end:	
	free(buff);
	free(file_name);
	return NULL;
	
}
