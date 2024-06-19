/* MUTEX
 * A synchronizwation primitive, Mutual exclusion lock,
 * only one thread can hold the lock and got the ownership and unlock
 *
 * In kernel, <linux/mutex.h>
 * struct mutex {
 *   atomic_t count;
 *   spinlock_t wait_lock;
 *   struct list_head wait_list;
 * };
 * Initialization in two ways
 * 1] Static
 *    When global mutex is used. Macro defined below
 *    DEFINE_MUTEX(name);
 *
 * 2] Dynamic
 *    When per-object mutexes used, here mutex is just a field in a heap allocated object. Macro is
 *    mutex_init(struct mutex *lock);
 *    initialize mutex to unlocked state (locked not allowed)
 *
 * Once mutex is initialized, it can be locked by any of the following
 * MUTEX LOCK 
 * This is to lock/aquire the mutex exclusively for the current task,
 * if mutex is not available then current task will sleep until it aquires the mutex
 * kernel memory where mutex resides must not be freed with mutex still locked, memseting 0 to mutex not allowed
 *   void mutex_lock(struct mutex *lock);
 *   int mutex_lock_interruptible(struct mutex *lock); :: returns -EINTR when a signal arrives	on waiting
 *   int mutex_trylock(struct mutex *lock); :: try to aquire lock return 1 on success and 0 on contention
 * MUTEX UNLOCK
 * This is used to unlock/release a mutex that has been locked by a task previously.
 *   void mutex_unlock(struct mutex *lock);
 * MUTEX STATUS
 *   int mutex_is_locked(struct mutex *lock); ::returns 1 if locked else 0
 * */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>    //kmalloc()
#include <linux/uaccess.h> //copy_from_user
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/err.h>
#include <linux/mutex.h>   //mutex_init
#include <linux/kthread.h> //kthread_run
#include <linux/sched.h>   //task_struct
#include <linux/delay.h>

struct mutex hwm_mutex;
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
    mutex_lock(&hwm_mutex);
    g_hwm_var++;
    pr_info("%s %lu\n", __FUNCTION__, g_hwm_var);
    mutex_unlock(&hwm_mutex);
    msleep(1000);
  }
  return 0;
}

int thread_function2(void *pv) {
  while (!kthread_should_stop()) {
    mutex_lock(&hwm_mutex);
    g_hwm_var++;
    pr_info("%s %lu\n", __FUNCTION__, g_hwm_var);
    mutex_unlock(&hwm_mutex);
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

  mutex_init(&hwm_mutex);
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
