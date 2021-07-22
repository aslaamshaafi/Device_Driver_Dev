#include <linux/module.h>
#include <linux/fs.h>
//#include <asm/uaccess.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/errno.h>
/*mutex(or binary semaphore --semaphore is muliptle and mutex is binary)—only owner can unlock it and mulitple mutex are not allowed and should not be used in interrupt-if it does not find lock it will go to sleep. 
Sleep to wakeup or viceversa is a timeconsuming. If the duration of lock is very small and spending more time on sleep wakeup mechanisim.
*/
#include <asm/current.h>
#include <linux/device.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/semaphore.h>

#include "tboost_char_device.h"

#define MAX_LENGTH 4000
#define SUCCESS 0
#define CHAR_DEV_NAME "First_Char"
#define LICENSE_TYPE "GPL"
#define DRIVER_AUTHOR "Muhammad Aslam aslaam.shafee@gmail.com"
#define DRIVER_DESC   "First Module"
static char *char_device_buf;
struct cdev *tboost_cdev;
dev_t mydev;
int count=1;
static struct class *tboost_class;
/* (1) declare struct semaphore */
static struct semaphore mysem;

static int char_dev_open(struct inode *inode,
			    struct file  *file)
{
	static int counter = 0;
	counter++;
	printk(KERN_INFO "Number of times open() was called: %d\n", counter);
	printk (KERN_INFO " MAJOR number = %d, MINOR number = %d\n",imajor (inode), iminor (inode));
	printk(KERN_INFO "Process id of the current process: %d\n",current->pid );
	printk (KERN_INFO "ref=%d\n", module_refcount(THIS_MODULE));
	return SUCCESS;
}

static int char_dev_release(struct inode *inode,
		            struct file *file)
{
	return SUCCESS;
}

static ssize_t char_dev_read(struct file *file, 
		            char *buf,
			    size_t lbuf,
			    loff_t *ppos)
{
	int maxbytes; /* number of bytes from ppos to MAX_LENGTH */
	int bytes_to_do; /* number of bytes to read */
	int nbytes; /* number of bytes actually read */

	maxbytes = MAX_LENGTH - *ppos;
	
	if( maxbytes > lbuf ) bytes_to_do = lbuf;
	else bytes_to_do = maxbytes;
	
	if( bytes_to_do == 0 ) {
		printk("Reached end of device\n");
		return -ENOSPC; /* Causes read() to return EOF */
	}

	printk (KERN_INFO "process %i (%s) going to sleep\n", current->pid,current->comm);
	/*(3) locked*/
	if (down_interruptible (&mysem)) {
        printk (KERN_INFO "process %i woken up by a signal\n", current->pid);
        return -ERESTARTSYS;
    }

    printk (KERN_INFO "process %i (%s) resuming\n", current->pid,current->comm);

	
	nbytes = bytes_to_do - 
	copy_to_user( buf, /* to */
			       char_device_buf + *ppos, /* from */
			       bytes_to_do ); /* how many bytes */
	/*(4) unlocked*/				   
        up(&mysem);
	//*ppos += nbytes;
	return nbytes;	
}


static ssize_t char_dev_write(struct file *file,
		            const char *buf,
			    size_t lbuf,
			    loff_t *ppos)
{
	int nbytes; /* Number of bytes written */
	int bytes_to_do; /* Number of bytes to write */
	int maxbytes; /* Maximum number of bytes that can be written */

	maxbytes = MAX_LENGTH - *ppos;

	if( maxbytes > lbuf ) bytes_to_do = lbuf;
	else bytes_to_do = maxbytes;

	if( bytes_to_do == 0 ) {
		printk("Reached end of device\n");
		return -ENOSPC; /* Returns EOF at write() */
	}

        //if (down_interruptible (&mysem)) {
        //printk (KERN_INFO "process %i woken up by a signal\n", current->pid);
        //return -ERESTARTSYS;
        //}
	down(&mysem);

	nbytes = bytes_to_do -
	         copy_from_user(char_device_buf + *ppos, /* to */
				buf, /* from */
				bytes_to_do ); /* how many bytes */
	*ppos += nbytes;

	ssleep(10);
	printk (KERN_INFO "process %i (%s) wake up readers...\n",current->pid, current->comm);
	up (&mysem);
	return nbytes;
}
#if 0
static loff_t char_dev_lseek (struct file *file, loff_t offset, int orig)
{
    loff_t testpos;
    switch (orig) {

    case 0:                 /* SEEK_SET */
            testpos = offset;
            break;
    case 1:                 /* SEEK_CUR */
            testpos = file->f_pos + offset;
            break;
    case 2:                 /* SEEK_END */
           testpos = MAX_LENGTH + offset;
           break;
    default:
          return -EINVAL;
    }
   
    testpos = testpos < MAX_LENGTH ? testpos : MAX_LENGTH;
    testpos = testpos >= 0 ? testpos : 0;
    file->f_pos = testpos;
    printk (KERN_INFO "Seeking to pos=%ld\n", (long)testpos);
    return testpos;
}


