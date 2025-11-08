#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt // defines the format of printk when using the macro pr_*

#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h> // needed for parameters
#include <linux/kernel.h> // good practice include for printk, even if it alredy includd by module.h

static int my_param =  0;

// variable name
// variable type
// permission
module_param(my_param, int, 0444);

//  parameter description
MODULE_PARM_DESC(my_param, "Just an int value.");

static int __init init_func(void)
{
	// moder way of doing -> printk(KERN_INFO "Driver loaded with value: %d\n", my_param);
	pr_info("Driver loaded with value: %d\n", my_param);
	return 0;
}

static void __exit exit_func(void)
{
	pr_info("Driver unloaded with value: %d\n", my_param);
}

module_init(init_func);
module_exit(exit_func);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("rafatrigo");
MODULE_DESCRIPTION("My first module with parameter");

