/* COMPLETION
 * Completion are made by using waitqueue and is used for synchronization between kernel threads
 *
 * 5 steps in Completion
 1 Initializing completion in linux
 2 Reinitializing completion in linux
 3 Waiting for completion (code is waiting and sleeping for something to finish)
 4 Wake up Task (Sending a signal to sleeping part)
 5 Check the status
 * 
 * Structure:
 * struct completion {
 *   unsigned int done;      :: completion flag
 *   wait_queue_head_t wait; :: waitqueue to place task on for waiting
 * }
 *
 * Initialization
 *  [STATIC] DECLARE_COMPLETION(data_read_done);
 *  [DYNAMIC] init_completion(struct completion *x);
 * ReInitialization
 *  reinit_comletion(struct completion *x);
 *
 * Waiting for completion
 *
 *  void wait_for_completion(struct completion *x); :: waits to be signalled |NOT interruptible and no timeout
 *  unsigned long wait_for_completion_timeout(struct completion *x, unsigned long timeout); :: where, "x" state of this completion, "timeout" timeout in jiffies, returns 0 if timed out else remaining jiffies
 *
 *  int wait_for_completion_interruptible(struct completion *x); :: returns -ERESTARTSYS if interrupted, 0 if completed
 *
 *  long wait_for_completion_interruptible_timeout(struct completion *x, unsigned long timeout); :: where, "x" state of this completion, "timeout" timeout in jiffies, returns -ERESTARTSYS if interrupted, 0 if timed out, positive if completed but number of jiffies left till timeout
 *
 *  int wait_for_completion_killable(struct completion *x); :: retuns -ERESTARTSYS if interrupted, 0 if completed
 *  long wait_for_completion_killable_timeout(struct completion *x, unsigned long timeout); :: where, -ERESTARTSYS if interrupted, 0 if timed out, positive if completed and number of jiffies left till timeout
 *  bool try_wait_completion(struct completion *x); :: returns 0 if completion not available, 1 if succeeded (IRQ)
 *
 * Waking up the task
 * void complete(struct completion *x);     :: wake up a single thread waiting on the queue
 * void complete_all(struct completion *x); :: wake up all the threads waiting on the queue
 *
 * Check the status
 * bool completion_done(struct completion *x); :: 0 if waiters, 1 if no waiters
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
#include <linux/err.h>
#include <asm/io.h>

#include <linux/kthread.h>
#include <linux/completion.h>

uint32_t read_count = 0;
static struct task_struct *wait_thread;

#ifdef STATIC
DECLARE_COMPLETION(data_read_done);
#else
static struct completion data_read_done;
#endif

dev_t dev = 0;
static struct class *dev_cls;
static struct cdev hwm_cdev;
int completion_flag = 0;

static int __init hwm_driver_init(void);
static void __exit hwm_driver_exit(void);

static int hwm_open(struct inode *inode, struct file *file);
static int hwm_release(struct inode *inode, struct file *file);
static ssize_t hwm_read(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static ssize_t hwm_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);

static struct file_operations fops = {
	.owner 		= THIS_MODULE,
	.open 		= hwm_open,
	.read 		= hwm_read,
	.write 		= hwm_write,
	.release 	= hwm_release,
};

static int wait_function(void *unused) {
  while (1) {
    pr_info("Waiting for event\n");
    wait_for_completion(&data_read_done);
    if (completion_flag == 2) {
      pr_info("Event came from exit function\n");
      return 0;
    }
    pr_info("Event came from Read function %d\n", ++read_count);
    completion_flag = 0;
  }
  do_exit();
  return 0;
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
  completion_flag = 1;
  if (!completion_done(&data_read_done))
    complete(&data_rad_done);
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

  wait_thread = kthread_run(wait_function, NULL, "completion thread");
  if (wait_thread) {
    pr_info("Thread created successfully\n");
  } else {
    pr_info("Thread creation failed\n");
  }


#ifndef STATIC
  /* Initialization of completion struture*/
  init_completion(&data_read_done);
#endif

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
  completion_flag = 2;
  if (!completion_done(&data_read_done))
    complete(&data_read_done);
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
MODULE_DESCRIPTION("completion experiment");
MODULE_VERSION("1.23");
