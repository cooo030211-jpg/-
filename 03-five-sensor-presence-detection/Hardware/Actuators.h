#ifndef __ACTUATORS_H
#define __ACTUATORS_H

#include "stm32f10x.h"

/* The connected control inputs are enabled by a high GPIO level. */
#define ACTUATOR_ACTIVE_LOW 0

void Actuators_Init(void);
void Actuators_SetLED(uint8_t on);
void Actuators_SetFan(uint8_t on);

#endif
