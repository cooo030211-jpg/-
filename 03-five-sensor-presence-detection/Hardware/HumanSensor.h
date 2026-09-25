#ifndef __HUMAN_SENSOR_H
#define __HUMAN_SENSOR_H

#include "stm32f10x.h"

void HumanSensor_Init(void);
uint8_t HumanSensor_IsPresent(void);

#endif
