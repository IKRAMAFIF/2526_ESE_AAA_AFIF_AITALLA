/*
 * motor.c
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#include "motor_control/motor.h"
#include <stdlib.h>
// Global variables for ramp control
static int target_speed_pwm;
static int current_speed_pwm;

// Private function prototypes for shell commands
int cmd_motor_start(h_shell_t* h_shell, int argc, char** argv);
int cmd_motor_stop(h_shell_t* h_shell, int argc, char** argv);
int cmd_motor_speed(h_shell_t* h_shell, int argc, char** argv);

void motor_init(void)
{
    // Initialize to 50% duty cycle
    current_speed_pwm = PWM_MAX_VAL / 2;
    target_speed_pwm  = PWM_MAX_VAL / 2;

    // Apply initial duty cycle: U = α, V = 1 − α
    __HAL_TIM_SET_COMPARE(MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL_U, current_speed_pwm);
    __HAL_TIM_SET_COMPARE(MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL_V, PWM_MAX_VAL - current_speed_pwm);

    // Ensure PWMs are stopped initially
    motor_stop_pwm();

    // Register shell commands
    shell_add(&hshell1, "start", cmd_motor_start, "Start Motor PWM generation");
    shell_add(&hshell1, "stop", cmd_motor_stop, "Stop Motor PWM generation");
    shell_add(&hshell1, "speed", cmd_motor_speed, "Set Motor speed");
}

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

void motor_set_PWM(int percent)
{
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    // Convert percent to PWM value
    target_speed_pwm = (percent * PWM_MAX_VAL) / 100;
}

void motor_ramp_task(void)
{
    // Smooth ramp toward target
    if (current_speed_pwm < target_speed_pwm) {
        current_speed_pwm += RAMP_STEP_INCREMENT;
        if (current_speed_pwm > target_speed_pwm)
            current_speed_pwm = target_speed_pwm;
    }
    else if (current_speed_pwm > target_speed_pwm) {
        current_speed_pwm -= RAMP_STEP_INCREMENT;
        if (current_speed_pwm < target_speed_pwm)
            current_speed_pwm = target_speed_pwm;
    }

    // Apply updated PWM
    __HAL_TIM_SET_COMPARE(MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL_U, current_speed_pwm);

    // Complementary: V = 1 − α
    int comp_pwm = PWM_MAX_VAL - current_speed_pwm;
    __HAL_TIM_SET_COMPARE(MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL_V, comp_pwm);
}

/* --- Shell Commands --- */

int cmd_motor_start(h_shell_t* h_shell, int argc, char** argv)
{
    motor_start_pwm();
    int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Motor ON\r\n");
    h_shell->drv.transmit(h_shell->print_buffer, size);
    return 0;
}

int cmd_motor_stop(h_shell_t* h_shell, int argc, char** argv)
{
    motor_stop_pwm();
    int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Motor OFF\r\n");
    h_shell->drv.transmit(h_shell->print_buffer, size);
    return 0;
}

int cmd_motor_speed(h_shell_t* h_shell, int argc, char** argv)
{
    if (argc < 2)
    {
        int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Usage: speed <value>\r\n");
        h_shell->drv.transmit(h_shell->print_buffer, size);
        return -1;
    }

    int speed_val = atoi(argv[1]);
    motor_set_PWM(speed_val);

    int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Target speed set to %d\r\n", speed_val);
    h_shell->drv.transmit(h_shell->print_buffer, size);
    return 0;
}
