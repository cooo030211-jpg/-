/*
 * Warehouse safety monitor.
 * MCU: STM32F103C8T6
 * Sensors: MQ-2 PA0, DHT11 PA3, water ADC PA4, PIR PA5
 * Outputs: green PA1, red PA2, servo PA8, fan PA10, pump PA11, buzzer PB13
 * Keys: K1 PB1 moves the cursor, K2 PB11 confirms.
 * Page 1 is live status. Page 2 is manual control.
 */
#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "Mosfet.h"
#include "Output.h"
#include "Sensors.h"
#include "Servo.h"
#include "OLED_HZ16.h"

typedef struct
{
	uint8_t temperature;
	uint8_t humidity;
	uint16_t smoke_raw;
	uint16_t water_raw;
	uint8_t person;
	uint8_t door_open;
	uint8_t fan;
	uint8_t pump;
	uint8_t green_led;
	uint8_t red_led;
	uint8_t buzzer;
	uint8_t dht_valid;
	uint8_t alarm_level;
} SystemState;

enum
{
	PAGE_STATUS = 0,
	PAGE_MANUAL = 1
};

enum
{
	STATUS_TEMPERATURE = 0,
	STATUS_HUMIDITY,
	STATUS_SMOKE,
	STATUS_WATER,
	STATUS_PERSON,
	STATUS_DOOR,
	STATUS_NEXT_PAGE,
	STATUS_COUNT
};

#define STATUS_VISIBLE_ROWS 4U
#define MANUAL_VISIBLE_ROWS 4U

enum
{
	MANUAL_MODE = 0,
	MANUAL_GREEN,
	MANUAL_RED,
	MANUAL_FAN,
	MANUAL_PUMP,
	MANUAL_BUZZER,
	MANUAL_DOOR,
	MANUAL_NEXT_PAGE,
	MANUAL_COUNT
};

#define TEMPERATURE_ALARM_THRESHOLD 50U

static SystemState g_state;
static uint8_t g_page = PAGE_STATUS;
static uint8_t g_menu_index = 0;
static uint8_t g_status_top = 0;
static uint8_t g_manual_top = 0;
static uint8_t g_auto_mode = 1;
static uint8_t g_manual_green = 0;
static uint8_t g_manual_red = 0;
static uint8_t g_manual_fan = 0;
static uint8_t g_manual_pump = 0;
static uint8_t g_manual_buzzer = 0;
static uint8_t g_manual_door = 1;
static uint8_t g_person_latch = 0;
static uint8_t g_door_open = 1;

static const uint8_t HZ_WENDU[] = {OLED_HZ_WEN, OLED_HZ_DU};
static const uint8_t HZ_SHIDU[] = {OLED_HZ_SHI, OLED_HZ_DU};
static const uint8_t HZ_YANWU[] = {OLED_HZ_YAN, OLED_HZ_WU2};
static const uint8_t HZ_YOUREN[] = {OLED_HZ_YOU, OLED_HZ_REN};
static const uint8_t HZ_WUREN[] = {OLED_HZ_WU, OLED_HZ_REN};
static const uint8_t HZ_MENZHUANGTAI[] =
	{OLED_HZ_MEN, OLED_HZ_ZHUANG, OLED_HZ_TAI};

#define HZ_LEN(x) ((uint8_t)(sizeof(x) / sizeof((x)[0])))

static uint8_t IsWaterAlarm(void)
{
	return (g_state.water_raw >= WATER_ALARM_THRESHOLD) ? 1 : 0;
}

static uint8_t IsSmokeWarning(void)
{
	return (g_state.smoke_raw >= SMOKE_WARNING_THRESHOLD) ? 1 : 0;
}

static uint8_t IsTemperatureWarning(void)
{
	return (g_state.dht_valid != 0) &&
		(g_state.temperature >= TEMPERATURE_ALARM_THRESHOLD);
}

static uint8_t IsSmokeFire(void)
{
	return (g_state.smoke_raw >= SMOKE_FIRE_THRESHOLD) ? 1 : 0;
}

static uint8_t IsIntrusion(void)
{
	return (g_door_open == 0) && (g_state.person != 0);
}

static void UpdateAlarmState(void)
{
	if (IsWaterAlarm() || IsSmokeFire() || IsIntrusion())
	{
		g_state.alarm_level = 2;
	}
	else if (IsSmokeWarning() || IsTemperatureWarning())
	{
		g_state.alarm_level = 1;
	}
	else
	{
		g_state.alarm_level = 0;
	}
}

