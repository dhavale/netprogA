#ifndef _NP_H
#define _NP_H

#include <netdb.h>
#define MAX(a,b) (a > b? a :b )
#define MAXLEN 400



struct in_addr basic_checks(const char * str_addr)
{
struct hostent *hp;

int ret;
struct in_addr addr_byte;

	ret = inet_pton(AF_INET,str_addr,&addr_byte);

	if(ret!=1)
	{
		hp = gethostbyname(str_addr);
    	if (hp == NULL) {
       	printf("gethostbyname() failed\n");
    	} else {
       	printf("%s = ", hp->h_name);
       	unsigned int i=0;
       	while ( hp -> h_addr_list[i] != NULL) {
        	  printf( "%s ", (char *)inet_ntoa( *( struct in_addr*)( hp -> h_addr_list[i])));
          	i++;
		}

			printf("\n");
	//addr_byte = 0;
	//memcpy(&addr_byte,hp->h_addr_list[0],sizeof(addr_byte));
	addr_byte = *(struct in_addr *) hp->h_addr_list[0];	
	}

	}

	else {
	hp = gethostbyaddr( (char *)&addr_byte,
                    sizeof(addr_byte),AF_INET);
         printf("\n(%s Name is : %s)\n",str_addr, hp->h_name);
	}
	printf("\nreturning from %s with %x",__FUNCTION__,addr_byte);	
	return addr_byte;
}
#endif
