#include <sys/wait.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include "np.h"

void sigch_handler(int signo)
{
    pid_t   pid;
    int     stat;
    while((pid = waitpid(-1, &stat, WNOHANG)) > 0){
       printf("Child %d terminated\n", pid);
    }
    return;	
}


int main(int argc, char *argv[])
{
    int pipe_fd[2],ret;
    pid_t cpid;
    char pipenum[10]={},choice[25]={};
    fd_set rset;
    int maxfdp;	
    char msg[MAXLEN]={};
    char str_addr[INET_ADDRSTRLEN]={};
    struct hostent *hp;
    unsigned int ip_addr;
    FD_ZERO(&rset);

	
    assert(argc==2);
//	basic_checks(argv[1]);
	
    if (pipe(pipe_fd) == -1) { 
	perror("pipe"); exit(EXIT_FAILURE);
    }

	signal(SIGCHLD,sigch_handler);

	    ret = inet_pton(AF_INET,argv[1],&ip_addr);
    
	
	if(ret==1) {
	/**user gave ip address and not host name**/
		hp = gethostbyaddr( (char *)&ip_addr,
                	    sizeof(ip_addr),AF_INET);
         	printf("\n(%s Name is : %s)\n",argv[1], hp->h_name);
		sprintf(str_addr,"%s",argv[1]);
	}
	if(ret!=1)
	{
		hp = gethostbyname(argv[1]);
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
		inet_ntop (hp->h_addrtype, hp->h_addr_list[0], str_addr, sizeof (str_addr));
	//addr_byte = 0;
	//memcpy(&addr_byte,hp->h_addr_list[0],sizeof(addr_byte));
	//addr_byte = *(struct in_addr *) hp->h_addr_list[0];	
	}

	}

	printf("\nclient will now connect to %s\n",str_addr);	
	
while(1){
/********************/
	memset(choice,0,sizeof(choice));
	printf("Name the servcie(echo,time,quit): ");
	ret =scanf("%s",choice);
	assert(ret==1);
#ifdef DEBUG
	printf("\nyour choice: %s",choice); 
#endif
	if(!strcmp(choice,"echo"))
	{
		/*fork echo child*/
		   cpid = fork();

	    if (cpid == -1) {
		 perror("fork"); exit(EXIT_FAILURE);
    		}

    		if (cpid == 0) {    /* Child writes into pipe */
		/* Child */ 
        	close(pipe_fd[0]);          /* Close unused read end */
		sprintf(pipenum,"%d",pipe_fd[1]);
			if ( (execlp("xterm", "xterm", "-e", "./echo_cli", str_addr,pipenum,(char *)0)) < 0)  {
            	/*print an appropriate error message here*/
			printf("execlp returned error \n");
			perror("execlp error: ");
        	   // exit(1);
         		}
           	//  close(pipe_fd[1]);
        //		exit(EXIT_SUCCESS);
    		}
		 else {            /* Parent reads from pipe */
			/* parent */
        		close(pipe_fd[1]);          /* Close unused write end */

			for(;;) /* forever */
			{
				FD_SET(fileno(stdin),&rset);
				FD_SET(pipe_fd[0],&rset);
				maxfdp = MAX(fileno(stdin),pipe_fd[0]) + 1;
				memset(msg,0,sizeof(msg));
				select(maxfdp,&rset,NULL,NULL,NULL);
				if(FD_ISSET(fileno(stdin),&rset))
				{
					ret= read(fileno(stdin),msg,MAXLEN);
					printf("\nPlease type in client terminal, typing here won't do any good!\n");
					//fflush(stdout);
					//fflush(stdin);
					FD_CLR(fileno(stdin),&rset);
				}
				if(FD_ISSET(pipe_fd[0],&rset))
				{
					ret = read(pipe_fd[0],msg,MAXLEN);
					if(ret<=0) break;
					if(write(fileno(stdout),msg,ret)<=0)
						break;
				}
	
			}
    			wait(NULL);	


		}
	}
	else if(!strcmp(choice,"time"))
	{
		/*fork time child*/
		   cpid = fork();

	    if (cpid == -1) {
		 perror("fork"); exit(EXIT_FAILURE);
    		}

    		if (cpid == 0) {    /* Child writes into pipe */
		/* Child */ 
        	close(pipe_fd[0]);          /* Close unused read end */
		sprintf(pipenum,"%d",pipe_fd[1]);
			if ( (execlp("xterm", "xterm", "-e", "./time_cli", str_addr,pipenum,(char *)0)) < 0)  {
            	/*print an appropriate error message here*/
			printf("execlp returned error \n");
			perror("execlp error: ");
        	   // exit(1);
         		}
           	//  close(pipe_fd[1]);
        //		exit(EXIT_SUCCESS);
    		}
		 else {            /* Parent reads from pipe */
			/* parent */
	        	close(pipe_fd[1]);          /* Close unused write end */
	
			for(;;) /* forever */
			{
				FD_SET(fileno(stdin),&rset);
				FD_SET(pipe_fd[0],&rset);
				maxfdp = MAX(fileno(stdin),pipe_fd[0]) + 1;
				memset(msg,0,sizeof(msg));
				select(maxfdp,&rset,NULL,NULL,NULL);
				if(FD_ISSET(fileno(stdin),&rset))
				{
					ret= read(fileno(stdin),msg,MAXLEN);
					printf("\nPlease don't type, why do you need to type in time service?!\n");
					//fflush(stdout);
					//fflush(stdin);
					FD_CLR(fileno(stdin),&rset);
				}
				if(FD_ISSET(pipe_fd[0],&rset))
				{
					ret = read(pipe_fd[0],msg,MAXLEN);
					if(ret<=0) break;
					if(write(fileno(stdout),msg,ret)<=0)
						break;
				}
	
			}
    	


		}	
	}
	else if(!strcmp(choice,"quit"))
	{
		/*quit*/
		break;
	}
}
 
/*********/
	}


	


