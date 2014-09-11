/*************************************************************************
	> File Name: config.c
	> Author: TianjueLi
	> Mail: mhpmii@163.com 
	> Created Time: 2014年06月23日 星期一 
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>  
#include <string.h>  
#include "config.h"  


#define Debug ( printf("func:%s.line :%d\n",__func__,__LINE__))

int GetConfigStringValue(char *pInFileName,char *pInSectionName,char *pInKeyName,char *pOutKeyValue)  
{  
	FILE *fpConfig;  
	char szBuffer[150];  
	char *pStr1,*pStr2;  
	int iRetCode = 0;  
	/*test*/  
	/* 
	 *     printf("pInFileName: %s !\n",pInFileName); 
	 *         printf("pInSectionName: %s !\n",pInSectionName); 
	 *             printf("pInKeyName: %s !\n",pInKeyName); 
	 *                 */  

	memset(szBuffer,0,sizeof(szBuffer));  
	if( NULL==( fpConfig=fopen(pInFileName,"r") ) )  
		return FILENAME_NOTEXIST;  

	while( !feof(fpConfig) )  
	{  
		if( NULL==fgets(szBuffer,150,fpConfig) )  
			break;  
		pStr1 = szBuffer ;    
		while( (' '==*pStr1) || ('\t'==*pStr1) )  
			pStr1++;  
		if( '['==*pStr1 )  
		{  
			pStr1++;  
			while( (' '==*pStr1) || ('\t'==*pStr1) )  
				pStr1++;  
			pStr2 = pStr1;  
			while( (']'!=*pStr1) && ('\0'!=*pStr1) )  
				pStr1++;  
			if( '\0'==*pStr1)     
				continue;  
			while( ' '==*(pStr1-1) )  
				pStr1--;      
			*pStr1 = '\0';  

			iRetCode = CompareString(pStr2,pInSectionName);   
			if( !iRetCode )/*检查节名*/  
			{  
				iRetCode = GetKeyValue(fpConfig,pInKeyName,pOutKeyValue);  
				fclose(fpConfig);  
				return iRetCode;  
			}     
		}                     
	}  

	fclose(fpConfig);  
	return SECTIONNAME_NOTEXIST;  

}     

/*区分大小写*/  
int CompareString(char *pInStr1,char *pInStr2)  
{  
	if( strlen(pInStr1)!=strlen(pInStr2) )  
	{  
		return STRING_LENNOTEQUAL;  
	}     

	/*while( toupper(*pInStr1)==toupper(*pInStr2) )*//*#include <ctype.h>*/  
	while( *pInStr1==*pInStr2 )  
	{  
		if( '\0'==*pInStr1 )  
			break;    
		pInStr1++;  
		pInStr2++;    
	}  

	if( '\0'==*pInStr1 )  
		return STRING_EQUAL;  

	return STRING_NOTEQUAL;   

}  

int GetKeyValue(FILE *fpConfig,char *pInKeyName,char *pOutKeyValue)  
{  
	char szBuffer[150];  
	char *pStr1,*pStr2,*pStr3;  
	unsigned int uiLen;  
	int iRetCode = 0;  

	memset(szBuffer,0,sizeof(szBuffer));      
	while( !feof(fpConfig) )  
	{     
		if( NULL==fgets(szBuffer,150,fpConfig) )  
			break;  
		pStr1 = szBuffer;     
		while( (' '==*pStr1) || ('\t'==*pStr1) )  
			pStr1++;  
		if( '#'==*pStr1 )     
			continue;  
		if( ('/'==*pStr1)&&('/'==*(pStr1+1)) )    
			continue;     
		if( ('\0'==*pStr1)||(0x0d==*pStr1)||(0x0a==*pStr1) )      
			continue;     
		if( '['==*pStr1 )  
		{  
			pStr2 = pStr1;  
			while( (']'!=*pStr1)&&('\0'!=*pStr1) )  
				pStr1++;  
			if( ']'==*pStr1 )  
				break;  
			pStr1 = pStr2;    
		}     
		pStr2 = pStr1;  
		while( ('='!=*pStr1)&&('\0'!=*pStr1) )  
			pStr1++;  
		if( '\0'==*pStr1 )    
			continue;  
		pStr3 = pStr1+1;  
		if( pStr2==pStr1 )  
			continue;     
		*pStr1 = '\0';  
		pStr1--;  
		while( (' '==*pStr1)||('\t'==*pStr1) )  
		{  
			*pStr1 = '\0';  
			pStr1--;  
		}  

		iRetCode = CompareString(pStr2,pInKeyName);  
		if( !iRetCode )/*检查键名*/  
		{  
			pStr1 = pStr3;  
			while( (' '==*pStr1)||('\t'==*pStr1) )  
				pStr1++;  
			pStr3 = pStr1;  
			while( ('\0'!=*pStr1)&&(0x0d!=*pStr1)&&(0x0a!=*pStr1) )  
			{  
				if( ('/'==*pStr1)&&('/'==*(pStr1+1)) )  
					break;  
		 		pStr1++;      
			}     
			*pStr1 = '\0';  
			uiLen = strlen(pStr3);  
			memcpy(pOutKeyValue,pStr3,uiLen);  
			*(pOutKeyValue+uiLen) = '\0';  
			return SUCCESS;  
		}  
	}  

	return KEYNAME_NOTEXIST;  
}  


