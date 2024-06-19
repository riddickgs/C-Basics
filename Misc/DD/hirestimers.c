/* HRTIMERS
 * HR-Timers maintaining a time-ordered data structure of timer
 * (Timers are inserted in time order to minimize processing
 *  at activation time) DS is Red-Black Tree
 * Users of HR Timers <<linux/hrtimer.h>>
 * struct hrtimer {
 *   struct rb_node node;                :: red black tree node for time ordered insertion 
 *   ktime_t expires;                    :: absolute expiry time
 *   int (* function)(struct hrtimer *); :: callback function returns HRTIMER_(NORESTART/RESTART) on onshot/recurring
 *   struct hrtimer_base *base;          :: pointer to timer base (per CPU per clock)
 * };                                    :: initialized by init_hrtimer_#CLOCKTYPE
 *
 * Kernel Timer API
 * ktime_t ktime_set(long secs, long nanosecs);
 * void hrtimer_init(struct hrtimer *timer, clockid_t clockid, enum hrtimer_mode mode);
 * where, "timer" to be initialized, "clockid" CLOCK_MONOTONIC/CLOCK_REALTIME, "mode" HRTIMER_MODE_(ABS/REL)
 *
 * Start high resolution timer
 * int hrtimer_start(struct hrtimer *timer, ktime_t time, enum hrtimer_mode mode);
 * where, "timer" to be added, "time" expiry time, "mode" HRTIMER_MODE_(ABS/REL), returns 0 on success 1 on active
 * This call is used to (re)start an hrtimer on the current CPU.
 *
 * Stop high resolution timer
 * int hrtimer_cancel(struct hrtimer *timer); :: returns 0 on inactive, 1 on active
 * int hrtimer_try_to_cancel(struct hrtimer *timer); :: try to deactivate, returns 0 on inactive, 1 on active, -1 callback
 *
 * Changing high resolution timer's timeout
 * u64 hrtimer_forward(struct hrtimer *timer, ktime_t now, ktime_t interval);
 * u64 hrtimer_forward_now(struct hrtimer *timer, ktime_t interval);
 * where, "timer" timer to forward, "now" forward past this time, "interval" interval to forward, returns no of overruns
 *
 * Check high resolution timer
 * ktime_t hrtimer_get_remaining(const struct hrtimer *timer); :: used to get remainging time of the timer
 * int hrtimer_callback_running(struct hrtimer *timer);        :: whether timer is running callback function [0,1]
 * ktime_t hrtimer_cb_get_time(struct hrtimer *timer);         :: get current time of the given timer
 *
 * Note: timers execuet from interrupt context, check using in_interrupt() function
 * 
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
#include <linux/ktime.h>
#include <linux/hrtimer.h>
#include <linux/interrupt.h>
#include <linux/err.h>
#include <asm/io.h>

#define TIMEOUT_SEC	(4)
#define TIMEOUT_NSEC	(1000000000L)
static struct hrtimer hwm_hrtimer;
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

enum hrtimer_restart hrtimer_cb(struct hrtimer *timer) {
  pr_info("HR timer Callback function called [%d]\n", count++);

  /* forward timer for 5 seconds interval */
  hrtimer_forward_now(timer, ktime_set(TIMEOUT_SEC, TIMEOUT_NSEC));
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
  if (request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "hwm_device", (void *)(irq_handler)) < 0) {
    pr_info("Failed to register IRQ\n");
    goto irq;
  }

  ktime_t ktime = ktime_set(TIMEOUT_SEC, TIMEOUT_NSEC);
  /* Initialize hrtimer */
  hrtimer_init(&hwm_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
  /* setup hrtimer to callback */
  hwm_hrtimer.function = hrtimer_cb;
  /* Start hrtimer */
  hrtimer_start(&hwm_hrtimer, ktime, HRTIMER_MODE_REL);

  pr_info("Device Driver insert..Done\n");
irq:
  free_irq(IRQ_NO, irq_handler);
r_class:
  class_destory(dev_cls);
r_cdev:
  cdev_del(&cdev);
  unregister_chrdev_region(dev, 1);
r_unreg:
  return -1;
}

static void __exit hwm_driver_exit(struct inode *inode, struct file *file) {
  /* Delete hrtimer */
  hrtimer_cancel(&hwm_hrtimer);
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
