/**将数据暂存到缓冲区**/
//Save_Msg(msg,pos,pack_nu,count);
void Save_Msg(char *msg,int n,int pk,int size)
{
	RECV_BUFF[n].pack_num   = pk;
	RECV_BUFF[n].is_used    = 1;
	RECV_BUFF[n].buff_size  = size;
	memcpy(RECV_BUFF[n].buff,msg,size);
}

/**处理数据**/
int Save_UDP(char *msg)
{
	int ret,fd;
	mode_t fdmode = (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	int wlen;


	/**--UDP 包结构--**/
	char version;
	char protocol;
	long long pack_nu;
	long long file_size;
	int file_name_length;
	char *file_name;
	int pack_length;
	char *pack;

	/**--获取包结构--**/
	//memcpy(&version,msg,1);
	memcpy(&protocol,msg+1,1);
	memcpy(&pack_nu,msg+2,8);
	memcpy(&file_size,msg+10,8);
	memcpy(&file_name_length,msg+18,4);

	file_name = (char *)malloc(file_name_length+1);

	memcpy(file_name,msg+22,file_name_length);
	*(file_name+file_name_length) = '\0';
	memcpy(&pack_length,msg+22+file_name_length,4);
	pack = msg + 22 + file_name_length + 4;

	//printf("======>> andy: pack_nu:%d\n",pack_nu);
	if((protocol == PT_START) || (protocol == PT_ONLY))
	{
		ret = CreateDir(file_name);
	}

	fd = open(file_name,O_RDWR|O_CREAT|O_APPEND,fdmode);
	if(fd == -1)
	{
		printf("open file %s error :%s\n",file_name,strerror(errno));
		ret =  -1;
		goto in_my_end;
	}

	wlen = write(fd,pack,pack_length);
	if(wlen != pack_length)
	{
		printf("write error : %s\n",strerror(errno));
		ret = -1;
		goto in_my_end;
	}

	//如果是 PT_ONLY 或者 PT_END 说明这个文件传输完成
	if((protocol == PT_END) || (protocol == PT_ONLY))
	{
		long long check = GetFileSize(file_name);
		if(check != file_size)
		{
			printf("**********--------Andy---> lost package!!!\n");
			char log[256];
			sprintf(log,"file %s is incomplete",file_name);
			WriteSysLog(LOG_PATH,log);
		}
		else
		{
			printf("Andy---> remove file%s\n",file_name);
			fflush(NULL);
			remove(file_name);
		}
	}

in_my_end:
	free(file_name);
	return ret;
}



void Per_Process(char *msg,int count)
{
	char version;
	char protocol;
	long long pack_nu;

	char tmp_msg[MAXDATASIZE] = {0};


	memcpy(&version,msg,1);
	memcpy(&protocol,msg+1,1);
	memcpy(&pack_nu,msg+2,8);
/*
	int file_name_length;
	char *file_name;
	memcpy(&file_name_length,msg+18,4);
	file_name = (char *)malloc(file_name_length+1);
	memcpy(file_name,msg+22,file_name_length);
	*(file_name+file_name_length) = '\0';
*/
	if(version != '\017')	/*不是我们要的包格式*/
		return;


	//int Current = pack_nu;
	int pos;

	if(pack_nu == 1)	/*这是一个新文件*/
	{
		//处理这个包,这个包编号为1，可以先处理掉
		Save_UDP(msg);
		SAVE = 1;

		if(Is_Empty() == -1)
		{
			//缓冲区无数据
			//去读下一个包
			return;
		}
		else	/*缓冲区有数据*/
		{

			//检查缓冲区有无SAVE+1,可能有包先于开始包到来
loop_1:		
			pos = Check_Next(SAVE+1);
			if(pos == -1)
			{
				printf("List clear all\n");
				Clear_All_Buff();/*没有说明是以前遗留的数据，处理掉*/
			}
			else
			{
 				//处理SAVE+1
				printf("List  out\n");
				SAVE = SAVE +1;
				memcpy(tmp_msg,(RECV_BUFF[pos].buff),RECV_BUFF[pos].buff_size);
				Save_UDP(tmp_msg);
				Clear_Buff(pos);
				goto loop_1 ;
			}

		}
	}
	else if(pack_nu == SAVE +1)		/*数据正常接收*/
	{
		//正常写入数据
		Save_UDP(msg);
		SAVE = pack_nu;

loop_2:	
		pos = Check_Next(SAVE + 1);
		if(pos == -1)
		{
			//缓冲区没有我们要的数据
			//去读下一个包
			return;
		}
		else
		{
			printf("List  out return:%d\n",pos);
			SAVE = SAVE +1;
			memcpy(tmp_msg,(RECV_BUFF[pos].buff),RECV_BUFF[pos].buff_size);
			Save_UDP(tmp_msg);
			Clear_Buff(pos);
			goto loop_2;
		}

	}
	else if(pack_nu > SAVE + 1)		/*数据发生了错位*/
	{
		if((pos = Is_Full()) != -1)		/*缓冲区未满，先暂存*/
		{
			printf("List tmp in  %d,Current = %d,SAVE = %d\n",pos,pack_nu,SAVE);
			Save_Msg(msg,pos,pack_nu,count);
			return ;
		}
		else if(Is_Full() == -1)	/*缓冲区已满*/
		{
			SAVE = SAVE +1;		/*丢弃SAVE + 1,这种情况意味着已经收到多于BUFF_COUNT个的包Save+1还没收到*/
			printf("Andy---> we lost pack %d\n",SAVE);
loop_3:
			pos = Check_Next(SAVE + 1);
			if(pos == -1)
			{
				//没有我们要的包，去读下一个
				return;
			}
			else /*处理SAVE+1*/
			{
				printf("List  out :%d\n",pos);
				SAVE = SAVE +1;
				memcpy(tmp_msg,(RECV_BUFF[pos].buff),RECV_BUFF[pos].buff_size);
				Save_UDP(tmp_msg);
				Clear_Buff(pos);
				goto loop_3;
			}

		}
	}
	else if(pack_nu < SAVE)	/*出现这种情况表示收到一个已经被放弃处理的包*/	
	{
		printf("List  return\n");
		/*不做任何处理*/
		return;
	}

}

