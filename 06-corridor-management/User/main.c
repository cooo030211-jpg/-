#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "SafetyControl.h"
#include "SafetyIO.h"
#include "SafetyMenu.h"
#include "SafetyDisplay.h"
#include "OLED_HZ16.h"

static void Display_ShowText(uint8_t line, uint8_t column, char *text)
{
	OLED_ShowString(line, column, text);
}

static void Display_ShowOnOff(uint8_t line, uint8_t column, uint8_t on)
{
	OLED_ShowChinese(line, column, on ? OLED_HZ_KAI : OLED_HZ_GUAN);
}

static void Display_ShowYesNo(uint8_t line, uint8_t column, uint8_t yes)
{
	OLED_ShowChinese(line, column, yes ? OLED_HZ_YOU : OLED_HZ_WU);
}

static void Display_ShowMode(uint8_t line, uint8_t column, SafetyMode mode)
{
	if (mode == SAFETY_MODE_AUTO)
	{
		OLED_ShowChinese(line, column, OLED_HZ_ZI);
		OLED_ShowChinese(line, column + 2, OLED_HZ_DONG);
	}
	else
	{
		OLED_ShowChinese(line, column, OLED_HZ_SHOU);
		OLED_ShowChinese(line, column + 2, OLED_HZ_DONG);
	}
}

static void Display_DrawStatic(const SafetyMenu *menu)
{
	OLED_Clear();

	if (menu->page == SAFETY_MENU_PAGE_SENSOR0)
	{
		OLED_ShowChinese(1, 1, OLED_HZ_YI);
		OLED_ShowChinese(1, 3, OLED_HZ_YANG);
		OLED_ShowChinese(1, 5, OLED_HZ_HUA);
		OLED_ShowChinese(1, 7, OLED_HZ_TAN);
		OLED_ShowChar(1, 9, ':');

		OLED_ShowChinese(2, 1, OLED_HZ_GUANG);
		OLED_ShowChinese(2, 3, OLED_HZ_ZHAO);
		OLED_ShowChar(2, 5, ':');

		OLED_ShowChinese(3, 1, OLED_HZ_YAN);
		OLED_ShowChinese(3, 3, OLED_HZ_WU2);
		OLED_ShowChar(3, 5, ':');

		OLED_ShowChinese(4, 1, OLED_HZ_REN);
		OLED_ShowChinese(4, 3, OLED_HZ_TI);
		OLED_ShowChar(4, 5, ':');
		OLED_ShowChinese(4, 9, OLED_HZ_ZHEN);
		OLED_ShowChinese(4, 11, OLED_HZ_DONG);
		OLED_ShowChar(4, 13, ':');
	}
	else if (menu->page == SAFETY_MENU_PAGE_SENSOR1)
	{
		OLED_ShowChinese(1, 1, OLED_HZ_KONG2);
		OLED_ShowChinese(1, 3, OLED_HZ_ZHI2);
		OLED_ShowChinese(1, 5, OLED_HZ_ZHUANG);
		OLED_ShowChinese(1, 7, OLED_HZ_TAI);

		OLED_ShowChinese(2, 1, OLED_HZ_DENG);
		OLED_ShowChinese(2, 3, OLED_HZ_GUANG);
		OLED_ShowChar(2, 5, ':');

		OLED_ShowChinese(3, 1, OLED_HZ_FENG);
		OLED_ShowChinese(3, 3, OLED_HZ_SHAN);
		OLED_ShowChar(3, 5, ':');

		OLED_ShowChinese(4, 1, OLED_HZ_FENG2);
		OLED_ShowChinese(4, 3, OLED_HZ_MING);
		OLED_ShowChinese(4, 5, OLED_HZ_QI2);
		OLED_ShowChar(4, 7, ':');
	}
	else if (menu->page == SAFETY_MENU_PAGE_MODE)
	{
		OLED_ShowChinese(1, 1, OLED_HZ_GONG);
		OLED_ShowChinese(1, 3, OLED_HZ_ZUO);
		OLED_ShowChinese(1, 5, OLED_HZ_MO);
		OLED_ShowChinese(1, 7, OLED_HZ_SHI2);

		OLED_ShowChinese(2, 1, OLED_HZ_DANG);
		OLED_ShowChinese(2, 3, OLED_HZ_QIAN);
		OLED_ShowChar(2, 5, ':');

		Display_ShowText(3, 1, "PA7");
		OLED_ShowChinese(3, 5, OLED_HZ_QIE);
		OLED_ShowChinese(3, 7, OLED_HZ_HUAN);
		Display_ShowText(4, 1, "PB13:");
	}
	else if (menu->page == SAFETY_MENU_PAGE_LIGHT)
	{
		OLED_ShowChinese(1, 1, OLED_HZ_SHOU);
		OLED_ShowChinese(1, 3, OLED_HZ_DONG);
		OLED_ShowChinese(1, 5, OLED_HZ_DENG);
		OLED_ShowChinese(1, 7, OLED_HZ_GUANG);
	}
	else if (menu->page == SAFETY_MENU_PAGE_FAN)
	{
		OLED_ShowChinese(1, 1, OLED_HZ_SHOU);
		OLED_ShowChinese(1, 3, OLED_HZ_DONG);
		OLED_ShowChinese(1, 5, OLED_HZ_FENG);
		OLED_ShowChinese(1, 7, OLED_HZ_SHAN);
	}
	else
	{
		OLED_ShowChinese(1, 1, OLED_HZ_SHOU);
		OLED_ShowChinese(1, 3, OLED_HZ_DONG);
		OLED_ShowChinese(1, 5, OLED_HZ_FENG2);
		OLED_ShowChinese(1, 7, OLED_HZ_MING);
		OLED_ShowChinese(1, 9, OLED_HZ_QI2);
	}

	if (menu->page == SAFETY_MENU_PAGE_LIGHT ||
		menu->page == SAFETY_MENU_PAGE_FAN ||
		menu->page == SAFETY_MENU_PAGE_ALARM)
	{
		OLED_ShowChinese(2, 1, OLED_HZ_ZHUANG);
		OLED_ShowChinese(2, 3, OLED_HZ_TAI);
		OLED_ShowChar(2, 5, ':');
		Display_ShowText(3, 1, "PA7");
		OLED_ShowChinese(3, 5, OLED_HZ_QIE);
		OLED_ShowChinese(3, 7, OLED_HZ_HUAN);
		Display_ShowText(4, 1, "PA5");
		OLED_ShowChinese(4, 5, OLED_HZ_FAN2);
		OLED_ShowChinese(4, 7, OLED_HZ_YE);
	}
}

