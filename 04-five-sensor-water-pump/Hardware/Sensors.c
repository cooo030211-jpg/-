#include "Sensors.h"
#include "Delay.h"

#define DHT11_GPIO GPIOA
#define DHT11_PIN  GPIO_Pin_3
#define WATER_PIN  GPIO_Pin_4
#define SMOKE_PIN  GPIO_Pin_0
#define PERSON_GPIO GPIOA
#define PERSON_PIN  GPIO_Pin_5

/* 检出电平极性：1 = 输出低电平表示有人（对射/反射光电、NPN 开路输出，配上拉）；
   0 = 输出高电平表示有人（HC-SR501 等推挽输出模块，配下拉）。
   换红外模块时只改这一个宏，模式配置和读取函数都会跟着变。 */
#define PERSON_ACTIVE_LOW 1

#if (PERSON_ACTIVE_LOW == 1)
#define PERSON_GPIO_MODE GPIO_Mode_IPU
#define PERSON_ACTIVE_LEVEL Bit_RESET
#else
#define PERSON_GPIO_MODE GPIO_Mode_IPD
#define PERSON_ACTIVE_LEVEL Bit_SET
#endif

static void DHT11_SetOutput(void)
{
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = DHT11_PIN;
	gpio.GPIO_Mode = GPIO_Mode_Out_OD;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);
}

static void DHT11_SetInput(void)
{
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = DHT11_PIN;
	gpio.GPIO_Mode = GPIO_Mode_IPU;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);
}

static uint8_t DHT11_WaitLevel(BitAction level, uint16_t timeout_us)
{
	while (GPIO_ReadInputDataBit(GPIOA, DHT11_PIN) != level)
	{
		if (timeout_us-- == 0)
		{
			return 0;
		}
		Delay_us(1);
	}
	return 1;
}

static uint8_t DHT11_ReadByte(void)
{
	uint8_t i;
	uint8_t value = 0;

	for (i = 0; i < 8; i++)
	{
		if (!DHT11_WaitLevel(Bit_SET, 100))
		{
			return 0;
		}
		Delay_us(40);
		value <<= 1;
		if (GPIO_ReadInputDataBit(GPIOA, DHT11_PIN) == Bit_SET)
		{
			value |= 1;
		}
		if (!DHT11_WaitLevel(Bit_RESET, 100))
		{
			return 0;
		}
	}
	return value;
}

static uint16_t ADC_ReadChannel(uint8_t channel)
{
	uint16_t value;

	ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_239Cycles5);
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET) {}
	value = ADC_GetConversionValue(ADC1);

	/* STM32F103 ADC为12位，避免异常值污染显示和报警逻辑。 */
	if (value > 4095U)
	{
		value = 4095U;
	}
	return value;
}

void Sensors_Init(void)
{
	GPIO_InitTypeDef gpio;
	ADC_InitTypeDef adc;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
		RCC_APB2Periph_ADC1, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	DHT11_SetOutput();
	GPIO_SetBits(GPIOA, DHT11_PIN);

	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = WATER_PIN | SMOKE_PIN;
	gpio.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &gpio);

	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = PERSON_PIN;
	gpio.GPIO_Mode = PERSON_GPIO_MODE;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(PERSON_GPIO, &gpio);

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
	while (ADC_GetResetCalibrationStatus(ADC1) != RESET) {}
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) != RESET) {}
}

uint16_t Sensors_ReadWaterRaw(void)
{
	return ADC_ReadChannel(ADC_Channel_4);
}

uint16_t Sensors_ReadSmokeRaw(void)
{
	return ADC_ReadChannel(ADC_Channel_0);
}

uint8_t Sensors_ReadPerson(void)
{
	/* 检出电平由 PERSON_ACTIVE_LOW 决定，见本文件顶部说明。 */
	return (GPIO_ReadInputDataBit(PERSON_GPIO, PERSON_PIN) == PERSON_ACTIVE_LEVEL) ? 1 : 0;
}

uint8_t Sensors_ReadDHT11(uint8_t *temperature_c, uint8_t *humidity_percent)
{
	uint8_t data[5];
	uint8_t i;

	if ((temperature_c == 0) || (humidity_percent == 0))
	{
		return 0;
	}

	DHT11_SetOutput();
	GPIO_ResetBits(GPIOA, DHT11_PIN);
	Delay_ms(20);
	GPIO_SetBits(GPIOA, DHT11_PIN);
	Delay_us(30);
	DHT11_SetInput();

	if (!DHT11_WaitLevel(Bit_RESET, 100) ||
		!DHT11_WaitLevel(Bit_SET, 100) ||
		!DHT11_WaitLevel(Bit_RESET, 100))
	{
		DHT11_SetOutput();
		GPIO_SetBits(GPIOA, DHT11_PIN);
		return 0;
	}

	for (i = 0; i < 5; i++)
	{
		data[i] = DHT11_ReadByte();
	}

	DHT11_SetOutput();
	GPIO_SetBits(GPIOA, DHT11_PIN);

	if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) != data[4])
	{
		return 0;
	}

	*humidity_percent = data[0];
	*temperature_c = data[2];
	return 1;
}