int GetConfigIntValue(char *pInFileName,char *pInSectionName,char *pInKeyName,int *pOutKeyValue)  
{  
	int iRetCode = 0;  
	char szKeyValue[16],*pStr;  

	memset(szKeyValue,0,sizeof(szKeyValue));  
	iRetCode = GetConfigStringValue(pInFileName,pInSectionName,pInKeyName,szKeyValue);  
	if( iRetCode )  
		return iRetCode;  
	pStr    = szKeyValue;  
	while( (' '==*pStr)||('\t'==*pStr))  
		pStr++;  
	if( ('0'==*pStr)&&( ('x'==*(pStr+1))||('X'==*(pStr+1)) ) )    
		sscanf(pStr+2,"%x",pOutKeyValue);  
	else  
		sscanf(pStr,"%d",pOutKeyValue);  

	return SUCCESS;   
}

/*******************************************************
 *	add by andy , check congfig line
 * ******************************************************/

int CheckConfigValue(char *pInFileName,char *pInSectionName,char *pInConfigValue)
{
	FILE *fpConfig;
	char szBuffer[150];
	char *pStr1,*pStr2;
	int iRetCode = 0;

	memset(szBuffer,0,sizeof(szBuffer));
	
	if( NULL==( fpConfig=fopen(pInFileName,"r") ) )  
		return FILENAME_NOTEXIST;  

	while(!feof(fpConfig))
	{	
		if(NULL == fgets(szBuffer,150,fpConfig))
			break;
		pStr1 = szBuffer;
		while((' '==*pStr1) || ('\t' == *pStr1 ))
			pStr1++;
		if('[' == *pStr1)
		{
			pStr1++;
			while((' '==*pStr1) || ('\t' == *pStr1 ))
				pStr1++;
			pStr2 = pStr1;
			while((']' != *pStr1) && ('\0' != *pStr1))
				pStr1++;
			if('\0' == *pStr1)
				continue;
			while(' ' == *pStr1)
				pStr1--;
			*pStr1 = '\0';

			iRetCode = CompareString(pStr2,pInSectionName);
			if( !iRetCode )
			{
				iRetCode = CheckLineValue(fpConfig,pInConfigValue);
				if(!iRetCode)
				{
					fclose(fpConfig);
					return iRetCode;
				}
			}
		}
	}

	fclose(fpConfig);

	return KEYNAME_NOTEXIST;
}


int CheckLineValue(FILE *fpConfig,char *pInKeyName)
{  
	char szBuffer[150];  
	char *pStr1,*pStr2;  
	int iRetCode = 0;  

	memset(szBuffer,0,sizeof(szBuffer));      
	while( !feof(fpConfig) )  
	{     
		if( NULL==fgets(szBuffer,150,fpConfig) )  
			break;  
		pStr1 = szBuffer;     
		while( (' '==*pStr1) || ('\t'==*pStr1) )  
			pStr1++;  
		if( '#'==*pStr1 )     
			continue;  
		if( ('/'==*pStr1)&&('/'==*(pStr1+1)) )    
			continue;     
		if( ('\0'==*pStr1)||(0x0d==*pStr1)||(0x0a==*pStr1) )      
			continue;     
		if( '['==*pStr1 )  
		{  
			pStr2 = pStr1;  
			while( (']'!=*pStr1)&&('\0'!=*pStr1) )  
				pStr1++;  
			if( ']'==*pStr1 )  
				break;  
			pStr1 = pStr2;    
		}     
		pStr2 = pStr1;  
		while( (' '!=*pStr1)&&('\0'!=*pStr1) )  
			pStr1++;  
		if( pStr2==pStr1 )  
			continue;     
		pStr1--;
		*pStr1 = '\0';  
	/*	while( (' '==*pStr1)||('\t'==*pStr1) )  
		{  
			*pStr1 = '\0';  
			pStr1--;  
		}  
	*/
		iRetCode = CompareString(pStr2,pInKeyName);  
		if(!iRetCode )/*检查键名*/  
		{  
			/*pStr1 = pStr3;  
			while( (' '==*pStr1)||('\t'==*pStr1) )  
				pStr1++;  
			pStr3 = pStr1;  
			while( ('\0'!=*pStr1)&&(0x0d!=*pStr1)&&(0x0a!=*pStr1) )  
			{  
			 	if( ('/'==*pStr1)&&('/'==*(pStr1+1)) )  
					break;  
 		 		pStr1++;      
			}     
			*pStr1 = '\0';  
			uiLen = strlen(pStr3);  
			memcpy(pOutKeyValue,pStr3,uiLen);  
			*(pOutKeyValue+uiLen) = '\0';  
			*/
			return SUCCESS;  
		}  
		else
		{
			continue;
		}
	}  
	return KEYNAME_NOTEXIST;  
}  

