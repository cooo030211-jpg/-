#include <stdio.h>
#include "../Hardware/ControlLogic.h"

static int check(const char *name, uint8_t actual, uint8_t expected)
{
	if (actual != expected)
	{
		printf("%s mismatch: expected %u, got %u\n",
			name, (unsigned int)expected, (unsigned int)actual);
		return 1;
	}
	return 0;
}

int main(void)
{
	ControlInput input;
	ControlOutput output;
	int failures = 0;

	input.co2_valid = 1;
	input.co2_ppm = 1200;
	input.light_is_dark = 1;
	input.human_present = 1;
	input.absent_seconds = 0;
	ControlLogic_Update(&input, &output);
	failures += check("high co2 fan", output.fan_on, 1);
	failures += check("dark with human led", output.led_on, 1);

	input.co2_ppm = 800;
	ControlLogic_Update(&input, &output);
	failures += check("normal co2 fan", output.fan_on, 0);

	input.co2_ppm = 1200;
	input.human_present = 0;
	input.absent_seconds = CONTROL_NO_PERSON_OFF_SECONDS - 1;
	ControlLogic_Update(&input, &output);
	failures += check("before ten minutes fan", output.fan_on, 1);
	failures += check("no human led", output.led_on, 0);

	input.absent_seconds = CONTROL_NO_PERSON_OFF_SECONDS;
	ControlLogic_Update(&input, &output);
	failures += check("after ten minutes fan", output.fan_on, 0);

	return failures;
}
