#include"myserver.h"
#include"threadpool.h"
int open_listenfd(char *port){
	struct addrinfo hints,*listp,*p;
	int listenfd,optval = 1;

	// get a list of potential server address, this is important
	memset(&hints,0,sizeof(struct addrinfo));

	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; // any IP address is ok
	hints.ai_flags |= AI_NUMERICSERV;
	getaddrinfo(NULL,port,&hints,&listp);

	for(p = listp;p;p = p->ai_next){
		listenfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol);
		if(listenfd<0)
			continue; // socket failed ,try the next

		// eliminate address already in use form bind
		setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(const void *)&optval,sizeof(int));
		
		//bind the descriptor to the address
		if(bind(listenfd,p->ai_addr,p->ai_addrlen) == 0)
			break;

		close(listenfd);

	}

	// clean up
	freeaddrinfo(listp);
	if(!p)
		return -1;  // all failed


	if(listen(listenfd,1024)<0){
		close(listenfd);
		return -1;
	}

	return listenfd;

	
}


int get_line(int sock,char *buf,int size){
	int i=0;
	char c = '\0';
	int n;

	while((i<size-1)&&(c !='\n'))
	{
		n = recv(sock,&c,1,0);

		if(n>0)
		{	
			if(c == '\r')
			{
				n = recv(sock,&c,1,MSG_PEEK);//flags MSG_PEEK means we just have a look and do not really get
				if((n>0)&&(c=='\n'))
					recv(sock,&c,1,0);
				else
					c = '\n';
			}
			buf[i] = c;
			i++;
		}else
			c = '\n';
	}

	buf[i] = '\0';

	return (i);
}


void *accept_request(void *arg){

	int client = (intptr_t)arg;
	char buf[1024];

	size_t numchars;
	char method[255];
	char url[255];
	char path[512];
	size_t i,j;
	struct stat st;
	int cgi = 0;   // if cgi == 1 ,means server decides this is a CGI program

	char *query_string = NULL;

	numchars = get_line(client,buf,sizeof(buf));
	printf("we get %s\n",buf);
	i = 0;j=0;

	while(!ISspace(buf[i]) && (i<sizeof(method)-1))
	{
		method[i] = buf[i];
		i++;
	}

	j = i;
	method[i] = '\0';
	i = 0;
	// assert the method
	if(strcasecmp(method,"GET") && strcasecmp(method,"POST"))
	{
		unaccept(client); // http method not accepted!
		return;
	}
	if(strcasecmp(method,"POST") == 0)
		cgi = 1;
	
	
	while(ISspace(buf[j]) && (j<numchars))
		j++;

	while(!ISspace(buf[j]) && (i<sizeof(url) -1) && (j<numchars))
	{
		url[i] = buf[j];
		i++;j++;
	}
	url[i] = '\0';
	
	if(strcasecmp(method,"GET") == 0)
	{
		query_string = url;
		while((*query_string !='?') && (*query_string !='\0'))
			query_string ++;
		if(*query_string == '?')
		{
			cgi = 1;
			*query_string = '\0';
			query_string ++;
		}
	}	
	// we add file path to path
	sprintf(path,"htdocs%s",url);

	if(path[strlen(path)-1] == '/')
		strcat(path,"index.html");
	
	if(stat(path,&st) == -1){
		while((numchars>0) && strcmp("\n",buf))
			numchars = get_line(client,buf,sizeof(buf));// continue read,clear the buffer,and respond not found
		not_found(client);
	}else{

		if((st.st_mode & S_IFMT) == S_IFDIR)
			strcat(path,"/index.html");

		if((st.st_mode & S_IXUSR) ||(st.st_mode & S_IXGRP)||(st.st_mode & S_IXOTH) ){

			cgi = 1;
		}
	
		if(!cgi){
			printf("Thread :%x,start execute CGI program\n",pthread_self());
			serve_file(client,path);
			printf("execute CGI program over!\n");
		}

		else{
			printf("Thread :%x,start serve page\n",pthread_self());
			execute_cgi(client,path,method,query_string);
			printf("serve page over!\n");
	
		}
	}
	close(client);
	return NULL;
}

