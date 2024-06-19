/* 4 bottom half mechanism available in linux to serve interrupts
 * workqueue
 * threaded IRQs
 * SOFTIRQs
 * tasklet
 *
 * SOFTIRQs is software interrupt requests, when we have more work to do in ISR then we can defer some work to do later.
 * Tasklets also build on softirq.
 *
 * How to use Softirqs?
 * steps to follow in order to use softirq
 * 	1. Edit the linux kernel source code
 * 	2. Declare softirq
 * 	3. Create softirq handler
 * 	4. Register the softirq with the handler
 * 	5. Trigger the softirq
 *
 * softirq are statically allocated at the compile time.
 * declared in linux/interrupt.h
 *
 * struct softirq_action {
 *   void (*action)(struct softirq_action *); //handler
 * };
 * There are couple of entries available in the array to register softirq
 * static struct softirq_action softirq_vec[NR_SOFIRQS]; :: max 32
 * refer enum in linux/interrupt.h
 1 Edit the kernel source code by 2 Declare softirq 
 * enum {
 * HI_SOFTIRQ=0,
 * TIMER_SOFTIRQ,
 * NET_TX_SOFTIRQ,
 * NET_RX_SOFTIRQ,
 * BLOCK_SOFTIRQ,
 * IRQ_POLL_SOFTIRQ,
 * TASKLET_SOFTIRQ, 
 * SCHED_SOFTIRQ,
 * HRTIMER_SOFTIRQ,
 * RCU_SOFTIRQ,  :: Preferable RCU should always be the last softirq
 * HWM_SOFTIRQ,  :: Declare our own softirq 
 *
 * NR_SOFTIRQS
 * };
 3 create handler
 * void softirq_handler(struct softirq_action *);
 4 register handler
 * void open_softirq(int nr, void (*action)(struct softirq_action *));
 5 Trigger the softirq
 * raise_softirq(unsigned int nr); :: nr is softirq entry
 * raise_softirq_irqoff(unsigned int nr); :: when interrupt is off
 * Above APIs (open_softirq, raise_softirq, raise_softirq_irqoff) are not exported by default
 *
 * softirq handler called in the following places
 * In the return from hardware interrupt code (ISR)
 * In the ksoftirqd kernel thread
 * In any code that explicitly checks and executes pending softirqs ex. networking subsystem
 *
 * softirqs are often raised from within interrupt handlers
 * inline void raise_softirq_irqoff(unsigned int nr) {
 *   if (!in_interrupt())  wakeup_ksoftirqd();
 * }
 * EXPORT_SYMBOL(raise_softirq_irqoff);
 * void raise_softirq(unsigned int nr) {
 *   raise_softirq_irqoff(nr);
 *   local_irq_restore(flags);
 * }
 * EXPORT_SYMBOL(raise_softirq);
 * void open_softirq(unsigned int nr, void (*action)(struct softirq_action *)) {
 *   softirq_vec[nr].action = action;
 * }
 * EXPORT_SYMBOL(open_softirq);
 * */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/uaccess.h>     //copy_from_user
#include <linux/slab.h>        //kmalloc
#include <linux/cdev.h>        //cdev_init
#include <linux/device.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/gpio.h> //GPIO

#define EN_DEBOUNCE

#ifdef EN_DEBOUNCE
#include <linux/jiffies.h>
extern unsigned long volatile jiffies;
unsigned long old_jiffy = 0;
#endif

/* LED is connected to this GPIO */
#define GPIO_21_OUT (21)

/* LED is connected to this GPIO */
#define GPIO_25_IN (25)

/* LED GPIO_25_IN togger value */
unsigned int led_toggle = 0;

/* store IRQ number */
unsigned int GPIO_irqNumber;

/* Interrupt handler for LED 25, called whenever raising edge detected */
static irqreturn_t gpio_irq_handler(int irq, void *dev_id) {
#ifdef EN_DEBOUNCE
	unsigned long diff = jiffies - old_jiffy;
	if (diff < 20) return IRQ_HANDLED;
	old_jiffy = jiffies;
#endif
	raise_softirq(HWM_SOFTIRQ);
	return IRQ_HANDLED;
}