static void Display_DrawDynamic(const SafetyMenu *menu,
	const SafetyInput *input,
	const SafetyOutput *output)
{
	SafetyDisplayOverview overview;

	SafetyDisplay_BuildOverview(input, output, &overview);

	if (menu->page == SAFETY_MENU_PAGE_SENSOR0)
	{
		OLED_ShowNum(1, 10, overview.co_raw, 4);
		OLED_ShowNum(2, 6, overview.light_raw, 4);
		OLED_ShowNum(3, 6, overview.smoke_raw, 4);
		Display_ShowYesNo(4, 6, overview.beam_blocked);
		Display_ShowYesNo(4, 14, overview.vibration_detected);
	}
	else if (menu->page == SAFETY_MENU_PAGE_SENSOR1)
	{
		Display_ShowOnOff(2, 7, overview.light_on);
		Display_ShowOnOff(3, 7, overview.fan_on);
		Display_ShowOnOff(4, 9, overview.alarm_on);
	}
	else if (menu->page == SAFETY_MENU_PAGE_MODE)
	{
		Display_ShowMode(2, 7, menu->mode);
	}
	else if (menu->page == SAFETY_MENU_PAGE_LIGHT)
	{
		Display_ShowOnOff(2, 7, menu->manual.light_on);
	}
	else if (menu->page == SAFETY_MENU_PAGE_FAN)
	{
		Display_ShowOnOff(2, 7, menu->manual.fan_on);
	}
	else
	{
		Display_ShowOnOff(2, 7, menu->manual.alarm_on);
		OLED_ShowNum(4, 6, SafetyIO_ReadBuzzerOutput(), 1);
	}
}

static void Display_Update(const SafetyMenu *menu,
	const SafetyInput *input,
	const SafetyOutput *output,
	uint8_t page_changed)
{
	if (page_changed)
	{
		Display_DrawStatic(menu);
	}
	Display_DrawDynamic(menu, input, output);
}

int main(void)
{
	SafetyMenu menu;
	SafetyInput input;
	SafetyOutput output;
	uint8_t key;
	uint8_t displayed_page = SAFETY_DISPLAY_PAGE_NONE;

	OLED_Init();
	SafetyIO_Init();
	SafetyMenu_Init(&menu);

	while (1)
	{
		key = SafetyIO_ReadKey();
		if (key == SAFETY_KEY_NEXT)
		{
			SafetyMenu_HandleKey(&menu, SAFETY_MENU_KEY_NEXT);
		}
		else if (key == SAFETY_KEY_SELECT)
		{
			SafetyMenu_HandleKey(&menu, SAFETY_MENU_KEY_SELECT);
		}

		input.co_raw = SafetyIO_ReadCO();
		input.light_raw = SafetyIO_ReadLight();
		input.smoke_raw = SafetyIO_ReadSmoke();
		input.light_dark =
			(input.light_raw >= SAFETY_LIGHT_DARK_RAW) ? 1 : 0;
		input.smoke_detected =
			SafetyControl_IsSmokeAlarm(input.smoke_raw);
		input.beam_blocked = SafetyIO_ReadBeamBlocked();
		input.vibration_detected = SafetyIO_ReadVibration();

		SafetyControl_Update(menu.mode, &input, &menu.manual, &output);
		SafetyIO_SetOutputs(&output);
		Display_Update(&menu, &input, &output,
			SafetyDisplay_EnterPage(menu.page, &displayed_page));

		Delay_ms(200);
	}
}
