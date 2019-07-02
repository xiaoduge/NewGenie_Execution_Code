#ifndef _TOCDATA_H_
#define _TOCDATA_H_

#include "DtypeStm32.h"


/************************************************
*Description: TOC calculation new algorithm data structure
*date: 2019.03.25
************************************************/
#define D_SAMPLESNUM 30     //data samples
#define D_VALUESNUM  6       //ValuesNum
typedef struct
{
	double a[D_VALUESNUM];
	int cursor;
	int total;
	double value;

}TocValues;



/************************************************
*Description: TOC calculation new algorithm
*date: 2019.03.25
************************************************/
void addTocValue(TocValues* TocValues, double value);
void calcTargetValue(TocValues* TocValues);
BOOL checkValuesDiff(TocValues* TocValues);

#endif

