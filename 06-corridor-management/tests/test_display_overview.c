#include <stdint.h>
#include <stdio.h>

#include "../Hardware/SafetyDisplay.h"

static int check_u16(const char *name, uint16_t actual, uint16_t expected)
{
	if (actual != expected)
	{
		printf("%s expected %u got %u\n",
			name, (unsigned int)expected, (unsigned int)actual);
		return 1;
	}
	return 0;
}

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
	SafetyInput input;
	SafetyOutput output;
	SafetyDisplayOverview overview;
	int failures = 0;

	input.co_raw = 1234U;
	input.light_raw = 2345U;
	input.smoke_raw = 3456U;
	input.light_dark = 1U;
	input.smoke_detected = 1U;
	input.beam_blocked = 0U;
	input.vibration_detected = 1U;
	output.light_on = 1U;
	output.fan_on = 0U;
	output.alarm_on = 1U;

	SafetyDisplay_BuildOverview(&input, &output, &overview);

	failures += check_u16("overview co", overview.co_raw, 1234U);
	failures += check_u16("overview light", overview.light_raw, 2345U);
	failures += check_u16("overview smoke raw", overview.smoke_raw, 3456U);
	failures += check_u8("overview light output", overview.light_on, 1U);
	failures += check_u8("overview fan output", overview.fan_on, 0U);
	failures += check_u8("overview alarm output", overview.alarm_on, 1U);
	failures += check_u8("overview smoke", overview.smoke_detected, 1U);
	failures += check_u8("overview person", overview.beam_blocked, 0U);
	failures += check_u8("overview vibration", overview.vibration_detected, 1U);

	return failures;
}
