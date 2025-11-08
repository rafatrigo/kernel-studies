#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt // defines the format of printk when using the macro pr_*

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h> // file_operations
#include <linux/cdev.h> // cdev_init, cdev_add
#include <linux/uaccess.h> // copy_to_user, copy_from_user
#include <linux/device.h> // class_create, device_create

#define DEVICE_NAME "my_device_name" // name of device that will appear in /dev/
#define CLASS_NAME "mychardev"
#define MSG_BUFFER_LEN 256

// Global variables
static int major_number; // major number assigned to our device driver
static dev_t dev_num; // device number (major + minor)
static struct cdev my_cdev; // character device structure
static struct class* mychar_class = NULL; // device class
static struct device* mychar_device = NULL; // device structure

static const  char *msg_to_user = "You read from the device!\n";
static int msg_size;
static char kernel_buffer[MSG_BUFFER_LEN];

// Function prototypes
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

// File operations structure
static struct file_operations fops =  {
    .owner = THIS_MODULE, // prevents unloading of module when operations are in use
    .open = dev_open, // called when the device is opened; when syscall open() is made call to dev_open
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init mychar_init(void)
{
    // Allocate a major number dynamically
    if(alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME) < 0)
    {
        pr_alert("Failed to allocate a major number\n");
        return -1;
    }
    major_number = MAJOR(dev_num); // extract major number
    pr_info("Registered correctly with major number %d\n", major_number);

    // Initialize the cdev structure
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;

    // Add the character device to kernel
    if(cdev_add(&my_cdev, dev_num, 1) < 0)
    {
        pr_alert("Failed to add cdev\n");
        // release a range of character device numbers that were previously allocated using alloc_chrdev_region
        unregister_chrdev_region(dev_num, 1);
        return -1;
    }
    pr_info("Character device added successfully\n");

    // Create device class
    mychar_class = class_create(CLASS_NAME);
    if (IS_ERR(mychar_class)) // IS_ERR - checks for error pointers
    {
        pr_alert("Failed to register device class\n");
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(mychar_class);
    }
    pr_info("Device class registered correctly\n");

    // Create device node in /dev/
    mychar_device = device_create(mychar_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(mychar_device))
    {
        pr_alert("Failed to create the device\n");
        class_destroy(mychar_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(mychar_device);
    }
    pr_info("Device created correctly\n");

    msg_size = strlen(msg_to_user);
    return 0;
}

static void __exit mychar_exit(void)
{
    // inverted order of cleanup
    device_destroy(mychar_class, dev_num); // remove the device
    class_destroy(mychar_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);
    pr_info("Goodbye from the character device driver!\n");
}

// Called when the device is opened
static int dev_open(struct inode *inodep, struct file *filep)
{
    pr_info("Device has been opened\n");
    return 0;
}

// Called when the device is closed/released
static int dev_release(struct inode *inodep, struct file *filep)
{
    pr_info("Device successfully closed\n");
    return 0;
}

// Called when the device is read from user space
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    int bytes_to_read;

    if(*offset > 0)
    {
        return 0; // EOF - End Of File
    }

    bytes_to_read = msg_size < len ? msg_size : len;

    //msg_to_user is in kernel space, buffer is in user space
    // we need to use copy_to_user to transfer data from kernel space to user space
    if(copy_to_user(buffer, msg_to_user, bytes_to_read) != 0)
    {
        pr_err("Failed to send data to user\n");
        return -EFAULT; // Bad address error
    }

    *offset += bytes_to_read;
    pr_info("Sent %d bytes to the user\n", bytes_to_read);
    return bytes_to_read;
}

// Called when the device is written to from user space
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    size_t bytes_to_write = len < (MSG_BUFFER_LEN - 1) ? len : (MSG_BUFFER_LEN - 1);

    // buffer is in user space, kernel_buffer is in kernel space
    // we need to use copy_from_user to transfer data from user space to kernel space
    if(copy_from_user(kernel_buffer, buffer, bytes_to_write) != 0)
    {
        pr_err("Failed to receive data from user\n");
        return -EFAULT; // Bad address error
    }

    kernel_buffer[bytes_to_write] = '\0'; // Null-terminate the string
    pr_info("Received %zu bytes from the user: %s\n", bytes_to_write, kernel_buffer);
    return bytes_to_write;
}

// Register module entry and exit points
module_init(mychar_init);
module_exit(mychar_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("rafatrigo");
MODULE_DESCRIPTION("A simple Linux character device driver");