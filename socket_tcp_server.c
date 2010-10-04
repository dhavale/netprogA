#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>

#define BUF_SIZE 100
int main ()
{
  int listener_socket_fd, service_socket_fd;
  struct sockaddr_in listener_socket_addr, service_socket_addr;
  int service_socket_addr_len;
  char buf[101];
  int len,cli_len;
  int set =1;

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

  printf ( "Client is connected\r\n" );

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

  close ( service_socket_fd );
  close ( listener_socket_fd );

  return 0;
}
