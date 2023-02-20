/**
 * @file aesdchar.c
 * @brief Functions and data related to the AESD char driver implementation
 *
 * Based on the implementation of the "scull" device driver, found in
 * Linux Device Drivers example code.
 *
 * @author Dan Walkes
 * @date 2019-10-22
 * @copyright Copyright (c) 2019
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/fs.h> // file_operations
#include "aesdchar.h"
#include "aesd_ioctl.h"

#include <linux/moduleparam.h>

#include <linux/kernel.h>	/* printk(), min() */
#include <linux/slab.h>		/* kmalloc() */

#include <linux/proc_fs.h>
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/fcntl.h>
#include <linux/poll.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/seq_file.h>

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
#define AESDCHAR_MAX 1000


struct aesd_dev {
	wait_queue_head_t inq, outq;       /* read and write queues */
	struct aesd_buffer_entry  entry[AESDCHAR_MAX];
	struct aesd_seekto seekto; 
	char *buffer, *end;                /* begin of buf, end of buf */
	int buffersize;                    /* used in pointer arithmetic */
	int size;
	char *rp, *wp;                     /* where to read, where to write */
	int nreaders, nwriters;            /* number of openings for r/w */
	struct fasync_struct *async_queue; /* asynchronous readers */
	struct mutex lock;              /* mutual exclusion mutex */
	struct cdev cdev;                  /* Char device structure */
};

int aesd_major =   0; // use dynamic major
int aesd_minor =   0;


MODULE_AUTHOR("Jose Aguas"); /** TODO: fill in your name **/
MODULE_LICENSE("Dual BSD/GPL");

static struct aesd_dev *aesd_device;

int aesd_p_buffer =  4000;//AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED;	/* buffer size */
int aesd_c_buffer =  32;//AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED;	/* buffer size */
static int spacefree(struct aesd_dev *dev);

int aesd_open(struct inode *inode, struct file *filp)
{
	int i;
	PDEBUG("open");
	/**
	 * TODO: handle open
	 */
	struct aesd_dev *dev;


	//printk(KERN_ALERT "max number of commands %d\n", aesd_p_buffer);
	dev = container_of(inode->i_cdev, struct aesd_dev, cdev);
	filp->private_data = dev;

	if (mutex_lock_interruptible(&dev->lock))
		return -ERESTARTSYS;
	if (!dev->buffer) {
		//PDEBUG("   Buffer befor kmalloc: %p\n", dev->buffer);
		/* allocate the buffer */
		dev->buffer = kmalloc(aesd_p_buffer, GFP_KERNEL);
		if (!dev->buffer) {
			mutex_unlock(&dev->lock);
			return -ENOMEM;
		}

		memset(dev->buffer,0,aesd_p_buffer);

		dev->buffersize = aesd_p_buffer;
		dev->end = dev->buffer + dev->buffersize;
		dev->rp = dev->wp = dev->buffer; /* rd and wr from the beginning */
	}

	for(i=0; i < AESDCHAR_MAX; i++){
		if (!dev->entry[i].buffptr) {
			/* allocate the buffer */
			dev->entry[i].buffptr = kmalloc(aesd_c_buffer, GFP_KERNEL);
			if (!dev->entry[i].buffptr) {
				mutex_unlock(&dev->lock);
				return -ENOMEM;
			}
			memset(dev->entry[i].buffptr,0,aesd_c_buffer);
		}
	}

	//PDEBUG("Device: %p\n",dev);
	/*		seq_printf(s, "   Queues: %p %p\n", p->inq, p->outq);*/
	//PDEBUG("   Buffer: %p to %p (%i bytes)\n", dev->buffer, dev->end, dev->buffersize);
	//PDEBUG("   rp %p   wp %p\n", dev->rp, dev->wp);
	/* use f_mode,not  f_flags: it's cleaner (fs/open.c tells why) */
	if (filp->f_mode & FMODE_READ)
		dev->nreaders++;
	if (filp->f_mode & FMODE_WRITE)
		dev->nwriters++;
	mutex_unlock(&dev->lock);

	return 0;

}

int aesd_release(struct inode *inode, struct file *filp)
{
	struct aesd_dev *dev = filp->private_data;

	PDEBUG("release");
	/**
	 * TODO: handle release
	 */
	/*
	   mutex_lock(&dev->lock);

	   kfree(dev->buffer);
	   dev->buffer = NULL;*/ /* the other fields are not checked on open */
	/*mutex_unlock(&dev->lock);*/

	return 0;
}

