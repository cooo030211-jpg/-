#include "stm32f10x.h"                  // Device header
#include "Delay.h"

#define KEY_GPIO       GPIOB
#define KEY_PIN        GPIO_Pin_6

void Key_Init(void)
{
	GPIO_InitTypeDef gpio;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = KEY_PIN;
	gpio.GPIO_Mode = GPIO_Mode_IPU;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_GPIO, &gpio);
}

uint8_t Key_GetNum(void)
{
	static uint8_t key_down = 0;

	if (GPIO_ReadInputDataBit(KEY_GPIO, KEY_PIN) == Bit_RESET)
	{
		if (!key_down)
		{
			Delay_ms(5);
			if (GPIO_ReadInputDataBit(KEY_GPIO, KEY_PIN) == Bit_RESET)
			{
				key_down = 1;
				return 1;
			}
		}
	}
	else
	{
		key_down = 0;
	}

	return 0;
}
