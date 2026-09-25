#include "SafetyControl.h"

uint8_t SafetyControl_IsSmokeAlarm(uint16_t smoke_raw)
{
	return (smoke_raw >= SAFETY_SMOKE_ALARM_RAW) ? 1U : 0U;
}

void SafetyControl_Update(SafetyMode mode,
	const SafetyInput *input,
	const ManualOutput *manual,
	SafetyOutput *output)
{
	if (mode == SAFETY_MODE_MANUAL)
	{
		output->light_on = manual->light_on ? 1 : 0;
		output->fan_on = manual->fan_on ? 1 : 0;
		output->alarm_on = manual->alarm_on ? 1 : 0;
		return;
	}

	output->light_on = (input->beam_blocked && input->light_dark) ? 1 : 0;
	output->fan_on = (input->co_raw >= SAFETY_CO_FAN_ON_RAW) ? 1 : 0;
	output->alarm_on =
		(input->smoke_detected || input->vibration_detected) ? 1 : 0;
}
