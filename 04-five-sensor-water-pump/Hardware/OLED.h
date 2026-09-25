#ifndef __OLED_H
#define __OLED_H

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, const char *String);
void OLED_ShowSmallChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowSmallString(uint8_t Line, uint8_t Column, const char *String);
void OLED_ShowSmallChinese(uint8_t Line, uint8_t Column, uint8_t Index);
void OLED_ShowSmallChineseString(uint8_t Line, uint8_t Column,
	const uint8_t *IndexList, uint8_t Count);
void OLED_ShowChinese(uint8_t Line, uint8_t Column, uint8_t Index);
void OLED_ShowChineseString(uint8_t Line, uint8_t Column,
	const uint8_t *IndexList, uint8_t Count);
void OLED_ShowSmallNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

#endif
