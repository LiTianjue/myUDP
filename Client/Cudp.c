#include "MY_UDP.h"

#include "Common.h"

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>			/*DIR*/



//#define WAIT_TIME	4000		/*两个UDP包的发送间隔时间,单位: 微秒*/

//#define SERVER_IP	"1.1.1.1"
#define SERVER_IP	"1.1.1.1"
#define SERVER_PORT	6708


#define MAXUSER	1	
#define TEST_PATH		"/root/andytest/"
#define CONFIG_FILE		"/root/etc/testConf.txt"
//#define CONFIG_FILE		"/home/andy/tmp/testConf.txt"


static int WAIT_TIME;
static int PACK_T;
static int my_count=0;

#include "protocol.h"
#include "SendFile.c"


int main(int argc,char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;


	Init_sockaddr_s(&servaddr,AF_INET,SERVER_IP,SERVER_PORT);
	
	 printf("we connet to  %s port : %d\n",inet_ntoa(servaddr.sin_addr),
			 ntohs(servaddr.sin_port));


	sockfd = Socket(AF_INET,SOCK_DGRAM,0);

	if(GetConfigIntValue(CONFIG_FILE,"COMMON","WAIT_TIME",&WAIT_TIME))
	{
		printf("Read Config File Fail\n");
	}
	if(GetConfigIntValue(CONFIG_FILE,"COMMON","PACK_T",&PACK_T))
	{
		printf("Read Config File Fail\n");
	}

	printf("Read wait time = %d\n",WAIT_TIME);


	//测试发送的文件
	DIR *d;
	struct dirent *file;
	struct stat sb;

	int depth = 1;

	char filename[256] = {0};
	char file_path[256] ={0};
	if(!(d = opendir(argv[1])))
	{
		printf("error opendir %s!!!\n",argv[1]);
		return -1;
	}

	while((file = readdir(d)) != NULL)
	{
		memset(filename,0,256);
		memset(file_path,0,256);
		strcpy(file_path,argv[1]);

		if(strncmp(file->d_name,".",1) == 0)
			continue;
		strcpy(filename,file->d_name);
		if(stat(file->d_name,&sb) >= 0 && S_ISDIR(sb.st_mode) && depth <= 3)
		{
			printf("有多级目录不支持,隐藏文件不支持\n");
			continue;
		}

		strcat(file_path,filename);

		printf("Send file :%s ...\t",file_path);
		fflush(NULL);

		SendPack(file_path,sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

		printf("done!\n");
		printf("--------------\n\n");
		fflush(NULL);
	}
	close(sockfd);
	printf("------------------------Send package %d------------\n",my_count);

	exit(0);
}

