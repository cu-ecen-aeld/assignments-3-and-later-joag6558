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



int aesd_major =   0; // use dynamic major
int aesd_minor =   0;

MODULE_AUTHOR("Jose Aguas"); /** TODO: fill in your name **/
MODULE_LICENSE("Dual BSD/GPL");

static struct aesd_dev *aesd_device;

int aesd_p_buffer =  4000;//AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED;	/* buffer size */

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
	
			
	for(i=0; i < AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED; i++){
		if (!dev->entry[i].buffptr) {
			/* allocate the buffer */
			dev->entry[i].buffptr = kmalloc(aesd_p_buffer, GFP_KERNEL);
			if (!dev->entry[i].buffptr) {
				mutex_unlock(&dev->lock);
				return -ENOMEM;
			}
		}

	}

	dev->full = false;
	dev->in_offs = 0;//wrap
	dev->out_offs = 0; 

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
int i;
    struct aesd_dev *dev = filp->private_data;
    
    PDEBUG("release");
    /**
     * TODO: handle release
     */
     mutex_lock(&dev->lock);
	for (i = 0; i < AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED; i++) { /* all the list items */

		kfree(dev->entry[i].buffptr);
	}  
	mutex_unlock(&dev->lock);   	
	
    return 0;
}

static char read_message[1000];
ssize_t aesd_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
	int i,j;
	int idx;
	
	//uint8_t offs;
	struct aesd_buffer_entry *rtnentry;
    //ssize_t retval = 0;
    PDEBUG("read %zu bytes with offset %lld",count,*f_pos);
    /**
     * TODO: handle read
     */
     struct aesd_dev *dev = filp->private_data;

	if (mutex_lock_interruptible(&dev->lock))
		return -ERESTARTSYS;

    /* Clear client message buffer*/
    memset(read_message, 0, sizeof(read_message));
	idx=0;
		
		
	for(i=0; i < AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED; i++){
  		rtnentry=&(dev->entry[i]);
		for(j=0; j < dev->entry[i].size; j++){
			read_message[idx++]=*(rtnentry->buffptr++);

		}

	}	
	if (copy_to_user(buf, read_message, idx)) {
		mutex_unlock (&dev->lock);
		return -EFAULT;
	}
	
	idx = count;
	
	mutex_unlock (&dev->lock);

	/* finally, awake any writers and return */
	wake_up_interruptible(&dev->outq);
	PDEBUG("\"%s\" did read %li bytes\n",current->comm, (long)count);
	return count;
    //return retval;
}
	


ssize_t aesd_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
int i;
    //ssize_t retval = -ENOMEM;
    //PDEBUG("write %zu bytes with offset %lld",count,*f_pos);
    /**
     * TODO: handle write
     */
     	struct aesd_dev *dev = filp->private_data;
	int result;

	if (mutex_lock_interruptible(&dev->lock))
		return -ERESTARTSYS;
	
	
	// check buffer full
	if( &(dev->entry[dev->in_offs]) >= &(dev->entry[AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED]) ){
		dev->full = true;
		dev->in_offs = 0;//wrap
		dev->out_offs = 0; 
	}
	
	if (copy_from_user(dev->entry[dev->in_offs].buffptr, buf, count)) {
		mutex_unlock(&dev->lock);
		return -EFAULT;
	}
	
	//advances buffer->out_offs to the new start location
	if(dev->full){
		dev->out_offs = dev->in_offs;
	}
	
	dev->entry[dev->in_offs].size = count;
		
	// one more string in buffer
	dev->in_offs++;

	mutex_unlock(&dev->lock);

	/* finally, awake any reader */
	wake_up_interruptible(&dev->inq);  /* blocked in read() and select() */

	PDEBUG("\"%s\" did write %li bytes\n",current->comm, (long)count);
	return count;
	
    //return retval;
}
struct file_operations aesd_fops = {
    .owner =    THIS_MODULE,
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
