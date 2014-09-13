/************************************
 * Title :  使用UDP发送文件
 * Auther:  Andy
 * Create:  2014/08/02
 ************************************/
/***----Change Log----***************
 *		Date			Auther			Desc
 * 2014/08/02			Andy		使用了新的封装格式
 *
 */



/***----将文件封装成数据包发送----***/
int SendPack(char *file_path,int sockfd,const struct sockaddr *pservaddr,socklen_t servlen)
{

	int fd ,ret;
	int rlen;

	char *msg;
	int msg_len;

	char recv_buff[MAXLINE] = {0};

	/*----UDP 包结构----**/
	char version = '\017';
	char protocol = '\014';
	long long pack_nu = 0;
	long long file_size;
	int file_name_length = strlen(file_path);
	char *file_name;
	int pack_length;
	char *pack;

	file_size = GetFileSize(file_path);

	msg_len = 1 + 1 + 8 + 8 + 4 +file_name_length + 4 + MAXLINE;

	msg = malloc(msg_len);
	file_name = strdup(file_path);

	//初始化部分发送包的数据结构
	memcpy(msg,&version,1);
	// +3 is pack_nu
	memcpy(msg +10,&file_size,8);
	memcpy(msg + 18,&file_name_length,4);
	memcpy(msg + 22,file_name,file_name_length);

	/*
	printf("version = %d\n",*msg);
	printf("protocol = %d\n",*((char *)(msg + 2)));
	printf("file_size = %lld\n",*((long long *)(msg + 4)));
	printf("file_name_length = %d\n",*((char *)(msg + 12)));
	printf("file_name = %s\n",msg + 13);
	printf("********************************\n");
	*/

	if(! (fd = open(file_name,O_RDONLY)))
	{
		perror("open file ");
		return -1;
	}
	
	int len_a = 22 + file_name_length + 4;
	int send_len;
	int send_count = 0;
	while((rlen = read(fd,recv_buff,MAXLINE)) != -1)
	{
		if(rlen == 0)
			break;
		
		if((send_count == 0)&&((send_count+rlen) == file_size))
			protocol = '\013';
		else if(send_count == 0)
			protocol = '\014';
		else if(send_count+rlen == file_size)
			protocol = '\016';
		else if(send_count + rlen < file_size)
			protocol = '\015';
		
		send_count +=rlen;

		memcpy(msg +1,&protocol,1);
		
		pack_nu++;

		memcpy(msg +1,&protocol,1);

		pack_length = rlen;
		memcpy(msg +2,&pack_nu,8);
		memcpy(msg + 22 + file_name_length,&rlen,4);
		memcpy(msg + 22  + file_name_length +4,recv_buff,rlen);

		send_len = len_a + rlen;
			
		//printf("pack_nu = %lld\n",*((long long *)(msg +2)));
		//printf("readlen  = %d\n",*((int *)(msg + 22 + file_name_length)));
		//printf("readbuff  = %s\n",(msg + 13 + file_name_length +4));
		//printf("send len = %d\n",send_len);
		

		//发送数据包msg , send_len
		if(pack_nu == 1)
			usleep(3000);
		Sendto(sockfd,msg,send_len,0,pservaddr,servlen);
		my_count++;
			//andy : sendto能立即返回？

		//这里还要添加一个延时作为流量控制
		if((pack_nu%PACK_T) == 0)
		usleep(WAIT_TIME);
		//printf("----------------------->>wait finished\n");
	}

	// 关闭文件释放资源
	close(fd);
//	free(file_name);
	free(msg);
	printf("\ntotal pack_nu = %lld\n",pack_nu);

	return 0;
}

