/* Read-Write lock
 * In a situation where multiple readers and one writer threads present read-write lock is useful
 * reader centered locking mechanism
 * 8 Static
 *   DEFINE_RWLOCK(hwm_rwlock); :: creates a rwlock_t variable
 *   and initialize to UNLOCKED STATE.
 *   expansion
 *   #define DEFINE_RWLOCK(x) rwlock_t x = __RW_LOCK_UNLOCKED(x);
 * 8 Dynamic
 *   rwlock_t hwm_rwlock;
 *   rwlock_init(&hwm_rwlock);
 * Approach 1 (locking between user context)
 *   void read_lock(rwlock_t *);
 *   void read_unlock(rwlock_t *);
 *   void write_lock(rwlock_t *);
 *   void write_unlock(rwlock_t *);
 * Approach 2 (locking betwen bottom halves)
 *   Same as Approach 1
 * Approach 3 (locking between user context and bottom halves)
 *   void read_lock_bh(rwlock_t *);  :: LOCK - disables softirqs,tasklet on that CPU and get lock
 *   void read_unlock_bh(rwlock_t *); :: UNLOCK - unlock and enable softirqs, tasklets and bhs
 *   void write_lock_bh(rwlock_t *);
 *   void write_unlock_bh(rwlock_t *);
 * Approach 4 (locking between Hard IRQ and bottom halves)
 *   read_lock_irq(rwlock_t *);
 *   read_unlock_irq(rwlock_t *);
 *   write_lock_irq(rwlock_t *);
 *   write_unlock_irq(rwlock_t *);
 * Approach 5 (Alternave way of approach 4)
 *   read_lock_irq_save(rwlock_t *, unsigned long flags);
 *   read_unlock_irq_restore(rwlock_t *, unsigned long flags);
 *   write_lock_irq_save(rwlock_t *, unsigned long flags);
 *   write_unlock_irq_restore(rwlock_t *, unsigned long flags);
 * Approach 6 (Locking between Hard IRQs)
 *   Same as approach 5
 *
 * 
 * */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>     //kmalloc
#include <linux/uaccess.h>  //copy_from_user
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>     //cdev_init
#include <linux/err.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>

DEFINE_RWLOCK(hwm_rwlock);
unsigned long g_hwm_var = 0;

dev_t dev = 0;
static struct class *dev_cls;
static struct cdev hwm_cdev;

static int __init hwm_init(void);
static void __exit hwm_exit(void);

static struct task_struct *hwm_thread1;
static struct task_struct *hwm_thread2;

static int hwm_open(struct inode *inode, struct file *file);
static int hwm_release(struct inode *inode, struct file *file);
static ssize_t hwm_read(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static ssize_t hwm_write(struct file *filp, const char *buf, size_t len, loff_t *off);

int thread_function1(void *pv);
int thread_function2(void *pv);

/*thread1 funcrton - Write */
int thread_function1(void *pv) {
  while (!kthread_should_stop()) {
    write_lock(&hwm_rwlock);
    g_hwm_var++;
    pr_info("%s %lu\n", __FUNCTION__, g_hwm_var);
    write_unlock(&hwm_rwlock);
    msleep(1000);
  }
  return 0;
}

/*thread1 function - Read */
int thread_function2(void *pv) {
  while (!kthread_should_stop()) {
    read_lock(&hwm_rwlock);
    g_hwm_var++;
    pr_info("%s %lu\n", __FUNCTION__, g_hwm_var);
    read_unlock(&hwm_rwlock);
    msleep(1000);
  }
  return 0;
}

static struct file_operations fops = {
  .owner = THIS_MODULE,
  .open = hwm_open,
  .read = hwm_read,
  .write = hwm_write,
  .release = hwm_release,
};


static int hwm_open(struct inode *inode, struct file *file) {
  pr_info("Open function called\n");
  return 0;
}

static int hwm_release(struct inode *inode, struct file *file) {
  pr_info("Release function called\n");
  return 0;
}

static ssize_t hwm_read(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
  pr_info("Read function called\n");
  return 0;
}
static ssize_t hwm_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
  pr_info("Write function called\n");
  return len;
}

static int __init hwm_init() {
  /* Allocate a Major Number */
  if (alloc_chrdev_region(&dev, 0, 1, "hwm_dev") < 0) {
    pr_info("cannot allocate major number\n");
    goto r_unreg;
  }
  pr_info("Major %d Minor %d\n", MAJOR(dev), MINOR(dev));

  /* Init cdev structure*/
  cdev_init(&hwm_cdev, &fops);

  /* Add a character device to the system */
  if (cdev_add(&hwm_dev, dev, 1) < 0) {
    pr_info("Cannot add device to the system\n");
    goto r_cdev;
  }

  /* Creating struct class */
  if (IS_ERR(dev_cls = class_create(THIS_MODULE, "hwm_class"))) {
    pr_info("Cannot create struct class\n");
    goto r_class;
  }

  if (IS_ERR(device_create(dev_cls, NULL, dev, NULL, "hwm_device"))) {
    pr_info("Cannot create device\n");
    goto r_class;
  }

  hwm_thread1 = kthread_run(thread_function1, NULL, "kthread1");
  if (hwm_thread1) {
    pr_info("kthread1 created successfully\n");
  }
  else {
    pr_info("failed to create kthread1\n");
    goto r_device;
  }
  hwm_thread2 = kthread_run(thread_function2, NULL, "kthread2");
  if (hwm_thread1) {
    pr_info("kthread1 created successfully\n");
  }
  else {
    pr_info("failed to create kthread1\n");
    goto r_device;
  }
  pr_info("Driver inserted successfully ... Done\n");

r_device:
  class_destroy(dev_cls);

r_class:
  unregister_chrdev_region(dev, 1);
  cdev_del(&hwm_cdev);

  return -1;
}

static void __exit hwm_exit() {
  kthread_stop(hwm_thread1);
  kthread_stop(hwm_thread2);
  device_destroy(dev_cls, dev);
  class_destroy(dev_cls);
  cdev_del(&hwm_cdev);
  unregister_chrdev_region(dev, 1);
}

module_init(hwm_init);
module_exit(hwm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Annonymous driver expert");
MODULE_DESCRIPTION("device driver mutex");
MODULE_VERSION(1.2);
