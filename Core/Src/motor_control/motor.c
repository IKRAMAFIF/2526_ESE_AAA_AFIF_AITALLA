/*
 * motor.c
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#include "motor_control/motor.h"
#include "tim.h"

void Motor_Start(void)
{
    // Démarrage du  U
    // Active le transistor du haut  ET celui du bas
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);

    // Démarrage du  V
    // Active le transistor du haut ET celui du bas
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
}

void Motor_Stop(void)
{
    // Arrêt des signaux PWM

    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);

    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
}

void Motor_SetSpeed(float alpha)
{
    //  Saturation de alpha entre 0.0 et 1.0
    if (alpha > 1.0f) alpha = 1.0f;
    if (alpha < 0.0f) alpha = 0.0f;

    // Récupération de la période actuelle

    uint32_t period_arr = __HAL_TIM_GET_AUTORELOAD(&htim1);

    //  Calcul des commandes U et V pour la commande décalée

    uint32_t ccr_u = (uint32_t)(alpha * (float)period_arr);
    uint32_t ccr_v = (uint32_t)((1.0f - alpha) * (float)period_arr);

    // Application aux registres du Timer
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, ccr_u);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, ccr_v);
}
