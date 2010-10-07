#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include "np.h"

int main (int argc, char * argv[])
{

  int service_socket_fd,pipe_fd,ret;
  struct sockaddr_in service_socket_addr;
  char *buf= malloc(MAXLEN);
  char msg[MAXLEN] = {};
  int len;
  //struct in_addr ip_addr;

  assert(argc==3);

 // ip_addr = basic_checks(argv[1]);
 
 
  	printf("connecting to: %s",argv[1]);
   
  if(argc==3){
	sscanf(argv[2],"%d",&pipe_fd);
	}
  else pipe_fd= fileno(stdout);


  service_socket_fd                   = socket ( AF_INET, SOCK_STREAM, 0 );
  service_socket_addr.sin_family      = AF_INET;
  service_socket_addr.sin_port        = htons(4455);
  //memcpy(&service_socket_addr.sin_addr,&ip_addr,sizeof(service_socket_addr.sin_addr));
// service_socket_addr.sin_addr = ip_addr;
  	if(inet_pton(AF_INET,argv[1],&service_socket_addr.sin_addr)!=1)
	{
		sprintf(msg,"\ncannot convert string addr to binary.. quitting");
		write(pipe_fd,msg,strlen(msg));
		return;
	}

#ifdef DEBUG
	printf("service_socket_fd : %d\n",service_socket_fd);
	
#endif
  fflush(stdout);

//  write (pipe_fd, "\n\n\n\nConnecting...\n" );
#ifdef DEBUG
printf("\nbefore connect\n");
#endif
  if ( connect ( service_socket_fd,
                 (struct sockaddr *)&service_socket_addr,
                 sizeof(service_socket_addr)
               )
      != 0 ){
   perror("Unable to connect!! \n");
   sprintf (msg,"Unable to connect %d\r\n",errno );
   write(pipe_fd,msg,strlen(msg));
   return 0;
  }

#ifdef DEBUG
printf("\nafter connect\n");
#endif

  sprintf(msg,"Connected to server");
  write ( pipe_fd,msg,strlen(msg));
 // write (pipe_fd,"\n test message..");


	printf("\nconnected to server\n");

	printf("\nStart typing!! :\n");
  for(;;){
    ret = read(fileno(stdin),buf,MAXLEN);
	if(ret==0)
	{
		sprintf(msg,"\nexiting from echo_cli\n");
		ret = write(pipe_fd,msg,strlen(msg));
		 break;
	}
    if (  write ( service_socket_fd, buf,ret)<= 0 ){
	
//	write(pipe_fd,"\n1.Server closed down..quitting");

	sprintf(msg,"\n1.Server closed down..quitting\n");
	
   	write(pipe_fd,msg,strlen(msg));
    	 break;
    }
    len = read ( service_socket_fd, buf, ret );
    if ( len == 0 ){
//	write(pipe_fd,"\n2.Server closed down..quitting");

	sprintf(msg,"\n1.Server closed down..quitting\n");

   	write(pipe_fd,msg,strlen(msg));
    	break;
    }
    buf[len] = '\0';
    printf ( "%s\r\n", buf );
  }

  close ( service_socket_fd );
  close(pipe_fd);

  return 0;
}
