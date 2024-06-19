/* TIMERS
 * a device counts down from a specified interval is called a timer 
 * Every timer a timer interrupt occurs, the value of an internal
 * kernel counter incremented. The counter is initialized to 0 at
 * system boot, it represents the number of clock ticks since last boot
 * 
 * Timers are used to detect when a device or network peer fails to 
 * respond within expected time.
 * 
 * USES of Kernel Timers
 0 Polling a device by checking	its state at regular interval when h/w
 * can't fire interrupts
 0 User wants to send message to the another device at regular intervals
 0 Send an error when some action didn't happen in a particular time period
 * Kernel Timer API
 *
 * struct timer_list {
 *   unsigned long expires;           :: expiration time in jiffies
 *   void (*function)(unsigned long); :: function called on expiration
 *   unsigned long data;              :: data passed to function
 * };
 * 
 * Ways to setup kernel timers
 * void fastcall init_timer(struct timer_list *timer); :: Here manually initialization of timer structure
 * void setup_timer(timer, func, data);                :: timer, function and data init, Old kernel
 * void timer_setup(timer, func, data);                :: timer, function and data init, New kernel
 *
 * MACROs
 * DEFINE_TIMER(_name, _function, _expires, _data);    :: _name: name of the structure; _expires: expiration time
 *
 * Start a kernel timer
 * void add_timer(struct timer_list *timer);           :: This will start a timer
 * Modify a kernel timer
 * int mod_timer(struct timer_list *timer, unsigned long expires);  :: Mod - 0 inactive timer, 1 active timer
 * Stop a kernel timer
 * int del_timer(struct timer_list *timer);            :: Del - 0 inactive timer, 1 active timer
 * int del_timer_sync(struct timer_list *timer);       :: deactivate and wait for callback ""
 *
 * Callers must not restart the timer, not called from interrupt context
 * Check timer status
 * int timer_pending(const struct timer_list *timer);  :: 0 not pending, 1 pending
 * */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/kobject.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/interrupt.h>
#include <linux/err.h>
#include <asm/io.h>

#define TIMEOUT 5000 //milliseconds
static struct timer_list hwm_timer;
static unsigned int count = 0;

dev_t dev = 0;
static struct class *dev_cls;
static struct cdev hwm_cdev;

static int __init hwm_driver_init(void);
static void __exit hwm_driver_exit(void);

static int hwm_open(struct inode *inode, struct file *file);
static int hwm_release(struct inode *inode, struct file *file);
static ssize_t hwm_read(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static ssize_t hwm_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static long hwm_ioctl(struct file *file, unsigned int cmd, unsigned long value);

static struct file_operations fops = {
	.owner 		= THIS_MODULE,
	.open 		= hwm_open,
	.read 		= hwm_read,
	.write 		= hwm_write,
	.release 	= hwm_release,
};

void timer_cb(struct timer_list *data) {
  pr_info("Timer Callback function called [%d]\n", count++);

  /* modify timer for 5 second interval */
  mod_timer(&hwm_timer, jiffies + msecs_to_jiffies(TIMEOUT));
}

static int hwm_open(struct inode *inode, struct file *file) {
  pr_info("Device file opened..\n");
  return 0;
}

static int hwm_release(struct inode *inode, struct file *file) {
  pr_info("Device file closed..\n");
  return 0;
}

static ssize_t hwm_read(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
  pr_info("Device file read\n");
  return 0;
}

static ssize_t hwm_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);
  pr_info("Device file write\n");
  return 0;
}

static int __init hwm_driver_init(struct inode *inode, struct file *file) {
  /* Get the Major Number */
  /* PROTO int alloc_chrdev_region(dev_t *, unsigned int, unsigned int, const char *); */
  if (alloc_chrdev_region(&dev, 1, 0, "hwm_dev") < 0) {
    pr_info("failed to get MAjor number");
    goto r_unreg;
  }
  pr_info("Major %d Minor %d\n", MAJOR(dev), MINOR(dev));

  /*cdev structure init */
  cdev_init(&hwm_cdev, &fops);

  /* Adding character device to the system */
  if (cdev_add(&hwm_cdev, dev, 1) < 0) {
    pr_info("Unable to add device to the system\n");
    goto r_cdev;
  }

  /* create class
   * PROTO: struct class *class_create(struct module *owner, const char *name); */
  if (IS_ERR(dev_cls = class_create(THIS_MODULE, "hwm_class"))) {
    pr_info("Unable to create class\n");
    goto r_cdev;
  }

  /*create device
   *PROTO: struct device *device_create(struct class *, struct device *parent, dev_t dev, void *data, const char *fmt); */
  if (IS_ERR(device_create(dev_cls, NULL, dev, NULL, "hwm_device"))) {
    pr_info("Unable to create device\n");
    goto r_class;
  }

  /* register interrupt 
   * PROTO: static inline int request_irq(unsigned int irq, irqhandler_t handler, unsigned long irqflags,
   * const char *name, void *dev_id); */

  /* setup timer to callback */
  timer_setup(&hwm_timer, timer_cb, 0);

  /* setup timer based on TIMEOUT value */
  mod_timer(&hwm_timer, jiffies + msecs_to_jiffies(TIMEOUT));

  pr_info("Device Driver insert..Done\n");
r_class:
  class_destory(dev_cls);
r_cdev:
  cdev_del(&cdev);
  unregister_chrdev_region(dev, 1);
r_unreg:
  return -1;
}

static void __exit hwm_driver_exit(struct inode *inode, struct file *file) {
  del_timer(&hwm_timer);
  device_destroy(dev_cls, dev);
  class_destroy(dev_cls);
  cdev_del(&hwm_dev);
  unregister_chrdev_region(dev, 1);
  printk(KERN_INFO "Removing device driver..Done\n");
}

module_init(hwm_driver_init);
module_exit(hwm_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Annonymous expert");
MODULE_DESCRIPTION("kernel timers experiment");
MODULE_VERSION("1.22");
