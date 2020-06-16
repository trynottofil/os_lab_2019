#include "sum.h"
#include <stdlib.h>
//struct SumArgs {
//	int *array;
//	int begin;
//	int end;
//};
int Sum(const struct SumArgs *args) 
{
	int sum = 0;
	// TODO: your code here 	
	for (size_t i = args->begin; i < args->end; i++)
	{
		sum += args->array[i];		
	}
	
	return sum;
}