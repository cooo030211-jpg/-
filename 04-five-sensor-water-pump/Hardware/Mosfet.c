#include "Mosfet.h"

static void Mosfet_Write(GPIO_TypeDef *Port, uint16_t Pin, uint8_t On)
{
	if (On != 0)
	{
		GPIO_SetBits(Port, Pin);
	}
	else
	{
		GPIO_ResetBits(Port, Pin);
	}
}

void Mosfet_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(MOSFET_FAN_GPIO_CLOCK | MOSFET_PUMP_GPIO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = MOSFET_FAN_PIN;
	GPIO_Init(MOSFET_FAN_GPIO_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = MOSFET_PUMP_PIN;
	GPIO_Init(MOSFET_PUMP_GPIO_PORT, &GPIO_InitStructure);

	/* Keep all loads off during startup. */
	GPIO_ResetBits(MOSFET_FAN_GPIO_PORT, MOSFET_FAN_PIN);
	GPIO_ResetBits(MOSFET_PUMP_GPIO_PORT, MOSFET_PUMP_PIN);
}

void Mosfet_SetFan(uint8_t On)
{
	Mosfet_Write(MOSFET_FAN_GPIO_PORT, MOSFET_FAN_PIN, On);
}

void Mosfet_SetPump(uint8_t On)
{
	Mosfet_Write(MOSFET_PUMP_GPIO_PORT, MOSFET_PUMP_PIN, On);
}
