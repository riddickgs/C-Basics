/* USB DEVICE DRIVER
 * usb driver structure
  struct usb_driver {
  const char * name;
  int (* probe) (struct usb_interface *intf,const struct usb_device_id *id); :: called when usb device is connected
  void (* disconnect) (struct usb_interface *intf); :: called when device is removed 
  int (* ioctl) (struct usb_interface *intf, unsigned int code,void *buf); :: drivers used to talk to userspace using usbfs
  int (* suspend) (struct usb_interface *intf, pm_message_t message); :: device is suspended by system
  int (* resume) (struct usb_interface *intf);  :: device is resumed by the system
  int (* reset_resume) (struct usb_interface *intf); :: suspended device being reset instead of resume 
  int (* pre_reset) (struct usb_interface *intf); :: called by usb_reset_device when device is about to be reset
  int (* post_reset) (struct usb_interface *intf); :: called by usb_reset_device after device has been reset
  const struct usb_device_id * id_table; :: supports ID table for hotplugging, should be set to called driver prob function
  struct usb_dynids dynids; :: list of dynamically added device ids for this driver
  struct usbdrv_wrap drvwrap; :: Driver-model core structure wrapper
  unsigned int no_dynamic_id:1; :: if set 1 USB core will not allow dynamic ids to be addded to this driver
  unsigned int supports_autosuspend:1; ::if set 0, USB core will not allow auto suspends for interfaces bound to this driver
  unsigned int soft_unbind:1;  :: if set 1, USB core will not kill URBs and disable endpoints before calling driver disconnect method
};

struct usb_interface {
  struct usb_host_interface * altsetting; :: Array of interface structures, each one include a set of endpoint configs
  struct usb_host_interface * cur_altsetting; :: current altsetting
  unsigned num_altsetting; :: number of altsettings defined
  struct usb_interface_assoc_descriptor * intf_assoc; :: interface association descriptor
  int minor; :: minor number assigned to this interface, it should be set in probe function
  enum usb_interface_condition condition; :: binding state of the interface: not bound, binding (probe), bound to a driver or unbinding (in disconnect)
  unsigned is_active:1; :: flag set when the interface is bound and not suspended
  unsigned sysfs_files_created:1; :: sysfs attributes exists
  unsigned ep_devs_created:1; :: endpoints child pseudo-devices exists
  unsigned unregistering:1; :: flag set when the interface is being unregistered 
  unsigned needs_remote_wakeup:1; :: flag set when driver requires remote-wakeup capability during auto suspend 
  unsigned needs_altsetting0:1; :: flag set when a set interface request for the altsetting 0 has been deferred
  unsigned needs_binding:1; :: flag set when driver should be re-probed or unbound following a reset or suspend operation it doesn't support
  unsigned reset_running:1; :: flag set to 1 if interface is currently running a queued reset to so that usb_cancel_queue_reset doesn't try to remove from the workqueue when running inside the worker thread. see __usb_queue_reset_device.
  struct device dev; :: driver model's view of this device
  struct device * usb_dev; :: points to sysfs representation for that device, if interface is bound to USB Major
  atomic_t pm_usage_cnt; :: PM usage counter, autosuspend is not allowed unless the counter is 0
  struct work_struct reset_ws; :: Used for scheduling resets during atomic context
};

**
 * struct usb_device_id - identifies USB devices for probing and hotplugging
 * @match_flags: Bit mask controlling which of the other fields are used to
 *	match against new devices. Any field except for driver_info may be
 *	used, although some only make sense in conjunction with other fields.
 *	This is usually set by a USB_DEVICE_*() macro, which sets all
 *	other fields in this structure except for driver_info.
 * @idVendor: USB vendor ID for a device; numbers are assigned
 *	by the USB forum to its members.
 * @idProduct: Vendor-assigned product ID.
 * @bcdDevice_lo: Low end of range of vendor-assigned product version numbers.
 *	This is also used to identify individual product versions, for
 *	a range consisting of a single device.
 * @bcdDevice_hi: High end of version number range.  The range of product
 *	versions is inclusive.
 * @bDeviceClass: Class of device; numbers are assigned
 *	by the USB forum.  Products may choose to implement classes,
 *	or be vendor-specific.  Device classes specify behavior of all
 *	the interfaces on a device.
 * @bDeviceSubClass: Subclass of device; associated with bDeviceClass.
 * @bDeviceProtocol: Protocol of device; associated with bDeviceClass.
 * @bInterfaceClass: Class of interface; numbers are assigned
 *	by the USB forum.  Products may choose to implement classes,
 *	or be vendor-specific.  Interface classes specify behavior only
 *	of a given interface; other interfaces may support other classes.
 * @bInterfaceSubClass: Subclass of interface; associated with bInterfaceClass.
 * @bInterfaceProtocol: Protocol of interface; associated with bInterfaceClass.
 * @bInterfaceNumber: Number of interface; composite devices may use
 *	fixed interface numbers to differentiate between vendor-specific
 *	interfaces.
 * @driver_info: Holds information used by the driver.  Usually it holds
 *	a pointer to a descriptor understood by the driver, or perhaps
 *	device flags.
 *
 * In most cases, drivers will create a table of device IDs by using
 * USB_DEVICE(), or similar macros designed for that purpose.
 * They will then export it to userspace using MODULE_DEVICE_TABLE(),
 * and provide it to the USB core through their usb_driver structure.
 *
 * See the usb_match_id() function for information about how matches are
 * performed.  Briefly, you will normally use one of several macros to help
 * construct these entries.  Each entry you provide will either identify
 * one or more specific products, or will identify a class of products
 * which have agreed to behave the same.  You should put the more specific
 * matches towards the beginning of your table, so that driver_info can
 * record quirks of specific products.
 *
struct usb_device_id {
##	 which fields to match against? 
	__u16		match_flags;

##	 Used for product specific matches; range is inclusive 
	__u16		idVendor;
	__u16		idProduct;
	__u16		bcdDevice_lo;
	__u16		bcdDevice_hi;

##	 Used for device class matches 
	__u8		bDeviceClass;
	__u8		bDeviceSubClass;
	__u8		bDeviceProtocol;

## 	Used for interface class matches 
	__u8		bInterfaceClass;
	__u8		bInterfaceSubClass;
	__u8		bInterfaceProtocol;

	 Used for vendor-specific interface matches 
	__u8		bInterfaceNumber;

##	not matched against 
	kernel_ulong_t	driver_info
		__attribute__((aligned(sizeof(kernel_ulong_t))));
};

 * id_table is used in hotplugging
 * const struct usb_device_id hwm_device_id[] =  { //array of struct usb_device_id
 *   { USB_DEVICE(USB_VENDOR_ID, USB_PRODUCT_ID) },
 *   { }
 * }; 
 * MODULE_DEVICE_TABLE(usb, hwm_device_id);
 *
 * */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

