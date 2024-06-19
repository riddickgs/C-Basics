/* I2C CLIENT
 *             I2C
 *            /   \
 *         BUS      DEVICES
 *        /   \     /      \
 * Adapters  Algo Client   Drivers
 * 
 * I2C device drivers steps
 * 1. Get the I2C adapter
 * 	struct i2c_adapter *i2c_get_adapter(int nr); :: nr is i2c bus number
 * 2. Create a oled_i2c_board_info structure and create device using that
 *	struct i2c_board_info {
 *		char type[I2C_NAME_SIZE]; :: chip type, to init i2c_client.name
 *		unsigned short flags; :: to init i2c_client.flags
 *		unsigned short addr;  :: stored in i2c_client.addr
 *		void *platform_data;  :: stored in i2c_client.dev.platform_data
 *		struct dev_archdata *archdata; :: copied into i2c_client.dev.archdata
 *		struct device_node *of_node;   :: pointer to OpenFirmware device node
 *		struct fwnode_handle *fwnode;  :: device node supplied by platform firmware
 *		int irq;		       :: stored in i2c_client.irq
 *	};
 *	I2C_BOARD_INFO(dev_type, dev_addr); :: macro to init necessary fields
 * 3. Create the i2c_device_id for the slave device and register that
 * 	struct i2c_client *i2c_new_device(struct i2c_adapter *adap, struct i2c_board_info const *info);
 * 	:: Above API will create a device and now create device id using i2c_device_id structure
 * 	struct i2c_device_id {
 *		char name[I2C_NAME_SIZE];   :: Slave name
 *		kernel_ulong_t driver_data; :: Data private to driver (passed to respective driver)
 * 	};
 * 	:: After this call MODULE_DEVICE_TABLE(i2c, my_table_id) in order to expose the driver along with
 * 	its I2C device table IDs to userspace
 * 	void i2c_unregister_device(struct i2c_client *client);
 * 	:: API to remove i2c_client created by i2c_new_client()
 * 4. Create the i2c_driver structure and add that to the i2c subsystem
 * 	struct i2c_driver {
 *		unsigned int class; :: what kind of i2c	device we instantiate (for detect)
 *		int (*attach_adapter) (struct i2c_adapter *); :: cb for bus addition (deprecated)
 *		int (*probe) (struct i2c_client *, struct i2c_device_id *); :: cb for device binding
 *		int (*remove) (struct i2c_client *); :: cb for device unbinding
 *		int (*shutdown) (struct i2c_client *); :: cb for device shutdown
 *		void (*alert) (struct i2c_client *, unsigned int data); :: Alert cb, e.g. SMBus Alert protocol
 *		int (*command) (struct i2c_client *cli, unsigned int cmd, void *arg); :: cb for bus wide signal
 *		struct device_driver driver; :: device driver model driver
 *		const struct i2c_device_id *id_table; :: list of i2c devices supported by this driver
 *		int (*detect) (struct i2c_client *, struct i2c_board_info *); :: cb for device detection
 *		const unsigned short *address_list; :: i2c addresses to probe (for detect)
 *		struct list_head clients; :: list of detected clients we created (for i2c-core use only)
 * 	};
 * 	Adding driver to the i2c subsystem
 *
 *      int i2c_register_driver(struct module *owner, struct i2c_driver *driver);
 * 	TODO: #define i2c_add_driver(driver) i2c_register_driver(THIS_MODULE, driver)
 * 	i2c_add_driver(struct i2c_driver *driver);
 * 	:: This will register the i2c driver, all i2c devices will be traversed.
 * 	   Once matched probe function will be called.
 * 	void i2c_del_driver(struct i2c_driver *driver); :: remove i2c driver
 * 5. Now driver is ready, we can transfer the data between master and slave
 * 	int i2c_master_send (const struct i2c_client *cli, const char *buf, int count);
 * 	:: send "buf" data, "count" bytes < 64K to slave "cli"; returns -ve on error, else num of bytes written
 * 	s32 i2c_smbus_write_byte(const struct i2c_client *cli, u8 value);
 * 	:: "value" byte to sent to slave "cli"; returns 0 on success, else -ve
 * 	s32 i2c_smbus_write_byte_data(const i2c_client *cli, u8 cmd, u8 value);
 * 	:: "cmd" byte interpreted by slave "cli" and value same as above api
 * 	s32 i2c_smbus_write_word_data(const struct i2c_client *cli, u8 cmd, u16 value);
 * 	:: "value" 16 bit word being written, rest same as above api
 * 	s32 i2c_smbus_write_block_data(const struct i2c_client *cli, u8 cmd, u8 len, const u8 *values);
 * 	:: "len" size of the data block, SMBus allows atmost 32 bytes, "values" byte array to be written
 * 	int i2c_master_recv(const i2c_client *cli, const char *buf, int count);
 * 	:: "buf" data will be read from the slave "cli" of "count" bytes <64 k; returns -ve on error, else
 * 	   number of bytes read
 * 	s32 i2c_smbus_read_byte(const struct i2c_client *cli);
 * 	:: read a byte from slave "cli"
 * 	s32 i2c_smbus_read_byte_data(const struct i2c_client *cli, u8 cmd);
 * 	:: read a byte from slave "cli" where "cmd" is byte interpreted by slave
 * 	s32 i2c_smbus_read_word_data(const struct i2c_client *cli, u8 cmd, u16 value);
 * 	:: read 16bit word from slave "cli" where "cmd" is byte interpreted by slave
 * 	s32 i2c_smbus_read_block_data(const struct i2c_client *cli, u8 cmd, u8 *values);
 * 	:: "values" byte array to read data from slave "cli" where "cmd" is byte interpreted by slave
 * 	int i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msg, int num);
 * 	:: "adap" handle to i2c bus, "msg" one or more message to execute before STOP is issued
 * 	  to terminate the operation; each msg begins with a START, "num" number of messages executed
 *
 * 6. Once done, then remove the device
 * 	void i2c_del_driver(struct i2c_driver *driver); :: remove i2c driver
 *
 *
 * */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>

