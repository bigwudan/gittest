#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h> 
#include <sys/time.h>    // for gettimeofday()
#include <signal.h>
#define BUFFER_SIZE 1024
#define HTTP_GET "GET /%s HTTP/1.1\r\nHOST: %s:%d\r\nAccept: */*\r\n\r\n"
#define NUM 10
#define TEST_HTML "asdasd\r\nxw\r\nxxxxx\r\nxw\r\ncccccc\r\nbn\r\nvvvvv\r\n0\r\n"
#define FINDED "\r\n"
#define MYWEB "http://wap.dev.epet.com/group/v226/detail.html?do=Login&uid="

void rescu_Parse(const char *pHtml, char *pRes)
{
    char *pFirst = NULL;
    char *pDur = NULL;
    char durNum = 0;
    pFirst = strstr(pHtml, FINDED);
    if(strcmp(pFirst, "\r\n0\r\n") == 0){
    }else{
        pFirst = pFirst+4+2;
        durNum = strstr(pFirst, FINDED) - pFirst;
        char sList[512] = {"\0"};
        strncpy(sList, pFirst ,durNum);
        strcat(pRes, sList);
        pFirst = pFirst + durNum;
        rescu_Parse(pFirst, pRes);
    }
}


int rescu_ParseA(const char *pHtml, char *pRes)
{
    char *pFirst = NULL;
    char *pDur = NULL;
    char durNum = 0;
    
    if(strlen(pFirst) == 5){
        return 0;
    }else{
        pFirst = pFirst+4+2;
        durNum = strstr(pFirst, FINDED) - pFirst;
        char sList[512] = {"\0"};
        strncpy(sList, pFirst ,durNum);
        strcat(pRes, sList);
        pFirst = pFirst + durNum;
        rescu_ParseA(pFirst, pRes);

    }
    
    // return 0;    
}


int main()
{
    char testList[1024] = {"\0"};
    rescu_Parse(TEST_HTML, testList);

     printf("%s\n", testList);

}