#define IS_NEW_METHOD_USED (1)
#define USB_VENDOR_ID		0x22d9
#define USB_PRODUCT_ID		0x2764

#define PRINT_USB_INTERFACE_DESCRIPTOR(i) {				\
	pr_info("___USB INTERFACE DESCRIPTOR___\n");			\
	pr_info("-------------------------------\n");			\
	pr_info("bLength: 0x%x\n", i.bLength);				\
	pr_info("bDescriptorType: 0x%x\n", i.bDescriptorType); 		\
	pr_info("bInterfaceNumber: 0x%x\n", i.bInterfaceNumber); 	\
	pr_info("bAlternateSetting: 0x%x\n", i.bAlternateSetting); 	\
	pr_info("bNumEndpoints: 0x%x\n", i.bNumEndpoints); 		\
	pr_info("bInterfaceClass: 0x%x\n", i.bInterfaceClass); 		\
	pr_info("bInterfaceSubclass: 0x%x\n", i.bInterfaceSubclass); 	\
	pr_info("bInterfaceProtocol: 0x%x\n", i.bInterfaceProtocol);	\
	pr_info("iInterface 0x%x\n", i.iInterface);			\
	pr_info("\n");							\
}

#define PRINT_USB_ENDPOINT_DESCRIPTOR( e )                          \
{                                                                   \
    pr_info("USB_ENDPOINT_DESCRIPTOR:\n");                          \
    pr_info("------------------------\n");                          \
    pr_info("bLength: 0x%x\n", e.bLength);                          \
    pr_info("bDescriptorType: 0x%x\n", e.bDescriptorType);          \
    pr_info("bEndPointAddress: 0x%x\n", e.bEndpointAddress);        \
    pr_info("bmAttributes: 0x%x\n", e.bmAttributes);                \
    pr_info("wMaxPacketSize: 0x%x\n", e.wMaxPacketSize);            \
    pr_info("bInterval: 0x%x\n", e.bInterval);                      \
    pr_info("\n");                                                  \
}

static int hwm_usb_probe(struct usb_interface *interface, const struct usb_device_id *id) {
  unsigned int i;
  unsigned int endpoints_count;
  struct usb_interface *iface_desc = interface->cur_altsetting;

  dev_info(&interface->dev, "USB Driver Probed: Vendor ID : 0x%02x,\tProduct ID : 0x%02x\n",
		  id->idVendor, id->idProduct);
  endpoints_count = iface_desc->desc.bNumEndpoints;
  PRINT_USB_INTERFACE_DESCRIPTOR(iface_desc->desc);

  for (i = 0; i < endpoints_count; i++) {
    PRINT_USB_ENDPOINT_DESCRIPTOR(iface_desc->endpoint[i].desc);
  }
  return 0;
}

static void hwm_usb_disconnect(struct usb_interface *interface) {
  dev_info(&interface->dev, "USB Driver Disconnected\n");
}

const struct usb_device_id hwm_usb_table[] = {
	{USB_DEVICE(USB_VENDOR_ID, USB_PRODUCT_ID)}, /* USB's vendor id and product id */
	{ } /* Terminating entry */
};

MODULE_DEVICE_TABLE(usb, hwm_usb_table);
static struct usb_driver hwm_usb_driver = {
  .name = "HWM USB Driver",
  .probe = hwm_usb_probe,
  .disconnect = hwm_usb_disconnect,
  .id_table = hwm_usb_table,
};

#if (IS_NEW_METHOD_USED == 0)
  module_usb_driver(hwm_usb_driver);
#else
  static int __init hwm_usb_driver_init(void) {
    return usb_register(&hwm_usb_driver);
  }
  static void __exit hwm_usb_driver_exit(void) {
    usb_deregister(&hwm_usb_driver);
  }
  module_init(hwm_usb_driver_init);
  module_exit(hwm_usb_driver_exit);
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ME");
MODULE_DESCIPTION("USB driver");
MODULE_VERSION("1.33");
