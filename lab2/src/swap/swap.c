#include "swap.h"

void Swap(char *left, char *right)
{
	char buffer;
    buffer = *right;
    *right = *left;
    *left= buffer;
}
