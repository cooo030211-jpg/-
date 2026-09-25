#ifndef __SENSORS_H
#define __SENSORS_H

#include "stm32f10x.h"

#define WATER_ALARM_THRESHOLD 1800U
#define SMOKE_WARNING_THRESHOLD 2000U
#define SMOKE_FIRE_THRESHOLD 2800U

void Sensors_Init(void);
uint16_t Sensors_ReadWaterRaw(void);
uint16_t Sensors_ReadSmokeRaw(void);
uint8_t Sensors_ReadPerson(void);
uint8_t Sensors_ReadDHT11(uint8_t *temperature_c, uint8_t *humidity_percent);

#endif
