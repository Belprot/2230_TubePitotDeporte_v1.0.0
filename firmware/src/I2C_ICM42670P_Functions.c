
#include "stdint.h"
#include "Mc32_I2cUtilCCS.h"
#include "imu/inv_imu_driver.h"
#include "imu/inv_imu_transport.h"
#include "I2C_ICM42670P_Functions.h"


/*  \Brief: The API is used as I2C bus write
*  \Return : Status of the I2C write
*  \param dev_addr : The device address of the sensor
*  \param reg_addr : Address of the first register,
*   will data is going to be written
*  \param reg_data : It is a value hold in the array,
*      will be used for write the value into the register
*  \param cnt : The no of byte of data to be write
*/


#define ICM42670P_INIT_VALUE 0
#define I2C_BUFFER_LEN 10
#define ICM42670P_I2C_BUS_WRITE_ARRAY_INDEX 1


extern struct inv_imu_serif myImuSertif;

int ICM42670P_I2C_bus_write(struct inv_imu_serif *serif, uint8_t reg, const uint8_t *buf, uint32_t len){
    
    int cursor = 0;
    
    i2c_start();
    i2c_write(ICM42670P_ADDR_W);
    i2c_write(reg);
    
    for (cursor = 0; cursor < len; cursor++){
        
        i2c_write(*(buf+cursor));
    }
    i2c_stop();
    return 0;
}

int ICM42670P_I2C_bus_read(struct inv_imu_serif *serif, uint8_t reg, uint8_t *buf, uint32_t len){
    
    uint8_t array[len];
    int cursor = 0;
    
    i2c_start();
    i2c_write(ICM42670P_ADDR_W);
    i2c_write(reg);
    i2c_start();
    i2c_write(ICM42670P_ADDR_R);
    
    for (cursor = 0; cursor < len; cursor++){
        
        if((cursor+1) < len)
            array[cursor] = i2c_read(1);
        else
            array[cursor] = i2c_read(0);
        
        buf[cursor] = array[cursor];
        //*(buf+cursor) = array[cursor];
    }
    i2c_stop();
    return 0;
}

/*  \Brief: The API is used as I2C bus read
*  \Return : Status of the I2C read
*  \param dev_addr : The device address of the sensor
*  \param reg_addr : Address of the first register,
*  will data is going to be read
*  \param reg_data : This data read from the sensor,
*   which is hold in an array
*  \param cnt : The no of byte of data to be read
*/

//int8_t ICM42670P_I2C_bus_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt){
//
//    int8_t ICM42670P_iERROR = ICM42670P_INIT_VALUE;
//    uint8_t array[I2C_BUFFER_LEN] = { ICM42670P_INIT_VALUE };
//    uint8_t stringpos = ICM42670P_INIT_VALUE;
//
//
//    array[ICM42670P_INIT_VALUE] = reg_addr;
//
//    i2c_start();
//    // Write asked register
//    ICM42670P_iERROR = i2c_write(dev_addr<<1);
//    ICM42670P_iERROR = i2c_write(reg_addr);
//    // Send read address 
//    i2c_reStart();
//    dev_addr = (dev_addr<<1) | 0b00000001;
//    ICM42670P_iERROR = i2c_write(dev_addr);
//
//
//    /* Please take the below API as your reference
//     * for read the data using I2C communication
//     * add your I2C read API here.
//     * "BNO055_iERROR = I2C_WRITE_READ_STRING(DEV_ADDR,
//     * ARRAY, ARRAY, 1, CNT)"
//     * BNO055_iERROR is an return value of SPI write API
//     * Please select your valid return value
//     * In the driver BNO055_SUCCESS defined as 0
//     * and FAILURE defined as -1
//     */
//    for (stringpos = ICM42670P_INIT_VALUE; stringpos < cnt; stringpos++)
//    {
//        if(((stringpos+1) < cnt)&&(cnt > ICM42670P_I2C_BUS_WRITE_ARRAY_INDEX))
//            array[stringpos] = i2c_read(1);
//        else
//        array[stringpos] = i2c_read(0);
//
//        *(reg_data + stringpos) = array[stringpos];
//
//    }
//
//    i2c_stop();
//
//    if(ICM42670P_iERROR)
//        ICM42670P_iERROR = -1;
//    else
//    ICM42670P_iERROR = 0;
//
//
//    return (int8_t)(ICM42670P_iERROR);
//}
