#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/sysfs.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/err.h>

#define EN_DEBOUNCE

#ifdef EN_DEBOUNCE
#include <linux/jiffies.h>
extern unsigned long volatile jiffies;
unsigned long old_jiffie = 0;
#endif
/* LED is connected to this gpio */
#define GPIO_21_OUT (21)
/* LED is connected to this gpio */
#define GPIO_25_IN (25)

/* GPIO_25_IN value toggle */
unsigned int led_toggle = 0;
/* Storing Irq number of the gpio */
unsigned int GPIO_irqNumber;

static irqreturn_t gpio_irqhandler(int irqno, void *id)
{
  static unsigned long flags = 0;
#ifdef EN_DEBOUNCE
  unsigned long diff = jiffies - old_jiffie;
  if (diff < 20) {
    return IRQ_HANDLED;
  }
  old_jiffie = jiffies;
#endif
  local_irq_save(flags);
  led_toggle = (led_toogle ^ 0x01);
  gpio_set_value(GPIO_21_OUT, led_toggle);
  pr_info("Interrupt occured: GPIO_21_OUT : %d", gpio_get_value(GPIO_21_OUT));
  local_irq_restore(flags);
  return IRQ_HANDLED;
}

dev_t dev = 0;
static struct class *dev_class;
static struct cdev hwm_cdev;

static int __init hwm_init(void);
static void __exit hwm_exit(void);

static ssize_t hwm_read(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static ssize_t hwm_write(struct file *filp, char *buf, size_t len, loff_t *off);
static int hwm_open(struct inode *inode, struct file *file);
static int hwm_release(struct inode *inode, struct file *file);

static file_operations fops = {
  .owner = THIS_MODULE,
  .open = hwm_open,
  .read = hwm_read,
  .write = hwm_write,
  .release = hwm_release
};

static int hwm_open(struct inode *inode, struct file *file) {
  pr_info("Device opened!!\n");
  return 0;
}
static int hwm_release(struct inode *inode, struct file *file) {
  pr_info("Device released!!\n");
  return 0;
}

/* This api will copy data to user from kernel */
static ssize_t hwm_read(struct file *filp, char __user *buf, size_t len, loff_t *off) {
  uint8_t gpio_state = 0;

  /* Reading the GPIO value */
  gpio_state = gpio_get_value(GPIO_21_OUT);
  //write to user
  len = 1;
  if (copy_to_user(buf, &gpio_state, len) < 0) {
    pr_info("Error: Not all bytes have been copied to user\n");
  }
  pr_info("Read function : GPIO_21_OUT %d\n", gpio_state);
  return 0;
}

/* This api will copy data to kernel from user */
static ssize_t hwm_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
{
  uint8_t readbuf[10];

  if (copy_from_user(readbuf, buf, 10) <0) {
    pr_info("Error: Not all the bytes have been copied from user\n");
  }

  if (readbuf[0] == '1') {
    gpio_set_value(GPIO_21_OUT, 1);
  } else if (readbuf[0] == '0') {
    gpio_set_value(GPIO_21_OUT, 0);
  } else {
    pr_info("Invalid value for LED, provide either 0 or 1\n");
  } 
  return len;
}

static int __init hwm_init(void) {
  if (alloc_chrdev_region(&dev, 0, 1, "hwm_dev") < 0) {
    pr_info("failed to get major number\n");
    goto r_unreg;
  }
  pr_info("Major %d Minor %d\n", MAJOR(dev), MINOR(dev));

  cdev_init(&hwm_cdev, &fops);
  if (cdev_add(&hwm_cdev, dev, 1) < 0) {
    pr_info("cannot add device to the system\n");
    goto r_cdev;
  }

  if (ISERR(dev_class = class_create(THIS_MODULE, "hwm_class"))) {
    pr_info("cannot create class\n");
    goto r_class;
  }

  if (IS_ERR(device_create(dev_class, NULL, dev, NULL, "hwm_device"))) {
    pr_info("cannot create device\n");
    goto r_device;
  }
  
  if (gpio_is_valid(GPIO_21_OUT) == false) {
    pr_info("GPIO_21_OUT is invalid\n");
    goto r_gpio_out;
  }

  if (gpio_request(GPIO_21_OUT, "GPIO_21_OUT") < 0) {
    pr_info("GPIO_21_OUT request failed\n");
    goto r_gpio_out;
  }

  gpio_direction_output(GPIO_21_OUT, 0);

  if (gpio_is_valid(GPIO_25_IN) == false) {
     pr_info("GPIO_25_IN is invalid\n");
     goto r_gpio_in;
  }

  if (gpio_request(GPIO_25_IN, "GPIO_25_IN") < 0) {
    pr_info("GPIO_25_IN request failed\n");
    goto r_gpio_in;
  }

  gpio_direction_input(GPIO_25_IN);

#ifndef EN_DEBOUNCE
  if (gpio_set_debounce(GPIO_25_IN, 200) < 0) {
    pr_info("failed to set debounce\n");
  }
#endif


  gpio_irqNumber = gpio_to_irq(GPIO_25_IN);
  pr_info("gpio_irqNumber %d\n", gpio_irqNumber);
  if (irq_request(gpio_irqNumber,
		  (void *)gpio_irq_handler,
		  IRQF_TRIGGER_RISING,
		  "hwm_device",
		  NULL)) {
    pr_info("cannot register irq\n");
    goto r_gpio_in;
  }
  pr_info("Device driver insertion ..Done!!\n");
  return 0;
r_gpio_in:
  gpio_free(GPIO_25_IN);
r_gpio_out:
  gpio_free(GPIO_21_OUT);
r_device:
  device_destroy(dev_class, dev);
r_class:
  class_destroy(dev_class);
r_cdev:
  cdev_del(&hwm_cdev);
r_unreg:
  unregister_chrdev_region(dev, 1);
  return -1;
}

static void __exit hwm_exit(void) {

  free_irq(gpio_irqNumber, NULL);
  gpio_free(GPIO_25_IN);
  gpio_free(GPIO_21_OUT);
  device_destroy(dev_class, dev);
  class_destroy(dev_class);
  cdev_del(&hwm_cdev);
  unregister_chrdev_region(dev, 1);
  pr_info("Device driver remove .. done!!\n");
}

module_init(hwm_init);
module_exit(hwm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Riddick");
MODULE_DESCRIPTION("Simple driver - GPIO in/out - GPIO interrupt");
MODULE_VERSION(1.42);