static void ReadSensors(void)
{
	uint8_t temperature;
	uint8_t humidity;

	g_state.smoke_raw = Sensors_ReadSmokeRaw();
	g_state.water_raw = Sensors_ReadWaterRaw();

	if (Sensors_ReadPerson() != 0)
	{
		g_person_latch = 8;
	}
	else if (g_person_latch > 0)
	{
		g_person_latch--;
	}
	g_state.person = (g_person_latch > 0) ? 1 : 0;

	if (Sensors_ReadDHT11(&temperature, &humidity) != 0)
	{
		g_state.temperature = temperature;
		g_state.humidity = humidity;
		g_state.dht_valid = 1;
	}
}

static void ReadFastSensors(void)
{
	g_state.smoke_raw = Sensors_ReadSmokeRaw();
	g_state.water_raw = Sensors_ReadWaterRaw();

	if (Sensors_ReadPerson() != 0)
	{
		g_person_latch = 8;
	}
	else if (g_person_latch > 0)
	{
		g_person_latch--;
	}
	g_state.person = (g_person_latch > 0) ? 1 : 0;
}

static void ApplyOutputs(void)
{
	uint8_t fan;
	uint8_t pump;
	uint8_t green_led;
	uint8_t red_led;
	uint8_t buzzer;

	/* The intrusion check reads the door state, so settle it first. */
	if (g_auto_mode == 0)
	{
		g_door_open = g_manual_door;
	}

	UpdateAlarmState();

	if (g_auto_mode != 0)
	{
		/* Automatic mode: door state comes from the last servo command. */
		fan = IsSmokeWarning();
		pump = IsWaterAlarm();
		green_led = (g_door_open != 0) && (g_state.person != 0);
		red_led = (g_state.alarm_level != 0);
		buzzer = (g_state.alarm_level == 2) ? 1 : 0;
	}
	else
	{
		/* Manual switches still work, but an active alarm is never masked
		   by them: testing an executor must not silence a real hazard. */
		fan = g_manual_fan;
		pump = g_manual_pump;
		green_led = g_manual_green;
		red_led = (uint8_t)(g_manual_red || (g_state.alarm_level != 0));
		buzzer = (uint8_t)(g_manual_buzzer || (g_state.alarm_level == 2));
	}

	g_state.fan = fan;
	g_state.pump = pump;
	g_state.green_led = green_led;
	g_state.red_led = red_led;
	g_state.buzzer = buzzer;
	g_state.door_open = g_door_open;

	Mosfet_SetFan(fan);
	Mosfet_SetPump(pump);
	Output_SetGreenLed(green_led);
	Output_SetRedLed(red_led);
	Output_SetBuzzer(buzzer);
	Servo_SetAngle(g_door_open != 0 ? 0 : 90);   /* 0 deg = open, 90 deg = closed */
}

static void ScrollStatusWindow(void)
{
	if (g_menu_index < g_status_top)
		g_status_top = g_menu_index;
	else if (g_menu_index >= g_status_top + STATUS_VISIBLE_ROWS)
		g_status_top = (uint8_t)(g_menu_index - STATUS_VISIBLE_ROWS + 1U);
}

static void ScrollManualWindow(void)
{
	if (g_menu_index < g_manual_top)
		g_manual_top = g_menu_index;
	else if (g_menu_index >= g_manual_top + MANUAL_VISIBLE_ROWS)
		g_manual_top = (uint8_t)(g_menu_index - MANUAL_VISIBLE_ROWS + 1U);
}

