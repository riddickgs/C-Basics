/* SPINLOCK
 * thread is trying to lock or aquire the mutex which
 * is not available then thread will go to sleep until
 * mutex is available.
 * In spinlock, if a process attemps to aquire a spinlock
 * and it is unavailable, process will keep trying(spinning)
 * until it aquires the lock. It creates small and fast lock
 * Spinlock initialization in linux
 * 8 Static
 *   DEFINE_SPINLOCK(hwm_spinlock); :: creates a spinlock_t variable
 *   and initialize to UNLOCKED STATE.
 *   expansion
 *   #define DEFINE_SPINLOCK(x) spinlock_t x = __SPIN_LOCK_UNLOCKED(x);
 * 8 Dynamic
 *   spinlock_t hwm_spinlock;
 *   spin_lock_init(&hwm_spinlock);
 * Approach 1 (locking between user context)
 *   void spin_lock(spinlock_t *);
 *   int spin_trylock(spinlock_t *);
 *   void spin_unlock(spinlock_t *);
 *   int spin_is_locked(spinlock_t *);
 * Approach 2 (locking betwen bottom halves)
 *   Same as Approach 1
 * Approach 3 (locking between user context and bottom halves)
 *   void spin_lock_bh(spinlock_t *);  :: LOCK - disables softirqs,tasklet on that CPU and get lock
 *   void spin_unlock_bh(spinlck_t *); :: UNLOCK - unlock and enable softirqs, tasklets and bhs
 * Approach 4 (locking between Hard IRQ and bottom halves)
 *   spin_lock_irq(spinlock_t *);
 *   spin_unlock_irq(spinlock_t *);
 * Approach 5 (Alternave way of approach 4)
 *   spin_lock_irq_save(spinlock_t *, unsigned long flags);
 *   spin_unlock_irq_restore(spinlock_t *, unsigned long flags);
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

DEFINE_SPINLOCK(hwm_spinlock);
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

int thread_function1(void *pv) {
  while (!kthread_should_stop()) {
    if (!spin_is_locked(&hwm_spinlock)) {
      pr_info("spin is not locked\n");
    }
    spin_lock(&hwm_spinlock);
    if (spin_is_locked(&hwm_spinlock)) {
      pr_info("spin is locked\n");
    }
    g_hwm_var++;
    pr_info("%s %lu\n", __FUNCTION__, g_hwm_var);
    spin_unlock(&hwm_spinlock);
    msleep(1000);
  }
  return 0;
}

int thread_function2(void *pv) {
  while (!kthread_should_stop()) {
    spin_lock(&hwm_spinlock);
    g_hwm_var++;
    pr_info("%s %lu\n", __FUNCTION__, g_hwm_var);
    spin_unlock(&hwm_spinlock);
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
