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
#include <time.h>
#include <stdbool.h>
#include "queue.h"
// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>


#define SOCKET_PORT 9000

// Semaphore variables
sem_t  sem_data;

static char client_message[50000];
static char client_message_tmp[50000];
pthread_t socketthreads[100];
extern int errno;
extern void int_handler();
extern void term_handler();
//extern void serve_clients();

static int server_sock, client_sock;
static int fromlen;
static char c;
static FILE *fp;
static struct sockaddr_in server_sockaddr, client_sockaddr;
static struct addrinfo *servinfo;
//static int file_idx;
 

// SLIST.
typedef struct slist_data_s slist_data_t;
struct slist_data_s {
     pthread_t * thread;
     //pthread_mutex_t * mutex;
    int client_sock;
     //unsigned int wait_to_release_ms;
     

   bool thread_complete_success;
    SLIST_ENTRY(slist_data_s) entries;
};

static timer_t timer_1;
struct timeval start_time_val;
static int tmp_time_stamp;
static int tmp_time_stamp_prev;

/* Close sockets after a Ctrl-C interrupt */
void int_handler()
{

  printf("\nsockets are being closed by Ctrl-c\n");
/*remove("/var/tmp/aesdsocketdata");
  close(client_sock);
  close(server_sock);*/

  exit(0);

}

/* Close sockets after a kill signal */
void term_handler()
{
    
  printf("\nsockets are being closed by kill signal\n");
/*remove("/var/tmp/aesdsocketdata");
  close(client_sock);
  close(server_sock);*/

  exit(0);

}

static int file_idx=0;
// server_clients Function
void* threadfunc(void* thread_param)
{
  int len;
    int new_recv;
 
  int i;
   char buffer[80];
     time_t rawtime;
     struct tm *info;
   time( &rawtime );

    
/*
   info = localtime( &rawtime );

   strftime(buffer,80,"timestamp:%F %H:%M:%S\n", info);
   printf("%s\n", buffer );
   len=strlen(buffer);
   printf("%d\n", len );
  */
  //struct tm tm;
/* Set tm to the correct time */
//char s[20]; /* strlen("2009-08-10 18:17:54") + 1 */
//strftime(s, 20, "%F %H:%M:%S", &tm);
//printf("timestamp:%s\n", s );

	// Lock the semaphore
	sem_wait(&sem_data);
	
    slist_data_t *thread_func_args = (slist_data_t *) thread_param;
  //printf("\nserver is connect to new client");
  


    /*thread_func_args->thread = (pthread_t *)pthread_self();


printf("current id: %lu\n", *(thread_func_args->thread ));*/
	    /* Clear client message buffer*/
	    memset(client_message_tmp, 0, sizeof(client_message_tmp));
	       // Receive client's message:
	    new_recv= recv(thread_func_args->client_sock, client_message_tmp, sizeof(client_message_tmp), 0);

	    fp = fopen("/var/tmp/aesdsocketdata","a+");
		    for (i = 0; i < new_recv; i++){
			client_message[file_idx] = client_message_tmp[i];
			printf("%c", client_message_tmp[i]);
			fputc(client_message[i], fp);
			file_idx++;

		    }


		    send(thread_func_args->client_sock, client_message, file_idx, 0);

            fclose(fp);
	    sem_post(&sem_data);

  sleep(10);

  
           sem_wait(&sem_data);


	   info = localtime( &rawtime );

	   strftime(buffer,80,"timestamp:%F %H:%M:%S\n", info);
	   len=strlen(buffer);

	    for (i = 0; i < len; i++){
		client_message[file_idx] = buffer[i];
		printf("%c", buffer[i]);
		file_idx++;

	    }
    	    fp = fopen("/var/tmp/aesdsocketdata","a+");
	    
	    
	    for (i = 0; i < file_idx; i++){
		fputc(client_message[i], fp);
	    }

	    
	    fclose(fp);
    	    // Unlock the semaphore

	    sem_post(&sem_data);
	


	printf("\nthread is done");
	thread_func_args->thread_complete_success = true;
    
    return thread_func_args;
    
}

int main(int argc, char *argv[])
{
  pid_t pid;
  /*struct hostent *hp;*/
  struct linger opt;
  int sockarg;
  int socksize = sizeof(struct sockaddr_in);
  int file_idx=0;
  int i=0;
  int idx=0;
  int len;
  int new_recv;
  int rc;



    int status;
    struct addrinfo hints;
    
   slist_data_t *datap=NULL;
   

   SLIST_HEAD(slisthead, slist_data_s) head;
   SLIST_INIT(&head);
   
   printf("program arguments!: %d\n", argc);
   
   /*

     time_t rawtime;
     struct tm *info;
   time( &rawtime );
   
*/

sem_init(&sem_data, 0, 1);
    
    memset(&hints, 0, sizeof(hints));

   hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
   hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
   hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
   hints.ai_protocol = 0;          /* Any protocol */
   hints.ai_canonname = NULL;
   hints.ai_addr = NULL;
   hints.ai_next = NULL;
    printf("a6 p1 v1!\n");

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

  fp = fopen("/var/tmp/aesdsocketdata","w");
  file_idx=0;

 
  
  	
	
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

 


        datap = malloc(sizeof(slist_data_t));

      datap->thread =  &socketthreads[idx++];

     datap->client_sock = client_sock;
     
     datap->thread_complete_success = false;
     
     
     SLIST_INSERT_HEAD(&head, datap, entries);
    
    rc = pthread_create(datap->thread, NULL, threadfunc, datap);
     
    if ( rc != 0 ) {
        printf("pthread_create failed with %d\n",rc);
        
    }


    // Read2 (remove).
    while (!SLIST_EMPTY(&head)) {

        datap = SLIST_FIRST(&head);
        if(datap->thread_complete_success){
		printf("\nsuccess!!!\n");
		           
		pthread_join(datap->thread, NULL);
		           
		SLIST_REMOVE_HEAD(&head, entries);
		free(datap);
        }

    }
/*printf("pthread index %d\n",idx);

	for(i=0;i<idx;i++){
		pthread_join(socketthreads[i], NULL);

    
        if(thread_data_params->thread_complete_success){
        
            printf("thread id: %lu\n", *(thread_data_params->thread_id ));
           //pthread_mutex_destroy(thread_data_params->mutex);
           //free(thread_data_params);
           
        }
        }*/

  } 
	    
  
  return 0;

}






