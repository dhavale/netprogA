#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>

int main ()
{
  int service_socket_fd;
  struct sockaddr_in service_socket_addr;
  char buf[101];
  int len;

  service_socket_fd                   = socket ( PF_INET, SOCK_STREAM, 0 );
  service_socket_addr.sin_family      = AF_INET;
  service_socket_addr.sin_port        = htons(4455);
  service_socket_addr.sin_addr.s_addr = 0x0100007f;

  printf ( "Connecting...\r\n" );

  if ( connect ( service_socket_fd,
                 (struct sockaddr *)&service_socket_addr,
                 sizeof(service_socket_addr)
               )
       == -1 ){
   printf ( "Unable to connect\r\n" );
   return 0;
  }

  printf ( "Connected to server\r\n" );

  for(;;){
    fgets(buf,100,stdin);
    if ( ! write ( service_socket_fd, buf, 1 + strlen(buf) ) ){
     break;
    }
    len = read ( service_socket_fd, buf, 100 );
    if ( len == 0 ){
     break;
    }
    buf[len] = '\0';
    printf ( "%s\r\n", buf );
  }

  close ( service_socket_fd );

  return 0;
}
