#ifndef __OUTPUT_H
#define __OUTPUT_H

#include "stm32f10x.h"

#define OUTPUT_LED_GPIO_PORT      GPIOA
#define OUTPUT_LED_GPIO_CLOCK     RCC_APB2Periph_GPIOA
#define OUTPUT_GREEN_LED_PIN      GPIO_Pin_1
#define OUTPUT_RED_LED_PIN        GPIO_Pin_2

#define OUTPUT_BUZZER_GPIO_PORT   GPIOB
#define OUTPUT_BUZZER_GPIO_CLOCK  RCC_APB2Periph_GPIOB
#define OUTPUT_BUZZER_PIN         GPIO_Pin_13

/* 输出有效电平：1 = 高电平动作，0 = 低电平动作。
   蜂鸣器实测为低电平有效（上电写低会直接响），故 BUZZER_ACTIVE_HIGH = 0；
   绿灯/红灯暂未实测，如出现"显示 1 却不亮"再改成 0。 */
#define BUZZER_ACTIVE_HIGH        0
#define GREEN_LED_ACTIVE_HIGH     1
#define RED_LED_ACTIVE_HIGH       1

void Output_Init(void);
void Output_SetGreenLed(uint8_t On);
void Output_SetRedLed(uint8_t On);
void Output_SetBuzzer(uint8_t On);

#endif
