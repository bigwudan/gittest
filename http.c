#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>
#include "http.h"

static int getCooketData(char **res_p, const char *htmlStr, const char *searchStr)
{
    if(*res_p != NULL){
        free(*res_p);
        *res_p = NULL;

    }
    char *tmp = strstr(htmlStr, searchStr);
    int tmpNum = strlen(searchStr);
    char *dian_p = NULL;
    tmp = tmp + tmpNum;
    dian_p = strchr(tmp, ';');
    tmpNum = dian_p - tmp;
    *res_p = (char *)calloc(tmpNum, sizeof(char));
    strncpy(*res_p, tmp, tmpNum);
}

//组织发送内容头文件
int http_get_send_content(char **pResult, const char *url, key_value *cookes,  int cookesNum)
{
    char *cookStr = NULL;
    if (cookes != NULL)
    {
        cookStr = strdup("Cookie:");
        int i = 0;
        for (i = 0; i < cookesNum; ++i)
        {
            int tmpCookesLen = strlen(cookes[i].key) + strlen(cookes[i].value) + strlen(cookStr) + 3;
            cookStr = realloc(cookStr, tmpCookesLen);
            strcat(cookStr, " ");
            strcat(cookStr, (cookes + i)->key);
            strcat(cookStr, "=");
            strcat(cookStr, (cookes + i)->value);
            strcat(cookStr, ";");
        }
    }
    int strNum = 0;
    strNum = strlen(HTTP_GET_HEAD) + strlen(url) + 4;
    if (cookStr != NULL)
    {
        strNum = strNum + strlen(cookStr);
    }
    *pResult = (char *)calloc(strNum, sizeof(char));
    if (*pResult == NULL)
    {
        return -1;
    }
    sprintf(*pResult, HTTP_GET_HEAD, url);
    if (cookStr != NULL)
    {
        strcat(*pResult, cookStr);
    }
    free(cookStr);
    cookStr = NULL;
    strcat(*pResult, "\r\n\r\n");
    return 1;
}

//分析数组
int http_parse_url(const char *url, char *host, char *file, int *port)
{
    char *ptr1 = NULL, *ptr2 = NULL;
    int len = 0;
    if (!url || !host || !file || !port)
    {
        return -1;
    }
    ptr1 = (char *)url;
    if (!strncmp(ptr1, "http://", strlen("http://")))
    {
        ptr1 += strlen("http://");
    }
    else
    {
        return -1;
    }

    ptr2 = strchr(ptr1, '/');

    if (ptr2)
    {
        len = strlen(ptr1) - strlen(ptr2);
        memcpy(host, ptr1, len);
        host[len] = '\0';
        if (*(ptr2 + 1))
        {
            memcpy(file, ptr2 + 1, strlen(ptr2) - 1);
            file[strlen(ptr2) - 1] = '\0';
        }
    }
    else
    {
        memcpy(host, ptr1, strlen(ptr1));
        host[strlen(ptr1)] = '\0';
    }

    //get host and ip
    ptr1 = strchr(host, ':');
    if (ptr1)
    {
        *ptr1++ = '\0';
        *port = atoi(ptr1);
    }
    else
    {
        *port = 80;
    }
    return 1;
}

//创建socket链接
int http_tcpclient_create(const char *host, int port)
{
    struct hostent *he;
    struct sockaddr_in server_addr;
    int socket_fd;

    if ((he = gethostbyname(host)) == NULL)
    {
        return -1;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)he->h_addr);

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        return -1;
    }

    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        return -1;
    }
    struct timeval timeout = {3, 0};  
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval)); 
    return socket_fd;
}
//发送信息
int http_tcpclient_send(int socket, char *buff)
{
    int sent = 0, tmpres = 0;
    int size = strlen(buff);
    while (sent < size)
    {
        tmpres = send(socket, buff + sent, size - sent, 0);
        if (tmpres == -1)
        {
            return -1;
        }
        sent += tmpres;
    }
    return sent;
}
//获取数据
int http_tcpclient_recv(int socket, char **lpbuff)
{
    int recvnum = 0;
    int count = 0;
    char tmpRec[1024] = {'\0'};
    char *new_ptr = NULL;
    int totRecNum = 0;
    memset(tmpRec,0,sizeof(char)*1024);

    while(1){
        count++;
        recvnum = recv(socket, tmpRec, 1024, 0);
        
        if(count == 1){
            memmove(*lpbuff, tmpRec, recvnum);
        }else{
            *lpbuff = realloc(*lpbuff, 1024*count);
            memmove(*lpbuff + totRecNum, tmpRec, recvnum);
        }
        totRecNum += recvnum;
        if(recvnum < 1024){
            break;
        }
    }

    if(strstr(*lpbuff, "Transfer-Encoding: chunked")){
        if(strstr(*lpbuff, "\r\n0\r\n")){
             return totRecNum;
        }else{
            while(1){
                count++;
                memset(tmpRec,0,sizeof(char)*1024);
                recvnum = recv(socket, tmpRec, 1024, 0);
                *lpbuff = realloc(*lpbuff, 1024*count);
                memmove(*lpbuff + totRecNum, tmpRec, recvnum);
                totRecNum += recvnum;
                if(strstr(tmpRec, "\r\n0\r\n")){
                    return totRecNum;
                }
            }
        }
    }else{
        return totRecNum;
    }



    return totRecNum;
}