/* softirq handler */
static void gpio_interrupt_softirq_handler(struct softirq_action *action) {
  led_toggle = (0x01 ^ led_toggle);             // toggle the old value
  gpio_set_value(GPIO_21_OUT, led_toggle);      // toggle the GPIO_21_OUT value
  pr_info(KERN_INFO "Interrupt Occured: GPIO_21_OUT : %d \n", gpio_get_value(GPIO_21_OUT));
}

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
  uint8_t gpio_state = 0;
  //reading gpio value
  gpio_state = gpio_get_value(GPIO_21_OUT);
  //write to user
  len = 1;
  if (copy_to_user(buf, &gpio_state, len) > 0) {
    pr_info("ERROR: Not all bytes copied from the user\n");
  }
  pr_info("Read function, GPIO state %d\n", gpio_state);
  return 0;
}
static ssize_t hwm_write(struct file *flp, const char *buf, size_t len, loff_t *off)
{
  uint8_t rec_buf[10] = {0};
  if (copy_from_user(rec_buf, buf, len) > 0) {
    pr_info("ERROR: not all the bytes copied from the user\n");
  }

  pr_info("Write function, GPIO_21 set %c\n", rec_buf[0]);

  if (rec_buf[0] == '1') {
    gpio_set_value(GPIO_21_OUT, 1);
  } else if (rec_buf[0] == '0') {
    gpio_set_value(GPIO_21_OUT, 0);
  } else {
    pr_info("Unknown command:  Please provide value either 0 or 1\n");
  }

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
    return r_unreg;
  }
  pr_info("Major %d Minor %d\n", MAJOR(dev), MINOR(dev));
  //init_wait_queue_head(&wq_hwm);
#endif
  //Creating cdev structure
  cdev_init(&hwm_cdev, &fops);

  // Adding device to the system
  if (cdev_add(&hwm_cdev, dev, 1) < 0) {
    pr_err("cannot add device to the system\n");
    goto r_cdev;
  }
  //creating struct class
  if (IS_ERR(dev_cls = class_create(THIS_MODULE, "hwm_class"))) {
    pr_info("cannot create the struct class\n");
    goto r_class;
  }
  //creating device
  if (IS_ERR(device_create(dev_cls, 1, NULL, dev, NULL,"hwm_device"))) {
    pr_info("Cannot create device\n");
    goto r_device;
  }
  /* Output gpio configuration 
   * Checking whether gpio is valid or not */
  if (gpio_is_valid(GPIO_21_OUT) == false) {
    pr_err("GPIO %d is not valid\n", GPIO_21_OUT);
    goto r_device;
  }


  /* Gpio request */
  if (gpio_request(GPIO_21_OUT, "GPIO_21_OUT") < 0) {
    pr_err("ERROR:GPIO %d request\n", GPIO_21_OUT);
    goto r_gpio_out;
  }

  /* Configure the GPIO as output */
  gpio_direction_output(GPIO_21_OUT, 0);

  /* Input GPIO configuration 
   * Cehcking gpio is valid */
  if (gpio_is_valid(GPIO_25_IN) == false) {
    pr_err("GPIO %d is not valid\n", GPIO_25_IN);
    goto r_device;
  }

  /* Gpio request */
  if (gpio_request(GPIO_25_IN, "GPIO_25_IN") < 0) {
    pr_err("ERROR: GPIO %d request\n", GPIO_25_IN);
    goto r_gpio_in;
  }

  /* Cofigure the GPIO as input */
  gpio_direction_input(GPIO_25_IN);

#ifndef EN_DEBOUNCE
  /* debounce the button with delay of 200ms */
  if (gpio_set_debounce(GPIO_25_IN, 200) < 0) {
    pr_err("debounce %d\n", GPIO_25_IN);
    //goto r_gpio_in;
  }
#endif
  /* Get the IRQ number for our GPIO */
  GPIO_irqNumber = gpio_to_irq(GPIO_25_IN);
  pr_info("IRQ num %d\n", GPIO_irqNumber);

  /* register interrupt */
  if (request_irq(GPIO_irqNumber,                  // IRQ number
		  (void *)gpio_irq_handler,        // irq handler
		  IRQF_TRIGGER_RISING,             // irq flag handler will called in rising edge
		  "hwm_device",                    // device name
		  NULL)) {                         // device id for shared IRQ
    printk(KERN_ERROR "my-device: cannot register IRQ");
    goto r_gpio_in;
  }

  open_softirq(HWM_SOFTIRQ, gpio_interrupt_softirq_handler);
  pr_info("Driver insertion .... Done\n");
  return 0;

r_gpio_in:
  gpio_free(GPIO_25_IN);
  free_irq(GPIO_irqNumber, (void*)(gpio_irq_handler));
r_gpio_out:
  gpio_free(GPIO_21_OUT);
r_device:
  device_destroy(dev_cls, dev);
r_class:
  class_destroy(dev_cls); //free class
r_cdev:
  cdev_del(&hwm_cdev);
r_unreg:
  unregister_devchr_region(dev, 1); //free dev
  return -1;
}

static void   __exit hwm_driver_exit(void)
{
  free_irq(GPIO_irqNumber, (void*)(gpio_irq_handler)); //free interrupt
  gpio_free(GPIO_25_IN);
  gpio_free(GPIO_21_OUT);
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