static void ShowStatusItem(uint8_t line, uint8_t item)
{
	OLED_ShowChar(line, 1, (g_menu_index == item) ? '>' : ' ');

	switch (item)
	{
	case STATUS_TEMPERATURE:
		OLED_ShowChineseString(line, 3, HZ_WENDU, HZ_LEN(HZ_WENDU));
		OLED_ShowChar(line, 7, ':');
		if (g_state.dht_valid != 0)
		{
			OLED_ShowNum(line, 8, g_state.temperature, 2);
			OLED_ShowChar(line, 10, 'C');
		}
		else
			OLED_ShowString(line, 8, "--C");
		break;

	case STATUS_HUMIDITY:
		OLED_ShowChineseString(line, 3, HZ_SHIDU, HZ_LEN(HZ_SHIDU));
		OLED_ShowChar(line, 7, ':');
		if (g_state.dht_valid != 0)
		{
			OLED_ShowNum(line, 8, g_state.humidity, 2);
			OLED_ShowChar(line, 10, '%');
		}
		else
			OLED_ShowString(line, 8, "--%");
		break;

	case STATUS_SMOKE:
		OLED_ShowChineseString(line, 3, HZ_YANWU, HZ_LEN(HZ_YANWU));
		OLED_ShowChar(line, 7, ':');
		OLED_ShowNum(line, 8, g_state.smoke_raw, 4);
		break;

	case STATUS_WATER:
		OLED_ShowChinese(line, 3, OLED_HZ_SHUI);
		OLED_ShowChar(line, 7, ':');
		OLED_ShowNum(line, 8, g_state.water_raw, 4);
		break;

	case STATUS_PERSON:
		OLED_ShowChinese(line, 3, OLED_HZ_REN);
		OLED_ShowChar(line, 7, ':');
		OLED_ShowChineseString(line, 8,
			g_state.person != 0 ? HZ_YOUREN : HZ_WUREN, 2);
		break;

	case STATUS_DOOR:
		OLED_ShowChineseString(line, 3, HZ_MENZHUANGTAI,
			HZ_LEN(HZ_MENZHUANGTAI));
		OLED_ShowChar(line, 9, ':');
		OLED_ShowChinese(line, 10,
			g_door_open != 0 ? OLED_HZ_KAI : OLED_HZ_GUAN);
		break;

	case STATUS_NEXT_PAGE:
		OLED_ShowString(line, 3, "NEXT PAGE");
		break;

	default:
		break;
	}
}

static void ShowStatus(void)
{
	uint8_t row;
	uint8_t item;

	for (row = 0; row < STATUS_VISIBLE_ROWS; row++)
	{
		item = (uint8_t)(g_status_top + row);
		if (item < STATUS_COUNT)
			ShowStatusItem((uint8_t)(row + 1U), item);
	}
}

static uint8_t ManualItemValue(uint8_t item)
{
	switch (item)
	{
	case MANUAL_MODE:   return g_auto_mode;
	case MANUAL_GREEN:  return g_manual_green;
	case MANUAL_RED:    return g_manual_red;
	case MANUAL_FAN:    return g_manual_fan;
	case MANUAL_PUMP:   return g_manual_pump;
	case MANUAL_BUZZER: return g_manual_buzzer;
	case MANUAL_DOOR:   return g_door_open;
	default:            return 0;
	}
}

static void ShowManual(void)
{
	uint8_t row;
	uint8_t item;
	uint8_t value;
	static const char *labels[MANUAL_COUNT] =
	{ "MODE", "GREEN", "RED", "FAN", "PUMP", "BEEP", "DOOR", "BACK" };

	for (row = 0; row < MANUAL_VISIBLE_ROWS; row++)
	{
		item = (uint8_t)(g_manual_top + row);
		if (item >= MANUAL_COUNT)
			continue;

		OLED_ShowChar((uint8_t)(row + 1U), 1,
			(g_menu_index == item) ? '>' : ' ');
		OLED_ShowString((uint8_t)(row + 1U), 3, labels[item]);
		if (item == MANUAL_NEXT_PAGE)
			continue;

		value = ManualItemValue(item);
		if (item == MANUAL_MODE)
			OLED_ShowString((uint8_t)(row + 1U), 10, value != 0 ? "AUTO" : "MAN ");
		else if (item == MANUAL_DOOR)
			OLED_ShowString((uint8_t)(row + 1U), 10, value != 0 ? "OPEN" : "CLOS");
		else
			OLED_ShowString((uint8_t)(row + 1U), 10, value != 0 ? "ON " : "OFF");
	}
}

static void ShowPage(void)
{
	static uint8_t last_page = 0xFF;
	static uint8_t last_status_top = 0xFF;
	static uint8_t last_manual_top = 0xFF;

	if (g_page != last_page ||
		(g_page == PAGE_STATUS && g_status_top != last_status_top) ||
		(g_page == PAGE_MANUAL && g_manual_top != last_manual_top))
	{
		OLED_Clear();
		last_page = g_page;
		last_status_top = g_status_top;
		last_manual_top = g_manual_top;
	}
	if (g_page == PAGE_STATUS)
		ShowStatus();
	else
		ShowManual();
}

