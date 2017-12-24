#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include "http.h"
#define BUFFER_SIZE 1024
#define UID_NUM 5000

int handle_connect(int varNum)
{
	char *pResult = NULL;
	char host_addr[BUFFER_SIZE] = {'\0'};
	char file[BUFFER_SIZE] = {'\0'};
	// char lpbuf[BUFFER_SIZE * 4] = {'\0'};

	char *lpbuf = calloc(1024, sizeof(char));

	char lpbuf1[BUFFER_SIZE * 4] = {'\0'};
	char lpbuf2[BUFFER_SIZE * 4] = {'\0'};
	char lpbuf3[BUFFER_SIZE * 4] = {'\0'};
	//char *pUrl = "http://wap.dev.epet.com/group/v226/detail.html?do=Login";
	char *pUrl = "http://wap.dev.epet.com/main.html";


	char *pHttpContent = NULL;
	key_value *vals_message_type = NULL;
	int port = 0;
	int socket_fd = -1;
	int flag = -1;
	int cookesNum = 0;
	//char *urlTemplet = "http://wap.dev.epet.com/group/v226/detail.html?do=Login&uid=";

	char *urlTemplet = "http://wap.dev.epet.com/main.html";
	char numCtr[10] = {'\0'};
	sprintf(numCtr, "%d", varNum);
	char *urlStr = NULL;
	urlStr = (char *)calloc((strlen(urlTemplet)), sizeof(char));
	strcat(urlStr, urlTemplet);
	flag = http_parse_url(pUrl, host_addr, file, &port);
	socket_fd = http_tcpclient_create(host_addr, port);
	flag = http_get_send_content(&pResult, urlStr, vals_message_type, cookesNum);


	flag = http_tcpclient_send(socket_fd, pResult);
	free(pResult);
	pResult = NULL;	
	flag = http_tcpclient_recv(socket_fd, &lpbuf);
	printf("lbuff=%s\n", lpbuf);
	free(lpbuf);
	lpbuf = NULL;
	close(socket_fd);
	
	return 1;
}

int readFile(int uids[UID_NUM])
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
	handle_connect(1);
	return 1;
	int uids[UID_NUM];
	readFile(uids);

	pid_t pid[UID_NUM];
	int i = 0;
	for (i = 0; i < UID_NUM; i++)
	{
		pid[i] = fork();
		if (pid[i] == 0)
		{
			handle_connect(uids[i]);
		}
		else
		{
			break;
		}
	}
	return 1;
}
