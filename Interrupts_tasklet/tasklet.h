extern void t_fun(unsigned long arg);
 
struct my_dat {
unsigned long jiffies; /* used for timestamp */
struct tasklet_struct tsk; /* used in dynamic tasklet solution */
struct work_struct work; /* used in dynamic workqueue solution */
};
struct my_dat my_data;
// static declaration (https://embetronicx.com/tutorials/linux/device-drivers/tasklet-static-method/)
/* Init the Tasklet by Static Method */
//DECLARE_TASKLET(tasklet,tasklet_fn, 1);
DECLARE_TASKLET (t_name, t_fun, (unsigned long)&my_data);
//for dynamic dclaration
/*
// Tasklet by Dynamic Method 
struct tasklet_struct *tasklet;
//Init the tasklet bt Dynamic Method /
tasklet  = kmalloc(sizeof(struct tasklet_struct),GFP_KERNEL);
if(tasklet == NULL) {
    printk(KERN_INFO "etx_device: cannot allocate Memory");
}
tasklet_init(tasklet,tasklet_fn,(unsigned long)&my_data);
*/
