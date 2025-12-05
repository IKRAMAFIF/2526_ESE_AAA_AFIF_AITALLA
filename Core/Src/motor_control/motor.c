/*
 * motor.c
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#include "motor_control/motor.h"
#include <stdlib.h>

// Internal State
static int pwm_target = 0;
static int pwm_current = 0;

// Forward Declarations
static int shell_cmd_run(h_shell_t* shell, int argc, char** argv);
static int shell_cmd_stop(h_shell_t* shell, int argc, char** argv);
static int shell_cmd_set_speed(h_shell_t* shell, int argc, char** argv);

//Initialization
void motor_init(void)
{
	// Default state: mid-range duty cycle
	pwm_current = PWM_MAX_VAL / 2;
	pwm_target  = PWM_MAX_VAL / 2;

	// Apply immediately
	__HAL_TIM_SET_COMPARE(MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL_U, pwm_current);
	__HAL_TIM_SET_COMPARE(MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL_V, pwm_current);

	// Ensure output is disabled at startup
	motor_stop_pwm();

	// Expose commands to shell
	shell_add(&hshell1, "start", shell_cmd_run, "Activate motor output");
	shell_add(&hshell1, "stop", shell_cmd_stop, "Deactivate motor output");
	shell_add(&hshell1, "speed", shell_cmd_set_speed, "Set target speed: speed <number>");
}

//PWM Control Helpers
void motor_start_pwm(void)
{
	HAL_TIM_PWM_Start(MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL_U);
	HAL_TIMEx_PWMN_Start(MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL_U);

	HAL_TIM_PWM_Start(MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL_V);
	HAL_TIMEx_PWMN_Start(MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL_V);
}

void motor_stop_pwm(void)
{
	HAL_TIM_PWM_Stop(MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL_U);
	HAL_TIMEx_PWMN_Stop(MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL_U);

	HAL_TIM_PWM_Stop(MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL_V);
	HAL_TIMEx_PWMN_Stop(MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL_V);
}

void motor_set_PWM(int requested_pwm)
{
	// Bound value within allowed PWM limits
	if (requested_pwm < PWM_MIN_VAL) requested_pwm = PWM_MIN_VAL;
	else if (requested_pwm > PWM_MAX_VAL) requested_pwm = PWM_MAX_VAL;

	pwm_target = requested_pwm;
}

void motor_ramp_task(void)
{
	// Smooth transition toward target
	if (pwm_current < pwm_target) {
		pwm_current += RAMP_STEP_INCREMENT;
		if (pwm_current > pwm_target) pwm_current = pwm_target;
	}
	else if (pwm_current > pwm_target) {
		pwm_current -= RAMP_STEP_INCREMENT;
		if (pwm_current < pwm_target) pwm_current = pwm_target;
	}

	// Update hardware register
	__HAL_TIM_SET_COMPARE(MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL_U, pwm_current);
	__HAL_TIM_SET_COMPARE(MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL_V, pwm_current);
}


// Shell Command Implementations
static int shell_cmd_run(h_shell_t* shell, int argc, char** argv)
{
	motor_start_pwm();
	int len = snprintf(shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "[Motor] ON\r\n");
	shell->drv.transmit(shell->print_buffer, len);
	return 0;
}

static int shell_cmd_stop(h_shell_t* shell, int argc, char** argv)
{
	motor_stop_pwm();
	int len = snprintf(shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "[Motor] OFF\r\n");
	shell->drv.transmit(shell->print_buffer, len);
	return 0;
}

static int shell_cmd_set_speed(h_shell_t* shell, int argc, char** argv)
{
	if (argc < 2) {
		int len = snprintf(shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Usage: speed <number>\r\n");
		shell->drv.transmit(shell->print_buffer, len);
		return -1;
	}

	int parsed_speed = atoi(argv[1]);
	motor_set_PWM(parsed_speed);

	int len = snprintf(shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Speed updated to %d\r\n", parsed_speed);
	shell->drv.transmit(shell->print_buffer, len);
	return 0;
}
