#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/sched/signal.h>
#include <linux/string.h>

#define  DEVICE_NAME "secret_device"
#define  CLASS_NAME  "mod" 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rogov Egor");
MODULE_DESCRIPTION("A module for Linux that creates a character device");
MODULE_VERSION("0.01");

static int      majorNumber;                  
static char     kernel_buffer[20000] = {0};
static char     state_str[50] = {0};
static int      size_of_buffer;              
static int      numberOpens = 0;              
static struct class*  chardrvClass  = NULL;
static struct device* chardrvDevice = NULL;
static DEFINE_MUTEX(char_mutex);   

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

static struct task_struct *task;

static int __init lkm_init(void)
{
    printk(KERN_INFO "Chardrv: Initializing the Char LKM\n");
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0){
        printk(KERN_ALERT "Chardrv failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "Chardrv: registered correctly with major number %d\n", majorNumber);

    // Register the device class
    chardrvClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(chardrvClass)){               
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(chardrvClass);          // Correct way to return an error on a pointer
    }
    printk(KERN_INFO "Chardrv: device class registered correctly\n");

    // Register the device driver
    chardrvDevice = device_create(chardrvClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(chardrvDevice)){               
        class_destroy(chardrvClass);           
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(chardrvDevice);
    }
    printk(KERN_INFO "Chardrv: device class created correctly\n");
    mutex_init(&char_mutex);
    return 0;
}

static void __exit lkm_exit(void)
{
    mutex_destroy(&char_mutex);  
    device_destroy(chardrvClass, MKDEV(majorNumber, 0));
    class_unregister(chardrvClass);
    class_destroy(chardrvClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "Chardrv: Goodbye from the LKM!\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
   if(!mutex_trylock(&char_mutex))
   { 
     printk(KERN_ALERT "Chardrv: device in use by another process");
     return -EBUSY;
   }
   numberOpens++;
   printk(KERN_INFO "Chardrv: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    int error_count = 0;
    size_of_buffer = 0;
    for_each_process(task)
    {
        if (task->state == 0)
            sprintf(state_str, "TASK_RUNNING");
        else if (task->state == 1)
            sprintf(state_str, "TASK_INTERRUPTIBLE");
        else if (task->state == 2)
            sprintf(state_str, "TASK_UNINTERRUPTIBLE");
        else if (task->state == 4)
            sprintf(state_str, "__TASK_STOPPED");
        else if (task->state == 8)
            sprintf(state_str, "__TASK_TRACED");
        else if (task->state == 16)
            sprintf(state_str, "EXIT_DEAD");
        else if (task->state == 32)
            sprintf(state_str, "EXIT_ZOMBIE");
        else if (task->state == 64)
            sprintf(state_str, "TASK_DEAD");
        else if (task->state == 128)
            sprintf(state_str, "TASK_WAKEKILL");
        else if (task->state == 256)
            sprintf(state_str, "TASK_WAKING");
        else if (task->state == 512)
            sprintf(state_str, "TASK_PARKED");
        else if (task->state == 1024)
            sprintf(state_str, "TASK_NOLOAD");
        else if (task->state == 2048)
            sprintf(state_str, "TASK_NEW");
        else if (task->state == 4096)
            sprintf(state_str, "TASK_STATE_MAX");
        else if (task->state == (16 | 32))
            sprintf(state_str, "EXIT_ZOMBIE , EXIT_DEAD");
        else if (task->state == (128 | 2))
            sprintf(state_str, "TASK_WAKEKILL , TASK_UNINTERRUPTIBLE");
        else if (task->state == (128 | 4))
            sprintf(state_str, "TASK_WAKEKILL , __TASK_STOPPED");
        else if (task->state == (128 | 8))
            sprintf(state_str, "TASK_WAKEKILL , __TASK_TRACED");
        else if (task->state == (2 | 1024))
            sprintf(state_str, "TASK_UNINTERRUPTIBLE , TASK_NOLOAD");
        else if (task->state == (1 | 2))
            sprintf(state_str, "TASK_INTERRUPTIBLE , TASK_UNINTERRUPTIBLE");
        else if (task->state == (1 | 2 | 4 | 8))
            sprintf(state_str, "TASK_NORMAL , __TASK_STOPPED , __TASK_TRACED");
        else 
            sprintf(state_str, "OTHER");
        sprintf(kernel_buffer + strlen(kernel_buffer), "PID=%d  PPID=%d  CPU=%d  STATE=%s\n", task->pid, task->real_parent->pid, task_cpu(task), state_str);
        size_of_buffer = strlen(kernel_buffer);
    }
    error_count = copy_to_user(buffer, kernel_buffer, size_of_buffer);
    memset(kernel_buffer, 0, size_of_buffer);

    if (error_count==0)
    {            
        printk(KERN_INFO "Chardrv: Sent %d characters to the user\n", size_of_buffer);
        return (size_of_buffer);
    }
    else 
    {
        printk(KERN_INFO "Chardrv: Failed to send %d characters to the user\n", error_count);
        return -EFAULT;              
    }
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    printk(KERN_ALERT "Operation not supported.\n");
    return -EINVAL;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    mutex_unlock(&char_mutex);
    printk(KERN_INFO "Chardrv: Device successfully closed\n");
    return (0);
}

module_init(lkm_init);
module_exit(lkm_exit);