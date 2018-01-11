#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include "http.h"
#define BUFFER_SIZE 1024
#define UID_NUM 5

int handle_connect(int varNum)
{
	char *pResult = NULL;
	char url[512] = {'\0'};
	char urlTest[512] = {'\0'};
	char host_addr[BUFFER_SIZE] = {'\0'};
	char file[BUFFER_SIZE] = {'\0'};
	char *lpbuf = calloc(1024, sizeof(char));

	char *lpbuf1 = calloc(1024, sizeof(char));

	char *pHttpContent = NULL;


	int port = 0;
	int flag = 0;
	int socket_fd = -1;
	int cookesNum = 0;



	key_value *vals_message_type = NULL;
	strcpy(url, "http://wap.sx.epet.com/group/v225/main.html");
	flag = http_parse_url(url, host_addr, file, &port);
	socket_fd = http_tcpclient_create(host_addr, port);
	sprintf(urlTest, "http://wap.sx.epet.com/group/v225/main.html?do=Login&uid=%d", varNum);
	flag = http_get_send_content(&pResult, urlTest, vals_message_type, cookesNum);
	flag = http_tcpclient_send(socket_fd, pResult);
	free(pResult);
	flag = http_tcpclient_recv(socket_fd, &lpbuf);
	http_parse_content(lpbuf, &vals_message_type, &cookesNum, &pHttpContent);
	free(lpbuf);

	flag = http_get_send_content(&pResult, "http://wap.sx.epet.com/group/v225/main.html?do=TestUid", vals_message_type, cookesNum);
	flag = http_tcpclient_send(socket_fd, pResult);
	
	flag = http_tcpclient_recv(socket_fd, &lpbuf1);
	

	printf("lpbuf1=%s\n", lpbuf1);
	
	
}


int main()
{
	handle_connect(11);
	return 1;
	pid_t pid[UID_NUM];
	int i = 0;
	for (i = 0; i < UID_NUM; i++)
	{
		pid[i] = fork();
		if (pid[i] == 0)
		{
			handle_connect(1);
		}
		else{
			handle_connect(1);
		}
		
	}
	return 1;

}
