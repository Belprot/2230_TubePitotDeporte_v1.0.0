/* 
 * File:   inv_imu_personnal_functions.h
 * Author: ricch
 *
 * Created on 17. mai 2023, 10:56
 */

#ifndef INV_IMU_PERSONNAL_FUNCTIONS_H
#define	INV_IMU_PERSONNAL_FUNCTIONS_H

#ifdef	__cplusplus
extern "C" {
#endif

    
    
    
    int get_imu_data(void);
    int configureImuDevice(void);
    int setupImuDevice(struct inv_imu_serif *icm_serif);
    void inv_imu_sleep_us(uint32_t us);
    void inv_imu_sleep_ms(uint32_t ms);
    uint64_t inv_imu_get_time_us(void);
    
    
    
    
    
    
    
    



#ifdef	__cplusplus
}
#endif

#endif	/* INV_IMU_PERSONNAL_FUNCTIONS_H */

