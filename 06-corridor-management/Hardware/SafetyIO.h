#ifndef __SAFETY_IO_H
#define __SAFETY_IO_H

#include "stm32f10x.h"
#include "SafetyControl.h"

/* Change these macros if a module behaves in reverse during hardware tests. */
#define SAFETY_SMOKE_ACTIVE_LEVEL     Bit_RESET
#define SAFETY_BEAM_BLOCKED_LEVEL     Bit_RESET
#define SAFETY_VIBRATION_ACTIVE_LEVEL Bit_SET

#define SAFETY_FAN_ACTIVE_LEVEL       Bit_SET
#define SAFETY_LED_ACTIVE_LEVEL       Bit_SET
#define SAFETY_BUZZER_ACTIVE_LEVEL    Bit_RESET

#define SAFETY_KEY_NEXT   1U
#define SAFETY_KEY_SELECT 2U

void SafetyIO_Init(void);
uint16_t SafetyIO_ReadCO(void);
uint16_t SafetyIO_ReadLight(void);
uint16_t SafetyIO_ReadSmoke(void);
uint8_t SafetyIO_ReadBeamBlocked(void);
uint8_t SafetyIO_ReadVibration(void);
uint8_t SafetyIO_ReadKey(void);
uint8_t SafetyIO_ReadBuzzerOutput(void);
void SafetyIO_SetOutputs(const SafetyOutput *output);

#endif
