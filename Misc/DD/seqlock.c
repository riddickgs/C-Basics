/* SEQLOCK
 * sequential lock - It is a reader-writer consistent mechanism which
 * is giving importance to the writer. Avoids writer starvation.
 0 when sequence is odd then writer writes and when sequence is even
   writing done.
 0 when sequence is even then reader reads and when sequence is odd
   reading done (wait for writer to finish writing)
 0 After reading the data reader will check the sequence number. If it
   is equal to old sequence number, everything is okay. Otherwise, repeat
   above step. readers simply retry until they read the same even sequence
   number before and after. reader never block but it may have to retry if
   write is in progress.
 0 Writer cannot be blocked for the reader and reader has to re-read the data
   when writer is writing
 *
 * Where to use seqlock
 * where read operation are more frequent than write.
 * where write access is rare but must be fast
 * That data is simple (no pointers) that needs to be protected. seqlock
 * cannot be used to protect DS involving pointers, because reader may be
 * following a pointer which is invalid while writer is changing DS.
 *
 * Seqlock APIs (linux/seqlock.h)
 * void seqlock_init(seqlock_t *lock);
 * WRITE OPERATIONS
 * void write_seqlock(seqlock_t *lock);
 * int write_tryseqlock(seqlock_t *lock); :: tries to get spinlock
 * void write_seqlock_irqsave(seqlock_t *lock, long flags); ::save whether interrupts were ON/OFF in a flag (IC)
 * void write_seqlock_irq(seqlock_t *lock); :: disable interrupts and take lock (IC)
 * void write_seqlock_bh(seqlock_t *lock); :: write from the bottom half 
 * void write_sequnlock(seqlock_t *lock); :: increments sequence number again and release the spinlock
 * void write_sequnlock_irqrestore(seqlock_t *lock, long flags); ::release the lock and restore the interrupts using flag 
 * void write_sequnlock_irq(seqlock_t *lock); :: release the lock and reenable the disable interrupts on that CPU
 * void write_sequnlock_bh(seqlock_t *lock); :: used from the bottom halves while reading
 * READ_OPERATIONS
 1 Begin the read and get the initial seq number
 2 Read the data
 3 Once reading is done, compare the current sequence number with an initial sequence number.
   If not matching with initial seq number or odd sequence number so writing is going on. retry again.
 * unsigned int read_seqbegin(seqlock_t *lock); :: read and return sequence number. Step 1
 * unsigned int read_seqbegin_irqsave(seqlock_t *lock, long flags); :: save whether interrupts ON/OFF in a flag
 * int read_seqretry(seqlock_t *lock, unsigned int seq_no); :: compare current sequence number with provided sequence number. If current sequence number is odd or not matching with initial sequence number (arg 2) means writing is on. returns 1
 * int read_seqretry_irqrestore(seqlock_t *lock, unsigned int seq_no, long flags); :: restore interrutps using flags
 * void read_seqlock(seqlock_t *lock); ::
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
#include <linux/seqlock.h>

seqlock_t hwm_seq_lock;
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
    write_seqlock(&hwm_seq_lock);
    g_hwm_var++;
    pr_info("%s %lu\n", __FUNCTION__, g_hwm_var);
    write_sequnlock(&hwm_seq_lock);
    msleep(1000);
  }
  return 0;
}

/*thread1 function - Read */
int thread_function2(void *pv) {
  unsigned int seq_no;
  unsigned long read_value;
  while (!kthread_should_stop()) {
	  do {
             seq_no = read_seqbegin(&hwm_rwlock);
	     read_value = g_hwm_var;
	  }while (read_seqretry(&hwm_seq_lock, seq_no));
    pr_info("%s %lu\n", __FUNCTION__, read_value);
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

  seqlock_init(&hwm_seq_lock);

  pr_info("Driver inserted successfully..Done\n");
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
