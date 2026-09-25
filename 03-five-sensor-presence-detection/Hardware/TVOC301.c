#include "TVOC301.h"

#define TVOC301_FRAME_LENGTH 9
#define TVOC301_HEADER_0     0x2C
#define TVOC301_HEADER_1     0xE4

static volatile uint16_t tvoc301_co2_ppm;
static volatile uint8_t tvoc301_co2_valid;
static uint8_t tvoc301_frame[TVOC301_FRAME_LENGTH];
static uint8_t tvoc301_index;

void TVOC301_Init(void)
{
	GPIO_InitTypeDef gpio;
	USART_InitTypeDef usart;
	NVIC_InitTypeDef nvic;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = GPIO_Pin_9;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);
	gpio.GPIO_Pin = GPIO_Pin_10;
	gpio.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &gpio);

	USART_StructInit(&usart);
	usart.USART_BaudRate = 9600;
	usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &usart);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	nvic.NVIC_IRQChannel = USART1_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 1;
	nvic.NVIC_IRQChannelSubPriority = 1;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

	USART_Cmd(USART1, ENABLE);
}

uint8_t TVOC301_GetCO2(uint16_t *co2_ppm)
{
	if (!tvoc301_co2_valid)
	{
		return 0;
	}
	*co2_ppm = tvoc301_co2_ppm;
	return 1;
}

void USART1_IRQHandler(void)
{
	uint8_t byte;
	uint8_t i;
	uint8_t checksum = 0;

	if (USART_GetITStatus(USART1, USART_IT_RXNE) == RESET)
	{
		return;
	}

	byte = (uint8_t)USART_ReceiveData(USART1);

	if (tvoc301_index == 0)
	{
		if (byte == TVOC301_HEADER_0)
		{
			tvoc301_frame[tvoc301_index++] = byte;
		}
		return;
	}

	if (tvoc301_index == 1)
	{
		if (byte == TVOC301_HEADER_1)
		{
			tvoc301_frame[tvoc301_index++] = byte;
		}
		else
		{
			tvoc301_index = (byte == TVOC301_HEADER_0) ? 1 : 0;
			if (tvoc301_index == 1)
			{
				tvoc301_frame[0] = byte;
			}
		}
		return;
	}

	tvoc301_frame[tvoc301_index++] = byte;
	if (tvoc301_index < TVOC301_FRAME_LENGTH)
	{
		return;
	}

	for (i = 0; i < 8; i++)
	{
		checksum = (uint8_t)(checksum + tvoc301_frame[i]);
	}
	if (checksum == tvoc301_frame[8])
	{
		tvoc301_co2_ppm = (uint16_t)(((uint16_t)tvoc301_frame[6] << 8) | tvoc301_frame[7]);
		tvoc301_co2_valid = 1;
	}
	tvoc301_index = 0;
}
