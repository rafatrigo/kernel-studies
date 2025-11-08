#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt // defines the format of printk when using the macro pr_*

#include <linux/module.h>
#include <linux/init.h>

static int __init init_func(void)
{
	// modern way of doing -> printk(KERN_INFO "hello Kernel!\n");
	pr_info("hello Kernel!\n");
	return 0;
}

static void __exit exit_func(void)
{
	pr_info("bye Kernel!\n");
}

module_init(init_func);
module_exit(exit_func);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("rafatrigo");
MODULE_DESCRIPTION("My first driver");
