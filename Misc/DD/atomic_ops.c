/* ATOMIC Variables
 *
 * typedef struct {
 *   int counter;
 * } atomic_t;
 *
 * #ifdef CONFIG_64BIT
 * typedef struct {
 *   long counter;
 * } atomic64_t;
 * #endif
 *
 * volatile type was added before but not required since in 
 * kernel shared memory will be locked via spinlock or memory barrier
 * Atomic APIs
 *
 * atomic_t etx_global_variable = ATOMIC_INIT(0);
 * long atomic_read(atomic_t *v);
 * void atomic_set(atomic_t *v, int i);
 * void atomic_add(int i, atomic_t *v);
 * void atomic_sub(int i, atomic_t *v);
 * void atomic_inc(atomic_t *v);
 * void atomic_dec(atomic_t *v);
 * int atomic_sub_and_test(int i, atomic_t *v);
 * int atomic_add_negative(int i, atomic_t *v);
 * long atomic_add_return(int i, atomic_t *v);
 * long atomic_sub_return(int i, atomic_t *v);
 * long atomic_inc_return(int i, atomic_t *v);
 * long atomic_dec_return(int i, atomic_t *v);
 * int atomic_dec_and_test(atomic_t *v);
 * int atomic_inc_and_test(atomic_t *v);
 * int atomic_add_unless(atomic_t *v, int a, int u); :: a is added to v and unless v is equal to u, return 0 if equal
 *
 * 64 Bit Atomic APIs
 * atomic64_t etx_global_variable = ATOMIC64_INIT(0);
 * long atomic64_read(atomic64_t *v);
 * void atomic64_set(atomic64_t *v, int i);
 * void atomic64_add(int i, atomic64_t *v);
 * void atomic64_sub(int i, atomic64_t *v);
 * void atomic64_inc(atomic64_t *v);
 * void atomic64_dec(atomic64_t *v);
 * int atomic64_sub_and_test(int i, atomic64_t *v);
 * int atomic64_add_negative(int i, atomic64_t *v);
 * long atomic64_add_return(int i, atomic64_t *v);
 * long atomic64_sub_return(int i, atomic64_t *v);
 * long atomic64_inc_return(int i, atomic64_t *v);
 * long atomic64_dec_return(int i, atomic64_t *v);
 * int atomic64_dec_and_test(atomic64_t *v);
 * int atomic64_inc_and_test(atomic64_t *v);
 * int atomic64_add_unless(atomic64_t *v, int a, int u); :: a is added to v and unless v is equal to u, return 0 if equal
 *
 * Atomic bit operations APIs                                       :: from addr
 * void set_bit(int nr, void *addr);                                :: Atomically set nrth bit
 * void clear_bit(int nr, void *addr);                              :: Atomically clear nrth bit
 * void change_bit(int nr, void *addr);                             :: Atomically toggle nrth bit
 * int test_and_set_bit(int nr, void *addr);                        :: Atomically set nrth bit and return prev val
 * int test_and_clear_bit(int nr, void *addr);                      :: Atomically clear nrth bit and return prev val
 * int test_and_change_bit(int nr, void *addr);                     :: Atomically toggle nrth bit and return prev val
 * int test_bit(int nr, void *addr);                                :: Atomically return val of nrth bit
 * int find_first_zero_bit(unsigned long *addr, unsigned int size); :: Atomically returns bit number of first zero bit
 * int find_first_size(unsiged long *addr, unsigned int size);      :: Atomically returns bit number of first set bit
 *
 * Non Atomic bit operations APIs                                   :: from addr
 * void _set_bit(int nr, void *addr);                               :: Non Atomically set nrth bit
 * void _clear_bit(int nr, void *addr);                             :: Non Atomically clear nrth bit
 * void _change_bit(int nr, void *addr);                            :: Non Atomically toggle nrth bit
 * int _test_and_set_bit(int nr, void *addr);                       :: Non Atomically set nrth bit and return prev val
 * int _test_and_clear_bit(int nr, void *addr);                     :: Non Atomically clear nrth bit and return prev val
 * int _test_and_change_bit(int nr, void *addr);                    :: Non Atomically toggle nrth bit and return prev val
 * int _test_bit(int nr, void *addr);                               :: Non Atomically return val of nrth bit
 * int _find_first_zero_bit(unsigned long *adr, unsigned int size); :: Non Atomically returns bit number of first zero bit
 * int _find_first_size(unsiged long *addr, unsigned int size);     :: Non Atomically returns bit number of first set bit
 *
 * */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/kobject.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/err.h>
