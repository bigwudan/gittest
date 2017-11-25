#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
int main()
{
    int temp[5] = {1, 2, 3, 4, 5};
    int(*p2)[5] = &temp;
    printf("*p2=%x\n", *p2);
    printf("p2=%x\n", p2);
    printf("temp=%x\n", &temp);
    assert(0);
    int i;
    for (i = 0; i < 5; i++)
    {
        printf("%d\n", *(*p2 + i));
    }
}