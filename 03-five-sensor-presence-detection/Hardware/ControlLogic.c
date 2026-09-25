#include "ControlLogic.h"

void ControlLogic_Update(const ControlInput *input, ControlOutput *output)
{
	if (!input->human_present &&
		input->absent_seconds >= CONTROL_NO_PERSON_OFF_SECONDS)
	{
		output->led_on = 0;
		output->fan_on = 0;
		return;
	}

	output->led_on = (input->human_present && input->light_is_dark) ? 1 : 0;
	output->fan_on = (input->co2_valid &&
		input->co2_ppm >= CONTROL_CO2_FAN_ON_PPM) ? 1 : 0;
}
