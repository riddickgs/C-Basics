/***************************************************************************//**
*  \Tested with Linux raspberrypi 5.4.51-v7l+
*
*  APIs for adding adapters
*  int i2c_bit_bus_data(struct i2c_adapter *adapter);
*  int i2c_bit_add_numbered_bus(struct i2c_adapter *adapter);
*
* *******************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/gpio.h>

#define ADAPTER_NAME     "HWM_I2C_ADAPTER"

/*------------ I2C related functions - Bit banging method - START --------------*/

#define SCL_GPIO  20    //GPIO act as SCL
#define SDA_GPIO  21    //GPIO act as SDA

#define I2C_DELAY usleep_range(5, 10) 

/* we can use one algo data structure to be passed to adapter structure
 * This structure is hw dependent function of bit style adapter to 
 * manipulate the line states. Used when only one adapter running
 * struct i2c_algo_bit_data {
 *   void *data;
 *   void (* setsda)(void *data, int state);
 *   void (* setscl)(void *data, int state);
 *   int (* getsda)(void *data);
 *   int (* getscl(void *data);
 *   int (* pre_xfer)(struct i2c_adapter *);
 *   int (* post_xfer)(struct i2c_adapter *);
 *   //local settings
 *   int udelay; //half clock cycle time in us| min 2 us in fast mode|min 5 us in standard mode and SMBus|min 50 us for SMBus mode
 *   int timeout;  // in jiffies
 *   bool can_do_atomic; // callback don't sleep, we can be atomic
 * };
 */
/*
** Function to read the SCL GPIO
*/
static bool HWM_I2C_Read_SCL(void)
{
  gpio_direction_input(SCL_GPIO);
  return gpio_get_value(SCL_GPIO);
}

/*
** Function to read the SDA GPIO
*/
static bool HWM_I2C_Read_SDA(void)
{
  gpio_direction_input(SDA_GPIO);
  return gpio_get_value(SDA_GPIO);
}

/*
** Function to clear the SCL GPIO
*/
static void HWM_I2C_Clear_SCL(void)
{
  gpio_direction_output(SCL_GPIO, 0);
  gpio_set_value(SCL_GPIO, 0);
}

/*
** Function to clear the SDA GPIO
*/
static void HWM_I2C_Clear_SDA(void)
{
  gpio_direction_output(SDA_GPIO, 0);
  gpio_set_value(SDA_GPIO, 0);
}

/*
** Function to set the SCL GPIO
*/
static void HWM_I2C_Set_SCL(void)
{
  gpio_direction_output(SCL_GPIO, 1);
  gpio_set_value(SCL_GPIO, 1);
}

/*
** Function to set the SDA GPIO
*/
static void HWM_I2C_Set_SDA(void)
{
  gpio_direction_output(SDA_GPIO, 1);
  gpio_set_value(SDA_GPIO, 1);
}

/*
** Function to Initialize the GPIOs
*/
static int HWM_I2C_Init( void )
{
  int ret = 0;

  do      //Break if any error
  {
    //Checking the SCL GPIO is valid or not
    if(gpio_is_valid(SCL_GPIO) == false){
          pr_err("SCL GPIO %d is not valid\n", SCL_GPIO);
          ret = -1;
          break;
    }

    //Checking the SDA GPIO is valid or not
    if(gpio_is_valid(SDA_GPIO) == false){
          pr_err("SDA GPIO %d is not valid\n", SDA_GPIO);
          ret = -1;
          break;
    }
    
    //Requesting the SCL GPIO
    if(gpio_request(SCL_GPIO,"SCL_GPIO") < 0){
          pr_err("ERROR: SCL GPIO %d request\n", SCL_GPIO);
          ret = -1;
          break;
    }

    //Requesting the SDA GPIO
    if(gpio_request(SDA_GPIO,"SDA_GPIO") < 0){
          pr_err("ERROR: SDA GPIO %d request\n", SDA_GPIO);
          //free already requested SCL GPIO
          gpio_free(SCL_GPIO);
          ret = -1;
          break;
    }
    
    /*
    ** configure the SCL GPIO as output, We will change the 
    ** direction later as per our need.
    */
    gpio_direction_output(SCL_GPIO, 1);

    /*
    ** configure the SDA GPIO as output, We will change the 
    ** direction later as per our need.
    */
    gpio_direction_output(SDA_GPIO, 1);

  } while(false);

  return ret;  
}