ssize_t aesd_read(struct file *filp, char __user *buf, size_t count,
		loff_t *f_pos)
{
	int i=0;
	int size=0;
	int idx=0;
	int k,j,l;
	char *c;
	char read_message[1000];
	//ssize_t retval = 0;
	//PDEBUG("read %zu bytes with offset %lld",count,*f_pos);
	/**
	 * TODO: handle read
	 */
	struct aesd_dev *dev = filp->private_data;

	memset(read_message, 0, sizeof(read_message));
	
	PDEBUG("dev->seekto.write_cmd_offset %d",dev->seekto.write_cmd_offset);

	if (mutex_lock_interruptible(&dev->lock))
		return -ERESTARTSYS;

	while (dev->rp == dev->wp) { /* nothing to read */
		mutex_unlock(&dev->lock); /* release the lock */
		//if (filp->f_flags & O_NONBLOCK)
		//PDEBUG("\"%s\" reading: going to sleep\n", current->comm);
		return -EAGAIN;
		//PDEBUG("\"%s\" reading: going to sleep\n", current->comm);
		//if (wait_event_interruptible(dev->inq, (dev->rp != dev->wp)))
		//	return -ERESTARTSYS; /* signal: tell the fs layer to handle it */
		/* otherwise loop, but first reacquire the lock */
		//if (mutex_lock_interruptible(&dev->lock))
		//	return -ERESTARTSYS;
	}

	c=dev->buffer;	

	while(*(c) != '\0'){

		//*(dev->entry[i].buffptr++)=*(c);
		//PDEBUG("%c\n",*(c));
		size++;
		if(*(c++) == '\n'){
			dev->entry[i].size=size;
			//PDEBUG("%d size\n",size);
			size=0;
			i++;
		}

	}

	//PDEBUG("%d commands\n",i);

	if (i > 10)
		k = (i-10);
	else /* the write pointer has wrapped, return data up to dev->end */
		k = 0;

	//PDEBUG("%d k\n",k);	
	c=dev->buffer;

	for(l=0; l < k; l++){

		for(j=0; j < dev->entry[l].size; j++){
			*(c++);
		}

	}

	dev->rp= (c + dev->seekto.write_cmd_offset + *f_pos);


	for(l=k; l < i; l++){

		for(j=0; j < dev->entry[l].size; j++){

			//*(dev->entry[l].buffptr) = *(c);
			//PDEBUG("%c\n",*(c));	
			idx++;
			//PDEBUG("entry: %c buff: %c\n",*(dev->entry[l].buffptr++), *(c++));
			//PDEBUG("entry: %c\n",read_message[idx++]);	
		}

	}

	/* ok, data is there, return something */
	if (dev->wp > dev->rp)
		count = min(count, (size_t)(dev->wp - dev->rp));
	else /* the write pointer has wrapped, return data up to dev->end */
		count = min(count, (size_t)(dev->end - dev->rp));
	count = (idx - *f_pos);
	dev->seekto.write_cmd_offset = *f_pos;
PDEBUG("%d count\n",count);
	if (copy_to_user(buf, dev->rp, count)) {
		mutex_unlock (&dev->lock);
		return -EFAULT;
	}


	dev->rp += count;
	if (dev->rp == dev->end)
		dev->rp = dev->buffer; /* wrapped */
	mutex_unlock (&dev->lock);

	/* finally, awake any writers and return */
	wake_up_interruptible(&dev->outq);
	PDEBUG("\"%s\" did read %d bytes\n",current->comm, idx);
	return count;
	//return retval;
}

/* Wait for space for writing; caller must hold device semaphore.  On
 * error the semaphore will be released before returning. */
static int aesd_getwritespace(struct aesd_dev *dev, struct file *filp)
{
	while (spacefree(dev) == 0) { /* full */
		DEFINE_WAIT(wait);

		mutex_unlock(&dev->lock);
		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;
		PDEBUG("\"%s\" writing: going to sleep\n",current->comm);
		prepare_to_wait(&dev->outq, &wait, TASK_INTERRUPTIBLE);
		if (spacefree(dev) == 0)
			schedule();
		finish_wait(&dev->outq, &wait);
		if (signal_pending(current))
			return -ERESTARTSYS; /* signal: tell the fs layer to handle it */
		if (mutex_lock_interruptible(&dev->lock))
			return -ERESTARTSYS;
	}
	return 0;
}	

