#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__
/*----UDP包格式说明----*/
/*
  --name--			--type--      --value--
   version			char         '\017'		
   protocol			char         as follow
   pack_nu			long long    start from 1
   file_size			long long  
   file_name_length		int
   file_name			char[]
   pack_length			int
   pack				char[]
*/

//protocol
#define PT_ONLY		'\013'
#define PT_START	'\014'
#define PT		'\015'
#define PT_END		'\016'

#define FOR_NEXT	'\011'
#define ALL_OVER	'\022'

#define	MAX_PATH		1024
#define MAXLINE			1024
#define MAX_BUFF		1024*2
#define MAX_PACK		1024

#define MAXDATASIZE		1024*2


/**--缓冲区的大小--**/
#define BUFF_SIZE		2048
#define BUFF_COUNT		8

#define	CONFIG_FILE		"/usr/app/stp/others/UdpConf.txt"
#define RUN_PATH		"/run/shm"

#define LOG_PATH		"/root/mytest_log"
#define TMP_FILE		"/tmp/test1"


#define SERVER_IP		"1.1.1.1"
//#define SERVER_IP		"127.0.0.1"
#define SERVER_PORT		6708

/*缓冲区的数据结构*/
typedef struct _buf_list
{
	int pack_num;
	int is_used;
	int buff_size;
	char buff[BUFF_SIZE];
}recv_buff;

/*消息队列的结构*/
#define MAXMSG		(8192 + sizeof(long))
#define WAIT_MSG	8

struct msgbuf {
	long mtype;
	char mtext[1];
};

typedef struct __MyPthread {
	int msqid;
	key_t key;
} MyPthread;


#endif
