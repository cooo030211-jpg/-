#ifndef __ANALOG_SENSORS_H
#define __ANALOG_SENSORS_H

#include "stm32f10x.h"

#define ANALOG_SENSOR_LIGHT_CHANNEL  ADC_Channel_4
#define ANALOG_SENSOR_MQ135_CHANNEL  ADC_Channel_5

void AnalogSensors_Init(void);
uint16_t AnalogSensors_ReadLight(void);
uint16_t AnalogSensors_ReadMQ135(void);

#endif