#define I2C_BUS_AVAILABLE  (         1) //i2c bus available
#define SLAVE_DEVICE_NAME  ("HWM_OLED") //device and driver name
#define SSD1306_SLAVE_ADDR (      0x3C) //SSD1306 slave Address
					//
static struct i2c_adaptor *hwm_i2c_adapter = NULL; //i2c adaptor structure
static struct i2c_client *hwm_i2c_client   = NULL; //i2c client structure

/* This function writes the data to the i2c client */
static int I2C_Write(unsigned char *buf, unsigned int len) {
	/* Sending Start condition, slave address with R/W bit 
	 * ACK/NACK and Stop condition will be handled internally */
  return i2c_master_send(hwm_i2c_client, buf, len);
}

/* This function read one byte of data from the i2c client */
static int I2C_Read(unsigned char *buf, unsigned int len) {
	/* Sending Start condition, slave address with R/W bit 
	 * ACK/NACK and Stop condition will be handled internally */
  return i2c_master_recv(hwm_i2c_client, buf, len);
}

/* This function is specific to SSD1306 OLED
 * Send the command/data to the OLED */
static void SSD1306_Write(bool is_cmd, unsigned char data) {
  unsigned char buf[2] = {0};
  int ret;

   /*
    ** First byte is always control byte. Data is followed after that.
    **
    ** There are two types of data in SSD_1306 OLED.
    ** 1. Command
    ** 2. Data
    **
    ** Control byte decides that the next byte is, command or data.
    **
    ** -------------------------------------------------------
    ** |              Control byte's | 6th bit  |   7th bit  |
    ** |-----------------------------|----------|------------|
    ** |   Command                   |   0      |     0      |
    ** |-----------------------------|----------|------------|
    ** |   data                      |   1      |     0      |
    ** |-----------------------------|----------|------------|
    **
    ** Please refer the datasheet for more information.
    **
    */
  if (is_cmd == true) {
    buf[0] = 0x00;
  }else {
    buf[0] = 0x40;
  }
  buf[1] = data;
  ret =  I2C_Write(buf, 2);
}

