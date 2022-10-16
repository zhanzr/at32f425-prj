#ifndef __OLED_H__
#define __OLED_H__

#include <stdint.h>

#define 	TYPE8X16		1
#define 	TYPE16X16		2
#define 	TYPE6X8			3

#define OLED_I2C_ADDR	0x78

extern void LCD_Init(void);
extern void LCD_CLS(void);
extern void LCD_CLS_y(char y);
extern void LCD_CLS_line_area(uint8_t start_x,uint8_t start_y,uint8_t width);
extern void LCD_P6x8Str(uint8_t x,uint8_t y,uint8_t *ch,const uint8_t *F6x8);
extern void LCD_P8x16Str(uint8_t x,uint8_t y,uint8_t *ch,const uint8_t *F8x16);
extern void LCD_P14x16Str(uint8_t x,uint8_t y,uint8_t ch[],const uint8_t *F14x16_Idx,const uint8_t *F14x16);
extern void LCD_P16x16Str(uint8_t x,uint8_t y,uint8_t *ch,const uint8_t *F16x16_Idx,const uint8_t *F16x16);
//extern void LCD_Print(uint8_t x, uint8_t y, uint8_t *ch);
extern void LCD_PutPixel(uint8_t x,uint8_t y);
extern void LCD_Print(uint8_t x, uint8_t y, uint8_t *ch,uint8_t char_size, uint8_t ascii_size);
extern void LCD_Rectangle(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t gif);
extern void Draw_BMP(uint8_t x,uint8_t y,const uint8_t *bmp); 
extern void LCD_Fill(uint8_t dat);
#endif

