#ifndef __CONTROL_LOGIC_H
#define __CONTROL_LOGIC_H

#include <stdint.h>

#define CONTROL_CO2_FAN_ON_PPM          1000U
#define CONTROL_LIGHT_DARK_THRESHOLD    1500U
#define CONTROL_NO_PERSON_OFF_SECONDS   600U
#define CONTROL_CO2_TIMEOUT_SECONDS     5U

typedef struct
{
	uint8_t co2_valid;
	uint16_t co2_ppm;
	uint8_t light_is_dark;
	uint8_t human_present;
	uint16_t absent_seconds;
} ControlInput;

typedef struct
{
	uint8_t led_on;
	uint8_t fan_on;
} ControlOutput;

void ControlLogic_Update(const ControlInput *input, ControlOutput *output);

#endif
