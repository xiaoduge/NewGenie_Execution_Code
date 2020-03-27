#ifndef _MFDDRIVER_H_
#define _MFDDRIVER_H_

#include <stdbool.h>

typedef enum
{
    MFD_STATE_CIP = 0X1, /* CHIP IN POSITION  */
    MFD_STATE_SIP = 0X2, /* SECURITY INFO IN POSITION */
    MFD_STATE_ALL = 0X3,
}MFD_STATE_ENUM;

void mfd_Init(int gpio,uint8_t ucAdr);
int mfd_isPresent(void);
int read_mfd(uint8_t *aucIn,uint8_t *aucOut);
int write_mfd(uint8_t *aucInfo);

#endif
