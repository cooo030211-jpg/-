#ifndef __TVOC301_H
#define __TVOC301_H

#include "stm32f10x.h"

void TVOC301_Init(void);
uint8_t TVOC301_GetCO2(uint16_t *co2_ppm);

#endif
