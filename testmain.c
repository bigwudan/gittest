#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"
#define BUFFER_SIZE 1024

int main()
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
	printf("lpbuf1=%s\n", lpbuf1);

	return 1;
}
