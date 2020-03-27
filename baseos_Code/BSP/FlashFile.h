#ifndef _FLASH_FILE_H_
#define _FLASH_FILE_H_

#define FF_PAGE_NO (9)

#define FF_MAX_FILE (3)

typedef enum
{
    FF_READ  = 0X1,
    FF_WRITE = 0X2   ,    
    FF_TRUNC = 0X4     ,
    FF_NUM,
}FF_ENUM;

typedef enum
{
    FF_FILENO_CTRL_MAP  = 0,
    FF_FILENO_TGT_MAP      ,    
    FF_FILENO_SCENARIO     ,
    FF_FILENO_NUM,
}FF_FILENO_ENUM;



typedef enum
{
    FF_ERR_CODE_SUCC         = 0,
    FF_ERR_CODE_INVALID_FILE    ,    
    FF_ERR_CODE_INVALID_OFFSET  ,   
    FF_ERR_CODE_INVALID_UNKNOW  ,
    FF_ERR_CODE_NUM,
}FF_ERR_CODE_ENUM;


void FF_Init(void);
char* FF_gets ( int ifileNo,	char* buff,		int len);
int FF_Write(int ifileNo,unsigned char *pBufffer,int iLength,unsigned int* bw);
int FF_Read(int ifileNo,unsigned char *pBufffer,int iLength,unsigned int* bw);
int FF_Seek(int ifileNo,int iType, int iOffset);
int FF_Open(int ifileNo,int iAccessType);
void FF_Close(int ifileNo);
int FF_GetSize(int ifileNo);


#endif

