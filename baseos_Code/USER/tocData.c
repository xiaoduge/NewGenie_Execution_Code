#include "tocData.h"
#include "math.h"


void addTocValue(TocValues* TocValues, double value)
{
	if(TocValues->total < D_VALUESNUM)
	{
		TocValues->total++;
	}

	if(TocValues->cursor >= D_VALUESNUM)
	{
		TocValues->cursor = 0;
	}
	TocValues->a[TocValues->cursor] = value;
	TocValues->cursor++;

	if(D_VALUESNUM == TocValues->total)
	{
		calcTargetValue(TocValues);
	}

}
void calcTargetValue(TocValues* TocValues)
{
	double temp = 0;
	for(int i = 0; i < D_VALUESNUM; i++)
	{
		temp += TocValues->a[i];
	}
	temp /= D_VALUESNUM;
	
	if(0 == TocValues->value)
	{
		TocValues->value = temp;
	}
	
	if(TocValues->value > temp)
	{
        TocValues->value = temp; 
	}

}

BOOL checkValuesDiff(TocValues* TocValues)
{
	double min = TocValues->a[0];
	double max = TocValues->a[0];

	for(int i = 1; i < D_VALUESNUM; i++)
	{
		if(TocValues->a[i] < min)
		{
			min = TocValues->a[i];
		}
		if(TocValues->a[i] > max)
		{
			max = TocValues->a[i];
		}
	}

	double diff = max - min;
    
    return diff > 1.0 ? FALSE : TRUE;

}



