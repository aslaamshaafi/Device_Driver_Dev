/*
 *  First module to learn how to write hello world in linux kernel module
 */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>

#define LICENSE_TYPE "GPL"
#define DRIVER_AUTHOR "Muhammad Aslam aslaam.shafee@gmail.com"
#define DRIVER_DESC   "First Module"

static int var_vector = 4;
static char *var_string = DRIVER_DESC;
static int var_intArray[4] = { 1, 2,3,4 };
static int arr_nump = 0;

/*  Like we do in main() with argc/argv, modules can take arguments using module_param(name,type, perm) macro defind in linux/moduleparam.h
 * @name:varilabe name used as exposed parameter
 * @type: type of @name
 * @perm: permission in procfs
	S_IRUSR | S_IWUSR | S_IXUSR --> owner of the file sys, read, write and execute
	S_IRGRP | S_IWGRP | S_IXGRP--> group wner of the file sys, read, write and execute
	S_IROTH | S_IWOTH | S_IXOTH--> other user of the file sys, read, write and execute
MODULE_PARM_DESC(_parm, desc) is used to describe how to use the parameter	
module_param_array(name,type,nump ,perm) same as of module_param() but it is for array
@nump: optional pointer can be make null	
 */

module_param(var_vector, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
MODULE_PARM_DESC(var_vector, "variable of integer type");
module_param(var_string, charp, S_IRUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(var_string, "A char paramter");

module_param_array(var_intArray, int, &arr_nump, S_IRUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(var_intArray, "An array of integers");
/*__init (optional) instruct the init function runs only once (initialization). After it runs, the kernel can free this memory to reuse it and you will see the kernel message:*/
//static int hello_data __initdata = 3;
/*There is also an __initdata which works similarly to __init but for init variables rather than functions.*/
//static int __init init_module(void) // call at the time of insmod /* A non 0 return means init_module failed; module can't be loaded. */
static int  init_hello(void) // call at the time of insmod /* A non 0 return means init_module failed; module can't be loaded. */
{
	int i;
	//printk(KERN_ALERT "Hello, world %d\n", hello_data);
	printk(KERN_INFO "init_module of %s  is called\n",DRIVER_DESC);
	printk(KERN_INFO "var_vector is an integer: %d\n", var_vector);
	printk(KERN_INFO "var_string is a string: %s\n", var_string);
	for (i = 0; i < (sizeof var_intArray / sizeof (int)); i++)
	{
		printk(KERN_INFO "var_intArray[%d] = %d\n", i, var_intArray[i]);
	}
	printk(KERN_INFO "got %d arguments for var_intArray.\n", arr_nump);
	return 0;
}

//static void __exit exit_module(void) // call at the time of rmmod
static void exit_hello(void) // call at the time of rmmod
{
	printk(KERN_INFO "exit_module of %s  is called\n",DRIVER_DESC);
}

module_init(init_hello);
module_exit(exit_hello);

//(1) Following macros are definded in linux/module.h
MODULE_LICENSE(LICENSE_TYPE); // It tell the license type since linux kernel is open source, we are definding our moduel as GPL
MODULE_AUTHOR(DRIVER_AUTHOR);// use "Name <email>" or just "Name"
MODULE_DESCRIPTION(DRIVER_DESC);// what is the function of module
MODULE_SUPPORTED_DEVICE("sample_module"); // will decribe in the next examples