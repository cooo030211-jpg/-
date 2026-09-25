#include "Servo.h"

void Servo_Init(void)
{
	GPIO_InitTypeDef gpio;
	TIM_TimeBaseInitTypeDef time_base;
	TIM_OCInitTypeDef oc;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO |
		RCC_APB2Periph_TIM1, ENABLE);

	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = GPIO_Pin_8;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);

	TIM_TimeBaseStructInit(&time_base);
	time_base.TIM_Prescaler = 71;
	time_base.TIM_CounterMode = TIM_CounterMode_Up;
	time_base.TIM_Period = 19999;
	time_base.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM1, &time_base);

	TIM_OCStructInit(&oc);
	oc.TIM_OCMode = TIM_OCMode_PWM1;
	oc.TIM_OutputState = TIM_OutputState_Enable;
	oc.TIM_Pulse = 500;
	oc.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM1, &oc);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
}

void Servo_SetAngle(uint8_t angle)
{
	uint16_t pulse;

	if (angle > 90)
	{
		angle = 90;
	}
	pulse = (uint16_t)(500U + ((uint16_t)angle * 1000U) / 90U);
	TIM_SetCompare1(TIM1, pulse);
}
