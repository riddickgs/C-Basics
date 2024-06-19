/* Part of workqueue file workqueue.c
 * Workqueue - calls from process context, schedulable
 * two ways to implement workqueue in linux
 * 1. Global work queue (Static/Dynamic) //here workqueue is not created, kernel-global workqueue is used
 * 2. creating own work queue
 *
 * CREATING OWN WORK QUEUE
 * core workqueue is created via structure workqueue_struct in which work will be placed
 * work will be added to the queue in the top half (interrupt context) and
 * execution will happen in the bottom half (Kenel context)
 * CREATE AND DESTROY WORKQUEUE
 * struct workqueue_struct *create_workqueue(name);
 * struct workqueue_struct *create_singlethread_workqueue(name);
 * #define create_workqueue(name)
 *   alloc_workqueue("%s", WQ_MEM_RECLAIM, 1, (name));
 * #define create_singlethread_workqueue(name)
 *   alloc_workqueue("%s", WQ_UNBOUND|WQ_MEM_RECLAIM, 1, (name));
 *                    fmt     flags                max_active
 * WQ_ *flags
 *  WQ_UNBOUND     - unbound wq are served by the special worker pools which host workers are not bound to any specific CPU
 *  WQ_FREEZABLE   - freezable wq participates in the freeze phase of the system suspend operations
 *  WQ_MEM_RECLAIM - all wq might be used in the memory reclaim paths must have this flag set
 *  WQ_HIGHPRI     - work items of HIGHPRI wq are queued to the highpri worker pool of the target CPU.
 *                   Highpri worker-pools are served by worker threads with elevated nice level
 *  WQ_CPU_INTENSIVE - runnable CPU-intensive work items will not prevent other work items 
 *                     in the same worker pool from starting execution.
 *  max active     - determines the maximum number of execution context per CPU
 *                   which can be assigned to the work items of a wq. This is per CPU attribute. Default 0[256]
 *                   @max_active 16, at most 16 work items of the wq can be executing at the same time per CPU.
 * Unless work items are expected to consume a huge amount of CPU cycles, using a bound wq is usually beneficial due to the increased level of locality in wq operations and work item execution
 * void destroy_workqueue(struct workqueue_struct *);
 *
 * QEUEING WORK to workqueue
 * int queue_work(struct workqueue_struct *wq, struct work_struct *work);
 * int queue_work_on(int cpu, struct workqueue_struct *wq, struct work_struct *work);
 * int queue_delayed_work(struct workqueue_struct *wq, struct work_struct *work, unsigned long delay);
 * int queue_delayed_work_on(int cpu, struct workqueue_struct *wq, struct work_struct *work, unsigned long delay);
 *
 *
 * */

#include <linux/kernel.h>
#include <linux/init.h>      //init function
#include <linux/module.h>    //module macros
#include <linux/kdev_t.h>
#include <linux/kobject.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <asm/io.h>
#include <linux/workqueue.h>  //required for workqueue

#define IRQ_NO 11
/* Declaring own workqueue structure variable */
static struct workqueue_struct *own_wq;

#ifdef STATIC
/* Creating work by static method */
static DECLARE_WORK(workqueue, workqueue_fn);
dev_t dev = MKDEV(235,0);
#else
/* Declaring Work structure */
static struct work_struct workqueue;
dev_t dev = 0; /* Declared here defined later */
#endif

void workqueue_fn(struct work_struct *work);

/* Executing work queue function */
void workqueue_fn(struct work_struct *work) {
  pr_info("Executing Workqueue function\n");
}

static irqreturn_t irq_handler(int irq, void *dev_id) {
  printk(KERNINFO "Shared IRQ : Interrupt occured\n");
  /* Allocating work to the queue */
  queue_work(own_wq, &workqueue);
  return IRQ_HANDLED;
}

volatile int hwm_value = 0;


struct kobject *kobj_ref;
static struct class *dev_cls;
static struct cdev hwm_cdev;

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
  struct irq_desc *desc;
  pr_info("Read function\n");
  desc = irq_to_desc(11);
  if (!desc) {
    return -EINVAL;
  }
  __this_cpu_write(vector_irq[59], desc);
  asm("int $0x3B"); //irq 11
  return 0;
}
static ssize_t hwm_write(struct file *flp, const char *buf, size_t len, loff_t *off)
{
  pr_info("Write function\n");
  return len;
}

static int __init hwm_driver_init(void) 
{
#ifdef STATIC
  register_chrdev_region(dev, 1, "hwm");
  printk(KERN_INFO, "Major %d Minor %d\n", MAJOR(dev), MINOR(dev));
#else
  if (alloc_chrdev_region(&dev, 0, 1, "hwm") < 0) {
    printk(KERN_ERR "cannot allocate major number for the device 1\n");
    goto r_unreg;
  }
  pr_info("Major %d Minor %d\n", MAJOR(dev), MINOR(dev));
  //init_wait_queue_head(&wq_hwm);
#endif
  cdev_init(&hwm_cdev, &fops);

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

  if (request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "hwm_device", (void *)(irq_handler))) { // register interrupt
    printk(KERN_INFO "my-device: cannot register IRQ");
    goto irq;
  }
  /* Creating own workqueue */
  own_wq = create_workqueue("own_wq");

  pr_info("Driver insertion .... Done\n");
  return 0;

irq:
  free_irq(IRQ_NO, (void*)(irq_handler));

r_sysfs:
  kobject_put(kobj_ref); //free kobject
  sysfs_remove_file(kobj_ref, &hwm_attr.attr); //remove file under /sys/kernel/hwm_sysfs/
r_device:
  device_destroy(dev_cls, dev);
r_class:
  class_destroy(dev_cls); //free class
r_unreg:
  unregister_devchr_region(dev, 1); //free dev

  return -1;
}

static void   __exit hwm_driver_exit(void)
{
  destroy_workqueue(own_wq);
  free_irq(IRQ_NO, (void*)(irq_handler)); //free interrupt
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
MODULE_VERSION(1.1);
