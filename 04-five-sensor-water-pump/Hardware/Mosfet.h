#ifndef __MOSFET_H
#define __MOSFET_H

#include "stm32f10x.h"

/* Motor loads only: fan on PA10, pump on PA11. */
#define MOSFET_FAN_GPIO_PORT   GPIOA
#define MOSFET_FAN_GPIO_CLOCK  RCC_APB2Periph_GPIOA
#define MOSFET_FAN_PIN         GPIO_Pin_10

#define MOSFET_PUMP_GPIO_PORT  GPIOA
#define MOSFET_PUMP_GPIO_CLOCK RCC_APB2Periph_GPIOA
#define MOSFET_PUMP_PIN        GPIO_Pin_11

void Mosfet_Init(void);
void Mosfet_SetFan(uint8_t On);
void Mosfet_SetPump(uint8_t On);

#endif
