
/* 
 * File:   adc_driver.c
 * Author: M.Ricchieri
 *
 * Created on 31. mai 2023, 08:55
 * 
 * Inspired by the "Mc32DriverAdc.c" file
 */

//----------------------------------------------------------------------------// Includes
#include "app.h"
#include "I2C_ICM42670P_Functions.h"
#include "imu/inv_imu_driver.h"
#include "imu/inv_imu_transport.h"
#include "Invn/EmbUtils/RingBuffer.h"


//----------------------------------------------------------------------------// Constants
#if !USE_FIFO
/* 
	 * Buffer to keep track of the timestamp when IMU data ready interrupt fires.
	 * The buffer can contain up to 64 items in order to store one timestamp 
     * for each packet in FIFO.
	 */
RINGBUFFER(timestamp_buffer, 64, uint64_t);
#endif


//----------------------------------------------------------------------------// get_imu_data
 /** @brief this function provides a way to retrieve IMU data either from a FIFO 
 *          buffer if USE_FIFO is defined, or directly from the registers if 
 *          USE_FIFO is not defined. 
 *          
 *   @param[in]     -               -
 *   @return        data from "inv_imu_get_data_from_registers" function or 
 *                  data from "inv_imu_get_data_from_fifo"
 */
int get_imu_data(void)
{
#if USE_FIFO
	return inv_imu_get_data_from_fifo(&myImuDevice);
#else
	return inv_imu_get_data_from_registers(&myImuDevice);
#endif
}


//----------------------------------------------------------------------------// configureImuDevice
 /** @brief This function configures the device in order to output gyro and 
 *          accelerometer.
 * 
 *   @return        rc              InvError structure parameter, 0 on success
 */
int configureImuDevice(void)
{
	int rc = 0;

	if (!USE_FIFO)
		rc |= inv_imu_configure_fifo(&myImuDevice, INV_IMU_FIFO_DISABLED);

	if (USE_HIGH_RES_MODE) {
		rc |= inv_imu_enable_high_resolution_fifo(&myImuDevice);
	} else {
		rc |= inv_imu_set_accel_fsr(&myImuDevice, ACCEL_CONFIG0_FS_SEL_4g);
		rc |= inv_imu_set_gyro_fsr(&myImuDevice, GYRO_CONFIG0_FS_SEL_250dps);
	}

	if (USE_LOW_NOISE_MODE) {
		rc |= inv_imu_set_accel_frequency(&myImuDevice, ACCEL_CONFIG0_ODR_50_HZ);
		rc |= inv_imu_set_gyro_frequency(&myImuDevice, GYRO_CONFIG0_ODR_200_HZ);
		rc |= inv_imu_enable_accel_low_noise_mode(&myImuDevice);
	} else {
		rc |= inv_imu_set_accel_frequency(&myImuDevice, ACCEL_CONFIG0_ODR_50_HZ);
		rc |= inv_imu_set_gyro_frequency(&myImuDevice, GYRO_CONFIG0_ODR_200_HZ);
		rc |= inv_imu_enable_accel_low_power_mode(&myImuDevice);
	}
//    rc |= inv_imu_set_accel_lp_avg(&myImuDevice, ACCEL_CONFIG1_ACCEL_FILT_AVG_8);

	rc |= inv_imu_enable_gyro_low_noise_mode(&myImuDevice);

	if (!USE_FIFO)
		inv_imu_sleep_us(GYR_STARTUP_TIME_US);

	return rc;
}



//----------------------------------------------------------------------------// setupImuDevice
 /** @brief This function is in charge of reseting and initializing IMU device. 
 *          It should be successfully executed before any access to IMU device.
 * 
 *   @param[in]     icm_serif       pointer to the serial interface structure
 *   @return        rc              InvError structure parameter, 0 on success
 */
int setupImuDevice(struct inv_imu_serif *icm_serif){
    
	int rc = 0;
	uint8_t who_am_i;

	// Initialization of the device
	rc = inv_imu_init(&myImuDevice, icm_serif, imu_callback);
	if (rc != INV_ERROR_SUCCESS){
        
		return rc;
	}

	// Check WHOAMI
	rc = inv_imu_get_who_am_i(&myImuDevice, &who_am_i);
	if (rc != INV_ERROR_SUCCESS){
        
		return rc;
	}

	if (who_am_i != ICM_WHOAMI){
        
		return INV_ERROR;
	}

#if !USE_FIFO
	RINGBUFFER_CLEAR(&timestamp_buffer);
#endif

	return rc;
}


//----------------------------------------------------------------------------// inv_imu_sleep_us
 /** @brief This function is in charge of delaying the program for a certain 
 *          time.
 *
 *   @param[in]     us              time in microsecond
 *   @return        -               -
 */
void inv_imu_sleep_us(uint32_t us){
    
    uint16_t finalValue;
    
    // Prepares the Timer3 and the counting variable
    DRV_TMR3_CounterClear();
//    appData.usCounter32 = 0;
    // Starts Timer3
    DRV_TMR3_Start();
    
//    finalValue = (us * 14.745600) + 8;  //+8 pour arrondir à la 1/2 us supérieure lors du passage float -> int
    finalValue = us * 15;
    
    // Wait until the while loop is not true anymore
//    while(appData.usCounter32 < us){}
    while (DRV_TMR3_CounterValueGet() < finalValue);
    // Stops Timer3
    DRV_TMR3_Stop();
}


//----------------------------------------------------------------------------// inv_imu_sleep_ms
 /** @brief This function is in charge of delaying the program for a certain 
 *          time.
 *
 *   @param[in]     ms              time in millisecond
 *   @return        -               -
 */
void inv_imu_sleep_ms(uint32_t ms){
    
    uint32_t i;
    for (i = 0; i < ms; i++) {
        
        inv_imu_sleep_us(1000);
    }
}


//----------------------------------------------------------------------------// inv_imu_get_time_us
 /** @brief This function is in charge of TIMOUT uses
 *
 *   @param[in]     -               -
 *   @return        xxx
 */
uint64_t inv_imu_get_time_us(void){
    
    // Not in int64 but normal
    return DRV_TMR2_CounterValueGet() / 15; // + appData.usCounter64;
}



