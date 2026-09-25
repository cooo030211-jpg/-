#include "SafetyMenu.h"

void SafetyMenu_Init(SafetyMenu *menu)
{
	menu->page = SAFETY_MENU_PAGE_SENSOR0;
	menu->mode = SAFETY_MODE_AUTO;
	menu->manual.light_on = 0;
	menu->manual.fan_on = 0;
	menu->manual.alarm_on = 0;
}

void SafetyMenu_HandleKey(SafetyMenu *menu, uint8_t key)
{
	if (key == SAFETY_MENU_KEY_NEXT)
	{
		if (menu->mode == SAFETY_MODE_AUTO)
		{
			menu->page++;
			if (menu->page > SAFETY_MENU_PAGE_MODE)
			{
				menu->page = SAFETY_MENU_PAGE_SENSOR0;
			}
		}
		else
		{
			menu->page++;
			if (menu->page >= SAFETY_MENU_PAGE_COUNT)
			{
				menu->page = SAFETY_MENU_PAGE_SENSOR0;
			}
		}
		return;
	}

	if (key != SAFETY_MENU_KEY_SELECT)
	{
		return;
	}

	if (menu->page == SAFETY_MENU_PAGE_MODE)
	{
		menu->mode = (menu->mode == SAFETY_MODE_AUTO) ?
			SAFETY_MODE_MANUAL : SAFETY_MODE_AUTO;
		if (menu->mode == SAFETY_MODE_MANUAL)
		{
			menu->page = SAFETY_MENU_PAGE_FAN;
		}
	}
	else if (menu->page == SAFETY_MENU_PAGE_LIGHT)
	{
		menu->mode = SAFETY_MODE_MANUAL;
		menu->manual.light_on = menu->manual.light_on ? 0 : 1;
	}
	else if (menu->page == SAFETY_MENU_PAGE_FAN)
	{
		menu->mode = SAFETY_MODE_MANUAL;
		menu->manual.fan_on = menu->manual.fan_on ? 0 : 1;
	}
	else if (menu->page == SAFETY_MENU_PAGE_ALARM)
	{
		menu->mode = SAFETY_MODE_MANUAL;
		menu->manual.alarm_on = menu->manual.alarm_on ? 0 : 1;
	}
}
