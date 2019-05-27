#include"myclient.h"


int main(int argc, char **argv){

	int clientfd;
	char *host,*port,buf[4096]; 
	if(argc !=3)
	{
	       	fprintf(stderr,"usage: %s <host> <port>\n",argv[0]); exit(0);
       	} 
	host = argv[1];
	port = argv[2];


	clientfd = open_clientfd(host,port);
	
	if(clientfd == -1)
	{
		fprintf(stderr,"oops: wrong!\n");
		exit(1);
	}
	
	const char *p = "GET /hello.cgi?a=1&b=2 HTTP/1.1\r\n";
	printf("begin :\n");
	ssize_t n = write(clientfd,p,strlen(p));
	printf("strlen(p) == %d\n",(int)strlen(p));
	sleep(1);
	
	n = read(clientfd,buf,4096);
	if(strlen(buf) == 0)
		printf("nothing get!\n");
	else
		printf("%s\n",buf); 

	printf("over!\n");
	close(clientfd);
	exit(0);
}
