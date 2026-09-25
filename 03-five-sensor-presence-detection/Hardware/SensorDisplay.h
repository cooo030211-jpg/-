#ifndef __SENSOR_DISPLAY_H
#define __SENSOR_DISPLAY_H

#include <stdint.h>

typedef struct
{
	uint8_t dht_ok;
	uint8_t temperature_c;
	uint8_t humidity_percent;
	uint16_t mq135_raw;
	uint16_t light_raw;
	uint16_t co2_ppm;
	uint8_t co2_ok;
	uint8_t human_present;
	uint8_t led_on;
	uint8_t fan_on;
} SensorSnapshot;

void SensorDisplay_FormatLines(const SensorSnapshot *snapshot, char lines[4][17]);
void SensorDisplay_Show(const SensorSnapshot *snapshot, uint8_t page, uint8_t full_refresh);

#endif