/*
** Function to Deinitialize the GPIOs
*/
static void HWM_I2C_DeInit( void )
{
  //free both the GPIOs
  gpio_free(SCL_GPIO);
  gpio_free(SDA_GPIO);
}

/*
** Function to send the START condition
**      ______  
**            \
**  SDA        \_____
**              :
**              :
**           ___:____
**  SCL     /
**      ___/ 
*/
static void HWM_I2C_Sart( void )
{
  HWM_I2C_Set_SDA();
  HWM_I2C_Set_SCL();
  I2C_DELAY;
  HWM_I2C_Clear_SDA();
  I2C_DELAY;  
  HWM_I2C_Clear_SCL();
  I2C_DELAY;  
}

/*
** Function to send the STOP condition
**                _____
**               /
**  SDA  _______/
**              :
**              :
**            __:______
**  SCL      /
**       ___/ 
*/
static void HWM_I2C_Stop( void )
{
  HWM_I2C_Clear_SDA();
  I2C_DELAY;
  HWM_I2C_Set_SCL();
  I2C_DELAY;
  HWM_I2C_Set_SDA();
  I2C_DELAY;
  HWM_I2C_Clear_SCL();
}

/*
** Function to reads the SDA to get the status
** 
** Retutns 0 for NACK, returns 1 for ACK
*/
static int HWM_I2C_Read_NACK_ACK( void )
{
  int ret = 1;

  //reading ACK/NACK
  I2C_DELAY;
  HWM_I2C_Set_SCL();
  I2C_DELAY;
  if( HWM_I2C_Read_SDA() )      //check for ACK/NACK
  {
    ret = 0;
  }

  HWM_I2C_Clear_SCL();
  return ret;
}

/*
** Function to send the 7-bit address to the slave
** 
** Retutns 0 if success otherwise negative number
*/
static int HWM_I2C_Send_Addr( u8 byte, bool is_read )
{
  int ret   = -1;
  u8  bit;
  u8  i     = 0;
  u8  size  = 7;

  //Writing 7bit slave address
  for(i = 0; i < size ; i++)
  {
    bit = ( ( byte >> ( size - ( i + 1 ) ) ) & 0x01 );  //store MSB value
    (bit) ? HWM_I2C_Set_SDA() : HWM_I2C_Clear_SDA();    //write MSB value     
    I2C_DELAY;
    HWM_I2C_Set_SCL();
    I2C_DELAY;
    HWM_I2C_Clear_SCL();
  }

  // Writing Read/Write bit (8th bit)
  (is_read) ? HWM_I2C_Set_SDA() : HWM_I2C_Clear_SDA();  //read = 1, write = 0
  I2C_DELAY;
  HWM_I2C_Set_SCL();
  I2C_DELAY;
  HWM_I2C_Clear_SCL();
  I2C_DELAY;

  if( HWM_I2C_Read_NACK_ACK() )
  {
    //got ACK
    ret = 0;
  }

  return ret;
}

/*
** Function to send the one byte to the slave
** 
** Retutns 0 if success otherwise negative number
*/
static int HWM_I2C_Send_Byte( u8 byte )
{
  int ret   = -1;
  u8  bit;
  u8  i     = 0;
  u8  size  = 7;

  for(i = 0; i <= size ; i++)
  {
    bit = ( ( byte >> ( size - i ) ) & 0x01 );        //store MSB value
    (bit) ? HWM_I2C_Set_SDA() : HWM_I2C_Clear_SDA();  //write MSB value     
    I2C_DELAY;
    HWM_I2C_Set_SCL();
    I2C_DELAY;
    HWM_I2C_Clear_SCL();
  }
  
  if( HWM_I2C_Read_NACK_ACK() )
  {
    //got ACK
    ret = 0;
  }

  return ret;
}

