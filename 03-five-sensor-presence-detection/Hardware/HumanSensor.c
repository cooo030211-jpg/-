#include "HumanSensor.h"

#define HUMAN_SENSOR_GPIO GPIOA
#define HUMAN_SENSOR_PIN  GPIO_Pin_0

void HumanSensor_Init(void)
{
	GPIO_InitTypeDef gpio;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = HUMAN_SENSOR_PIN;
	gpio.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(HUMAN_SENSOR_GPIO, &gpio);
}

uint8_t HumanSensor_IsPresent(void)
{
	return (GPIO_ReadInputDataBit(HUMAN_SENSOR_GPIO, HUMAN_SENSOR_PIN) == Bit_SET) ? 1 : 0;
}
