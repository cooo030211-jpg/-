#include "AnalogSensors.h"

static uint16_t AnalogSensors_ReadChannel(uint8_t channel)
{
	uint32_t timeout = 100000;

	ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_239Cycles5);
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
	{
		if (timeout-- == 0)
		{
			return 0;
		}
	}
	return ADC_GetConversionValue(ADC1);
}

static uint16_t AnalogSensors_ReadAverage(uint8_t channel)
{
	uint8_t i;
	uint32_t sum = 0;

	for (i = 0; i < 8; i++)
	{
		sum += AnalogSensors_ReadChannel(channel);
	}
	return (uint16_t)(sum / 8);
}

void AnalogSensors_Init(void)
{
	GPIO_InitTypeDef gpio;
	ADC_InitTypeDef adc;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	gpio.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &gpio);

	ADC_DeInit(ADC1);
	ADC_StructInit(&adc);
	adc.ADC_Mode = ADC_Mode_Independent;
	adc.ADC_ScanConvMode = DISABLE;
	adc.ADC_ContinuousConvMode = DISABLE;
	adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	adc.ADC_DataAlign = ADC_DataAlign_Right;
	adc.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &adc);
	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1) != RESET)
	{
	}
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) != RESET)
	{
	}
}

uint16_t AnalogSensors_ReadLight(void)
{
	return AnalogSensors_ReadAverage(ANALOG_SENSOR_LIGHT_CHANNEL);
}

uint16_t AnalogSensors_ReadMQ135(void)
{
	return AnalogSensors_ReadAverage(ANALOG_SENSOR_MQ135_CHANNEL);
}
