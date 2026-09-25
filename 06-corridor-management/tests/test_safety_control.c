#include <stdio.h>

#include "../Hardware/SafetyControl.h"

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
	ManualOutput manual;
	int failures = 0;

	failures += check_u8("smoke below threshold",
		SafetyControl_IsSmokeAlarm(SAFETY_SMOKE_ALARM_RAW - 1U), 0);
	failures += check_u8("smoke at threshold",
		SafetyControl_IsSmokeAlarm(SAFETY_SMOKE_ALARM_RAW), 1);

	input.co_raw = SAFETY_CO_FAN_ON_RAW + 1U;
	input.light_dark = 1;
	input.beam_blocked = 1;
	input.smoke_detected = 0;
	input.vibration_detected = 0;
	manual.light_on = 0;
	manual.fan_on = 0;
	manual.alarm_on = 0;
	SafetyControl_Update(SAFETY_MODE_AUTO, &input, &manual, &output);
	failures += check_u8("auto light", output.light_on, 1);
	failures += check_u8("auto fan", output.fan_on, 1);
	failures += check_u8("auto alarm clear", output.alarm_on, 0);

	input.co_raw = SAFETY_CO_FAN_ON_RAW - 1U;
	input.beam_blocked = 0;
	input.smoke_detected = 1;
	SafetyControl_Update(SAFETY_MODE_AUTO, &input, &manual, &output);
	failures += check_u8("auto light no person", output.light_on, 0);
	failures += check_u8("auto fan low co", output.fan_on, 0);
	failures += check_u8("auto smoke alarm", output.alarm_on, 1);

	input.smoke_detected = 0;
	input.vibration_detected = 1;
	SafetyControl_Update(SAFETY_MODE_AUTO, &input, &manual, &output);
	failures += check_u8("auto vibration alarm", output.alarm_on, 1);

	manual.light_on = 1;
	manual.fan_on = 0;
	manual.alarm_on = 1;
	input.co_raw = SAFETY_CO_FAN_ON_RAW + 1U;
	input.light_dark = 0;
	input.beam_blocked = 0;
	input.smoke_detected = 0;
	input.vibration_detected = 0;
	SafetyControl_Update(SAFETY_MODE_MANUAL, &input, &manual, &output);
	failures += check_u8("manual light", output.light_on, 1);
	failures += check_u8("manual fan", output.fan_on, 0);
	failures += check_u8("manual alarm", output.alarm_on, 1);

	return failures;
}
