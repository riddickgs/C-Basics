/* Linked list in linux kernel (built-in linked list (DLL))
 * Functions to process list
 * INITIALIZE Linked list
 * #define LIST_HEAD_INIT(name) { &(name), &(name) }
 * #define LIST_HEAD(name)\
 *   struct list_head name = LIST_HEAD_INIT(name)
 * struct list_head {
 *   struct list_head *prev;
 *   struct list_head *next;
 * };
 * LIST_HEAD(linked_list);
 * LIST_HEAD_INIT(name);
 *
 * CREATE A NODE in linked list
 * INIT_LIST_HEAD(struct list_head *list);
 * struct my_list {
 *   struct list_head *list;
 *   int data;
 * }
 *
 * struct my_list mynode;
 * INIT_LIST_HEAD(&mynode.list);
 * mynode.data = 10;
 * ADD a node to a linked list after its head
 * inline void list_add(struct list_head *new, struct list_head *head);
 * ADD a node to a linked list before its head
 * inline void list_add_tail(struct list_head *new, struct list_head *head);
 * DELETE a node from linked list
 * inline void list_del(struct list_head *entry);
 * inline void list_del_init(struct list_head *entry); //disconnect but not frees memory
 * REPLACE a node from linked list
 * inline void list_replace(struct list_head *old, struct list_head *new);
 * inline void list_replace_init(struct list_head *old, struct list_head *new);
 * MOVE node in linked list
 * inline void list_move(struct list_head *list, struct list_head *head);
 * inline void list_move_tail(struct list_head *list, struct list_head *head);
 * ROTATE node in linked list
 * inline void list_rotate_left(struct list_head *head);
 * CHECK node is last entry
 * inline int list_is_last(const struct list_head *list, const struct list_head *head);
 * inline int list_empty(const struct list_head *head);
 * inline int list_is_singular(const struct list_head *head);
 * SPLIT
 * inline void list_cut_position(struct list_head *list, struct list_head *head, struct list_head *entry);
 * JOIN
 * inline void list_splice(const struct list_head *list, struct list_head *head);
 * TRAVERSE
 * list_entry(ptr, type, member);
 * list_for_each(pos, head);
 * list_for_each_entry(pos, head, member);
 * list_for_each_entry_safe ( pos, n, head, member);
 * list_for_each_prev(pos, head);
 * list_for_each_entry_reverse(pos, head, member);
 * */

#include <linux/kernel.h>
#include <linux/init.h>      //init function
#include <linux/module.h>    //module macros
#include <linux/kdev_t.h>
#include <linux/kobject.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <asm/io.h>
#include <linux/list.h>
#include <linux/workqueue.h>  //required for workqueue

#define IRQ_NO 11
/* Declaring own workqueue structure variable */
static struct workqueue_struct *own_wq;

#ifdef STATIC
/* Creating work by static method */
static DECLARE_WORK(workqueue, workqueue_fn);
dev_t dev = MKDEV(235,0);
#else
/* Declaring Work structure */
static struct work_struct workqueue
dev_t dev = 0; /* Declared here defined later */
#endif

struct my_list {
  struct list_head list;
  int data;
};

/* Declare and init the head node of the linked list */
LIST_HEAD(Head_Node);


void workqueue_fn(struct work_struct *work);

/* Executing work queue function */
void workqueue_fn(struct work_struct *work) {
  struct my_list *temp_node = NULL;
  pr_info("Executing Workqueue function\n");

  temp_node = kmalloc(sizeof(struct my_list), GFP_KERNEL);
  temp_node->data = hwm_value;
  INIT_LIST_HEAD(&temp_node->list);
  list_add_tail(&temp_node->list, &Head_Node);
}

static irqreturn_t irq_handler(int irq, void *dev_id) {
  printk(KERNINFO "Shared IRQ : Interrupt occured\n");
  /* Allocating work to the queue */
  queue_work(own_wq, &workqueue);
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
  struct irq_desc *desc;
  struct my_list *temp;
  int count = 0;
  pr_info("Read function\n");
  list_for_each_entry(temp, &Head_Node, list) {
    printk(KERN_INFO "Node %d, data %d\n", count++, temp->data);
  }
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
  cdev_init(&hwm_cdev, &fops);
  hwm_cdev.owner = THIS_MODULE;
  hwm_cdev.ops = &fops;

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
  /* Creating own workqueue */
  own_wq = create_workqueue("own_wq");

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
  struct my_list *cursor, *temp;

  list_for_each_entry_safe(cursor, temp, &Head_Node, list) {
    list_del(&cursor->list);
    kfree(cursor);
  }
  destroy_workqueue(own_wq);
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
MODULE_VERSION(1.1);
