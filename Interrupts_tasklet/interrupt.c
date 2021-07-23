#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include "tasklet.h"
// bottom half --
/*
Workqueue – Executed in a process context.
Threaded IRQs
Softirqs – Executed in an atomic context.
Tasklet in Linux – Executed in an atomic context.
*/
#define SHARED_IRQ 12 
#define LICENSE_TYPE "GPL"
#define DRIVER_AUTHOR "Muhammad Aslam aslaam.shafee@gmail.com"
#define DRIVER_DESC   "First Module"
extern void t_fun(unsigned long t_arg);
static int irq = SHARED_IRQ, my_dev_id, irq_counter = 0;
module_param (irq, int, S_IRUGO);	//to show in /sys/module/ only
//(2)IRQ
static irqreturn_t my_interrupt (int irq, void *dev_id)
{
	// top half
    irq_counter++;
    printk (KERN_INFO "In the ISR: counter = %d, id= %d\n", irq_counter, (int*)dev_id);
	// bottom half --optional
	/*
	Tasklets are atomic, so we cannot use sleep() and such synchronization primitives like mutexes, semaphores, etc. from them. 
	But we can use spinlock. Tasklets are executed by the principle of non-preemptive scheduling, one by one, in turn.
	We can create a tasklet in Two ways. static and dynamic method.
	*/
    tasklet_schedule(&t_name);
	return IRQ_HANDLED;
    //return IRQ_NONE;            /* we return IRQ_NONE because we are just observing */
}

static int __init my_init (void)
{
	// (1)IRQ
    if (request_irq(irq, my_interrupt, IRQF_SHARED, "my_interrupt", &my_dev_id))
        return -1;
    printk (KERN_INFO "Successfully loading ISR handler\n");
    return 0;
}

static void __exit my_exit (void)
{
    synchronize_irq (irq);
    free_irq (irq, &my_dev_id);
    printk (KERN_INFO "Successfully unloading \n");
}

module_init (my_init);
module_exit (my_exit);

// Following macros are definded in linux/module.h
MODULE_LICENSE(LICENSE_TYPE); // It tell the license type since linux kernel is open source, we are definding our moduel as GPL
MODULE_AUTHOR(DRIVER_AUTHOR);// use "Name <email>" or just "Name"
MODULE_DESCRIPTION(DRIVER_DESC);// what is the function of module
MODULE_SUPPORTED_DEVICE("sample_module"); // will decribe in the next examples
