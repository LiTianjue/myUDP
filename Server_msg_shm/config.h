/*************************************************************************
	> File Name: config.h
	> Author: TianjueLi
	> Mail: mhpmii@126.com 
	> Created Time: 2014年06月23日 星期一 
 ************************************************************************/

#ifndef _CONFIG_H_
#define _CONFIG_H_


#define  SUCCESS           0x00 /*成功*/  
#define  FAILURE           0x01 /*失败*/  
  
#define  FILENAME_NOTEXIST      0x02 /*配置文件名不存在*/  
#define  SECTIONNAME_NOTEXIST    0x03 /*节名不存在*/  
#define  KEYNAME_NOTEXIST      0x04 /*键名不存在*/  
#define  STRING_LENNOTEQUAL     0x05 /*两个字符串长度不同*/  
#define  STRING_NOTEQUAL       0x06 /*两个字符串内容不相同*/  
#define  STRING_EQUAL        0x00 /*两个字符串内容相同*/  
  


#define COMMOM			"COMMON"
#define IP_TABLES		"IP_TABLES"
#define PORT_TABLES	"PORT_TABLES"


/**-----------------------------------------------------**/

int CompareString(char *pInStr1,char *pInStr2);  
int GetKeyValue(FILE *fpConfig,char *pInKeyName,char *pOutKeyValue);  
int GetConfigIntValue(char *pInFileName,char *pInSectionName,char *pInKeyName,int *pOutKeyValue);  
int GetConfigStringValue(char *pInFileName,char *pInSectionName,char *pInKeyName,char *pOutKeyValue);  

/**----Add by Andy------2014/06/24---------------***/  

int CheckConfigValue(char *pInFileName,char *pInSectionName,char *pInConfigValue);


int CheckLineValue(FILE *fpConfig,char *pInKeyName);

/**----Add bu Andy----2014/07/28-------------**/

void GetIP_PORT(char *pIn,char *OutIP,int *OutPort);

void GetApp_IP(char *Inline,char *pOutIP,int *pOutPort,int n);

void GetApp_Name(char *config_path,char *pOutName,int n);

int GetApp_Num(char *config_path);

void GetUDP_IP_PORT(char *file,char *OutIP,int *OutPort,int n);

void GetTCP_IP_PORT(char *file,char *OutIP,int *OutPort,int n);







#endif
