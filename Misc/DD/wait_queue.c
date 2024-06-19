#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/kobject.h> //kobject is defined here, glue to bind sysfs and kernel

uint32_t read_count = 0;
static struct task_struct *wait_thread;
#ifdef STATIC
dev_t dev = MKDEV(235,0);
DECLARE_WAIT_QUEUE_HEAD(wq_hwm);
#else
dev_t dev = 0; /* Declared here defined later */
wait_queue_head_t wq_hwm;
#endif

/* once wait queue is initialized, a process may use it to go to sleep.
 *  Below macros are available for different uses
 *  Process is put to sleep (TASK_UNINTERRUPTIBLE) until the
 *  condition is true. Condition is checked everytime wq is woken up
 *  wait_event(wq, condition) sleep until condition gets true
 *  wait_event_timeout(wq, condition, timeout) condition true or timeout elapses
 *  wait_event_cmd(wq, condn, cmd1, cmd2) condition true, cmd1 before sleep, cmd2 after sleep
 *  wait_event_interruptible(wq, condition) condition true or signal received 
 *  wait_event_interruptible_timeout(wq, condn, timeout) condition true or signal received or timeout elapsed
 *  wait_event_killable(wq, condition) condition true or signal is received
 * 
 * WAKE UP queued task
 * wake_up(&wq) wake up a non-interruptible slept process from wait_queue
 * wake_up_all(&wq) wake up all the processes from wait_queue
 * wake_up_interruptible(&wq) wake up a interruptible slept process from the wait queue
 * wake_up_sync(&wq) and wake_up_interruptible_sync(&wq)
 * wake up call cause immediate reschedule to happen means other processes might run before wake up returns
 * synchronous variant make awakened process runnable
 * process cycle: New -> Ready -> Running -> wait -> Ready -> Running -> Terminated */

static struct class *dev_cls;
static struct cdev hwm_cdev;
int wait_queue_flag = 0;

static int    __init hwm_driver_init(void);
static void   __exit hwm_driver_exit(void);

static int hwm_open(struct inode *inode, struct file *file);
static int hwm_release(struct inode *inode, struct file *file);
static ssize_t hwm_read(struct file *flp, char __user *buf, size_t len, loff_t *off);
static ssize_t hwm_write(struct file *flp, const char *buf, size_t len, loff_t *off);

static file_operations fops = {
   .module = THIS_MODULE,
   .read = hwm_read,
   .write = hwm_write,
   .open = hwm_open,
   .release = hwm_release,
};

/* Thread function */
static int wait_function(void *unused) {
   while(!kthread_should_stop()) {
     pr_info("waiting for Event..\n");
     wait_event_interruptible(wq_hwm, wait_queue_flag!=0);
     if (wait_queue_flag == 2) {
       pr_info("wait came from exit function\n");
       return 0;
     }

     pr_info("Event came from read function - %d\n", read_count++);
     wait_queue_flag = 0;
     msleep(1000);
   }
   do_exit();
   return 0;
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
  wait_queue_flag = 1;
  wake_up_interruptible(&wq_hwm);
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
#endif
  cdev_init(&hwm_cdev, &fops);
  hwm_cdev.owner = THIS_MODULE;
  hwm_cdev.ops = &fops;

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

  init_wait_queue_head(&wq_hwm);
  wait_thread = kthread_run(wait_function, NULL, "wait_thread");
  if (wait_thread) {
    pr_info("Thread created successfully\n");
    wake_up_process(wait_thread);
  } else {
    pr_err("Thread creation failed\n");
  }
  pr_info("Driver insertion .... Done\n");
  return 0;

r_device:
  class_destroy(dev_cls);

r_class:
  unregister_devchr_region(dev, 1);
  return -1;
}

static void   __exit hwm_driver_exit(void)
{
  wait_queue_flag = 2;
  wake_up_interruptible(&wq_hwm);
  kthread_stop(wait_thread);
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
