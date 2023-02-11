/*
 * aesdchar.h
 *
 *  Created on: Oct 23, 2019
 *      Author: Dan Walkes
 */

#ifndef AESD_CHAR_DRIVER_AESDCHAR_H_
#define AESD_CHAR_DRIVER_AESDCHAR_H_

//#define AESD_DEBUG 1  //Remove comment on this line to enable debug

#undef PDEBUG             /* undef it, just in case */
#ifdef AESD_DEBUG
#  ifdef __KERNEL__
     /* This one if debugging is on, and kernel space */
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG "aesdchar: " fmt, ## args)
#  else
     /* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif

#define AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED 10

struct aesd_buffer_entry
{
    /**
     * A location where the buffer contents in buffptr are stored
     */
     char *buffptr;
    /**
     * Number of bytes stored in buffptr
     */
    size_t size;
};
struct aesd_dev {
	struct aesd_buffer_entry  entry[AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED];
	    /**
     * The current location in the entry structure where the next write should
     * be stored.
     */
    uint8_t in_offs;
    /**
     * The first location in the entry structure to read from
     */
    uint8_t out_offs;
    
        /**
     * set to true when the buffer entry structure is full
     */
    bool full;
    
        wait_queue_head_t inq, outq;       /* read and write queues */
        char *buffer, *end;                /* begin of buf, end of buf */
        int buffersize;                    /* used in pointer arithmetic */
        char *rp, *wp;                     /* where to read, where to write */
        int nreaders, nwriters;            /* number of openings for r/w */
        struct mutex lock;              /* mutual exclusion mutex */
        struct cdev cdev;                  /* Char device structure */
};

#endif /* AESD_CHAR_DRIVER_AESDCHAR_H_ */
