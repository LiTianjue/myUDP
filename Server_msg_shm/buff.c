//检查缓冲区有没有SAVE + 1返回缓冲区的位置 
int Check_Next(int next)
{
	int i ;
	for(i = 0;i < BUFF_COUNT;i++)
	{
		if(RECV_BUFF[i].is_used != 0)
			if(RECV_BUFF[i].pack_num == next)
				return i;
	}
	return -1;
}

// 检查缓冲区是否满了
int Is_Full(void)
{
	int i;
	for(i = 0;i< BUFF_COUNT;i++)
	{
		if(RECV_BUFF[i].is_used == 0)
			return i;	/*缓冲区未满,返回一个位置*/
	}
	return -1;/*缓冲区满了*/
}

int Is_Empty(void)
{
	int i;
	for(i = 0;i< BUFF_COUNT;i++)
	{
		if(RECV_BUFF[i].is_used == 1)
			return 0;	/*缓冲区非空*/
	}
	return -1;/*缓冲区是空*/
}


//清空缓冲区
void Clear_All_Buff(void)
{
	int i;
	for(i = 0;i < BUFF_COUNT;i++)
	{
		RECV_BUFF[i].is_used = 0;
	}
}

void Clear_Buff(int i)
{
	RECV_BUFF[i].is_used = 0;
}

