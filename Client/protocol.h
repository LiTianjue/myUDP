#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__
/*----UDP包格式说明----*/
/*
  --name--			--type--      --value--
   version			char         '\017'		
   protocol			char         as follow
   pack_nu			long long    start from 1
   file_size		long long  
   file_name_length	int
   file_name		char[]
   pack_length		int
   pack				char[]
*/

//protocol
#define PT_ONLY		'\013'
#define PT_START	'\014'
#define PT			'\015'
#define PT_END		'\016'

#define FOR_NEXT	'\011'
#define ALL_OVER	'\022'

#define	MAX_PATH	1024
#define MAXLINE		1024
#define MAX_BUFF	1024*2
#define MAX_PACK	1024

#define RUN_PATH		"/run/shm/"

#endif
