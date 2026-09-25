#include "SafetyDisplay.h"

uint8_t SafetyDisplay_EnterPage(uint8_t page, uint8_t *last_page)
{
	if (*last_page == page)
	{
		return 0;
	}

	*last_page = page;
	return 1;
}

void SafetyDisplay_BuildOverview(const SafetyInput *input,
	const SafetyOutput *output,
	SafetyDisplayOverview *overview)
{
	overview->co_raw = input->co_raw;
	overview->light_raw = input->light_raw;
	overview->smoke_raw = input->smoke_raw;
	overview->light_on = output->light_on;
	overview->fan_on = output->fan_on;
	overview->alarm_on = output->alarm_on;
	overview->smoke_detected = input->smoke_detected;
	overview->beam_blocked = input->beam_blocked;
	overview->vibration_detected = input->vibration_detected;
}
