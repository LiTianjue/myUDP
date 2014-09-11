pthread_t tid;

struct msgbuf *msgbuf;


MyPthread my_test;

int c ,oflag,mqid;
oflag = IPC_CREAT;
oflag |= IPC_EXCL;

key_t my_key;
my_key = ftok(TMP_FILE,0);
if(my_key == -1)
	perror("key error!");


mqid = msgget(my_key, oflag);
if(mqid == -1)
	perror("msgget error");

my_test.msqid = mqid;
my_test.key = my_key;


while  循环之前做事情 
pthread_create(&tid,NULL,&File_Process,(void *)mytest);


接收到消息之后发送消息给队列
msfbuf = calloc(sizeif(long) + rlen,sizeof(char));
memcpy(msg->mtext,msg,rlen);
long long pack_nu;
memcpy(&pack_nu,msg+2,8);
msg->mtype = (long)pack_nu;

if((msgsnd(mqid,msgbuf,rlen,0) == -1))
	perror("msgsnd error");

