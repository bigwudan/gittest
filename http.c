#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
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
    free(buff);
    buff = NULL;
    return sent;
}

//获取数据
int http_tcpclient_recv(int socket, char *lpbuff)
{
    int recvnum = 0;
    recvnum = recv(socket, lpbuff, 1024 * 4, 0);
    return recvnum;
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
