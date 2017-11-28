#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "http.h"
#define BUFFER_SIZE 1024

void handle_connect(varNum)
{
	char *pResult = NULL;
	char host_addr[BUFFER_SIZE] = {'\0'};
	char file[BUFFER_SIZE] = {'\0'};
	char lpbuf[BUFFER_SIZE * 4] = {'\0'};
	char lpbuf1[BUFFER_SIZE * 4] = {'\0'};
	char *pUrl = "http://wap.dev.epet.com/group/v226/detail.html?do=Login";
	char *pHttpContent = NULL;
	key_value *vals_message_type = NULL;
	int port = 0;
	int socket_fd = -1;
	int flag = -1;
	int cookesNum = 0;
	flag = http_parse_url(pUrl, host_addr, file, &port);
	socket_fd = http_tcpclient_create(host_addr, port);
	flag = http_get_send_content(&pResult, "/group/v226/detail.html?do=Login", vals_message_type, cookesNum);
	flag = http_tcpclient_send(socket_fd, pResult);
	flag = http_tcpclient_recv(socket_fd, lpbuf);
	http_parse_content(lpbuf, &vals_message_type, &cookesNum, &pHttpContent);
	flag = http_get_send_content(&pResult, "/group/v226/detail.html?do=ShowLogin", vals_message_type, cookesNum);
	flag = http_tcpclient_send(socket_fd, pResult);
	flag = http_tcpclient_recv(socket_fd, lpbuf1);
	printf("varNum=%d\n", varNum);
	close(socket_fd);
	// printf("lpbuf1=%s\n", lpbuf1);
}

int readFile(int uids[4])
{
	FILE *pFile = NULL;
	char *pChar = NULL;
	char *pStr = NULL;
	char uidList[10]; 
	int num = 0; 
	int count = 0;
	pFile = fopen("uidconfig.txt", "r");
	if(pFile == NULL){
		return -1;
	}else{
		while(fgets(uidList, sizeof(uidList), pFile) != NULL){
			pChar = strchr(uidList, ',');
			num = pChar - uidList;
			pStr = (char *)malloc(sizeof(char)*num);
			strncpy(pStr, uidList, num);
			num = atoi(pStr);
			uids[count] = num;
			free(pStr);
			pStr = NULL;
			count++;
		}
	}
	fclose(pFile);
	return 0;

}

int main()
{
	int uids[4];
	readFile(uids);

	int tmpi;
	for(tmpi = 0; tmpi < 4; tmpi++){
		printf("uids=%d\n", uids[tmpi]);			
	}
	return 0;
	int totNum = 3;
	pid_t pid[totNum];
	int i = 0;
	for (i = 0; i < totNum; i++)
	{
		pid[i] = fork();
		if (pid[i] == 0)
		{
			handle_connect(i);
		}
		else
		{
			break;
		}
	}

	return 1;
}
