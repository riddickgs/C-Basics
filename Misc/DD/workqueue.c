/* 4 bottom half mechanism avaiable in linux to server interrupts
 * WORKQUEUE
 * threaded IRQs
 * softirqs
 * tasklet
 *
 * WORKQUEUE - calls from process context, schedulable
 * two ways to implement workqueue in linux
 * 1. Global work queue (Static/Dynamic) //here workqueue is not created, kernel-global workqueue is used
 * 2. creating own work queue
 *
 * GLOBAL WORK QUEUE
 * DECLARE_WORK(name, void (*func)(void *));  //create work
 * where, name is identifier of struct work_struct
 *        func is Function to be scheduled in this workqueue
 * SCHEDULE WORK to the workqueue, below function puts a job on kernel-global workqueue
 * int schedule_work(struct work_struct *work);
 * int schedule_delayed_work(struct delayed_work *dwork, unsigned long delay);
 * int schedule_work_on(int cpu, struct work_struct *work);
 * int schedule_delayed_work_on(int cpu, struct delayed_work *dwork, unsigned long delay);
 * 
 * DELETE WORK from workqueue
 * int flush_work(struct work_struct *work);   // flush a particular work item
 * void flush_scheduled_work(void)             // flush all jobs from kernel-global workqueue
 *
 * CANCEL WORK from workqueue
 * int cancel_work_sync(struct work_struct *work); // terminate the work in the queue or block until callback finished
 * int cancel_delayed_work_sync(struct delayed_work *dwork); // terminate the delayed work
 *
 * CHECK the workqueue
 * int work_pending(struct work_struct work);
 * int delayed_work_pending(struct work_struct work);
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

/* #define IRQ0_VECTOR (FIRST_EXTERNAL_VECTOR + 0x10)   // where FIRST_EXTERNAL_VECTOR is 0x20 (32)
 * Hence, IRQ11 = IRQ0+11 = 0x20+0x10+0xB = 0x3B
 */
#define IRQ_NO 11


#ifdef STATIC
/* Creating work by static method */
DECLARE_WORK(workqueue, workqueue_fn);
dev_t dev = MKDEV(235,0);
#else
/* Declaring Work structure */
static struct work_struct workqueue
dev_t dev = 0; /* Declared here defined later */
#endif

void workqueue_fn(struct work_struct *work);

/* Executing work queue function */
void workqueue_fn(struct work_struct *work) {
  pr_info("Executing Workqueue function\n");
}

static irqreturn_t irq_handler(int irq, void *dev_id) {
  printk(KERNINFO "Shared IRQ : Interrupt occured\n");
#ifdef STATIC
  schedule_work(workqueue);
#else
  schedule_work(&workqueue);
#endif
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

  kobj_ref = kobject_create_and_add("hwm_sysfs", kernel_kobj);  //creating directory /sys/kernel/hwm_sysfs/
  if (sysfs_create_file(kobj_ref, &hwm_attr.attr)) {
    pr_info("cannot create sysfs file ..\n");
    goto r_sysfs;
  }

  if (request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "hwm_device", (void *)(irq_handler))) { // register interrupt
    printk(KERN_INFO "my-device: cannot register IRQ");
    goto irq;
  }
  /* Creating work by Dynamic method */
  INIT_WORK(&workqueue, workqueue_fn);

  pr_info("Driver insertion .... Done\n");
  return 0;

irq:
  free_irq(IRQ_NO, (void*)(irq_handler));

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
MODULE_VERSION(1.2);
