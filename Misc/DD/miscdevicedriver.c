/* MISCDRIVER
 * difference between character and miscellaneous device drivers
 * misc will have major number 10 and minor form 0 to 255 whereas
 * character can have any major number.
 * Device node/file will be automatically generated in misc driver
 * whereas user has to create device node or device file in character driver
 * 
 * For simple device use misc driver to avoid wasting RAM space.
 *
 * misc device structure
 *
 * struct miscdevice {
 *   int minor;                       :: minor number
 *   const char *name;                :: name of device
 *   file_operations *fops;           :: pointer to file operation structure
 *   struct miscdevice *next, *prev;  :: misc device circular linked list pointers
 * };
 *
 * misc device APIs
 * int misc_register(struct miscdevice *misc);
 * int misc_unregister(struct miscdevice *misc);
 *
 * */

#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

static int hwm_open(struct inode *inode, struct file *file) {
  pr_info("Hwm msic device opened\n");
  return 0;
}

static int hwm_release(struct inode *inode, struct file *file) {
  pr_info("Hwm misc device closed\n");
  return 0;
}

/* Reading from misc device */
static ssize_t hwm_read(struct file *filp, char __user *buf, size_t len, loff_t *off) {
  pr_info("Hwm misc device reading\n");
  return 0;
}

/* Writing to misc device */
static ssize_t hwm_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
  pr_info("Hwm misc device writing\n");
  return len;
}
struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = hwm_open,
	.read = hwm_read,
	.write = hwm_write,
	.release = hwm_release,
	.llseek = no_llseek,
};

struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.fops = &fops,
	.name = "Simple Hwm Misc device",
};

static int __init hwm_misc_init(void) {
   int error = misc_register(&misc); 
   if (error) {
      pr_info("failed to register misc device\n");
      return error;
   }
   pr_info("Misc device init .. Done\n");
   return 0;
}

static void __exit hwm_misc_exit(void) {
  misc_unregister(&misc);
  pr_info("Misc device exit .. Done\n");
}

module_init(hwm_misc_init);
module_exit(hwm_msic_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Annonymous Device driver");
MODULE_DESCRIPTION("miscellaneous device driver");
MODULE_VERSION(1.31);
