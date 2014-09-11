#define MAXMSG	(8192 + sizeof(long)) /*linux 可发送的消息的最长字节数*/

struct msgbuf {
	long mtype;
	char mtext[1];
};		/*消息队列的数据结构*/

typedef struct __MyPthread {
	int msqid;
	key_t key;
} MyPthread;	/*文件处理线程的结构*/

void File_Process(void *消息队列,key键值)
{
	int fd,mqid;
	char *buff = malloc(MAXMSG);
	int n;

	long pack_total;

	/**这里需要有几个变量统计接收和处理文件包的长度**/
	long pack_saved; /*统计处理的包个数，用于判断文件完整*/
	long pack_move; /*统计包处理到的位置，用于在文件不完整的时候也退出循环*/
	/**pakc_saved == pack_move 文件完整**/
	/**pack_saved = 文件总大小/1024 文件处理完成**/
again:	
	pack_saved = 0;
	pack_move = 1;

	while(1)
	{	
		if(pack_saved == 0)
			//解析出文件包的总个数;
			pack_total = ;
			//pack_move 不能大于 pack_total;

		n = msgrcv(mqid,buff,MAXMSG,pack_move,0);
		//这里是要设置超时的噢，
		/*	{
				超时的设置：
					一定时间接收不到指定编号的包，丢弃这个包去接收下一个
					//没有别的策略就用论询好了
					pack_move ++;
					n = msgrcv(mqid,buff,MAXMSG,pack_move,0);
					//这个 msgrcv() 是非阻塞的调用
					//用goto 直到我们找到队列中有存在我们需要的包
					
					在这个过程中由于已经出现超时等待了
					所以在丢弃包之后直接去找到我们需要的包
					中间不再做超时等待而是认为我们的包已经丢失(避免再次等待造成队列堆积)
			} */
		/*正常解析移动这个标记*/
		pack_move ++;

		//解析文件
		if(/*包编号为1*/)	/*包编号可以不解析，直接读取pack_move*/
		{
			fd = open();			/**打开文件**/
		}

		write();					/**写文件 **/
		pack_saved++;				/**写文件成功，记录**/

		//检查文件大小
		if(pack_total == pack_saved)
		{
			//文件完整传输完成了，无丢包错序
			//完成相应的操作，退出循环
		}
		else if(pack_total == pack_move)
		{
			//已经接收到最后一个包了，但是文件不完整
			//写错误日志;
		}
	}

	/**循环外处理**/
	/**退出循环表示一个文件处理完成，对该文件的资源进行处理**/
	/*返回循环去处理下一个文件*/
	free(buf);
}
