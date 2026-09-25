#ifndef __SAFETY_MENU_H
#define __SAFETY_MENU_H

#include "SafetyControl.h"

#define SAFETY_MENU_KEY_NEXT   1U
#define SAFETY_MENU_KEY_SELECT 2U

#define SAFETY_MENU_PAGE_COUNT 6U
#define SAFETY_MENU_PAGE_SENSOR0 0U
#define SAFETY_MENU_PAGE_SENSOR1 1U
#define SAFETY_MENU_PAGE_MODE    2U
#define SAFETY_MENU_PAGE_FAN     3U
#define SAFETY_MENU_PAGE_LIGHT   4U
#define SAFETY_MENU_PAGE_ALARM   5U

typedef struct
{
	uint8_t page;
	SafetyMode mode;
	ManualOutput manual;
} SafetyMenu;

void SafetyMenu_Init(SafetyMenu *menu);
void SafetyMenu_HandleKey(SafetyMenu *menu, uint8_t key);

#endif
