#include <stdio.h>
#include <string.h>

#include "../Hardware/SensorDisplay.h"

static int oled_clear_calls;
static int oled_chinese_calls;
static uint32_t oled_co2_number;
static uint8_t oled_co2_length;

void OLED_Clear(void)
{
    oled_clear_calls++;
}

void OLED_ShowChinese(uint8_t line, uint8_t column, uint8_t index)
{
    (void)line;
    (void)column;
    (void)index;
    oled_chinese_calls++;
}

void OLED_ShowChar(uint8_t line, uint8_t column, char character)
{
    (void)line;
    (void)column;
    (void)character;
}

void OLED_ShowString(uint8_t line, uint8_t column, char *string)
{
    (void)line;
    (void)column;
    (void)string;
}

void OLED_ShowNum(uint8_t line, uint8_t column, uint32_t number, uint8_t length)
{
	if (line == 3 && column == 10)
	{
		oled_co2_number = number;
		oled_co2_length = length;
	}
}

static int expect_line(const char *name, const char *actual, const char *expected)
{
    if (strcmp(actual, expected) != 0)
    {
        printf("%s mismatch: expected [%s], got [%s]\n", name, expected, actual);
        return 1;
    }
    return 0;
}

int main(void)
{
    SensorSnapshot snapshot;
    char lines[4][17];
    int failures = 0;

    snapshot.dht_ok = 1;
    snapshot.temperature_c = 26;
    snapshot.humidity_percent = 58;
    snapshot.mq135_raw = 1234;
    snapshot.light_raw = 86;
    snapshot.co2_ok = 1;
    snapshot.co2_ppm = 650;
    snapshot.human_present = 1;
    snapshot.led_on = 1;
    snapshot.fan_on = 0;

    SensorDisplay_FormatLines(&snapshot, lines);

    failures += expect_line("line1", lines[0], "T:26C H:58%");
    failures += expect_line("line2", lines[1], "CO2:650ppm");
    failures += expect_line("line3", lines[2], "MQ:1234 L:86");
    failures += expect_line("line4", lines[3], "H:Y L:1 F:0");

    snapshot.dht_ok = 0;
    snapshot.co2_ok = 0;
    snapshot.human_present = 0;
    SensorDisplay_FormatLines(&snapshot, lines);

    failures += expect_line("dht fail", lines[0], "T:--C H:--%");
    failures += expect_line("human no", lines[3], "H:N L:1 F:0");
    failures += expect_line("co2 invalid", lines[1], "CO2:----ppm");

    oled_clear_calls = 0;
    oled_chinese_calls = 0;
    SensorDisplay_Show(&snapshot, 1, 0);
	if (oled_clear_calls != 0 || oled_chinese_calls != 4)
	{
        printf("dynamic page refresh mismatch: clear=%d, chinese=%d\n",
            oled_clear_calls, oled_chinese_calls);
		failures++;
	}

	snapshot.co2_ok = 1;
	snapshot.co2_ppm = 1095;
	oled_co2_number = 0;
	oled_co2_length = 0;
	SensorDisplay_Show(&snapshot, 0, 1);
	if (oled_co2_number != 1095 || oled_co2_length != 4)
	{
		printf("co2 OLED mismatch: number=%lu, length=%u\n",
			(unsigned long)oled_co2_number, (unsigned int)oled_co2_length);
		failures++;
	}

	return failures;
}
