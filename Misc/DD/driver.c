#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>

dev_t dev = 0;
static struct class *dev_cls;
static struct cdev hwm_cdev;


static int __init hwm_init(void);
static void __exit hwm_exit(void);

static int hwm_open(struct inode *inode, struct file *file);
static int hwm_release(struct inode *inode, struct file *file);
static ssize_t hwm_read(struct file *filp, const struct __user *buf, size_t len, loff_t *off);
static ssize_t hwm_write(struct file *filp, const struct __user *buf, size_t len, loff_t *off);

struct file_operations fops = {
  .owner = THIS_MODULE,
  .open = hwm_open,
  .read = hwm_read,
  .write = hwm_write,
  .release = hwm_release
};

static int __init hwm_init(void)
  if (alloc_chrdev_region(&dev, 0, 1, "hello_world_module") < 0) {
    printk(KERN_ERR "Cannot allocate major number for the device\n");
    return -1;
  }
  pr_info("Major %d Minor %d\n", MAJOR(dev), MINOR(dev));

  //initializing character dev structure
  cdev_init(&hwm_cdev, &fops);
  //adding device id to character dev
  if (cdev_add(&hwm_cdev, dev, 1) < 0) {
    pr_info("cannot add device to the system\n");
    goto r_class;
  }
  if (IS_ERR(dev_cls=class_create(THIS_MODULE, "hwm_class"))) {
    pr_err("Cannot create class for the device\n");
    goto r_device;
  }

  if (IS_ERR(device_create(dev_cls, NULL, dev, NULL, "hwm_device"))) {
    pr_err("cannot create device\n");
    goto r_class;
  }
r_class:
	class_destroy(dev_cls);
r_device:
	device_destroy(dev_cls, dev);
	cdev_del(&hwm_cdev);
	unregister_chrdev_region(dev, 1);
}

static void __exit hwm_exit(void)
  class_destroy(dev_cls);
  device_destroy(dev_cls, dev);
  cdev_del(&hwm_cdev);
  unregister_chrdev_region(dev, 1);
}
init_module(hwm_init);
exit_module(hwm_exit);

MODULE_LICENSE("GPL")
MODULE_AUTHOR("RIddick");
MODULE_DESCRIPTION("Simple driver program");
MODULE_VERSION("1.21");
