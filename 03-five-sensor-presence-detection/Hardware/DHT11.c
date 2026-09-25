#include "DHT11.h"
#include "Delay.h"

#define DHT11_GPIO GPIOA
#define DHT11_PIN  GPIO_Pin_6

static void DHT11_SetOutput(void)
{
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = DHT11_PIN;
	gpio.GPIO_Mode = GPIO_Mode_Out_OD;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DHT11_GPIO, &gpio);
}

static void DHT11_SetInput(void)
{
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = DHT11_PIN;
	gpio.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(DHT11_GPIO, &gpio);
}

static uint8_t DHT11_WaitForLevel(BitAction level, uint16_t timeout_us)
{
	while (GPIO_ReadInputDataBit(DHT11_GPIO, DHT11_PIN) != level)
	{
		if (timeout_us-- == 0)
		{
			return 0;
		}
		Delay_us(1);
	}
	return 1;
}

void DHT11_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	DHT11_SetOutput();
	GPIO_SetBits(DHT11_GPIO, DHT11_PIN);
}

uint8_t DHT11_Read(uint8_t *temperature_c, uint8_t *humidity_percent)
{
	uint8_t data[5] = {0, 0, 0, 0, 0};
	uint8_t i;
	uint8_t bit;

	DHT11_SetOutput();
	GPIO_ResetBits(DHT11_GPIO, DHT11_PIN);
	Delay_ms(20);
	GPIO_SetBits(DHT11_GPIO, DHT11_PIN);
	Delay_us(30);
	DHT11_SetInput();

	if (!DHT11_WaitForLevel(Bit_RESET, 100) ||
		!DHT11_WaitForLevel(Bit_SET, 100) ||
		!DHT11_WaitForLevel(Bit_RESET, 100))
	{
		DHT11_SetOutput();
		GPIO_SetBits(DHT11_GPIO, DHT11_PIN);
		return 0;
	}

	for (i = 0; i < 40; i++)
	{
		if (!DHT11_WaitForLevel(Bit_SET, 100))
		{
			DHT11_SetOutput();
			GPIO_SetBits(DHT11_GPIO, DHT11_PIN);
			return 0;
		}

		Delay_us(40);
		bit = (GPIO_ReadInputDataBit(DHT11_GPIO, DHT11_PIN) == Bit_SET) ? 1 : 0;
		data[i / 8] <<= 1;
		data[i / 8] |= bit;

		if (!DHT11_WaitForLevel(Bit_RESET, 100))
		{
			DHT11_SetOutput();
			GPIO_SetBits(DHT11_GPIO, DHT11_PIN);
			return 0;
		}
	}

	DHT11_SetOutput();
	GPIO_SetBits(DHT11_GPIO, DHT11_PIN);

	if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) != data[4])
	{
		return 0;
	}

	*humidity_percent = data[0];
	*temperature_c = data[2];
	return 1;
}
