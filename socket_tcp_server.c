#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>

#define BUF_SIZE 100

void echo_server_func(void *);

int main ()
{
  int listener_socket_fd, service_socket_fd;
  struct sockaddr_in listener_socket_addr, service_socket_addr;
  int service_socket_addr_len;
  char buf[101];
  int len,cli_len;
  int set =1;
  pthread_t my_thread;

  listener_socket_fd                    = socket ( PF_INET, SOCK_STREAM, 0 );
  listener_socket_addr.sin_family       = AF_INET;
  listener_socket_addr.sin_port         = htons(4455);
  listener_socket_addr.sin_addr.s_addr  = INADDR_ANY;


  if(setsockopt(listener_socket_fd, SOL_SOCKET, SO_REUSEADDR, (void *) &set, sizeof(set)) < 0)
	  printf ("\nCan't set SO_REUSEADDR on main socket"); 

 if( (bind ( listener_socket_fd,
         (struct sockaddr *)&listener_socket_addr,
         sizeof(listener_socket_addr)
       ))<0)
	{
		perror("Error binding..");
	}

  printf ( "Listening for connection...\r\n" );

  listen ( listener_socket_fd, 5 );

	while(1)
	{
		
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
		printf("\nStarting thread for desc %d",service_socket_fd);
		pthread_create(&my_thread,NULL,(void*)&echo_server_func,(void *)&service_socket_fd);
	}
	
//  close ( service_socket_fd );
  close ( listener_socket_fd );

  return 0;
}

 void echo_server_func(void* data)
{
	int *addr = (int *)data;
	int service_socket_fd = *addr;
	char buf[BUF_SIZE]={};
	int len; 
	printf("\n inside thread for service socket %d ",service_socket_fd);

  for(;;){
    memset(buf,0,BUF_SIZE);
    len = read ( service_socket_fd, buf, BUF_SIZE );
    if ( len < 0){
	printf("\nServer quiting error %d",len);
	perror("Error! ");
     break;
    }
	if(len==0) break;

    buf[len] = '\0';
    if(len){
    	printf( "%s\r\n", buf );
    	write ( service_socket_fd, buf, len );
	}
  }

 printf("\nReturning from thread with desc %d!\n",service_socket_fd);
 close(service_socket_fd);
 pthread_exit(0);

}	
