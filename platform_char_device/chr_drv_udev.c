/*
useful links
https://github.com/jeyaramvrp/kernel-module-programming/tree/master/sample-platform-driver
https://www.youtube.com/watch?v=n1wHgGmDXwE&ab_channel=AlokPrasad
*/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/platform_device.h>      /* For platform devices */
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <asm/current.h>
#include <linux/device.h>
#include <linux/slab.h>

#define MAX_LENGTH 4000
#define SUCCESS 0
#define CHAR_DEV_NAME "First_Char"
#define LICENSE_TYPE "GPL"
#define DRIVER_AUTHOR "Muhammad Aslam aslaam.shafee@gmail.com"
#define DRIVER_DESC   "First Module"

static char *char_device_buf;
struct cdev *dynamic_cdev;
dev_t mydev;
int count=1;
static struct class *dynamic_class;

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
		printk("READ: Reached end of device\n");
		return -ENOSPC; /* Causes read() to return EOF */
	}
	
	nbytes = bytes_to_do - 
		 copy_to_user( buf, /* to */
			       char_device_buf + *ppos, /* from */
			       bytes_to_do ); /* how many bytes */

	*ppos += nbytes;
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
		printk("Write: Reached end of device\n");
		return -ENOSPC; /* Returns EOF at write() */
	}

	nbytes = bytes_to_do -
	         copy_from_user( char_device_buf + *ppos, /* to */
				 buf, /* from */
				 bytes_to_do ); /* how many bytes */
	*ppos += nbytes;
	return nbytes;
}


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


static struct file_operations char_dev_fops = {
	.owner = THIS_MODULE,
	.read = char_dev_read,
	.write = char_dev_write,
	.open = char_dev_open,
	.release = char_dev_release,
	.llseek = char_dev_lseek
};

static int my_pdrv_probe(struct platform_device *pdev)
{
	int ret;
	// MKDEV is used to manually allocate the Major&minor number..for this user has to check which major numer is available by using  cat /proc/devices
	/*mydev = MKDEV(MAJORNO,MINORNO);
	*/
	// (1) unlike MKDEV, this will create major and minor number  dynamically with requested minor number 0
	if (alloc_chrdev_region (&mydev, 0, count, CHAR_DEV_NAME) < 0) {
            printk (KERN_ERR "failed to reserve major/minor range\n");
            return -1;
    }
	// (2) this will allocate memory for device of type cdev
        if (!(dynamic_cdev = cdev_alloc())) {
            printk (KERN_ERR "cdev_alloc() failed\n");
            unregister_chrdev_region (mydev, count);
            return -1;
 	}
	// (3) it intialized the cdev (1st approach)
	/*dynamic_cdev->ops = &char_dev_fops;  // The file_operations structure
	dynamic_cdev->owner = THIS_MODULE;*/
	//2nd approach 
	cdev_init(dynamic_cdev,&char_dev_fops);
	// (4) it add the cdev to the system and system becomes live after this call
	ret=cdev_add(dynamic_cdev,mydev,count);
	if( ret < 0 ) {
		printk(KERN_INFO "Error registering device driver\n");
	        cdev_del (dynamic_cdev);
                unregister_chrdev_region (mydev, count); 	
		return -1;
	}
/*1: device tree will be modified using below call class_create and device_create (device.h)
   2: Modification of device tree will be picked by hot plug daemon 
   3: Now hot plug should trigger a event (an add event 
       because we are creating a device class and device)
   4: Add event should be notifiled to the udev.  
   */
   // (5) instad of using mknod /dev/CHAR_DEV_NAME c majorNo MinorNo ..the following command add the device automaticlally into file system
   //either use one of the existing classes, or create your own with create_class (THIS_MODULE, class_name)
	dynamic_class = class_create (THIS_MODULE, "VIRTUAL"); // VIRTUAL can be any name
	//In order the device to actually apear in the file system, you have to call device_create (class, parent_dev, devno, device_name).	
    device_create (dynamic_class, NULL, mydev, NULL, "%s", CHAR_DEV_NAME);

	printk(KERN_INFO"\nDevice Registered: %s\n",CHAR_DEV_NAME);
	printk (KERN_INFO "Major number = %d, Minor number = %d\n", MAJOR (mydev),MINOR (mydev));
	//memset(char_device_buf,'\0',MAX_LENGTH);
	char_device_buf =(char *)kmalloc(MAX_LENGTH,GFP_KERNEL); // void * kmalloc(size_t size, int flags) declared in <linux/slab.h> The region of memory allocated is physically contiguous.void kfree(const void *ptr)
	return 0;
}

static  int  my_pdrv_remove(struct platform_device *pdev)
{
	 device_destroy (dynamic_class, mydev);
     class_destroy (dynamic_class);
	 cdev_del(dynamic_cdev);
	 unregister_chrdev_region(mydev,1);
	 kfree(char_device_buf);
	 printk(KERN_INFO "\n Driver unregistered \n");
	 return 0;
}

static struct platform_driver mypdrv = {
    .probe      = my_pdrv_probe,
    .remove     = my_pdrv_remove,
    .driver     = {
        .name     = "platform-dummy-char",
        .owner    = THIS_MODULE,
    },
};
module_platform_driver(mypdrv);
//or you can also use it
/*
static int __init init_callback_fn(void)
{
	printk("Initializing platform-dummy-char\n");
	return platform_driver_register(&mypdrv);
}

static void __exit exit_callback_fn(void)
{
	printk("Exiting platform-dummy-char\n");
	platform_driver_unregister(&mypdrv);
}
module_init(init_callback_fn);
module_exit(exit_callback_fn);
*/
// Following macros are definded in linux/module.h
MODULE_LICENSE(LICENSE_TYPE); // It tell the license type since linux kernel is open source, we are definding our moduel as GPL
MODULE_AUTHOR(DRIVER_AUTHOR);// use "Name <email>" or just "Name"
MODULE_DESCRIPTION(DRIVER_DESC);// what is the function of module
MODULE_SUPPORTED_DEVICE("sample_module"); // will decribe in the next examples