#include <asm/io.h>

#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/sched.h>

atomic_t hwm_global_var = ATOMIC_INIT(0); /* Atomic integer variable */
unsigned int hwm_bit_check = 0;

static struct task_struct *hwm_thread1;
static struct task_struct *hwm_thread2;

dev_t dev = 0;
static struct class *dev_cls;
static struct cdev hwm_cdev;

static int __init hwm_driver_init(void);
static void __exit hwm_driver_exit(void);

static int hwm_open(struct inode *inode, struct file *file);
static int hwm_release(struct inode *inode, struct file *file);
static ssize_t hwm_read(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static ssize_t hwm_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);
int thread_func1(void *pv);
int thread_func2(void *pv);

static struct file_operations fops = {
	.owner 		= THIS_MODULE,
	.open 		= hwm_open,
	.read 		= hwm_read,
	.write 		= hwm_write,
	.release 	= hwm_release,
};

int thread_fucn1(void *pv) {
  unsigned int prev_value = 0;
  while (!kthread_should_stop()) {
    atomic_inc(&hwm_global_var);
    prev_value = test_and_change_bit(1, (void*)&hwm_bit_check);
    pr_info("Function1 (value %u) [bit:%x]\n", atomic_read(&hwm_global_var), prev_value)
    msleep(1000);
  }
  return 0;
}

int thread_fucn2(void *pv) {
  unsigned int prev_value = 0;
  while (!kthread_should_stop()) {
    atomic_inc(&hwm_global_var);
    prev_value = test_and_change_bit(1, (void*)&hwm_bit_check);
    pr_info("Function1 (value %u) [bit:%x]\n", atomic_read(&hwm_global_var), prev_value)
    msleep(1000);
  }
  return 0;
}

static int hwm_open(struct inode *inode, struct file *file) {
  pr_info("Device file opened..\n");
  return 0;
}

static int hwm_release(struct inode *inode, struct file *file) {
  pr_info("Device file closed..\n");
  return 0;
}

static ssize_t hwm_read(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
  pr_info("Device file read\n");
  return 0;
}

static ssize_t hwm_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);
  pr_info("Device file write\n");
  return 0;
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

  hwm_thread1 = kthread_run(thread_func1, NULL, "thread1");
  if (wait_thread) {
    pr_info("Thread1 created successfully\n");
  } else {
    pr_info("Thread1 creation failed\n");
  }

  hwm_thread2 = kthread_run(thread_func2, NULL, "thread2");
  if (wait_thread) {
    pr_info("Thread2 created successfully\n");
  } else {
    pr_info("Thread2 creation failed\n");
  }

  pr_info("Device Driver insert..Done\n");
r_class:
  class_destory(dev_cls);
r_cdev:
  cdev_del(&cdev);
  unregister_chrdev_region(dev, 1);
r_unreg:
  return -1;
}

static void __exit hwm_driver_exit(struct inode *inode, struct file *file) {
  kthread_stop(hwm_thread1);
  kthread_stop(hwm_thread2);
  device_destroy(dev_cls, dev);
  class_destroy(dev_cls);
  cdev_del(&hwm_dev);
  unregister_chrdev_region(dev, 1);
  printk(KERN_INFO "Removing device driver..Done\n");
}

module_init(hwm_driver_init);
module_exit(hwm_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Annonymous expert");
MODULE_DESCRIPTION("Atomic experiment");
MODULE_VERSION("1.24");
