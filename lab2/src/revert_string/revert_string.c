#include "revert_string.h"

void RevertString(char *str)
{
	int lenght=strlen(str);
    char*newstring = (char*) calloc(lenght, sizeof(char));
    int i=0;
    for (i=lenght-1; i>-1;i--)
    {
        newstring[lenght-i-1]=str[i];
    }
    strcpy(str, newstring);
}

