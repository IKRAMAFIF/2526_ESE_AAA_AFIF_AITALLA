/*
 * input_encoder.h
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#ifndef INC_ACQUISITION_INPUT_ENCODER_H_
#define INC_ACQUISITION_INPUT_ENCODER_H_


#include "stm32G4xx_hal.h"



#define VELOCITY_MEASUREMENT_FREQ_HZ 100
#define VELOCITY_DT (1.0f / VELOCITY_MEASUREMENT_FREQ_HZ)
#define ENCODER_PULSES_PER_REVOLUTION 1024
#define ENCODER_TICKS_PER_REVOLUTION (ENCODER_PULSES_PER_REVOLUTION * 4)


extern TIM_HandleTypeDef htim3;
extern volatile float encoder_speed_rps;
void QEI_Start(void);
int32_t QEI_Get_Count(void);
void QEI_Calculate_Speed(void);




#endif /* INC_ACQUISITION_INPUT_ENCODER_H_ */
