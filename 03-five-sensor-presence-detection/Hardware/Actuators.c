#include "Actuators.h"

#define ACTUATOR_GPIO       GPIOB
#define ACTUATOR_LED_PIN    GPIO_Pin_12
#define ACTUATOR_FAN_PIN    GPIO_Pin_13

static void Actuators_Write(uint16_t pin, uint8_t on)
{
#if ACTUATOR_ACTIVE_LOW
	if (on)
	{
		GPIO_ResetBits(ACTUATOR_GPIO, pin);
	}
	else
	{
		GPIO_SetBits(ACTUATOR_GPIO, pin);
	}
#else
	if (on)
	{
		GPIO_SetBits(ACTUATOR_GPIO, pin);
	}
	else
	{
		GPIO_ResetBits(ACTUATOR_GPIO, pin);
	}
#endif
}

void Actuators_Init(void)
{
	GPIO_InitTypeDef gpio;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = ACTUATOR_LED_PIN | ACTUATOR_FAN_PIN;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ACTUATOR_GPIO, &gpio);

	Actuators_Write(ACTUATOR_LED_PIN, 0);
	Actuators_Write(ACTUATOR_FAN_PIN, 0);
}

void Actuators_SetLED(uint8_t on)
{
	Actuators_Write(ACTUATOR_LED_PIN, on);
}

void Actuators_SetFan(uint8_t on)
{
	Actuators_Write(ACTUATOR_FAN_PIN, on);
}
