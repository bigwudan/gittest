#ifndef _HTTP_H  
#define _HTTP_H
#define HTTP_GET_HEAD "GET %s HTTP/1.1\r\n"\
	"Host: wap.dev.epet.com\r\n"\
    "Connection: keep-alive\r\n"\
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"\
    "Upgrade-Insecure-Requests: 1\r\n"\
    "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/48.0.2564.97 Safari/537.36\r\n"\
    "Accept-Language: zh-CN,zh;q=0.8\r\n"

#define RECV_BUFF_SIZE 1024

typedef struct {  
    char *key;  
    char *value;  
} key_value;  
extern int http_get_send_content(char **pResult, const char *url, key_value *cookes, int cookesNum); 
extern int http_parse_url(const char *url,char *host,char *file,int *port);
extern int http_tcpclient_create(const char *host, int port);
extern int http_tcpclient_send(int socket, char *buff);
extern int http_tcpclient_recv(int socket,char **lpbuff);
extern int http_parse_content(char *pContent, key_value **pCookes, int *pCookesNum, char **pHttpContent);

extern int recv_data_base(int socket, char ** lpbuff);

#endif  