#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>

int main (int argc, char * argv[])
{
  int service_socket_fd,pipe_fd;
  struct sockaddr_in service_socket_addr;
  char *buf= malloc(101);
  int len;

  if(argc==2)
	pipe_fd =atoi(argv[1]);
else pipe_fd= fileno(stdout);


  service_socket_fd                   = socket ( PF_INET, SOCK_STREAM, 0 );
  service_socket_addr.sin_family      = AF_INET;
  service_socket_addr.sin_port        = htons(4455);
  service_socket_addr.sin_addr.s_addr = 0x0100007f;

  fflush(stdout);

  write (pipe_fd, "\n\n\n\nConnecting...\n" );

  if ( connect ( service_socket_fd,
                 (struct sockaddr *)&service_socket_addr,
                 sizeof(service_socket_addr)
               )
       == -1 ){
   write (pipe_fd, "Unable to connect\r\n" );
   return 0;
  }

  write ( pipe_fd,"Connected to server\n" );
  write (pipe_fd,"\n test message..");

  for(;;){
    buf = fgets(buf,100,stdin);
	if(buf==NULL) break;
    if (  write ( service_socket_fd, buf, 1 + strlen(buf) )<= 0 ){
	
	write(pipe_fd,"\n1.Server closed down..quitting");
     break;
    }
    len = read ( service_socket_fd, buf, 100 );
    if ( len == 0 ){
	write(pipe_fd,"\n2.Server closed down..quitting");
     break;
    }
    buf[len] = '\0';
    printf ( "%s\r\n", buf );
  }

  close ( service_socket_fd );

  return 0;
}
