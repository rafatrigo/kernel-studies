#include <linux/module.h>
#include <linux/init.h>

static int __init init_func(void)
{
	printk("hello Kernel!\n");
	return 0;
}

static void __exit exit_func(void)
{
	printk("bye Kernel!\n");
}

module_init(init_func);
module_exit(exit_func);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("rafatrigo");
MODULE_DESCRIPTION("My first driver");
