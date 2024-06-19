/* I2C Buses
 *
 * 1. Algorithm structure
 * struct i2c_algorithm {
 *   int (* master_xfer) (struct i2c_adapter *adap, struct i2c_msg *msgs, int num);
 *   :: Issues a set of i2c txns to the given I2C adapter; called when I2C read-write APIs called from client driver
 *   int (* smbus_xfer) (struct i2c_adapter *adap, u16 addr, unsigned short flags, char read_write, u8 cmd, int size,  union i2c_smbus_data *data);
 *   :: Issues SMBus txns to the given I2C adapter; called when I2C SMBus read-write APIs called from client driver
 *   u32 (* functionality) (struct i2c_adapter *adap);
 *   :: returns the flags that this algorith/adapter pair supports from I2C_FUNC_* flags;
 *   #if IS_ENABLED(CONFIG_I2C_SLAVE)
 *     int (* reg_slave) (struct i2c_client *client);
 *     :: register given client to the i2c slave mode of this adapter
 *     int (* unreg_slave) (struct i2c_client *client);
 *     :: unregister given client to the i2c slave mode of this adapter
 *   #endif
 * };
 *
 * struct i2c_adapter {
 *  struct module *owner; :: Owner of the module usually THIS_MODULE
 *  unsigned int class;   :: type of i2c class devices that this driver supports. Set one of I2C_CLASS_* based on need
 *  const struct i2c_algorithm *algo; :: a pointer to struct i2c_algorith structure
 *  void *algo_data; :: 
 *  struct rt_mutex bus_lock; :: Mutex lock
 *  int timeout; :: Timeout in jiffies
 *  int retries; :: Number of retries
 *  struct device dev; :: the adapter device
 *  int nr; :: bus number which we wants to create. applicable for i2c_add_numbered_adapter()
 *  char name[48]; :: i2c bus driver name
 *  struct completion dev_released;
 *  struct mutex userspace_clients_lock;
 *  struct list_head userspace_clients;
 * };
 *
 0 After creating two structures, Add the adapter to the i2c subsystem
 *
 * int i2c_add_adapter(struct i2c_adapter *adap);
 * :: returns 0 when new bus number is allocated and stored in adap->nr, else -ve on error
 * int i2c_add_numbered_adapter(struct i2c_adapter *adap);
 * :: here adap->nr is already initialized, returns 0 when specified adapter is available for clients using bus number
 *    provided in adap->nr, otherwise -ve on error
 *
 0 Delete the adapter
 * int i2c_del_adapter(struct i2c_adapter *adap);
 *
 * */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kernel.h>

#define ADAPTER_NAME     "HWM_I2C_ADAPTER"

/*
** This function used to get the functionalities that are supported
** by this bus driver.
*/
static u32 hwm_func(struct i2c_adapter *adapter)
{
    return (I2C_FUNC_I2C             |
            I2C_FUNC_SMBUS_QUICK     |
            I2C_FUNC_SMBUS_BYTE      |
            I2C_FUNC_SMBUS_BYTE_DATA |
            I2C_FUNC_SMBUS_WORD_DATA |
            I2C_FUNC_SMBUS_BLOCK_DATA);
}

/*
** This function will be called whenever you call I2C read, wirte APIs like
** i2c_master_send(), i2c_master_recv() etc.
*/
static s32 hwm_i2c_xfer( struct i2c_adapter *adap, struct i2c_msg *msgs,int num )
{
    int i;

    for(i = 0; i < num; i++)
    {
        int j;
        struct i2c_msg *msg_temp = &msgs[i];

        pr_info("[Count: %d] [%s]: [Addr = 0x%x] [Len = %d] [Data] = ", i, __func__, msg_temp->addr, msg_temp->len);

        for( j = 0; j < msg_temp->len; j++ )
        {
            pr_cont("[0x%02x] ", msg_temp->buf[j]);
        }
    }
    return 0;
}

/*
** This function will be called whenever you call SMBUS read, wirte APIs
*/
static s32 hwm_smbus_xfer(  struct i2c_adapter *adap,
                            u16 addr,
                            unsigned short flags,
                            char read_write,
                            u8 command,
                            int size,
                            union i2c_smbus_data *data
                         )
{
    pr_info("In %s\n", __func__);
    return 0;
}

/*
** I2C algorithm Structure
*/
static struct i2c_algorithm hwm_i2c_algorithm = {
    .smbus_xfer     = hwm_smbus_xfer,
    .master_xfer    = hwm_i2c_xfer,
    .functionality  = hwm_func,
};

/*
** I2C adapter Structure
*/
static struct i2c_adapter hwm_i2c_adapter = {
    .owner  = THIS_MODULE,
    .class  = I2C_CLASS_HWMON,//| I2C_CLASS_SPD, not defined here
    .algo   = &hwm_i2c_algorithm,
    .name   = ADAPTER_NAME,
};


/*
** Module Init function
*/
static int __init hwm_driver_init(void)
{
    int ret = -1;

    ret = i2c_add_adapter(&hwm_i2c_adapter);

    pr_info("Bus Driver Added!!!\n");
    return ret;
}

/*
** Module Exit function
*/
static void __exit hwm_driver_exit(void)
{
    i2c_del_adapter(&hwm_i2c_adapter);
    pr_info("Bus Driver Removed!!!\n");
}

module_init(hwm_driver_init);
module_exit(hwm_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Riddick");
MODULE_DESCRIPTION("Simple I2C Bus driver explanation");
MODULE_VERSION("1.35");
