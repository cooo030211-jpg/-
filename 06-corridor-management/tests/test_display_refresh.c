#include <stdint.h>
#include <stdio.h>

#define SAFETY_DISPLAY_PAGE_NONE 0xFFU

uint8_t SafetyDisplay_EnterPage(uint8_t page, uint8_t *last_page);

static int check_u8(const char *name, uint8_t actual, uint8_t expected)
{
	if (actual != expected)
	{
		printf("%s expected %u got %u\n",
			name, (unsigned int)expected, (unsigned int)actual);
		return 1;
	}
	return 0;
}

int main(void)
{
	uint8_t last_page = SAFETY_DISPLAY_PAGE_NONE;
	int failures = 0;

	failures += check_u8("first page redraw",
		SafetyDisplay_EnterPage(0, &last_page), 1);
	failures += check_u8("first page remembered", last_page, 0);
	failures += check_u8("same page does not redraw",
		SafetyDisplay_EnterPage(0, &last_page), 0);
	failures += check_u8("next page redraw",
		SafetyDisplay_EnterPage(1, &last_page), 1);
	failures += check_u8("next page remembered", last_page, 1);

	return failures;
}
