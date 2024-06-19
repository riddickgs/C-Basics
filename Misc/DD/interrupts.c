#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/err.h>
#include <asm/io.h>
#include <linux/interrupt.h>

#define IRQ_NO 11

/* Register an IRQ, called from process context */
int request_irq(unsigned int irq,       //IRQ number to allocate
	       	irq_handler_t handler,  //handler function 
	       	unsigned long flags,    //bitmask of flags such as IRQF_DISABLE,IRQF_SHARED,IRQF_SAMPLE_RANDOM,IRQF_TIMER
		const char *name,       //use to find device name using this IRQ
	       	void *dev_id);          //unique cookie to remove only desired interrupt from interrupt line
/* Frees an IRQ based on dev_id if shared called from process context */
int free_irq(unsigned int irq,          //unique IRQ number
		void *dev_id);          //last param of request_irq
int enable_irq(unsigned int irq);       //re-enable irq
int disable_irq(unsigned int irq);      //disable irq
int disable_irq_nosync(unsigned int irq);  //disable an irq but wait until there is an handler executed
int in_irq();                           //return true when in interrupt handler
int in_interrupt();                     //return true when in interrupt handler or bottom half
/* Used vector range 
 * vectors 0..31                                         : system traps and exception - hardcoded events
 * vectors 32..127                                       : device interrupts
 * vector  128                                           : legacy int80 syscall interface
 * vectors 129..INVALIDATE_TLB_VECTOR_START-1 except 204 : device interrupts
 * vectors INVALIDATE_TLB_VECTOR_START..255              : special interrupts
 * IRQ0 is mapped to vector using macro
 * #define IRQ0_VECTOR (FIRST_EXTERNAL_VECTOR + 0x10)   // where FIRST_EXTERNAL_VECTOR is 0x20 (32)
 * Hence, IRQ11 = IRQ0+11 = 0x20+0x10+0xB = 0x3B
 * asm("$0x3B") will raise interrupt IRQ11
 * */

static irqreturn_t irq_handler(int irq, void *dev_id) {
  printk(KERNINFO "Shared IRQ : Interrupt occured\n");
  return IRQ_HANDLED;
}

volatile int hwm_value = 0;

#ifdef STATIC
dev_t dev = MKDEV(235,0);
#else
dev_t dev = 0; /* Declared here defined later */
#endif
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
    return -1;
  }
  pr_info("Major %d Minor %d\n", MAJOR(dev), MINOR(dev));
  //init_wait_queue_head(&wq_hwm);
#endif
  //Creating cdev structure
  cdev_init(&hwm_cdev, &fops);

  // Adding device to the system
  if (cdev_add(&hwm_cdev, dev, 1) < 0) {
    pr_err("cannot add device to the system\n");
    goto r_class;
  }
  //creating struct class
  if (IS_ERR(dev_cls = class_create(THIS_MODULE, "hwm_class"))) {
    pr_info("cannot create the struct class\n");
    goto r_class;
  }
  //creating device
  //struct device *device_create(struct class *, struct device *parent, dev_t dev, void *data, const char *fmt);
  if (IS_ERR(device_create(dev_cls, NULL, dev, NULL,"hwm_device"))) {
    pr_info("Cannot create device\n");
    goto r_device;
  }
  //creating sysfs file for hwm_value
  kobj_ref = kobject_create_and_add("hwm_sysfs", kernel_kobj);  //creating directory /sys/kernel/hwm_sysfs/
  if (sysfs_create_file(kobj_ref, &hwm_attr.attr)) {
    pr_info("cannot create sysfs file ..\n");
    goto r_sysfs;
  }

  if (request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "hwm_device", (void *)(irq_handler))) { // register interrupt
    printk(KERN_INFO "my-device: cannot register IRQ");
    goto irq;
  }

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
