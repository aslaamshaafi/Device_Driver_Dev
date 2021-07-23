#include <linux/module.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/init.h>
#include "tasklet.h"

#define LICENSE_TYPE "GPL"
#define DRIVER_AUTHOR "Muhammad Aslam aslaam.shafee@gmail.com"
#define DRIVER_DESC   "First Module"

typedef struct simp_t {
	int i;
	int j;
} simp;
void t_fun(unsigned long);
EXPORT_SYMBOL(t_fun);

static simp t_data;
void t_fun(unsigned long t_arg) 
{
	simp *datum = (simp *)t_arg;
	printk(KERN_INFO "Entering t_fun, datum->i = %d, jiffies = %ld\n",datum->i, jiffies);
	printk(KERN_INFO "Entering t_fun, datum->j = %d, jiffies = %ld\n",datum->j, jiffies);
}

//DECLARE_TASKLET (t_name, t_fun, (unsigned long)&my_data);

static int __init my_init(void) 
{
	printk(KERN_INFO "\nHello: my_init loaded at address 0x%p\n",my_init);
	t_data.i = 100;
	t_data.j = 200;
	printk(KERN_INFO "scheduling my tasklet, jiffies = %ld\n", jiffies);
	tasklet_schedule(&t_name);
	return 0;
}

static void __exit my_exit(void) 
{
	printk(KERN_INFO "\nHello: my_exit loaded at address 0x%p\n",my_exit);
}

module_init(my_init);
module_exit(my_exit);
// Following macros are definded in linux/module.h
MODULE_LICENSE(LICENSE_TYPE); // It tell the license type since linux kernel is open source, we are definding our moduel as GPL
MODULE_AUTHOR(DRIVER_AUTHOR);// use "Name <email>" or just "Name"
MODULE_DESCRIPTION(DRIVER_DESC);// what is the function of module
MODULE_SUPPORTED_DEVICE("sample_module"); // will decribe in the next examples