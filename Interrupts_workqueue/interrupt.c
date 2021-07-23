#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
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

struct work_struct *wq;
static int irq = SHARED_IRQ, my_dev_id, irq_counter = 0;
module_param (irq, int, S_IRUGO);	//to show in /sys/module/ only

void t_fun(struct work_struct *work) 
{
	printk(KERN_INFO "Entering t_fun, jiffies = %ld\n", jiffies);
	printk(KERN_INFO "Entering t_fun, jiffies = %ld\n", jiffies);
}
//(2)IRQ
static irqreturn_t my_interrupt (int irq, void *dev_id)
{
	// top half
    irq_counter++;
    printk (KERN_INFO "In the ISR: counter = %d, id= %d\n", irq_counter, (int*)dev_id);
	// bottom half --optional
	/*
	workqueue are process context, If the deferred work needs to sleep, then workqueue is used.
	If the deferred work need not sleep, then softirq or tasklet are used. they are preemtible	
	We can create a workqueue in Two ways. static and dynamic method.
	*/
	schedule_work(wq);
	return IRQ_HANDLED;
    //return IRQ_NONE;            /* we return IRQ_NONE because we are just observing */
}

static int __init my_init (void)
{
	// (1)IRQ
    if (request_irq(irq, my_interrupt, IRQF_SHARED, "my_interrupt", &my_dev_id))
        return -1;
    printk (KERN_INFO "Successfully loading ISR handler\n");
	
    //struct work_data * data;
    //wq = create_workqueue("wq_test");	// dynamic way..there is static way (https://embetronicx.com/tutorials/linux/device-drivers/workqueue-in-linux-kernel/)
    //data = kmalloc(sizeof(struct work_data), GFP_KERNEL);
    INIT_WORK(wq, t_fun);
	
	schedule_work(wq);
   // queue_work(wq, &data->work);	
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
