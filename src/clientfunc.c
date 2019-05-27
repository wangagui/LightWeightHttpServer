#include"myclient.h"
int open_clientfd(char *hostname,char *port){
	int clientfd;

	struct addrinfo hints,*listp,*p;

	//get a list of potential server address
	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;// open a connection
	hints.ai_flags = AI_NUMERICSERV; // using a numeric port
	hints.ai_flags |= AI_ADDRCONFIG; // recommended for connections
	getaddrinfo(hostname,port,&hints,&listp);// we get listp which is a list of potential server address

	// we chose one server address from listp that we can successfully connect to
	
	for(p = listp;p;p = p->ai_next){
		clientfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol);
		if(clientfd <0)
			continue;  // socket failed,try the next

		// connect to the server
		if(connect(clientfd,p->ai_addr,p->ai_addrlen)!=-1)
			break;  // connect success

		close(clientfd);  // connect failed ,try another
	}

	//Clean up
	freeaddrinfo(listp);
	if(!p)
		return -1;  //means p at NULL, all connect failed
	else
		return clientfd;

}
