#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define _GNU_SOURCE // use the gnu extension so we have pthread_tryjoin_mp available
#include <pthread.h>
#include "unity.h"
#include <stdbool.h>
#include <errno.h>



// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    bool success = false;
    int rc;
    
    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    struct thread_data *thread_func_args = (struct thread_data *) thread_param;
    /*
    thread_func_args->thread = pthread_self();
    */
printf("\nwait_to_obtain_ms: %d and wait_to_release_ms: %d\n", thread_func_args->wait_to_obtain_ms,thread_func_args->wait_to_release_ms);
    if ((thread_func_args->wait_to_obtain_ms) > 0 ) {
        /**
         * Sleep for 20 * the amount of sleep before lock to
         * to check for thread joinable.  There's no guarantee this will be enough so don't fail
         * if it's not, but print a warning about what likely is wrong if the pthread_join step fails.
         */
        usleep((thread_func_args->wait_to_obtain_ms)*1000);
    }
    

	    rc = pthread_mutex_lock(thread_func_args->mutex);
	    if ( rc != 0 ) {
		printf("pthread_mutex_lock failed with %d\n",rc);
		
	    } else {
	    

	       if ((thread_func_args->wait_to_release_ms) > 0 ) {
		   /**
		    * Sleep for 20 * the amount of sleep before lock to
		    * to check for thread joinable.  There's no guarantee this will be enough so don't fail
		    * if it's not, but print a warning about what likely is wrong if the pthread_join step fails.
		    */
		   TEST_ASSERT_EQUAL_INT_MESSAGE(0,usleep((thread_func_args->wait_to_release_ms)*1000),
		            "usleep was interrupted");
	       }

	       pthread_mutex_unlock(thread_func_args->mutex);
	       
	       success = true;
	    
	    }


    thread_func_args->thread_complete_success = success;
    
    return thread_func_args;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
     bool success = true;
     int rc;
     
     struct thread_data *thread_data_params;
     
     
     thread_data_params = (void *)malloc(sizeof(struct thread_data));

    /*
     thread declartion
     */
     
     thread_data_params->mutex = mutex;
     
     thread_data_params->wait_to_obtain_ms = wait_to_obtain_ms;
     
     thread_data_params->wait_to_release_ms = wait_to_release_ms;
     
     printf("\nwait_to_obtain_ms: %d and wait_to_release_ms: %d\n", thread_data_params->wait_to_obtain_ms,thread_data_params->wait_to_release_ms);
     
    
    rc = pthread_create(thread, NULL, threadfunc, thread_data_params);
     
    if ( rc != 0 ) {
        printf("pthread_create failed with %d\n",rc);
        success = false;
        
    }
    
    /*
    * wait for thread to finish
    */
    
    /*rc = pthread_join(*thread, (void *)&thread_data_params);

    if ( rc != 0 ) {
        printf("pthread_join failed with %d\n",rc);
        success = false;
        
    }*/
    
 /*
    rc = pthread_equal((thread_data_params->thread), (thread_data_params->thread));
printf("\n\nrc %d\n", rc);
    if ( rc) {
        
        if(thread_data_params->thread_complete_success){
        
            printf("clearing \n");
           pthread_mutex_destroy(thread_data_params->mutex);
           free(thread_data_params);
           
        }
        
    }*/
    
    return success;
}

