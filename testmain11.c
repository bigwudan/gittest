#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "http.h"
#define BUFFER_SIZE 1024
#define UID_NUM 1


char *pToken[] = {
	"20801f6500f52e9c95155a2086ff7cf1",
	"eb1956cbcd134ddbf5722e71e5cce1b7",
	"944f30431428d0fdcc00294102ec2683",
	
};




int handle_connect(int varNum, char *token)
{
	char *pResult = NULL;
	char host_addr[BUFFER_SIZE] = {'\0'};
	char file[BUFFER_SIZE] = {'\0'};
	char lpbuf[BUFFER_SIZE * 4] = {'\0'};
	char lpbuf1[BUFFER_SIZE * 4] = {'\0'};
	char lpbuf2[BUFFER_SIZE * 4] = {'\0'};
	char lpbuf3[BUFFER_SIZE * 4] = {'\0'};
	char *pUrl = "http://wap.dev.epet.com/group/v226/detail.html?do=Login";
	char *pHttpContent = NULL;
	key_value *vals_message_type = NULL;
	int port = 0;
	int socket_fd = -1;
	int flag = -1;
	int cookesNum = 0;
	char *urlTemplet = "http://wap.dev.epet.com/group/v226/detail.html?do=Login&uid=";
	char numCtr[10] = {'\0'};
	sprintf(numCtr, "%d", varNum);
	char *urlStr = NULL;
	urlStr = (char *)calloc((strlen(urlTemplet)+strlen(numCtr)), sizeof(char));
	strcat(urlStr, urlTemplet);
	strcat(urlStr, numCtr);
	flag = http_parse_url(pUrl, host_addr, file, &port);
	socket_fd = http_tcpclient_create(host_addr, port);
	flag = http_get_send_content(&pResult, urlStr, vals_message_type, cookesNum);
	flag = http_tcpclient_send(socket_fd, pResult);
	free(pResult);
	pResult = NULL;	

	char *testBuf = NULL;

	flag = recv_data_base(socket_fd, &testBuf);

	
	free(testBuf);
	testBuf = NULL;
	// printf("lpbuf=%s\n", testBuf);

	return 1;


	//加入redies
	http_parse_content(lpbuf, &vals_message_type, &cookesNum, &pHttpContent);
	flag = http_get_send_content(&pResult, "http://wap.dev.epet.com/group/v226/detail.html?do=CheckState&tid=1611&token=20801f6500f52e9c95155a2086ff7cf1", vals_message_type, cookesNum);
	flag = http_tcpclient_send(socket_fd, pResult);
	free(pResult);
	pResult = NULL;
	flag = http_tcpclient_recv(socket_fd, lpbuf1);

	//生成报名记录
	flag = http_get_send_content(&pResult, "http://wap.dev.epet.com/group/v226/detail.html?do=Pay&adid=1&tid=1611&token=20801f6500f52e9c95155a2086ff7cf1", vals_message_type, cookesNum);
	flag = http_tcpclient_send(socket_fd, pResult);
	free(pResult);
	pResult = NULL;	
	flag = http_tcpclient_recv(socket_fd, lpbuf2);

	//支付成功
	flag = http_get_send_content(&pResult, "http://wap.dev.epet.com/group/v226/detail.html?do=TestPaySuccess&tid=1611", vals_message_type, cookesNum);
	flag = http_tcpclient_send(socket_fd, pResult);
	free(pResult);
	pResult = NULL;	
	flag = http_tcpclient_recv(socket_fd, lpbuf3);

	// printf("lpbuf3=%s\n", lpbuf3);
	close(socket_fd);
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
	char *p = "abcde";
	int num = strlen(p);
	printf("num1=%x\n", *(p+5));
	printf("num2=%x\n", *(p+4));
	printf("num3=%x\n", *(p+3));
	return 1;
	int testNum = 0;
	int uids[UID_NUM];
	readFile(uids);
	pid_t pid[UID_NUM];
	char tokenStr[32];
	int tokenNum = sizeof(pToken)/sizeof(pToken[0]);
	int is_token = 1;
	int i = 0;
	handle_connect(1, NULL);
	return 1;
	for (i = 0; i < UID_NUM; i++)
	{
		pid[i] = fork();
		if (pid[i] == 0)
		{
			if(is_token == 1){
				handle_connect(uids[i], pToken[rand()%tokenNum]);
			}else{
				handle_connect(uids[i], NULL);	
			}
			//handle_connect(uids[i]);
		}
		else
		{
			break;
		}
	}
	return 1;
}
