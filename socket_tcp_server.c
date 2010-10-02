#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>

int main ()
{
  int listener_socket_fd, service_socket_fd;
  struct sockaddr_in listener_socket_addr, service_socket_addr;
  int service_socket_addr_len;
  char buf[101];
  int len;

  listener_socket_fd                    = socket ( PF_INET, SOCK_STREAM, 0 );
  listener_socket_addr.sin_family       = AF_INET;
  listener_socket_addr.sin_port         = 10000;
  listener_socket_addr.sin_addr.s_addr  = INADDR_ANY;

  bind ( listener_socket_fd,
         (struct sockaddr *)&listener_socket_addr,
         sizeof(listener_socket_addr)
       );

  printf ( "Listening for connection...\r\n" );

  listen ( listener_socket_fd, 1 );

  service_socket_fd = accept ( listener_socket_fd,
                               (struct sockaddr *)&service_socket_addr,
                               &service_socket_addr_len
                             );

  printf ( "Client is connected\r\n" );

  for(;;){
    len = read ( service_socket_fd, buf, 100 );
    if ( ( len == 0 ) || ( ! strcmp ( buf, "bye" ) ) ){
     break;
    }
    buf[len] = '\0';
    printf( "%s\r\n", buf );
    write ( service_socket_fd, buf, len );
  }

  close ( service_socket_fd );
  close ( listener_socket_fd );

  return 0;
}
