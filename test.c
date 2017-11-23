#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {  
    char *key;  
    char *value;  
} key_value;  

int main()
{
    int n = sizeof(key_value);
    printf("%d\n", n);
}