static int SSD1306_DisplayInit(void) {
    msleep(100);  //delay

    SSD1306_Write(true, 0xAE);// Entire Display OFF
    SSD1306_Write(true, 0xD5); // Set Display Clock Divide Ratio and Oscillator Frequency
    SSD1306_Write(true, 0x80); // Default Setting for Display Clock Divide Ratio and Oscillator Frequency that is recommended
    SSD1306_Write(true, 0xA8); // Set Multiplex Ratio
    SSD1306_Write(true, 0x3F); // 64 COM lines
    SSD1306_Write(true, 0xD3); // Set display offset
    SSD1306_Write(true, 0x00); // 0 offset
    SSD1306_Write(true, 0x40); // Set first line as the start line of the display
    SSD1306_Write(true, 0x8D); // Charge pump
    SSD1306_Write(true, 0x14); // Enable charge dump during display on
    SSD1306_Write(true, 0x20); // Set memory addressing mode
    SSD1306_Write(true, 0x00); // Horizontal addressing mode
    SSD1306_Write(true, 0xA1); // Set segment remap with column address 127 mapped to segment 0
    SSD1306_Write(true, 0xC8); // Set com output scan direction, scan from com63 to com 0
    SSD1306_Write(true, 0xDA); // Set com pins hardware configuration
    SSD1306_Write(true, 0x12); // Alternative com pin configuration, disable com left/right remap
    SSD1306_Write(true, 0x81); // Set contrast control
    SSD1306_Write(true, 0x80); // Set Contrast to 128
    SSD1306_Write(true, 0xD9); // Set pre-charge period
    SSD1306_Write(true, 0xF1); // Phase 1 period of 15 DCLK, Phase 2 period of 1 DCLK
    SSD1306_Write(true, 0xDB); // Set Vcomh deselect level
    SSD1306_Write(true, 0x20); // Vcomh deselect level ~ 0.77 Vcc
    SSD1306_Write(true, 0xA4); // Entire display ON, resume to RAM content display
    SSD1306_Write(true, 0xA6); // Set Display in Normal Mode, 1 = ON, 0 = OFF
    SSD1306_Write(true, 0x2E); // Deactivate scroll
    SSD1306_Write(true, 0xAF); // Display ON in normal mode

   return 0;
}

static void SSD1306_Fill(unsigned char data) {
  unsigned int total = 128 * 8; // 8 pages X 128 Columns X 8 Bits of data
  unsigned int i;

  for (i = 0; i < total; i++) {
    SSD1306_Write(false, data);	
  }
}

static int hwm_oled_probe(struct i2c_client *client, struct i2c_device_id *id)
{
  SSD1306_DisplayInit(void);

  /* Fill oled with this data*/
  SSD1306_Fill(0xFF);

  pr_info("Oled Probed!!\n");
  return 0;
}

static int hwm_oled_remove(struct i2c_client *client) {
  
  /* Fill oled with this data*/
  SSD1306_Fill(0x00);

  pr_info("Oled Removed!!\n");
  return 0;
}
/* Structure that have slave device id */
static const struct i2c_device_id hwm_oled_id[] = {
	{SLAVE_DEVICE_NAME, 0}, /* Name, specific device data */
  	{}
	};
MODULE_DEVICE_TABLE(i2c, hwm_oled_id);

/* i2c_driver structure to be added to linux */
static struct i2c_driver hwm_oled_driver = {
	.driver = {
		.name = SLAVE_DEVICE_NAME,
		.owner = THIS_MODULE 
	},
	.probe = hwm_oled_probe;
	.remove = hwm_oled_remove;
	.id_table = hwm_oled_id;
};

staric struct i2c_board_info oled_i2c_board_info = {
  I2C_BOARD_INFO(SLAVE_DEVICE_NAME, SSD1306_SLAVE_ADDR)
};

static int __int hwm_driver_init(void) {
  int ret = -1;

  hwm_i2c_adapter = i2c_get_adapter(I2C_BUS_AVAILABLE);
  if (hwm_i2c_adapter != NULL) {
    hwm_i2c_client = i2c_new_device(hwm_i2c_adapter, &oled_i2c_board_info);
    if (hwm_i2c_client != NULL) {
      i2c_add_driver(&hwm_oled_driver);
      ret = 0;
    }
    /* i2c_get_adapter internally calls get_device and increments reference count of the device
     * hence i2c_put_adapter is called here to decrement reference count of the device */
    i2c_put_adapter(hwm_i2c_adapter);
  }
  pr_info("Driver Added!!\n");
  return ret;
}

static void __exit hwm_driver_exit(void) {
  i2c_unregister_device(hwm_i2c_client);
  i2c_del_driver(&hwm_oled_driver);
  pr_info("Driver Removed!!\n");
}

module_init(hwm_driver_init);
module_exit(hwm_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Riddick");
MODULE_DISCRIPTION("Simple i2c driver implementation");
MODULE_VERSION(1.22);
