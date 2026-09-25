#include "SafetyIO.h"
#include "Delay.h"

#define CO_ADC_CHANNEL ADC_Channel_0
#define LIGHT_ADC_CHANNEL ADC_Channel_1
#define SMOKE_ADC_CHANNEL ADC_Channel_2

#define BEAM_GPIO GPIOB
#define BEAM_PIN  GPIO_Pin_10

#define VIBRATION_GPIO GPIOB
#define VIBRATION_PIN  GPIO_Pin_11

#define FAN_GPIO GPIOA
#define FAN_PIN  GPIO_Pin_9

#define LED_GPIO GPIOB
#define LED_PIN  GPIO_Pin_12

#define BUZZER_GPIO GPIOB
#define BUZZER_PIN  GPIO_Pin_13

#define KEY_GPIO GPIOA
#define KEY_NEXT_PIN   GPIO_Pin_5
#define KEY_SELECT_PIN GPIO_Pin_7

static uint16_t SafetyIO_ReadAdcChannel(uint8_t channel)
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

static void SafetyIO_Write(GPIO_TypeDef *gpio,
	uint16_t pin,
	BitAction active_level,
	uint8_t on)
{
	GPIO_WriteBit(gpio, pin, on ? active_level :
		(active_level == Bit_SET ? Bit_RESET : Bit_SET));
}

void SafetyIO_Init(void)
{
	GPIO_InitTypeDef gpio;
	ADC_InitTypeDef adc;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
		RCC_APB2Periph_GPIOB |
		RCC_APB2Periph_ADC1, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	gpio.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Pin = KEY_NEXT_PIN | KEY_SELECT_PIN;
	gpio.GPIO_Mode = GPIO_Mode_IPU;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_GPIO, &gpio);

	gpio.GPIO_Pin = BEAM_PIN | VIBRATION_PIN;
	gpio.GPIO_Mode = GPIO_Mode_IPU;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio);

	gpio.GPIO_Pin = FAN_PIN;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FAN_GPIO, &gpio);

	gpio.GPIO_Pin = LED_PIN | BUZZER_PIN;
	GPIO_Init(LED_GPIO, &gpio);

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

	SafetyIO_Write(FAN_GPIO, FAN_PIN, SAFETY_FAN_ACTIVE_LEVEL, 0);
	SafetyIO_Write(LED_GPIO, LED_PIN, SAFETY_LED_ACTIVE_LEVEL, 0);
	SafetyIO_Write(BUZZER_GPIO, BUZZER_PIN, SAFETY_BUZZER_ACTIVE_LEVEL, 0);
}

uint16_t SafetyIO_ReadCO(void)
{
	uint8_t i;
	uint32_t sum = 0;

	for (i = 0; i < 8; i++)
	{
		sum += SafetyIO_ReadAdcChannel(CO_ADC_CHANNEL);
	}

	return (uint16_t)(sum / 8U);
}

uint16_t SafetyIO_ReadLight(void)
{
	uint8_t i;
	uint32_t sum = 0;

	for (i = 0; i < 8; i++)
	{
		sum += SafetyIO_ReadAdcChannel(LIGHT_ADC_CHANNEL);
	}

	return (uint16_t)(sum / 8U);
}

uint16_t SafetyIO_ReadSmoke(void)
{
	uint8_t i;
	uint32_t sum = 0;

	for (i = 0; i < 8; i++)
	{
		sum += SafetyIO_ReadAdcChannel(SMOKE_ADC_CHANNEL);
	}

	return (uint16_t)(sum / 8U);
}

uint8_t SafetyIO_ReadBeamBlocked(void)
{
	return (GPIO_ReadInputDataBit(BEAM_GPIO, BEAM_PIN) ==
		SAFETY_BEAM_BLOCKED_LEVEL) ? 1 : 0;
}

uint8_t SafetyIO_ReadVibration(void)
{
	return (GPIO_ReadInputDataBit(VIBRATION_GPIO, VIBRATION_PIN) ==
		SAFETY_VIBRATION_ACTIVE_LEVEL) ? 1 : 0;
}

uint8_t SafetyIO_ReadKey(void)
{
	static uint8_t last_state = 0x03;
	uint8_t state = 0;
	uint8_t changed;
	uint8_t key = 0;

	if (GPIO_ReadInputDataBit(KEY_GPIO, KEY_NEXT_PIN) == Bit_RESET)
	{
		state |= 0x01;
	}
	if (GPIO_ReadInputDataBit(KEY_GPIO, KEY_SELECT_PIN) == Bit_RESET)
	{
		state |= 0x02;
	}

	changed = (uint8_t)(state & (uint8_t)(~last_state));
	if (changed != 0)
	{
		Delay_ms(10);
		if ((changed & 0x01) != 0)
		{
			key = SAFETY_KEY_NEXT;
		}
		else if ((changed & 0x02) != 0)
		{
			key = SAFETY_KEY_SELECT;
		}
	}

	last_state = state;
	return key;
}

uint8_t SafetyIO_ReadBuzzerOutput(void)
{
	return (uint8_t)GPIO_ReadOutputDataBit(BUZZER_GPIO, BUZZER_PIN);
}

void SafetyIO_SetOutputs(const SafetyOutput *output)
{
	SafetyIO_Write(FAN_GPIO, FAN_PIN, SAFETY_FAN_ACTIVE_LEVEL, output->fan_on);
	SafetyIO_Write(LED_GPIO, LED_PIN, SAFETY_LED_ACTIVE_LEVEL, output->light_on);
	SafetyIO_Write(BUZZER_GPIO, BUZZER_PIN,
		SAFETY_BUZZER_ACTIVE_LEVEL, output->alarm_on);
}