static uint8_t DisplayChanged(void)
{
	static uint16_t last_smoke;
	static uint16_t last_water;
	static uint8_t last_temp;
	static uint8_t last_humi;
	static uint8_t last_person;
	static uint8_t last_door;
	static uint8_t last_valid;
	static uint8_t last_page = 0xFF;
	static uint8_t last_menu_index = 0xFF;
	static uint8_t last_status_top = 0xFF;
	static uint8_t last_manual_top = 0xFF;

	if (g_state.smoke_raw != last_smoke || g_state.water_raw != last_water ||
		g_state.temperature != last_temp || g_state.humidity != last_humi ||
		g_state.person != last_person || g_door_open != last_door ||
		g_state.dht_valid != last_valid || g_page != last_page ||
		g_menu_index != last_menu_index || g_status_top != last_status_top ||
		g_manual_top != last_manual_top)
	{
		last_smoke = g_state.smoke_raw;
		last_water = g_state.water_raw;
		last_temp = g_state.temperature;
		last_humi = g_state.humidity;
		last_person = g_state.person;
		last_door = g_door_open;
		last_valid = g_state.dht_valid;
		last_page = g_page;
		last_menu_index = g_menu_index;
		last_status_top = g_status_top;
		last_manual_top = g_manual_top;
		return 1;
	}
	return 0;
}

static void ManualAction(void)
{
	if (g_menu_index == MANUAL_NEXT_PAGE)
	{
		g_page = PAGE_STATUS;
		g_menu_index = 0;
		g_status_top = 0;
		return;
	}

	if (g_menu_index == MANUAL_MODE)
	{
		if (g_auto_mode != 0)
		{
			g_manual_door = g_door_open;
			g_auto_mode = 0;
		}
		else
		{
			g_auto_mode = 1;
		}
		return;
	}

	/* The door has two states in both modes: it is the intrusion
	   detection input, not a manual output request. */
	if (g_menu_index == MANUAL_DOOR)
	{
		if (g_auto_mode != 0)
		{
			g_door_open = !g_door_open;
		}
		else
		{
			g_manual_door = !g_manual_door;
		}
		return;
	}

	/* Other manual outputs are only active after selecting MAN mode. */
	if (g_auto_mode != 0)
		return;

	switch (g_menu_index)
	{
	case MANUAL_GREEN:  g_manual_green = !g_manual_green; break;
	case MANUAL_RED:    g_manual_red = !g_manual_red; break;
	case MANUAL_FAN:    g_manual_fan = !g_manual_fan; break;
	case MANUAL_PUMP:   g_manual_pump = !g_manual_pump; break;
	case MANUAL_BUZZER: g_manual_buzzer = !g_manual_buzzer; break;
	default: break;
	}
}

static void HandleKey(uint8_t key_num)
{
	if (key_num == 1)
	{
		if (g_page == PAGE_STATUS)
		{
			g_menu_index++;
			if (g_menu_index >= STATUS_COUNT)
				g_menu_index = 0;
			ScrollStatusWindow();
		}
		else
		{
			g_menu_index++;
			if (g_menu_index >= MANUAL_COUNT)
				g_menu_index = 0;
			ScrollManualWindow();
		}
	}
	else if (key_num == 2)
	{
		if (g_page == PAGE_STATUS)
		{
			if (g_menu_index == STATUS_NEXT_PAGE)
			{
				g_page = PAGE_MANUAL;
				g_menu_index = 0;
				g_manual_top = 0;
			}
		}
		else
		{
			ManualAction();
		}
	}
}

int main(void)
{
	uint8_t key_num;
	uint16_t loop_count = 0;
	uint8_t display_due = 1;

	/* 输出脚必须最先初始化：OLED_Init 内有上百毫秒空循环延时，
	   那期间 PB13 等还是浮空输入，低有效蜂鸣器会悬空误响。 */
	Output_Init();
	Mosfet_Init();
	Key_Init();
	OLED_Init();
	Sensors_Init();
	Servo_Init();

	ReadSensors();
	ApplyOutputs();
	ShowPage();

	while (1)
	{
		key_num = Key_GetNum();
		if (key_num != 0)
		{
			HandleKey(key_num);
			ApplyOutputs();
			display_due = 1;
		}

		if ((loop_count % 4) == 0)
		{
			ReadFastSensors();
			ApplyOutputs();
			if (DisplayChanged() != 0)
				display_due = 1;
		}

		if (loop_count >= 40)
		{
			loop_count = 0;
			ReadSensors();
			ApplyOutputs();
			if (DisplayChanged() != 0)
				display_due = 1;
		}

		if (display_due != 0)
		{
			ShowPage();
			display_due = 0;
		}

		loop_count++;
		Delay_ms(50);
	}
}
