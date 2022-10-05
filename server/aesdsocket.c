#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fs.h>


#define SOCKET_PORT 9000

static char client_message[100000];
extern int errno;
extern void int_handler();
extern void term_handler();
extern void serve_clients();

static int server_sock, client_sock;
static int fromlen;
static char c;
static FILE *fp;
static struct sockaddr_in server_sockaddr, client_sockaddr;
static struct addrinfo *servinfo;
 

/* Close sockets after a Ctrl-C interrupt */
void int_handler()
{

  printf("\nsockets are being closed by Ctrl-c\n");
remove("/tmp/aesdsocketdata");
  close(client_sock);
  close(server_sock);

  exit(0);

}

/* Close sockets after a kill signal */
void term_handler()
{
    
  printf("\nsockets are being closed by kill signal\n");
remove("/tmp/aesdsocketdata");
  close(client_sock);
  close(server_sock);

  exit(0);

}

void serve_clients(void)
{
  int len;
  int file_idx;
  int i;
  
  
}

int main(int argc, char *argv[])
{
  pid_t pid;
  /*struct hostent *hp;*/
  struct linger opt;
  int sockarg;
  int socksize = sizeof(struct sockaddr_in);
  int len;
  int file_idx;
  int i;
  int new_recv;
  


    int status;
    struct addrinfo hints;


    
    memset(&hints, 0, sizeof(hints));

   hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
   hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
   hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
   hints.ai_protocol = 0;          /* Any protocol */
   hints.ai_canonname = NULL;
   hints.ai_addr = NULL;
   hints.ai_next = NULL;
    printf("a5 p2 v22!\n");

    status = getaddrinfo(NULL, "9000", &hints, &servinfo);
    if (status != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
    
    /* creating the socket */ 
  if((server_sock=socket(PF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("Failed to socket socket");
    exit(-1);
  }

  bzero((char*) &server_sockaddr, sizeof(server_sockaddr));
  /* Address family = Internet */
  server_sockaddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  server_sockaddr.sin_port = htons(SOCKET_PORT);
  /* Set IP address to localhost */
  server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);//INADDR_ANY = 0.0.0.0
  /*bcopy (hp->h_addr, &server_sockaddr.sin_addr, hp->h_length);*/

  /*bind socket to the source WHERE THE PACKET IS COMING FROM*/
  if(bind(server_sock, (struct sockaddr *) &server_sockaddr,
     sizeof(server_sockaddr)) < 0) 
  {
    perror("Server: bind");
    exit(-1);
  }

  freeaddrinfo(servinfo);

  /* turn on zero linger time so that undelivered data is discarded when
     socket is closed
   */
   
  opt.l_onoff = 1;
  opt.l_linger = 0;

  sockarg = 1;
 
  setsockopt(server_sock, SOL_SOCKET, SO_LINGER, (char*) &opt, sizeof(opt));
  setsockopt(client_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&sockarg, sizeof(int));
  
  signal(SIGINT, int_handler);
  signal(SIGTERM, term_handler);

  fp = fopen("/tmp/aesdsocketdata","w");
  
  if (argc > 1){
	  if(strncmp("-d", argv[1], 2) == 0){
	  
	     printf("daemon to run in the background!\n");
	     
	     pid = fork();
	     if(pid == -1){
		return -1;
	     } else if (pid != 0){
		exit (EXIT_SUCCESS);
	     }
	     
	     /* create new session and process group*/
	     if(setsid() == -1){
		return -1;
	     }
	     
	     /* close all open files--NR_OPEN is overkill, but works*/
	     /*for(i=0;i<NR_OPEN;i++){
		close(i);
	     }*/
	     
	     /* redirect fd's 0,1,2 to /dev/null */
	     open("/dev/null",O_RDWR); /* sdin */
	     dup(0); /* stdout */
	     dup(0); /* stderror */
	     
	     for(;;)
	  {

	    /* Listen on the socket */
	    if(listen(server_sock, 5) < 0)
	    {
	      perror("Server: listen");
	      exit(-1);
	    }

	    /* Accept connections */
	    if((client_sock=accept(server_sock, 
			           (struct sockaddr *)&client_sockaddr,
			           &fromlen)) < 0) 
	    {
	      perror("Server: accept");
	      exit(-1);
	    }

	    do{
           fp = fopen("/tmp/aesdsocketdata","a+");
		    /* Clear client message buffer*/
		    memset(client_message, 0, sizeof(client_message));
		       // Receive client's message:
		    new_recv= recv(client_sock, client_message, sizeof(client_message), 0);
		    printf("new_recv: %d\n", new_recv);
		    /*printf("message: %s\n", client_message);*/
		    printf("message length: %zu\n", strlen(client_message));
		    len=strlen(client_message);
		 
		    
			    for (i = 0; i < len; i++){
				fputc(client_message[i], fp);

			    }
		    	    fclose(fp);


		    			   /* Clear client message buffer*/
			    memset(client_message, 0, sizeof(client_message));
			    printf("reading from file...\n");
			    fp = fopen("/tmp/aesdsocketdata","r");
			    file_idx=0;

				/* Reading the string from file*/
			    while((c = fgetc(fp)) != EOF)
			    {

			       if(file_idx < sizeof(client_message)){
				  client_message[file_idx]=c;
				  /*putchar(c);*/
				  /*printf("%c", c);*/
				  file_idx++;
			       }
			    }

			    fclose(fp);
		    /*if( client_message[i-1] !='\n'){
			client_message[i] ='\n';
			fputc(client_message[i], fp);
		    }*/
		    if((new_recv > 0) && (new_recv < 40)){
		    		    


			    printf("sending number of bytes : %d\n", file_idx);
			    send(client_sock, client_message, file_idx, 0);
	           }
	           else if(file_idx > 16424){

	            printf("sending number of bytes : %d\n", file_idx);
			    send(client_sock, client_message, file_idx, 0);
	           }
	    
	    }while(new_recv > 0);
	    


	  } 
	  
	  }
  }
  else {
	  
	    printf("program running in the foreground!\n");
	    for(;;)
	  {

	    /* Listen on the socket */
	    if(listen(server_sock, 5) < 0)
	    {
	      perror("Server: listen");
	      exit(-1);
	    }

	    /* Accept connections */
	    if((client_sock=accept(server_sock, 
		                   (struct sockaddr *)&client_sockaddr,
		                   &fromlen)) < 0) 
	    {
	      perror("Server: accept");
	      exit(-1);
	    }

           
           do{
           fp = fopen("/tmp/aesdsocketdata","a+");
		    /* Clear client message buffer*/
		    memset(client_message, 0, sizeof(client_message));
		       // Receive client's message:
		    new_recv= recv(client_sock, client_message, sizeof(client_message), 0);
		    printf("new_recv: %d\n", new_recv);
		    /*printf("message: %s\n", client_message);*/
		    printf("message length: %zu\n", strlen(client_message));
		    len=strlen(client_message);
		 
		    
			    for (i = 0; i < len; i++){
				fputc(client_message[i], fp);

			    }
		    	    fclose(fp);


		    			   /* Clear client message buffer*/
			    memset(client_message, 0, sizeof(client_message));
			    printf("reading from file...\n");
			    fp = fopen("/tmp/aesdsocketdata","r");
			    file_idx=0;

				/* Reading the string from file*/
			    while((c = fgetc(fp)) != EOF)
			    {

			       if(file_idx < sizeof(client_message)){
				  client_message[file_idx]=c;
				  /*putchar(c);*/
				  /*printf("%c", c);*/
				  file_idx++;
			       }
			    }

			    fclose(fp);
		    /*if( client_message[i-1] !='\n'){
			client_message[i] ='\n';
			fputc(client_message[i], fp);
		    }*/
		    if((new_recv > 0) && (new_recv < 40)){
		    		    


			    printf("sending number of bytes : %d\n", file_idx);
			    send(client_sock, client_message, file_idx, 0);
	           }
	           else if(file_idx > 16424){

	            printf("sending number of bytes : %d\n", file_idx);
			    send(client_sock, client_message, file_idx, 0);
	           }
	    
	    }while(new_recv > 0);


	    


	  } 
	    
   }
  
  return 0;

}






