/**
 * @file aesd-circular-buffer.c
 * @brief Functions and data related to a circular buffer imlementation
 *
 * @author Dan Walkes
 * @date 2020-03-01
 * @copyright Copyright (c) 2020
 *
 */

#ifdef __KERNEL__
#include <linux/string.h>
#else
#include <string.h>
#endif
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
#include <stdio.h>

#include "aesd-circular-buffer.h"
#define _GNU_SOURCE // use the gnu extension so we have pthread_tryjoin_mp available
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <errno.h>

//pthread_mutex_t cb_mutex = PTHREAD_MUTEX_INITIALIZER;
// Semaphore variables
//sem_t  sem_data;
/**
 * @param buffer the buffer to search for corresponding offset.  Any necessary locking must be performed by caller.
 * @param char_offset the position to search for in the buffer list, describing the zero referenced
 *      character index if all buffer strings were concatenated end to end
 * @param entry_offset_byte_rtn is a pointer specifying a location to store the byte of the returned aesd_buffer_entry
 *      buffptr member corresponding to char_offset.  This value is only set when a matching char_offset is found
 *      in aesd_buffer.
 * @return the struct aesd_buffer_entry structure representing the position described by char_offset, or
 * NULL if this position is not available in the buffer (not enough data is written).
 */
//static char message[250];
struct aesd_buffer_entry *aesd_circular_buffer_find_entry_offset_for_fpos(struct aesd_circular_buffer *buffer,
            size_t char_offset, size_t *entry_offset_byte_rtn )
{
 
    /**
    * TODO: implement per description
    */

struct aesd_buffer_entry *rtnentry;
size_t char_offset_tmp=0;

	for(int index=0; index < buffer->in_offs; index++){
                *entry_offset_byte_rtn = index;
                rtnentry=&(buffer->entry[index]);
		for(int data_index=0; data_index < buffer->entry[index].size; data_index++){
			  //printf("entry: %c\n", *(entryptr->buffptr++));
			if(char_offset_tmp++ == char_offset){
				return rtnentry;

			}

		}

	}
		
    return NULL;
}

/**
* Adds entry @param add_entry to @param buffer in the location specified in buffer->in_offs.
* If the buffer was already full, overwrites the oldest entry and advances buffer->out_offs to the
* new start location.
* Any necessary locking must be handled by the caller
* Any memory referenced in @param add_entry must be allocated by and/or must have a lifetime managed by the caller.
*/
void aesd_circular_buffer_add_entry(struct aesd_circular_buffer *buffer, const struct aesd_buffer_entry *add_entry)
{
    /**
    * TODO: implement per description
    */
 
 
	// check buffer full
	if( &(buffer->entry[buffer->in_offs]) >= &(buffer->entry[AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED]) ){
		buffer->full = true;
		buffer->in_offs = 0;//wrap
		buffer->out_offs = 0; 
	}



	// mutex lock

	//advances buffer->out_offs to the new start location
	if(buffer->full){
		buffer->out_offs = buffer->in_offs;
	}

	// add string in buffer
	buffer->entry[buffer->in_offs].buffptr = add_entry->buffptr;// put data in buffer
	buffer->entry[buffer->in_offs].size = add_entry->size;

	// one more string in buffer
	buffer->in_offs++;


}

/**
* Initializes the circular buffer described by @param buffer to an empty struct
*/
void aesd_circular_buffer_init(struct aesd_circular_buffer *buffer)
{
    memset(buffer,0,sizeof(struct aesd_circular_buffer));
}

