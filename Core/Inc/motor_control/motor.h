/*
 * motor.h
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */
#ifndef MOTOR_DRIVER_H_
#define MOTOR_DRIVER_H_

#include "stm32g4xx_hal.h"
#include "tim.h"
#include "user_interface/shell.h"

// ---- Hardware Configuration ----
#define MOTOR_PWM_TIMER        (&htim1)
#define MOTOR_CH_U            TIM_CHANNEL_1
#define MOTOR_CH_V            TIM_CHANNEL_2

// ---- PWM Range ----
#define PWM_RANGE_MAX         8500   // Timer ARR+1 (period = 8499)
#define PWM_RANGE_MIN         0

// ---- Ramp Behavior ----
#define RAMP_INCREMENT_STEP   50     // Speed adjustment per ramp cycle
#define RAMP_INTERVAL_MS      10     // Call frequency for ramp update

/**
 * @brief  Prepare the motor system: initial PWM state and CLI bindings.
 */
void motor_init(void);

/**
 * @brief  Update the target PWM duty cycle.
 * @param  value  Desired duty cycle, automatically clamped to valid boundaries.
 *
 * Note: Middle value (PWM_RANGE_MAX/2) corresponds to idle/stop condition.
 */
void motor_set_PWM(int value);

/**
 * @brief  Enable PWM output on both motor phases (U & V).
 */
void motor_start_pwm(void);

/**
 * @brief  Disable PWM output on both motor phases.
 */
void motor_stop_pwm(void);

/**
 * @brief  Smoothly transition the active PWM value toward the target value.
 *         Should be executed periodically based on RAMP_INTERVAL_MS.
 */
void motor_ramp_task(void);

#endif /* MOTOR_DRIVER_H_ */
