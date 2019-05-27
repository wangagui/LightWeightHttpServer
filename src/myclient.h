#ifndef TEST_H
#define TEST_H
#include<string.h>
#include<stdio.h>
#include<netdb.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

#define MAXLINE 256
int open_clientfd(char *hostname,char *port);


#endif
