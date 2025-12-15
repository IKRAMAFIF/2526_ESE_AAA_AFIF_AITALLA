/*
 * analog_input.c
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */


#include "acquisition/input_analog.h"
#include "stm32g4xx_hal.h"
#include <stdint.h>

extern ADC_HandleTypeDef hadc1;

static uint16_t Read_ADC(uint32_t channel) {
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = channel;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_47CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    return HAL_ADC_GetValue(&hadc1);
}

float CurrentSensor_Read(void) {
    uint16_t adc_out = Read_ADC(ADC_CHANNEL_1);
    uint16_t adc_ref = Read_ADC(ADC_CHANNEL_2);

    float Vout = (adc_out * 3.3f) / 4095.0f;
    float Vref = (adc_ref * 3.3f) / 4095.0f;
    return (Vout - Vref) / CURRENT_SENSOR_SENSITIVITY;
}

