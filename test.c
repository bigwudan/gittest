#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
int main()
{
    char *pTest = NULL;
    pTest = (char *)malloc(100*sizeof(char));
    sprintf(pTest, "%s", "httt");

    //printf("test=%s\n", pTest);
    free(pTest);
    free(pTest);
    
    printf("test=%s\n", pTest);
    return 1;


}