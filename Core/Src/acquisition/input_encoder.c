/*
 * input_encoder.c
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#include "acquisition/input_encoder.h"

#define ENCODER_MAX_COUNT 0xFFFF
volatile float encoder_speed_rps = 0.0f;
static int32_t last_position = 0;

void QEI_Start(void)
{
    // Démarre le Timer 3 en mode Interface Encodeur
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
}


int32_t QEI_Get_Count(void)
{

    return (int32_t)__HAL_TIM_GET_COUNTER(&htim3);
}


void QEI_Reset_Count(void)
{
    __HAL_TIM_SET_COUNTER(&htim3, 0);
}


void QEI_Calculate_Speed(void)
{
    //  Lire la position actuelle
    int32_t current_position = QEI_Get_Count();

    //  Calculer le changement de position
    int32_t delta_ticks = current_position - last_position;

    //  Mettre à jour la position précédente
    last_position = current_position;

    //  Calculer la vitesse en Ticks par seconde
    float speed_ticks_per_second = (float)delta_ticks / VELOCITY_DT;

    //  Convertir en Tours par seconde et stocker dans la variable volatile
    encoder_speed_rps = speed_ticks_per_second / (float)ENCODER_TICKS_PER_REVOLUTION;
}
