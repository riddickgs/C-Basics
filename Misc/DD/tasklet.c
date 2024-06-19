#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/kobject.h>
#include <linux/fs.h>
#include <linux/slab.h>        //kmalloc
#include <linux/sysfs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/err.h>
#include <asm/io.h>
#include <linux/uaccess.h>     //copy_from_user

#define IRQ_NO 11

/* 4 bottom half mechanisms available for serving interrupts
 * workqueue
 * threaded IRQs
 * softirqs
 * TASKLET
 *
 * tasklet in linux a small thread with neither stack nor context and scheduled by one CPU.
 * this is executed in atomic context
 * Two ways to create tasklet
 * Static
 * Dynamic
 *
 * structure
 * struct tasklet_struct {
 *   struct tasklet *next;        :: next tasklet in line
 *   unsigned long state;         :: tasklet's state (TASKLET_STATE_SCHED/TASKLET_STATE_RUN)
 *   atomic_t count;              :: non-zero/0 for disabled/enabled
 *   void (*func)(unsigned long); :: pfunction that needs to schedule for execution at a later time
 *   unsigned long data;          :: data to be passed to the function
 * };
 *
 * Macros used to create tasklet
 * DECLARE_TASKLET(name, func, data); :: name of the structure to be created
 *              (OR)
 * tasklet.next = NULL;
 * tasklet.func = taskelt_fn;
 * tasklet.count = 0; //tasklet enabled
 * tasklet.data = 1;
 * tasklet.state = TASKLET_STATE_SCHED;
 *
 * DECLARE_TASKLET_DISABLED(name,func,data);
 *              (OR)
 * tasklet.next = NULL;
 * tasklet.func = taskelt_fn;
 * tasklet.count = 1; //tasklet disable
 * tasklet.data = 1;
 * tasklet.state = TASKLET_STATE_SCHED;
 *
 * enable/disable tasklet
 * void tasklet_enable(struct tasklet_struct *);
 * void tasklet_disable(struct tasklet_struct *);
 * void tasklet_disable_nosync(struct tasklet_struct *);
 *
 * Schedule the tasklet (Normal/High priority)
 * void tasklet_schedule(struct tasklet_struct *);
 * void tasklet_hi_schedule(struct tasklet_struct *);
 * void tasklet_hi_schedule_first(struct tasklet_struct *);
 * 
 * Kill tasklet :: This will wait for its completion and then kill it
 * void tasklet_kill(struct tasklet_struct *);
 * void tasklet_kill_immediate(struct tasklet_struct *, unsigned int cpu);
 *
 * DYNAMIC : function used to initialize tasklet dynamically
 * void tasklet_init(struct tasklet_struct *t, void (*)(unsigned long), unsigned long data);
 *          (OR)
 * tasklet->next  = NULL;
 * tasklet->state = TASKLET_STATE_SCHED
 * tasklet->func = tasklet_fn;
 * tasklet->data = 0;
 * atomic_set(tasklet->count, 0);
 * */

void tasklet_fn(unsigned long);

#ifdef STATIC
DECLARE_TASKLET(tasklet, tasklet_fn, 1);      // tasklet by static method
#else
static struct tasklet_struct *tasklet = NULL; // tasklet by Dynamic Method
#endif

void tasklet_fn(unsigned long arg) {
  printk(KERN_INFO "Executing tasklet function arg %d\n", arg);
}

/* Interrupt handler to IRQ 11 */
static irqreturn_t irq_handler(int irq, void *dev_id) {
  printk(KERNINFO "Shared IRQ : Interrupt occured\n");
  /* Scheduling task to tasklet */
#ifdef STATIC
  tasklet_schedule(&tasklet);
#else
  tasklet_schedule(tasklet);
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
  pr_info("Read function\n");
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
    return -1;
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
  //here tasklet is dynamically created
  tasklet = kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL);
  if (tasklet == NULL) {
    printk(KERN_INFO "Failed to allocate memory for tasklet_struct\n");
    goto irq;
  }
  tasklet_init(tasklet, tasklet_fn, 0);

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
#ifdef STATIC
  tasklet_kill(&tasklet);
#else
  tasklet_kill(tasklet);
  if (tasklet) kfree(tasklet);
#endif
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
MODULE_DESCRIPTION("Linux Device driver (tasklet static method)");
MODULE_VERSION(1.1);