/*********************************************/
/* 为Sudp程序添加 配置文件读取的具体代码
 * 2014/07/29
 ************/
void GetIP_PORT(char *pIn,char *OutIP,int *OutPort)
{
	int i;
	char *tmp = strdup(pIn);
	for(i = 0 ;i < strlen(pIn);i++)
	{
		if(*(tmp+i) == ':')
		{
			*(tmp+i) = '\0';
			strcpy(OutIP,tmp);
			sscanf(tmp+i+1,"%d",OutPort);
		}
	}
	free(tmp);
}

void GetApp_IP(char *Inline,char *pOutIP,int *pOutPort,int n)
{
	int i = 0;
	int count = 0;
	char ip_port[30] = {0};

	char *save = strdup(Inline);

	int pos = 0;

	for(i = 0;i<(strlen(save)+1) ;i++)
	{
		if(*(save + i) == ',' || *(save + i) == '\0')
		{
			if(count + 1 == n)
			{
				/*
				if(count == 0)
				{
					memcpy(ip_port,save + pos ,i - pos);
				}
				else
				{
					memcpy(ip_port,save + pos + 1 ,i - pos -1);
				}
				*/
				memcpy(ip_port,save + pos ,i - pos);
				ip_port[i] = '\0';
				GetIP_PORT(ip_port,pOutIP,pOutPort);
				free(save);
				return ;
			}
			count += 1;
			if(count +1 == n)
			{
				pos = i+1;
			}
		}
	}
}

void GetApp_Name(char *config_path,char *pOutName,int n)
{
	int i = 0;
	int count = 0;
	char save[128] = {0};
	GetConfigStringValue(config_path,"COMMON","appName",save);
	int pos = 0;

	for(i = 0;i<(strlen(save)+1) ;i++)
	{
		if(*(save + i) == ',' || *(save + i) == '\0')
		{
			if(count + 1 == n)
			{

				memcpy(pOutName,save + pos ,i - pos);
				*(pOutName+(i-pos)) = '\0';
				//save[i] = '\0';
				return ;
			}
			count += 1;
			if(count +1 == n)
			{
				pos = i+1;
			}
		}
	}
}

int GetApp_Num(char *config_path)
{
	int i = 0;
	int count = 0;
	char Inline[128] = {0};
	GetConfigStringValue(config_path,"COMMON","appName",Inline);
	if(Inline == NULL)
	{
		return -1;
	}

	for(i = 0;i<(strlen(Inline)+1) ;i++)
	{
		if(*(Inline + i) == ',' || *(Inline + i) == '\0')
		{
			count +=1;
		}
	
	}
	return count;
}

void GetUDP_IP_PORT(char *file,char *OutIP,int *OutPort,int n)
{
	char UDP_Port[256] = {0};
	char ip_port[30];
	
	GetConfigStringValue(file,"COMMON","UDP_Port",UDP_Port);

	GetApp_IP(UDP_Port,OutIP,OutPort,n);
}

void GetTCP_IP_PORT(char *file,char *OutIP,int *OutPort,int n)
{
	char TCP_Port[256] = {0};
	char ip_port[30];
	
	GetConfigStringValue(file,"COMMON","TCP_Port",TCP_Port);

	GetApp_IP(TCP_Port,OutIP,OutPort,n);
}
