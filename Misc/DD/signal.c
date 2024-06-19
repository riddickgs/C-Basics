#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/kobject.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/err.h>
#include <asm/io.h>

#define SIGHWM 44
/* #define IRQ0_VECTOR (FIRST_EXTERNAL_VECTOR + 0x10)   // where FIRST_EXTERNAL_VECTOR is 0x20 (32)
 * Hence, IRQ11 = IRQ0+11 = 0x20+0x10+0xB = 0x3B
 */
#define IRQ_NO 11
#define REG_CURRENT_TASK __IOW('a','a',int32_t*)

static struct task_struct *task = NULL;
static int signum = 0;

int32_t value = 0;

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
	.unlocked_ioctl = hwm_ioctl,
	.release 	= hwm_release,
};

/* Interrupt handler for IRQ 11 */
static irqreturn_t irq_handler(unsigned int irq, void *dev_id) {
  struct siginfo info;
  printk(KERN_INFO "Shared IRQ: Interrupt handler\n");
  /* Sending signal to app */
  info.si_signo = SIGHWM;
  info.si_code = SI_QUEUE;
  info.si_int = 1;

  if (task != NULL) {
    printk(KERN_INFO "Sending signal to app\n");
    /*PROTO int send_sig_info(int, struct siginfo *, struct task_struct *); */
    if(send_sig_info(SIGHWM, &info, task) < 0) {
      printk(KERN_INFO "Failed to send signal to app\n");
    }
  }
  return IRQ_HANDLED;
}


static int hwm_open(struct inode *inode, struct file *file) {
  pr_info("Device file opened..\n");
  return 0;
}

static int hwm_release(struct inode *inode, struct file *file) {
  struct task_struct *ref_task = get_current();
  pr_info("Device file closed..\n");
  /* Delete the task */
  if (ref_task == task) {
    task = NULL;
  }
  return 0;
}

static ssize_t hwm_read(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
  pr_info("Device file read\n");
  asm("int $0x3B"); /* Triggering interrupt IRQ 11 */
  return 0;
}

static ssize_t hwm_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);
  pr_info("Device file write\n");
  return 0;
}
static long hwm_ioctl(struct file *file, unsigned int cmd, unsigned long value) {
  if (cmd == REG_CURRENT_TASK) {
    pr_info("REG_CURRENT_TASK\n");
    task = get_current();
    signum = SIGHWM;
  }
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
  free_irq(IRQ_NO, irq_handler);
  device_destroy(dev_cls, dev);
  class_destroy(dev_cls);
  cdev_del(&hwm_dev);
  unregister_chrdev_region(dev, 1);
  printk(KERN_INFO "Removing device driver..\n");
}

module_init(hwm_driver_init);
module_exit(hwm_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Annonymous expert");
MODULE_DESCRIPTION("signal experiment");
MODULE_VERSION("1.22");
