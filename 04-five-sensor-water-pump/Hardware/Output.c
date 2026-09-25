#include "Output.h"

/* On 是业务语义（1=响/亮），ActiveHigh 决定该语义对应哪个电平。 */
static void Output_Write(GPIO_TypeDef *Port, uint16_t Pin, uint8_t On,
	uint8_t ActiveHigh)
{
	uint8_t Level;

	if (ActiveHigh != 0)
	{
		Level = (On != 0) ? 1 : 0;
	}
	else
	{
		Level = (On != 0) ? 0 : 1;
	}

	if (Level != 0)
	{
		GPIO_SetBits(Port, Pin);
	}
	else
	{
		GPIO_ResetBits(Port, Pin);
	}
}

void Output_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(OUTPUT_LED_GPIO_CLOCK | OUTPUT_BUZZER_GPIO_CLOCK, ENABLE);

	/* 先把 ODR 预置成各自的静音电平（输入模式下即可写入），再切推挽输出；
	   否则输出模式建立的那一瞬间会先驱动动作电平，低有效蜂鸣器会"上电叫一声"。 */
	Output_Write(OUTPUT_BUZZER_GPIO_PORT, OUTPUT_BUZZER_PIN, 0, BUZZER_ACTIVE_HIGH);
	Output_Write(OUTPUT_LED_GPIO_PORT, OUTPUT_GREEN_LED_PIN, 0, GREEN_LED_ACTIVE_HIGH);
	Output_Write(OUTPUT_LED_GPIO_PORT, OUTPUT_RED_LED_PIN, 0, RED_LED_ACTIVE_HIGH);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = OUTPUT_GREEN_LED_PIN | OUTPUT_RED_LED_PIN;
	GPIO_Init(OUTPUT_LED_GPIO_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = OUTPUT_BUZZER_PIN;
	GPIO_Init(OUTPUT_BUZZER_GPIO_PORT, &GPIO_InitStructure);
}

void Output_SetGreenLed(uint8_t On)
{
	Output_Write(OUTPUT_LED_GPIO_PORT, OUTPUT_GREEN_LED_PIN, On,
		GREEN_LED_ACTIVE_HIGH);
}

void Output_SetRedLed(uint8_t On)
{
	Output_Write(OUTPUT_LED_GPIO_PORT, OUTPUT_RED_LED_PIN, On,
		RED_LED_ACTIVE_HIGH);
}

void Output_SetBuzzer(uint8_t On)
{
	Output_Write(OUTPUT_BUZZER_GPIO_PORT, OUTPUT_BUZZER_PIN, On,
		BUZZER_ACTIVE_HIGH);
}