void unaccept(int client){
	char buf[1024];
	sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</TITLE></HEAD>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}


void not_found(int client){
	char buf[1024];
	sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "your request because the resource specified\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "is unavailable or nonexistent.\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}

void execute_cgi(int client,const char *path,const char *method,const char *query_string){
	char buf[1024];
	// pipe for IPC bettwen father and child
	int cgi_output[2];
	int cgi_input[2];
	pid_t pid;
	int status;
	int i;
	char c;
	int numchars = 1;
	int content_length = -1;

	if(strcasecmp(method,"GET") == 0)
		while((numchars>0) && !strcmp("\n",buf))
			numchars = get_line(client,buf,sizeof(buf));
	else if(strcasecmp(method,"POST") == 0)
	{
		numchars = get_line(client,buf,sizeof(buf));
		while((numchars >0)&&!strcmp("\n",buf))
		{
			buf[15] = '\0';
			if(strcasecmp(buf,"Content-Length:")==0)
				content_length = atoi(&(buf[16]));
			numchars = get_line(client,buf,sizeof(buf));
		}

		if(content_length == -1){
			bad_request(client);
			return;
		}
	}

	if(pipe(cgi_output)<0){
		cannot_execute(client);
		return;
	}
	if(pipe(cgi_input)<0){
		cannot_execute(client);
		return;
	}

	sprintf(buf,"HTTP/1.0 200 OK\r\n");
	send(client ,buf,strlen(buf),0);


	if((pid =fork())<0){
		cannot_execute(client);
		return;
	}

	if(pid==0)
	{
		dup2(cgi_output[1],STDOUT);
		dup2(cgi_input[0],STDIN);
		close(cgi_output[0]);
		close(cgi_input[1]);

		execl(path,NULL);
		exit(0);
	}
	else
	{
		close(cgi_output[1]);
		close(cgi_input[0]);
		if(strcasecmp(method,"POST") == 0)
			for(i = 0;i<content_length;i++){
				recv(client,&c,1,0);
				write(cgi_input[1],&c,1);
			}
		while(read(cgi_output[0],&c,1)>0)
			send(client,&c,1,0);

		close(cgi_output[0]);
		close(cgi_input[1]);
		waitpid(pid,&status,0);
	}
}

void serve_file(int client,const char *filename)
{
	FILE *resource = NULL;
	char buf[1024];
	int numchars = 1;

	//clear the rec-buffer
	while((numchars>0)&&!strcmp("\n",buf))
		numchars = get_line(client,buf,sizeof(buf));
	
	resource = fopen(filename,"r");
	
	if(resource == NULL){
		printf("not found here\n");
		not_found(client);
		
	}
	else{
		printf("header \n");
		headers(client,filename); // send header
		cat(client,resource);  // send data
	}
	
	fclose(resource);

}

void cat(int client,FILE *resource)
{

	char buf[1024];
	
	fgets(buf,sizeof(buf),resource);
	while(!feof(resource)){
		send(client,buf,strlen(buf),0);
		fgets(buf,sizeof(buf),resource);
	}
}

void headers(int client,const char *filename)
{
	char buf[1024];
	
	(void)filename;  // use filename to determine file type
	
	strcpy(buf, "HTTP/1.0 200 OK\r\n");
	send(client, buf, strlen(buf), 0);
	//strcpy(buf, SERVER_STRING);
	//send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n");
	send(client, buf, strlen(buf), 0);
	strcpy(buf, "\r\n");
	send(client, buf, strlen(buf), 0);

}

void bad_request(int client)
{
	char buf[1024];

	sprintf(buf,"Bad Request!\r\n");
	send(client,buf,sizeof(buf),0);
}

void cannot_execute(int client)
{
	char buf[1024];

	sprintf(buf,"CGI program cannot execute!\r\n");
	send(client,buf,strlen(buf),0);

}