/* How much space is free? */
static int spacefree(struct aesd_dev *dev)
{
	if (dev->rp == dev->wp)
		return dev->buffersize - 1;
	return ((dev->rp + dev->buffersize - dev->wp) % dev->buffersize) - 1;
}
ssize_t aesd_write(struct file *filp, const char __user *buf, size_t count,
		loff_t *f_pos)
{
	//ssize_t retval = -ENOMEM;
	//PDEBUG("write %zu bytes with offset %lld",count,*f_pos);
	/**
	 * TODO: handle write
	 */
	struct aesd_dev *dev = filp->private_data;
	int result;

	if (mutex_lock_interruptible(&dev->lock))
		return -ERESTARTSYS;

	/* Make sure there's space to write */
	result = aesd_getwritespace(dev, filp);
	if (result)
		return result; /* aesd_getwritespace called up(&dev->sem) */


	if (dev->wp >= dev->rp)
		count = min(count, (size_t)(dev->end - dev->wp)); /* to end-of-buf */
	else /* the write pointer has wrapped, fill up to rp-1 */
		count = min(count, (size_t)(dev->rp - dev->wp - 1));

	if (copy_from_user(dev->wp, buf, count)) {
		mutex_unlock(&dev->lock);
		return -EFAULT;
	}
	dev->wp += count;
	if (dev->wp == dev->end)
		dev->wp = dev->buffer; /* wrapped */
	mutex_unlock(&dev->lock);

	/* finally, awake any reader */
	wake_up_interruptible(&dev->inq);  /* blocked in read() and select() */

	PDEBUG("\"%s\" did write %li bytes\n",current->comm, (long)count);
	return count;

	//return retval;
}

/*
 * The "extended" operations -- only seek
 */

loff_t aesd_llseek(struct file *filp, loff_t off, int whence)
{
	struct aesd_dev *dev = filp->private_data;
	loff_t newpos;
	
	PDEBUG("\"%s\" utility %d whence %d off\n",current->comm, whence, off);

	switch(whence) {
	  case 0: /* SEEK_SET */
		newpos = off;
		PDEBUG("SEEK_SET newpos = off %d \n",newpos);
		break;

	  case 1: /* SEEK_CUR */
		newpos = filp->f_pos + off;
		PDEBUG("SEEK_CUR newpos = filp->f_pos + off %d \n",newpos);
		//count = min(count, (size_t)(dev->end - dev->wp)); /* to end-of-buf */
		break;

	  case 2: /* SEEK_END */
		newpos = dev->size + off;
		PDEBUG("SEEK_END newpos = dev->size + off %d \n",newpos);
		break;

	  default: /* can't happen */
		return -EINVAL;
	}
	if (newpos < 0) return -EINVAL;
	filp->f_pos = newpos;
	return newpos;
}

struct file_operations aesd_fops = {
	.owner =    THIS_MODULE,
	.llseek =   aesd_llseek,
	.read =     aesd_read,
	.write =    aesd_write,
	.open =     aesd_open,
	.release =  aesd_release,
};

static int aesd_setup_cdev(struct aesd_dev *dev)
{
	int err, devno = MKDEV(aesd_major, aesd_minor);

	cdev_init(&dev->cdev, &aesd_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &aesd_fops;
	err = cdev_add (&dev->cdev, devno, 1);
	if (err) {
		printk(KERN_ERR "Error %d adding aesd cdev", err);
	}
	return err;
}

void aesd_cleanup_module(void)
{
	int i;
	dev_t devno = MKDEV(aesd_major, aesd_minor);

	cdev_del(&aesd_device->cdev);

	/**
	 * TODO: cleanup AESD specific poritions here as necessary
	 */

	kfree(aesd_device);

	unregister_chrdev_region(devno, 1);

}

int aesd_init_module(void)
{
	dev_t dev = 0;
	int result;
	result = alloc_chrdev_region(&dev, aesd_minor, 1,
			"aesdchar");
	aesd_major = MAJOR(dev);
	if (result < 0) {
		printk(KERN_WARNING "Can't get major %d\n", aesd_major);
		return result;
	}

	/* 
	 * allocate the devices -- we can't have them static, as the number
	 * can be specified at load time
	 */
	aesd_device = kmalloc( sizeof(struct aesd_dev), GFP_KERNEL);
	if (!aesd_device) {
		result = -ENOMEM;
		goto fail;  /* Make this more graceful */
	}
	memset(aesd_device,0,sizeof(struct aesd_dev));

	/**
	 * TODO: initialize the AESD specific portion of the device
	 */

	init_waitqueue_head(&(aesd_device->inq));
	init_waitqueue_head(&(aesd_device->outq));
	mutex_init(&aesd_device->lock);

	result = aesd_setup_cdev(aesd_device);

	if( result ) {
		unregister_chrdev_region(dev, 1);
	}

	return 0; /* succeed */

fail:
	aesd_cleanup_module();
	return result;

}





module_init(aesd_init_module);
module_exit(aesd_cleanup_module);