//获取数据
int http_tcpclient_recvbak(int socket, char **lpbuff)
{
    int recvnum = 0;
    int count = 0;
    char tmpRec[1024] = {'\0'};
    char *new_ptr = NULL;
    int totRecNum = 0;
    while(1){
        memset(tmpRec,0,sizeof(char)*1024);
        recvnum = recv(socket, tmpRec, 1024, 0);
        totRecNum += recvnum; 
        count++;
        new_ptr = realloc(*lpbuff, 1024*count);
        if (!new_ptr) {
            assert(NULL);
        }
        *lpbuff = new_ptr;
        if(count == 1){
            memmove(*lpbuff, tmpRec, 1024);
        }else{
            strcat(*lpbuff, tmpRec);
        }    
        if(recvnum < 1024){
            break;  
        }
    }
    
    if(strstr(*lpbuff, "Transfer-Encoding: chunked")){
        if(strstr(*lpbuff, "\r\n0\r\n")){
             return totRecNum;
        }else{
            while(1){
                memset(tmpRec,0,sizeof(char)*1024);
                recvnum = recv(socket, tmpRec, 1024, 0);
                totRecNum += recvnum; 
                count++;
                new_ptr = realloc(*lpbuff, 1024*count);
                strcat(*lpbuff, tmpRec);
                if(strstr(tmpRec, "\r\n0\r\n")){
                    return totRecNum;
                }
            }
        }
    }else{
        return totRecNum;
    }
    return totRecNum;
}


//分析数据
int http_parse_content( char *pContent, key_value **pCookes, int *pCookesNum, char **pHttpContent)
{
    char *p = NULL;
    int durNum = 0;
    char *pCookesKey = NULL;
    getCooketData(&pCookesKey, pContent, "X15t_ssid=");
    if (pCookesKey != NULL)
    {
        (*pCookesNum)++;
        *pCookes = malloc(sizeof(key_value) * 3);
        (*pCookes)->key = "X15t_ssid";
        (*pCookes)->value = strdup(pCookesKey);
    }
    getCooketData(&pCookesKey, pContent, "X15t_cs_auth=");
    if (pCookesKey != NULL)
    {
        (*pCookesNum)++;
        (*pCookes + 1)->key = "X15t_cs_auth";
        (*pCookes + 1)->value = strdup(pCookesKey);
    }
    getCooketData(&pCookesKey, pContent, "X15t_mall_uid=");
    if (pCookesKey != NULL)
    {
        (*pCookesNum)++;
        (*pCookes + 2)->key = "X15t_mall_uid";
        (*pCookes + 2)->value = strdup(pCookesKey);
    }
    free(pCookesKey);
    pCookesKey = NULL;
    *pHttpContent = strdup(strstr(pContent, "\r\n\r\n") + 8);
    return 1;
}

/**
 * 获得数据
 * 
 */ 
int recv_data_base(int socket, char ** lpbuff)
{
	int recvNum = 0;
	int num = 1;
	*lpbuff = (char *)calloc(RECV_BUFF_SIZE, sizeof(char));
	while(1){
		*lpbuff = (char *)realloc(*lpbuff, num*RECV_BUFF_SIZE*sizeof(char)); 
		char buf[RECV_BUFF_SIZE] = {'\0'};
		recvNum = recv(socket, buf, RECV_BUFF_SIZE, 0);
		strcat(*lpbuff, buf);
		if(recvNum <=0 || recvNum < RECV_BUFF_SIZE ){
			break;
		}
		num++;
	}

    //chunked
	if(strstr(*lpbuff, "Transfer-Encoding: chunked") != NULL){

        int buffLen = strlen(*lpbuff);
        *lpbuff+buffLen;
        *lpbuff+buffLen-1;
        *lpbuff+buffLen-2;

        
        printf("2=%x\n", *(*lpbuff+buffLen-1));
        printf("3=%x\n", *(*lpbuff+buffLen-2));
        printf("4=%x\n", *(*lpbuff+buffLen-3));
        printf("5=%x\n", *(*lpbuff+buffLen-4));
        printf("6=%x\n", *(*lpbuff+buffLen-5));

        printf("buf=%s\n", *lpbuff);

        return 1;


        char *p_isEnd = strstr(*lpbuff, "0\r\n");
        if(p_isEnd == NULL){

            printf("is NULL\n");      
            printf("buf = %s\n", p_isEnd);          
        }else{
            printf("no NULL\n");              
            printf("buf = %s\n", p_isEnd);      
        }



        return 1;
		char *pTest = strstr(*lpbuff, "\r\n\r\n");
		pTest = pTest + strlen("\r\n\r\n");
		char *pTest2 = strstr(pTest , "\r\n");
		int testNum = pTest2 - pTest;
		char chunckContentNum[2] = {'\0'}; 
		strncpy(chunckContentNum, pTest, 2); //
		char *pContent = pTest2 + strlen("\r\n");
		// char *pB = strstr(pContent, "\r\n\r\n");

		char contentList[17] = {'\0'};
		strncpy(contentList, pContent, 16); //
		char *pEnd = strstr(pContent, "\r\n");

		if(strlen(pEnd) == 2){
			char buf1[222] = {'\0'};
			recvNum = recv(socket, buf1, 222, 0);

			// char buf2[222] = {'\0'};
			// recvNum = recv(socket, buf2, 222, 0);

			printf("rec1=%x\n", buf1[0]);
			printf("rec2=%x\n", buf1[1]);
			printf("rec3=%x\n", buf1[2]);
			printf("rec4=%x\n", buf1[3]);
			printf("rec5=%x\n", buf1[4]);
		}else{
			printf("rec1=%s\n", pContent);
		}
	}else{
		printf("no chunked\n");
	}
	return 1;
}