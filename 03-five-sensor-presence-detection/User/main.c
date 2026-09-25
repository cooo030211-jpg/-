#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "AnalogSensors.h"
#include "Actuators.h"
#include "ControlLogic.h"
#include "DHT11.h"
#include "HumanSensor.h"
#include "SensorDisplay.h"
#include "TVOC301.h"
#include "Key.h"

int main(void)
{
	SensorSnapshot snapshot = {0};
	ControlInput control_input;
	ControlOutput control_output;
	uint8_t temperature_c;
	uint8_t humidity_percent;
	uint16_t co2_ppm;
	uint16_t absent_seconds = 0;
	uint8_t display_page = 0;
	uint8_t need_refresh = 1;
	uint8_t key_tick;

	OLED_Init();
	AnalogSensors_Init();
	DHT11_Init();
	HumanSensor_Init();
	TVOC301_Init();
 	Actuators_Init();
	Key_Init();
	OLED_Clear();

	while (1)
	{
		snapshot.mq135_raw = AnalogSensors_ReadMQ135();
		snapshot.light_raw = AnalogSensors_ReadLight();
		snapshot.human_present = HumanSensor_IsPresent();
		if (snapshot.human_present)
		{
			absent_seconds = 0;
		}
		else if (absent_seconds < CONTROL_NO_PERSON_OFF_SECONDS)
		{
			absent_seconds++;
		}
		snapshot.dht_ok = DHT11_Read(&temperature_c, &humidity_percent);
		if (snapshot.dht_ok)
		{
			snapshot.temperature_c = temperature_c;
			snapshot.humidity_percent = humidity_percent;
		}
		snapshot.co2_ok = TVOC301_GetCO2(&co2_ppm);
		if (snapshot.co2_ok)
		{
			snapshot.co2_ppm = co2_ppm;
		}

		control_input.co2_valid = snapshot.co2_ok;
		control_input.co2_ppm = snapshot.co2_ppm;
		control_input.light_is_dark =
			(snapshot.light_raw > CONTROL_LIGHT_DARK_THRESHOLD) ? 1 : 0;
		control_input.human_present = snapshot.human_present;
		control_input.absent_seconds = absent_seconds;
		ControlLogic_Update(&control_input, &control_output);
		Actuators_SetLED(control_output.led_on);
		Actuators_SetFan(control_output.fan_on);
		snapshot.led_on = control_output.led_on;
		snapshot.fan_on = control_output.fan_on;

		SensorDisplay_Show(&snapshot, display_page, need_refresh);
		need_refresh = 0;
		for (key_tick = 0; key_tick < 100; key_tick++)
		{
			Delay_ms(10);
			if (Key_GetNum() == 1)
			{
				display_page = (display_page == 0) ? 1 : 0;
				SensorDisplay_Show(&snapshot, display_page, 1);
			}
		}
	}
}