static struct my_data my_data = {
    .i = -100,
    .x = 100,
    .s = "Linux Kernel",
};

static long chrdev_unlocked_ioctl (struct file *fp, unsigned int cmd, unsigned long arg)
{
    int size, rc, direction;
    void __user *ioargp = (void __user *)arg;

    if (_IOC_TYPE (cmd) != TBOOST_MAGIC) {
        printk (KERN_INFO " got invalid case, CMD=%d\n", cmd);
        return -EINVAL;
    }

    direction = _IOC_DIR (cmd);
    size = _IOC_SIZE (cmd);

    switch (direction) {

    case _IOC_WRITE:
        	printk(KERN_INFO  " reading = %d bytes from user-space and writing to device\n",size);
            	rc = copy_from_user (&my_data, ioargp, size);
        	printk(KERN_INFO "    my_data.i = %d\n    (int)my_data.x = %ld\n    my_data.s = %s\n",
                                 my_data.i, my_data.x, my_data.s);
        	return rc;
        	break;

    case _IOC_READ:
        	printk (KERN_INFO  " reading device and writing = %d bytes to user-space\n", size);
        	printk (KERN_INFO  "    my_data.i = %d\n    (int)my_data.x = %ld\n    my_data.s = %s\n",
             				my_data.i, my_data.x, my_data.s);
        	rc = copy_to_user (ioargp, &my_data, size);
       		return rc;
        	break;

    default:
       		 printk (KERN_INFO " got invalid case, CMD=%d\n", cmd);
        	return -EINVAL;
    }
}
#endif

static struct file_operations char_dev_fops = {
	.owner = THIS_MODULE,
	.read =  char_dev_read,
	.write = char_dev_write,
	//.unlocked_ioctl = chrdev_unlocked_ioctl,
	//.llseek = char_dev_lseek,
	.open = char_dev_open,
	.release = char_dev_release,
};

static __init int char_dev_init(void)
{
	int ret;

	if (alloc_chrdev_region (&mydev, 0, count, CHAR_DEV_NAME) < 0) {
            printk (KERN_ERR "failed to reserve major/minor range\n");
            return -1;
    }

        if (!(tboost_cdev = cdev_alloc ())) {
            printk (KERN_ERR "cdev_alloc() failed\n");
            unregister_chrdev_region (mydev, count);
            return -1;
 	}
	cdev_init(tboost_cdev,&char_dev_fops);

	ret=cdev_add(tboost_cdev,mydev,count);
	if( ret < 0 ) {
		printk(KERN_INFO "Error registering device driver\n");
	        cdev_del (tboost_cdev);
                unregister_chrdev_region (mydev, count); 	
		return -1;
	}

	tboost_class = class_create (THIS_MODULE, "VIRTUAL");
        device_create (tboost_class, NULL, mydev, NULL, "%s", CHAR_DEV_NAME);

	printk(KERN_INFO"\nDevice Registered: %s\n",CHAR_DEV_NAME);
	printk (KERN_INFO "Major number = %d, Minor number = %d\n", MAJOR (mydev),MINOR (mydev));
/* (2) init struct semaphore */
        sema_init(&mysem, 1);
	char_device_buf =(char *)kmalloc(MAX_LENGTH,GFP_KERNEL);
	return 0;
}

static __exit void  char_dev_exit(void)
{
	 device_destroy (tboost_class, mydev);
      class_destroy (tboost_class);
	 cdev_del(tboost_cdev);
	 unregister_chrdev_region(mydev,1);
	 kfree(char_device_buf);
	 printk(KERN_INFO "\n Driver unregistered \n");
}
module_init(char_dev_init);
module_exit(char_dev_exit);

// Following macros are definded in linux/module.h
MODULE_LICENSE(LICENSE_TYPE); // It tell the license type since linux kernel is open source, we are definding our moduel as GPL
MODULE_AUTHOR(DRIVER_AUTHOR);// use "Name <email>" or just "Name"
MODULE_DESCRIPTION(DRIVER_DESC);// what is the function of module
MODULE_SUPPORTED_DEVICE("sample_module"); // will decribe in the next examples
