#ifndef __SAFETY_DISPLAY_H
#define __SAFETY_DISPLAY_H

#include "SafetyControl.h"

#define SAFETY_DISPLAY_PAGE_NONE 0xFFU

typedef struct
{
	uint16_t co_raw;
	uint16_t light_raw;
	uint16_t smoke_raw;
	uint8_t light_on;
	uint8_t fan_on;
	uint8_t alarm_on;
	uint8_t smoke_detected;
	uint8_t beam_blocked;
	uint8_t vibration_detected;
} SafetyDisplayOverview;

uint8_t SafetyDisplay_EnterPage(uint8_t page, uint8_t *last_page);
void SafetyDisplay_BuildOverview(const SafetyInput *input,
	const SafetyOutput *output,
	SafetyDisplayOverview *overview);

#endif
