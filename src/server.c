#include<stdio.h>

#include"myserver.h"
#include"threadpool.h"

CThread_pool *pool = NULL;
int main(){
	pool_init(3);//create thread pool


	int server_sock = -1;
	int client_sock = -1;
	char *port="4000";
	struct sockaddr_in client_name;
	socklen_t client_name_len = sizeof(client_name);
	pthread_t newthread;	
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,
			PTHREAD_CREATE_DETACHED);


	server_sock = open_listenfd(port);
      	printf("http are running on port %s\n",port);

	int n = 4;
	while(n--){
		client_sock = accept(server_sock,(struct sockaddr*)&client_name,&client_name_len);
		if(client_sock==-1)
			perror("accept");
	
		pool_add_worker(accept_request,(void *)(intptr_t)client_sock);
	}
	pool_destroy();
	close(server_sock);	
	pthread_attr_destroy(&attr);
	return 0;
}

