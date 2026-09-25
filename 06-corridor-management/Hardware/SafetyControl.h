#ifndef __SAFETY_CONTROL_H
#define __SAFETY_CONTROL_H

#include <stdint.h>

#define SAFETY_CO_FAN_ON_RAW 2000U
#define SAFETY_LIGHT_DARK_RAW 2000U
#define SAFETY_SMOKE_ALARM_RAW 2000U

typedef enum
{
	SAFETY_MODE_AUTO = 0,
	SAFETY_MODE_MANUAL = 1
} SafetyMode;

typedef struct
{
	uint16_t co_raw;
	uint16_t light_raw;
	uint16_t smoke_raw;
	uint8_t light_dark;
	uint8_t beam_blocked;
	uint8_t smoke_detected;
	uint8_t vibration_detected;
} SafetyInput;

typedef struct
{
	uint8_t light_on;
	uint8_t fan_on;
	uint8_t alarm_on;
} ManualOutput;

typedef struct
{
	uint8_t light_on;
	uint8_t fan_on;
	uint8_t alarm_on;
} SafetyOutput;

uint8_t SafetyControl_IsSmokeAlarm(uint16_t smoke_raw);
void SafetyControl_Update(SafetyMode mode,
	const SafetyInput *input,
	const ManualOutput *manual,
	SafetyOutput *output);

#endif