/*
** Function to read the one byte from the slave
** 
** Retutns 0 if success otherwise negative number
*/
static int HWM_I2C_Read_Byte( u8 *byte )
{
  int ret = 0;
  
  //TODO: Implement this

  return ret;
}

/*
** Function to send the number of bytes to the slave
** 
** Retutns 0 if success otherwise negative number
*/
static int HWM_I2C_Send( u8 slave_addr, u8 *buf, u16 len )
{
  int ret = 0;
  u16 num = 0;

  do      //Break if any error
  {

    if( HWM_I2C_Send_Addr(slave_addr, false) < 0 )   // Send the slave address
    {
      pr_err("ERROR: HWM_I2C_Send_Byte - Slave Addr\n");
      ret = -1;
      break;
    }

    for( num = 0; num < len; num++)
    {
      if( HWM_I2C_Send_Byte(buf[num]) < 0 )   // Send the data bytes
      {
        pr_err("ERROR: HWM_I2C_Send_Byte - [Data = 0x%02x]\n", buf[num]);
        ret = -1;
        break;
      }
    }
  } while(false);

  return ret;  
}

/*
** Function to read the number of bytes from the slave
** 
** Retutns 0 if success otherwise negative number
*/
static int HWM_I2C_Read( u8 slave_addr, u8 *buf, u16 len )
{
  int ret = 0;
  
  //TODO: Implement this

  return ret; 
}

/*------------ I2C related functions - Bit banging method - END ----------------*/

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
  s32 ret = 0;
  do      //Break if any error
  {

    if( HWM_I2C_Init() < 0 )                  // Init the GPIOs
    {
      pr_err("ERROR: HWM_I2C_Init\n");
      break;
    }

    HWM_I2C_Sart();                           // Send START conditon
    
    for(i = 0; i < num; i++)
    {
      //int j;
      struct i2c_msg *msg_temp = &msgs[i];

      if( HWM_I2C_Send(msg_temp->addr, msg_temp->buf, msg_temp->len) < 0 )
      {
        ret = 0;
        break;
      }
      ret++;
#if 0
      pr_info("[Count: %d] [%s]: [Addr = 0x%x] [Len = %d] [Data] = ", i, __func__, msg_temp->addr, msg_temp->len);
      
      for( j = 0; j < msg_temp->len; j++ )
      {
          pr_cont("[0x%02x] ", msg_temp->buf[j]);
      }
#endif
    }
  } while(false);

  HWM_I2C_Stop();                             // Send STOP condtion

  HWM_I2C_DeInit();                           // Deinit the GPIOs

  return ret;
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

  // TODO: Implement this

  return 0;
}
static struct i2c_algo_bit_data hwm_bit_data = {
	.setsda = HWM_I2C_Clear_SDA,
	.setscl = HWM_I2C_Clear_SCL,
	.getsda = HWM_I2C_Read_SDA,
	.getscl = HWM_I2C_Read_SCL,
	.udelay = 5,
	.timeout = 100, /* 100 ms*/

};
/*
** I2C algorithm Structure
*/
static struct i2c_algorithm hwm_i2c_algorithm = {
  .smbus_xfer       = hwm_smbus_xfer,
  .master_xfer    = hwm_i2c_xfer,
  .functionality  = hwm_func,
};

/*
** I2C adapter Structure
*/
static struct i2c_adapter hwm_i2c_adapter = {
  .owner  = THIS_MODULE,
  .class  = I2C_CLASS_HWMON,//| I2C_CLASS_SPD,
  .algo   = &hwm_i2c_algorithm,
  .name   = ADAPTER_NAME,
  .algo_data = &hwm_bit_data;
  .nr     = 5,
};

/*
** Module Init function
*/
static int __init hwm_driver_init(void)
{
  int ret = -1;
  HWM_I2C_Init();
  ret = i2c_bit_add_numbered_bus(&hwm_i2c_adapter);
  
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
MODULE_VERSION("1.37");
