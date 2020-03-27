#ifndef _LEAKAGE_H_
#define _LEAKAGE_H_

void LeakageInit(int iRate);
void LeakageSetup(uint32_t ulDetPeriod,uint32_t ulThd);
int LeakageGetState(void);
void LeakageTickTask(void);

#endif

