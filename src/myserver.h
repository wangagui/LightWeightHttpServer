#ifndef MY_SERVER_H
#define MY_SERVER_H
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<ctype.h>
#include<sys/stat.h>
#include<pthread.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<assert.h>
#define ISspace(x) isspace((int)(x))
#define MAXLINE 256

#define SERVER_STRING "Server: waghttpd/0.1.0\r\n"
#define STDIN 0
#define STDOUT 1
#define STDERR 2 

extern pthread_mutex_t mutex;
extern int num;
int open_listenfd(char *port);
void *accept_request(void *arg);
int get_line(int,char *,int);
void unaccept(int);
void not_found(int);
void serve_file(int ,const char*);
void execute_cgi(int,const char *,const char *,const char *);
void cat(int ,FILE *);
void headers(int ,const char *);
void bad_request(int);
void cannot_execute(int);
#endif
