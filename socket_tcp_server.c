#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include "np.h"

void echo_server_func(void *);
void time_server_func(void *);

int main ()
{
  int listener_socket_fd, service_socket_fd,time_service_fd,time_socket_fd;
  struct sockaddr_in listener_socket_addr, service_socket_addr;
  struct sockaddr_in time_socket_addr,time_service_addr;
  int service_socket_addr_len;
  int time_service_addr_len;
  char buf[MAXLEN];
  int len,cli_len,maxfdp;
  int set =1;
  pthread_t my_thread;
  fd_set rset;

  listener_socket_fd                    = socket ( PF_INET, SOCK_STREAM, 0 );
  listener_socket_addr.sin_family       = AF_INET;
  listener_socket_addr.sin_port         = htons(4455);
  listener_socket_addr.sin_addr.s_addr  = htonl(INADDR_ANY);


  if(setsockopt(listener_socket_fd, SOL_SOCKET, SO_REUSEADDR, (void *) &set, sizeof(set)) < 0)
	  printf ("\nCan't set SO_REUSEADDR on main socket"); 

 if( (bind ( listener_socket_fd,
         (struct sockaddr *)&listener_socket_addr,
         sizeof(listener_socket_addr)
       ))<0)
	{
		perror("Error binding..");
	}

  printf ( "Listening for echo connection...\r\n" );

/******time service setup**********/
  time_socket_fd                    = socket ( PF_INET, SOCK_STREAM, 0 );
  time_socket_addr.sin_family       = AF_INET;
  time_socket_addr.sin_port         = htons(4457);
  time_socket_addr.sin_addr.s_addr  = htonl(INADDR_ANY);


  if(setsockopt(time_socket_fd, SOL_SOCKET, SO_REUSEADDR, (void *) &set, sizeof(set)) < 0)
	  printf ("\nCan't set SO_REUSEADDR on main socket"); 

 if( (bind ( time_socket_fd,
         (struct sockaddr *)&time_socket_addr,
         sizeof(time_socket_addr)
       ))<0)
	{
		perror("Error binding..");
	}

  printf ( "Listening for time service connection...\r\n" );

/**time service setup done**/
  listen ( listener_socket_fd, 5 );
  listen ( time_socket_fd,5 );
	FD_ZERO(&rset);
	while(1)
	{
		FD_SET(listener_socket_fd, &rset);
		FD_SET(time_socket_fd, &rset);
		maxfdp = MAX(listener_socket_fd,time_socket_fd) + 1;
		select(maxfdp, &rset, NULL, NULL, NULL);
			
		if(FD_ISSET(listener_socket_fd,&rset))
		{
			/*echo accept*/
			cli_len = sizeof(service_socket_addr);
 	  		memset(&service_socket_addr,0,sizeof(service_socket_addr));
 	  		service_socket_fd = accept ( listener_socket_fd,
 	       	                	       (struct sockaddr *)&service_socket_addr,
 	       		                       &cli_len
 	       	        	             );
	
			  if(service_socket_fd < 0){
				printf("\nAccept returned %d", service_socket_fd);
				perror("\nError accepting" );
				return -1;
				}
				printf("\nStarting echo thread for desc %d",service_socket_fd);
				pthread_create(&my_thread,NULL,(void*)&echo_server_func,(void *)&service_socket_fd);
		}
		
		if(FD_ISSET(time_socket_fd,&rset))
		{
			/*time accept*/
			cli_len = sizeof(service_socket_addr);
 	  		memset(&time_service_addr,0,sizeof(time_service_addr));
 	  		time_service_fd = accept ( time_socket_fd,
 	       	                	       (struct sockaddr *)&time_service_addr,
 	       		                       &cli_len
 	       	        	             );
	
			  if(time_service_fd < 0){
				printf("\nAccept returned %d", time_service_fd);
				perror("\nError accepting" );
				return -1;
				}
				printf("\nStarting time thread for desc %d",time_service_fd);
				pthread_create(&my_thread,NULL,(void*)&time_server_func,(void *)&time_service_fd);
		}
 	  	
	}
	
//  close ( service_socket_fd );
  close ( listener_socket_fd );
  close ( time_socket_fd );
  return 0;
}

 void echo_server_func(void* data)
{
	int *addr = (int *)data;
	int service_socket_fd = *addr;
	char buf[MAXLEN]={};
	int len; 
	printf("\n echo thread for service socket %d ",service_socket_fd);

  for(;;){
    memset(buf,0,MAXLEN);
    len = read ( service_socket_fd, buf, MAXLEN );
    if ( len < 0){
	printf("\nServer quiting error %d",len);
	perror("Error! ");
     break;
    }
	if(len==0) break;

    buf[len] = '\0';
    if(len){
#ifdef DEBUG
    	printf( "%s\r\n", buf );
#endif
    	write ( service_socket_fd, buf, len );
	}
  }

 printf("\nClosing echo client %d!\n",service_socket_fd);
 close(service_socket_fd);
 pthread_exit(0);

}	

void time_server_func(void *data)
{
	int sockfd = *(int *)data;
	printf("time thread Connected to client %d\n",sockfd);
	time_t ticks;
	char    buf[MAXLEN];
	struct timeval tv;
	fd_set		rset;
	FD_ZERO(&rset);
			for ( ; ; ) {
				FD_SET(sockfd, &rset);
				tv.tv_sec = 5;
				tv.tv_usec = 0;
				select(sockfd + 1, &rset, NULL, NULL, &tv);
				if (FD_ISSET(sockfd, &rset)) {
					printf("[TIME SER]:Client terminated.\n");
					break;
				}
				ticks = time(NULL);
				snprintf(buf, sizeof(buf), "%.24s\r\n", (char *)ctime(&ticks));
				write(sockfd, buf, strlen(buf));
			}

	printf("\nClosing time client %d!\n",sockfd);
	close(sockfd);
	pthread_exit(0);

}
