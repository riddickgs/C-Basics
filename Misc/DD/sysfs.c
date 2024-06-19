#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/kobject.h> //kobject is defined here, glue to bind sysfs and kernel

volatile int hwm_value = 0;

#ifdef STATIC
dev_t dev = MKDEV(235,0);
//DECLARE_WAIT_QUEUE_HEAD(wq_hwm);
#else
dev_t dev = 0; /* Declared here defined later */
//wait_queue_head_t wq_hwm;
#endif
struct kobject *kobj_ref;
/* struct kobject {
 *   char *kname;                 // 
 *   char name[KOBJ_NAME_LEN];    // name of the kobject
 *   struct kref kref;            // provide reference counting
 *   struct list_head entry;      //
 *   struct kobject *parent;      // parent of current kobject
 *   struct kset *kset;           // a group of kobjects all embedded in structure of same type
 *   struct kobj_type *ktype;     // type associated with the kobject
 *   struct dentry *dentry;       // points to a sysfs_dirent structure represents this kobject in sysfs
 * }
 * Creating a directory in /sys
 * struct kobject *kobject_create_and_add(const char *name, struct kobject *parent);
 * 	/sys/kernel/ , parent = kernel_kobj
 *      /sys/firmware, parent = firmware_kobj
 *      /sys/fs,       parent = fs_kobj
 *      /sys/,         parent = NULL
 *      kobject_put(kobj_ref);    // freeing kobject
 * Creating a file in  /sys
 * struct kobj_attribute {
 *   struct attribute attr;       // represents file to be created
 *   ssize_t (*show)(struct kobject *kobj, struct kobj_attribute *attr, char *buf); // read 
 *   ssize_t (*store)(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count); //write
 * }
 * create a attribute
 *    __ATTR(name, permission, show_ptr, store_ptr);
 *
 *    sysfs_create_file(struct kobject *kobj, const struct attribute *attr);
 *    sysfs_remove_file(struct kobject *kobj, const struct attribute *attr);
 * */
static struct class *dev_cls;
static struct cdev hwm_cdev;
int wait_queue_flag = 0;

static int    __init hwm_driver_init(void);
static void   __exit hwm_driver_exit(void);

static int hwm_open(struct inode *inode, struct file *file);
static int hwm_release(struct inode *inode, struct file *file);
static ssize_t hwm_read(struct file *flp, char __user *buf, size_t len, loff_t *off);
static ssize_t hwm_write(struct file *flp, const char *buf, size_t len, loff_t *off);

static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);

struct kobj_attribute hwm_attr = __ATTR(hwm_value, 0666, sysfs_show, sysfs_store);

static file_operations fops = {
   .module = THIS_MODULE,
   .read = hwm_read,
   .write = hwm_write,
   .open = hwm_open,
   .release = hwm_release,
};

static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
  pr_info("Reading sysfs\n");
  return sprintf(buf, "%d", hwm_value);
}

static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
{
   pr_info("Writing sysfs\n");
   sscanf(buf, "%d", &hwm_value);
   return count;
}
static int hwm_open(struct inode *inode, struct file *file)
{
  pr_info("Device file opened\n");
  return 0;
}
static int hwm_release(struct inode *inode, struct file *file)
{
  pr_info("Device file closed\n");
  return 0;
}
static ssize_t hwm_read(struct file *flp, char __user *buf, size_t len, loff_t *off)
{
  pr_info("Read function\n");
  //wait_queue_flag = 1;
  //wake_up_interruptible(&wq_hwm);
  return 0;
}
static ssize_t hwm_write(struct file *flp, const char *buf, size_t len, loff_t *off)
{
  pr_info("Write function\n");
  return len;
}

static int    __init hwm_driver_init(void) 
{
#ifdef STATIC
  register_chrdev_region(dev, 1, "hwm");
  printk(KERN_INFO, "Major %d Minor %d\n", MAJOR(dev), MINOR(dev));
#else
  if (alloc_chrdev_region(&dev, 0, 1, "hwm") < 0) {
    printk(KERN_ERR "cannot allocate major number for the device 1\n");
    return -1;
  }
  pr_info("Major %d Minor %d\n", MAJOR(dev), MINOR(dev));
  //init_wait_queue_head(&wq_hwm);
#endif
  cdev_init(&hwm_cdev, &fops);
  //hwm_cdev.owner = THIS_MODULE;
  //hwm_cdev.ops = &fops;

  if (cdev_add(&hwm_cdev, dev, 1) < 0) {
    pr_err("cannot add device to the system\n");
    goto r_class;
  }
  if (IS_ERR(dev_cls = class_create(THIS_MODULE, "hwm_class"))) {
    pr_info("cannot create the struct class\n");
    goto r_class;
  }

  if (IS_ERR(device_create(dev_cls, 1, NULL, dev, NULL,"hwm_device"))) {
    pr_info("Cannot create device\n");
    goto r_device;
  }

  kobj_ref = kobject_create_and_add("hwm_sysfs", kernel_kobj);  //creating directory /sys/kernel/hwm_sysfs/
  if (sysfs_create_file(kobj_ref, &hwm_attr.attr)) {
    pr_info("cannot create sysfs file ..\n");
    goto r_sysfs;
  }
  pr_info("Driver insertion .... Done\n");
  return 0;

r_sysfs:
  kobject_put(kobj_ref); //free kobject
  sysfs_remove_file(kobj_ref, &hwm_attr.attr); //remove file under /sys/kernel/hwm_sysfs/

r_device:
  class_destroy(dev_cls); //free class

r_class:
  unregister_devchr_region(dev, 1); //free dev
  return -1;
}

static void   __exit hwm_driver_exit(void)
{
  //wait_queue_flag = 2;
  //wake_up_interruptible(&wq_hwm);
  kobject_put(kobj_ref); //free kobject
  sysfs_remove_file(kobj_ref, &hwm_attr.attr); //remove file under /sys/kernel/hwm_sysfs/
  device_destory(dev_cls, dev);
  class_destroy(dev_cls);
  cdev_del(&hwm_cdev);
  unregister_chrdev_region(dev, 1);
  pr_info("Device Driver remove ...Done");
}

init_module(hwm_driver_init);
exit_module(hwm_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Annonymous device Driver expert");
MODULE_DESCRIPTION("Linux Device driver (wait queue static method)");
MODULE_VERSION(1:1.0);
