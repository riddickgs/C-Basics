#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>    //dev_t
#include <linux/fs.h>  
#include <linux/device.h>   //create_device
#include <linux/kdev_t.h>  //struct class
#include <linux/cdev.h>    //cdev structure
#include <linux/err.h>     //IS_ERR
#include <linux/slab.h>    //kmalloc
#include <linux/uaccess.h> //copy_from_user
#include <linux/ioctl.h>   //ioctl function
#include <linux/proc_fs.h> //proc_create

#define mem_size 1024
#define WR_VALUE __IOW('a', 'a', int32_t*)
#define RD_VALUE __IOR('a', 'b', int32_t*)
int32_t value = 0;
//statically loading the major and minor number
#ifdef STATIC_MMN
dev_t dev = MKDEV(235, 0);
#else
dev_t dev = 0;
#endif

static struct class *dev_cls;
static struct cdev hwm_cdev;
uint8_t hwm_array[20] = "try_proc_array\n";
uint8_t *kernel_buffer;
static int len = 1;
static struct proc_dir_entry parent;
//function prototypes

static int __init hwm_init(void);
static void __exit hwm_exit(void);

static int hwm_open(struct inode *inode, struct file *file);
static int hwm_release(struct inode *inode, struct file *file);
static ssize_t hwm_read(struct file *filp, const struct __user *buf, size_t len, loff_t *off); 
static ssize_t hwm_write(struct file *filp, const struct __user *buf, size_t len, loff_t *off);

static long hwm_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);

static int open_proc(struct inode *inode, struct file *file);
static int release_proc(struct inode *inode, struct file *file);
static ssize_t read_proc(struct file *procfs, const struct __user *buf, size_t len, loff_t *off);
static ssize_t write_proc(struct file *procfs, const struct __user *buf, size_t len, loff_t *off);

struct file_operations fops = {
  .owner = THIS_MODULE,
  .read = hmw_read,
  .write = hwm_write,
  .open = hwm_open,
  .unlocked_ioctl = hwm_ioctl,
  .release = hwm_release,
};

#if (LINUX_KERNEL_VERSION >505)
//procfs structure
struct proc_ops proc_fops = {
  .proc_open = open_proc,
  .proc_read = read_proc,
  .proc_write = write_proc,
  .proc_release = release_proc,
};

#else
//file_operations structure
struct file_operations proc_fops = {
  .open = open_proc,
  .read = read_proc,
  .write = write_proc,
  .release = release_proc,
}
#endif

static int open_proc(struct inode *inode, struct file *file) {
   pr_info("proc file opened\n");
   return 0;
}

static int release_proc(struct inode *inode, struct file *file) {
  pr_info("proc file released\n");
  return 0;
}

static ssize_t read_proc(struct file *file, const struct __user *buf, size_t length, loff_t *off) {
  pr_info("proc file read\n");
  if (len) { len = 0;}
  else {len=1; return 0;}

  if (copy_to_user(buf, hwm_array, 20)) {
    pr_info("Data send error!\n");
  }
  return length;
} 

static ssize_t write_proc(struct file *file, const struct __user *buf, size_t len, loff_t *off) {
  pr_info("proc file write\n");

  if (copy_from_user(hwm_array, buf, len)) {
    pr_info("proc file write err!\n");
  }
  return len;
}

static int hwm_open(struct inode *inode, struct file *file){
  //allocating the memory here
  if ((kernel_buffer = kmalloc(mem_size, GFP_KERNEL)) == 0) {
    pr_err("Failed to allocate kernel memory\n");
    return -1;
  }
  pr_info("Driver open ffunction is called\n");
  return 0;
}

static int hwm_release(struct inode *inode, struct file *file) {
  //free the allocated buffer
  kfree(kernel_buffer);
  pr_info("Driver release fucntion is called\n");
  return 0;
}

static ssize_t hwm_read(struct file *filp, const struct __user *buf, size_t len, loff_t *off) {
  //this is reading data from kernel space to user space
  if (copy_to_user(buf, kernel_buffer, len)) {
    pr_info("failed to read data\n");
  }
  pr_info("Driver read function is called\n");
  return mem_size;
}

static ssize_t hwm_write(struct file *filp, const struct __user *buf, size_t len, loff_t *off) {
  //this is writing from user space to kernel space
  if (copy_from_user(kernel_buffer, buf, len)) {
    pr_info("failed to write data\n")
  }
  pr_info("Driver write fucntion is called! data write done!\n");
  return len;
}

static long hwm_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
  //ioctl funtion definition
  switch (cmd) {
     case WR_VALUE:
	     if (copy_from_user(&value,(int32_t*)&arg, sizeof(value))){
		     pr_info("failed to write data\n");
             }
	     pr_err("value %d \n", value);
	     break;
     case RD_VALUE:
	     if (copy_to_user((int32_t*)&arg, &value, sizeof (value))) {
	       pr_err("failed to read value\n");
	     }
	     break;
     default:
	     pr_info("default\n");
	     break;
  }
  return 0;
}

static int __init hwm_init(void)
{
  //register major and minor numbers for driver hello_world_module

#ifdef STATIC_MMN
  register_chrdev_region(dev, 1, "hello_world_module");
  printk(KERN_INFO "Major = %d\tMinor = %d\n", MAJOR(dev), MINOR(dev));
#else
  if (alloc_chrdev_region(&dev, 0, 1, "hello_world_module") < 0) {
    printk(KERN_ERR "Cannot allocate major number for the device 1\n");
    return -1; 
  }
  pr_info("Major %d Minor %d", MAJOR(dev), MINOR(dev));
#endif
  cdev_init(&hwm_cdev, &fops);
  if (cdev_add(&hwm_cdev, dev, 1) < 0) {
     pr_info("cannot add device to the system\n");
     goto r_class;
  }

  if (IS_ERR(dev_cls=class_create(THIS_MODULE, "hwm_class"))) {
    pr_err("Cannot create class for the device\n");
    goto r_device;
  }

  if (IS_ERR(device_create(dev_cls, NULL, dev, NULL, "hwm_device"))) {
    pr_err("cannot create device\n")
    goto r_class;
  }
  //This will create /proc directory
  parent = proc_mkdir("hwm", NULL);
  if (parent == NULL) {
     pr_err("failed to create /proc entry\n");
     goto r_device;
  }

  //This will under proc "/proc/hwm"
  proc_create("hwm_proc", 0666, parent, proc_fops);
  strcpy(kernel_buffer, "Hello World!!");
  printk(KERN_INFO "Welcome to simple Driver Program\n");
  printk(KERN_WARNING "simple Driver is initialized successfully\n");
  return 0;

r_class:
	class_destroy(dev_cls);  
r_device:
	proc_remove(parent);
	unregister_chrdev_region(dev, 1);
        cdev_del(&hwm_cdev);
	return -1;
}

static void __exit hwm_exit(void)
{
  proc_remove(parent);
  device_destroy(dev_cls, dev);
  class_destory(dev_cls);
  cdev_del(&hwm_cdev);
  unregister_chrdev_region(dev, 1); //clean up
  printk(KERN_INFO "simple Driver remove successfully");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Riddick");
MODULE_DESCRIPTION("A simple driver (Automatically creating a device file)");
MODULE_VERSION("1:1.0");
