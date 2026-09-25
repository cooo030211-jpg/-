#include "SensorDisplay.h"
#include "OLED.h"
#include "OLED_HZ16.h"
#include <stdio.h>

static void SensorDisplay_ClearLine(char *line)
{
	uint8_t i;
	for (i = 0; i < 16; i++)
	{
		line[i] = ' ';
	}
	line[16] = '\0';
}

void SensorDisplay_FormatLines(const SensorSnapshot *snapshot, char lines[4][17])
{
	uint8_t i;

	for (i = 0; i < 4; i++)
	{
		SensorDisplay_ClearLine(lines[i]);
	}

	if (snapshot->dht_ok)
	{
		(void)snprintf(lines[0], 17, "T:%uC H:%u%%",
			(unsigned int)snapshot->temperature_c,
			(unsigned int)snapshot->humidity_percent);
	}
	else
	{
		(void)snprintf(lines[0], 17, "T:--C H:--%%");
	}

	if (snapshot->co2_ok)
	{
		(void)snprintf(lines[1], 17, "CO2:%uppm",
			(unsigned int)snapshot->co2_ppm);
	}
	else
	{
		(void)snprintf(lines[1], 17, "CO2:----ppm");
	}

	(void)snprintf(lines[2], 17, "MQ:%u L:%u",
		(unsigned int)snapshot->mq135_raw,
		(unsigned int)snapshot->light_raw);
	(void)snprintf(lines[3], 17, "H:%s L:%u F:%u",
		snapshot->human_present ? "Y" : "N",
		(unsigned int)snapshot->led_on,
		(unsigned int)snapshot->fan_on);
}

static void SensorDisplay_ShowValue(uint8_t line, uint8_t column, uint16_t value)
{
	OLED_ShowNum(line, column, value, 4);
}

void SensorDisplay_Show(const SensorSnapshot *snapshot, uint8_t page, uint8_t full_refresh)
{
	if (full_refresh)
	{
		OLED_Clear();
	}

	if (page == 0)
	{
		if (full_refresh)
		{
			OLED_ShowChinese(1, 1, OLED_HZ_WEN);
			OLED_ShowChinese(1, 3, OLED_HZ_DU);
			OLED_ShowChar(1, 5, ':');

			OLED_ShowChinese(2, 1, OLED_HZ_SHI);
			OLED_ShowChinese(2, 3, OLED_HZ_DU);
			OLED_ShowChar(2, 5, ':');

			OLED_ShowChinese(3, 1, OLED_HZ_ER);
			OLED_ShowChinese(3, 3, OLED_HZ_YANG);
			OLED_ShowChinese(3, 5, OLED_HZ_HUA);
			OLED_ShowChinese(3, 7, OLED_HZ_TAN);
			OLED_ShowChar(3, 9, ':');

			OLED_ShowChinese(4, 1, OLED_HZ_KONG);
			OLED_ShowChinese(4, 3, OLED_HZ_QI);
			OLED_ShowChinese(4, 5, OLED_HZ_ZHI);
			OLED_ShowChinese(4, 7, OLED_HZ_LIANG);
			OLED_ShowChar(4, 9, ':');
		}
		if (snapshot->dht_ok) OLED_ShowNum(1, 6, snapshot->temperature_c, 2);
		else OLED_ShowString(1, 6, "--");
		if (full_refresh) OLED_ShowChar(1, 8, 'C');

		if (snapshot->dht_ok) OLED_ShowNum(2, 6, snapshot->humidity_percent, 2);
		else OLED_ShowString(2, 6, "--");
		if (full_refresh) OLED_ShowChar(2, 8, '%');

		if (snapshot->co2_ok) OLED_ShowNum(3, 10, snapshot->co2_ppm, 4);
		else OLED_ShowString(3, 10, "----");

		SensorDisplay_ShowValue(4, 10, snapshot->mq135_raw);
	}
	else
	{
		if (full_refresh)
		{
			OLED_ShowChinese(1, 1, OLED_HZ_GUANG);
			OLED_ShowChinese(1, 3, OLED_HZ_ZHAO);
			OLED_ShowChar(1, 5, ':');

			OLED_ShowChinese(2, 1, OLED_HZ_REN);
			OLED_ShowChinese(2, 3, OLED_HZ_TI);
			OLED_ShowChar(2, 5, ':');

			OLED_ShowChinese(3, 1, OLED_HZ_DENG);
			OLED_ShowChinese(3, 3, OLED_HZ_GUANG);
			OLED_ShowChar(3, 5, ':');

			OLED_ShowChinese(4, 1, OLED_HZ_FENG);
			OLED_ShowChinese(4, 3, OLED_HZ_SHAN);
			OLED_ShowChar(4, 5, ':');
		}
		SensorDisplay_ShowValue(1, 6, snapshot->light_raw);
		OLED_ShowChinese(2, 6, snapshot->human_present ? OLED_HZ_YOU : OLED_HZ_WU);
		OLED_ShowChinese(2, 8, OLED_HZ_REN);
		OLED_ShowChinese(3, 6, snapshot->led_on ? OLED_HZ_KAI : OLED_HZ_GUAN);
		OLED_ShowChinese(4, 6, snapshot->fan_on ? OLED_HZ_KAI : OLED_HZ_GUAN);
	}
}
