#include <stdio.h>

#include "../Hardware/SafetyMenu.h"

static int check_u8(const char *name, uint8_t actual, uint8_t expected)
{
	if (actual != expected)
	{
		printf("%s expected %u got %u\n",
			name, (unsigned int)expected, (unsigned int)actual);
		return 1;
	}
	return 0;
}

int main(void)
{
	SafetyMenu menu;
	int failures = 0;

	SafetyMenu_Init(&menu);
	failures += check_u8("initial page", menu.page, 0);
	failures += check_u8("initial mode", menu.mode, SAFETY_MODE_AUTO);

	SafetyMenu_HandleKey(&menu, SAFETY_MENU_KEY_NEXT);
	failures += check_u8("next page", menu.page, 1);

	SafetyMenu_HandleKey(&menu, SAFETY_MENU_KEY_NEXT);
	failures += check_u8("auto mode page", menu.page, SAFETY_MENU_PAGE_MODE);

	SafetyMenu_HandleKey(&menu, SAFETY_MENU_KEY_NEXT);
	failures += check_u8("auto mode skips manual pages", menu.page,
		SAFETY_MENU_PAGE_SENSOR0);

	menu.page = SAFETY_MENU_PAGE_MODE;
	SafetyMenu_HandleKey(&menu, SAFETY_MENU_KEY_SELECT);
	failures += check_u8("select manual", menu.mode, SAFETY_MODE_MANUAL);
	failures += check_u8("manual starts at fan page", menu.page,
		SAFETY_MENU_PAGE_FAN);

	SafetyMenu_HandleKey(&menu, SAFETY_MENU_KEY_SELECT);
	failures += check_u8("toggle fan", menu.manual.fan_on, 1);

	SafetyMenu_HandleKey(&menu, SAFETY_MENU_KEY_NEXT);
	failures += check_u8("manual next light", menu.page,
		SAFETY_MENU_PAGE_LIGHT);
	SafetyMenu_HandleKey(&menu, SAFETY_MENU_KEY_SELECT);
	failures += check_u8("toggle light", menu.manual.light_on, 1);

	SafetyMenu_HandleKey(&menu, SAFETY_MENU_KEY_NEXT);
	failures += check_u8("manual next alarm", menu.page,
		SAFETY_MENU_PAGE_ALARM);
	SafetyMenu_HandleKey(&menu, SAFETY_MENU_KEY_SELECT);
	failures += check_u8("toggle alarm", menu.manual.alarm_on, 1);

	SafetyMenu_HandleKey(&menu, SAFETY_MENU_KEY_NEXT);
	failures += check_u8("manual returns to sensor page", menu.page,
		SAFETY_MENU_PAGE_SENSOR0);

	menu.page = SAFETY_MENU_PAGE_MODE;
	SafetyMenu_HandleKey(&menu, SAFETY_MENU_KEY_SELECT);
	failures += check_u8("select auto", menu.mode, SAFETY_MODE_AUTO);
	failures += check_u8("auto stays at mode page", menu.page,
		SAFETY_MENU_PAGE_MODE);

	return failures;
}
